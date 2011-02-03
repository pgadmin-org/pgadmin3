//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgPgConn.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "debugger/dbgPgConn.h"
#include "utils/sysLogger.h"

#include <stdexcept>

#ifdef __WXMSW__
#include <winsock.h>
typedef u_long in_addr_t;
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
// dbgPgConn constructors
//
//    A dbgPgConn object encapsulates the connection to a PostgreSQL server.  This
//  class is a wrapper around a Pgconn that provides a convenient constructor
//  and a few member functions.
//
//    The constructor creates a new thread and connects to the specified server

dbgPgConn::dbgPgConn(frmDebugger *frame, const wxString &server, const wxString &database, const wxString &userName, const wxString &password, const wxString &port, int sslmode, const wxString &applicationname)
	: m_frame(frame)
{
	Init( server, database, userName, password, port, sslmode, applicationname, true );
}

dbgPgConn::dbgPgConn(frmDebugger *frame, const dbgConnProp &props, bool startThread )
	: m_frame(frame)
{
	Init(  props.m_host, props.m_database, props.m_userName, props.m_password, props.m_port, props.m_sslMode, props.m_applicationName, startThread );
}

void dbgPgConn::Init( const wxString &server, const wxString &database, const wxString &username, const wxString &password, const wxString &port, int sslmode, const wxString &applicationname, bool startThread )
{
	bool utfConnectString = false;
	bool libcConnectString = false;

	m_pgConn       = NULL;
	m_majorVersion = 0;
	m_minorVersion = 0;
	m_debuggerApiVersion = DEBUGGER_UNKNOWN_API;
	m_isEdb = false;

	if( startThread )
		m_workerThread = new dbgPgThread( *this );
	else
		m_workerThread = NULL;

	wxString 	msg;
	wxString	delimiter;

	// To keep the user interface thread responsive while we're waiting for the
	// PostgreSQL server, we create a separate thread to interact with the
	// database - the worker thread sleeps until the GUI thread signals that it
	// has some work to do.  When the result set arrives from the server, the
	// worker thread creates a DBResult event and posts it to the GUI thread's
	// event queue.

	if( m_workerThread )
	{
		m_workerThread->Create();
		m_workerThread->Run();
	}

	// Figure out the hostname/IP address
	struct hostent *host;
	in_addr_t addr;
	wxString hostip, hostname;

#ifdef __WXMSW__
	struct in_addr ipaddr;
#else
	unsigned long ipaddr;
#endif

#ifndef __WXMSW__
	if (!(server.IsEmpty() || server.StartsWith(wxT("/"))))
	{
#endif
		addr = inet_addr(server.ToAscii());
		if (addr == INADDR_NONE) // szServer is not an IP address
		{
			host = gethostbyname(server.ToAscii());
			if (host == NULL)
			{
				wxLogError(__("Could not resolve hostname %s"), server.c_str());
				return;
			}

			memcpy(&(ipaddr), host->h_addr, host->h_length);
			hostip = wxString::FromAscii(inet_ntoa(*((struct in_addr *) host->h_addr_list[0])));
			hostname = server;
		}
		else
		{
			hostip = server;
			hostname = server;
		}
#ifndef __WXMSW__
	}
	else
		hostname = server;
#endif

	// Build up a connection string
	wxString connectParams;

	if(hostname.Length())
	{
		connectParams.Append(wxT( "host="));
		connectParams.Append(hostname);

		msg += delimiter + server;
		delimiter = _(":");
	}

	if (hostip.Length())
	{
		connectParams.Append(wxT(" hostaddr="));
		connectParams.Append(hostip);
	}

	if( port.Length())
	{
		connectParams += wxT(" port=");
		connectParams += port;

		msg += delimiter + port;
		delimiter = _(":");
	}


	if( database.Length())
	{
		connectParams.Append(wxT(" dbname="));
		connectParams.Append(qtConnString(database));

		msg += delimiter + database;
		delimiter = _(":");
	}

	if(username.Length())
	{
		connectParams.Append(wxT(" user="));
		connectParams.Append(username );

		msg += delimiter + username;
		delimiter =  _(":");
	}

	if(password.Length())
	{
		connectParams.Append(wxT(" password="));
		connectParams.Append(password);
	}

	switch (sslmode)
	{
		case 1:
			connectParams.Append(wxT(" sslmode=require"));
			break;

		case 2:
			connectParams.Append(wxT(" sslmode=prefer"));
			break;

		case 3:
			connectParams.Append(wxT(" sslmode=allow"));
			break;

		case 4:
			connectParams.Append(wxT(" sslmode=disable"));
			break;

		case 5:
			connectParams.Append(wxT(" sslmode=verify-ca"));
			break;

		case 6:
			connectParams.Append(wxT(" sslmode=verify-full"));
			break;

		default:
			break;
	}

	connectParams.Trim(true);
	connectParams.Trim(false);

#ifdef HAVE_CONNINFO_PARSE
	if (!applicationname.IsEmpty())
	{
		// Check connection string with application_name
		char *errmsg;
		wxString connectParams_with_applicationname = connectParams + wxT(" application_name='") + applicationname + wxT("'");
		if (PQconninfoParse(connectParams_with_applicationname.mb_str(wxConvUTF8), &errmsg))
		{
			utfConnectString = true;
			connectParams = connectParams_with_applicationname;
		}
		else if (PQconninfoParse(connectParams_with_applicationname.mb_str(wxConvLibc), &errmsg))
		{
			libcConnectString = true;
			connectParams = connectParams_with_applicationname;
		}
	}
#endif

	m_frame->getStatusBar()->SetStatusText( wxString::Format(_( "Connecting to %s" ), msg.c_str()), 1 );
	wxCharBuffer cstrUTF = connectParams.mb_str(wxConvUTF8);
	wxCharBuffer cstrLibc = connectParams.mb_str(wxConvLibc);

	if (!libcConnectString)
		m_pgConn = PQconnectdb(cstrUTF);
	else
		m_pgConn = PQconnectdb(cstrLibc);

	if (PQstatus(m_pgConn) != CONNECTION_OK)
	{
		PQfinish(m_pgConn);
		m_pgConn = PQconnectdb(cstrLibc);
	}

	if( PQstatus( m_pgConn ) == CONNECTION_OK )
	{
		m_frame->getStatusBar()->SetStatusText( wxString::Format(_( "Connected to %s" ), msg.c_str()), 1 );
		PQsetClientEncoding( m_pgConn, "UNICODE" );
	}
	else
	{
		throw( std::runtime_error( PQerrorMessage( m_pgConn )));
	}
}

dbgPgConn::~dbgPgConn()
{
	Close();
}

////////////////////////////////////////////////////////////////////////////////
// isConnected()
//
//    Returns true if the connection to the PostgreSQL server is alive and well,
//  otherwise, returns false.

bool dbgPgConn::isConnected( void ) const
{
	if( m_pgConn && PQstatus( m_pgConn ) == CONNECTION_OK )
		return( true );
	else
		return( false );
}

////////////////////////////////////////////////////////////////////////////////
// getName()
//
//     This function returns a user-friendly descriptive name for this connection.
//     The result is "hostname/database".

const wxString dbgPgConn::getName() const
{
	wxString 	result = wxString( PQhost( m_pgConn ), wxConvUTF8 );

	result += wxT( "/" );
	result.Append( wxString( PQdb( m_pgConn ), wxConvUTF8 ));

	return( result );

}

////////////////////////////////////////////////////////////////////////////////
// getHost()
//
//    Returns the name of the host (or the IP address) that we're connected to.

const wxString dbgPgConn::getHost() const
{
	return( wxString( PQhost( m_pgConn ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getDatabase()
//
//    Returns the name of the database that we're connected to

const wxString dbgPgConn::getDatabase() const
{
	return( wxString( PQdb( m_pgConn ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getConnection()
//
//    Returns the libpq connection handle for this dbgPgConn

PGconn *dbgPgConn::getConnection()
{
	return( m_pgConn );
}

////////////////////////////////////////////////////////////////////////////////
// startCommand()
//
//     The GUI thread invokes startCommand() when the user asks us to execute a
//    command.  We pass off the real work to the worker thread.

void dbgPgConn::startCommand( const wxString &command, wxEvtHandler *caller, wxEventType eventType, dbgPgParams *params )
{
	wxLogSql(wxT("%s"), command.c_str());

	m_workerThread->startCommand(command, caller, eventType, params);
}

PGresult *dbgPgConn::waitForCommand( const wxString &command )
{
	wxLogSql(wxT("%s"), command.c_str());

	PGresult *result = PQexec( m_pgConn, command.mb_str( wxConvUTF8 ));

	return( result );
}

////////////////////////////////////////////////////////////////////////////////
// setNoticeHandler()
//
//     Register a NOTICE handler with the libpq library - libpq will invoke the
//  given handler whenever a NOTICE arrives on this connection. libpq will
//    pass 'arg' to the handler. 'handler' is typically a static function and
//    'arg' is often a pointer to an object.  That lets you use a regular member
//    function as a callback (because 'arg' is mapped into a 'this' pointer by the
//     callback function).

void dbgPgConn::setNoticeHandler( PQnoticeProcessor handler, void *arg )
{
	PQnoticeProcessor p = NULL;
	p = PQsetNoticeProcessor( m_pgConn, handler, arg );
}

void dbgPgConn::Close()
{
	// Attempt to cancel any ongoing query
	Cancel();

	// Wait a tenth of a second or so for things to sort themselves out.
	// Otherwise on Windows things can get funky here ...
	wxMilliSleep(100);

	if (m_workerThread)
	{
		m_workerThread->Die();
		m_workerThread->Wait();

		delete m_workerThread;
		m_workerThread = NULL;
	}

	if (m_pgConn)
		PQfinish(m_pgConn);

	m_pgConn = NULL;
}

void dbgPgConn::Cancel()
{
	// Attempt to cancel any ongoing query
	if (m_pgConn)
	{
		PGcancel *cancel = PQgetCancel(m_pgConn);
		char errbuf[256];
		PQcancel(cancel, errbuf, sizeof(errbuf));
		PQfreeCancel(cancel);
	}
}

// Check the backend version
bool dbgPgConn::BackendMinimumVersion(int major, int minor)
{
	if (!m_majorVersion)
	{
		wxString version = GetVersionString();
		sscanf(version.ToAscii(), "%*s %d.%d", &m_majorVersion, &m_minorVersion);
		m_isEdb = version.Upper().Matches(wxT("ENTERPRISEDB*"));

		// EnterpriseDB 8.3 beta 1 & 2 and possibly later actually have PostgreSQL 8.2 style
		// catalogs. This is expected to change either before GA, but in the meantime we
		// need to check the catalogue version in more detail, and if we don't see what looks
		// like a 8.3 catalog, force the version number back to 8.2. Yuck.
		if (m_isEdb && m_majorVersion == 8 && m_minorVersion == 3)
		{
			PGresult *res;
			wxString result;

			res = waitForCommand(wxT( "SELECT count(*) FROM pg_attribute WHERE attname = 'proconfig' AND attrelid = 'pg_proc'::regclass"));

			if (PQresultStatus(res) == PGRES_TUPLES_OK)
			{
				// Retrieve the query result and return it.
				result = wxString(PQgetvalue(res, 0, 0), wxConvUTF8);

				// Cleanup & exit
				PQclear(res);
			}
			if (result == wxT("0"))
				m_minorVersion = 2;
		}
		else
			m_isGreenplum = version.Upper().Matches(wxT("*GREENPLUM DATABASE*"));
	}

	return m_majorVersion > major || (m_majorVersion == major && m_minorVersion >= minor);
}

// Check the EDB backend version
bool dbgPgConn::EdbMinimumVersion(int major, int minor)
{
	return BackendMinimumVersion(major, minor) && GetIsEdb();
}

// Get the debugger API version
DebuggerApiVersions dbgPgConn::DebuggerApiVersion()
{
	if (m_debuggerApiVersion > 0)
		return m_debuggerApiVersion;

	// The v1 protocol didn't have pldbg_get_proxy_info()
	wxString result;

	PGresult *res = waitForCommand(wxT( "SELECT count(*) FROM pg_proc WHERE proname = 'pldbg_get_proxy_info';"));

	if (PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		// Retrieve the query result and return it.
		if (wxString(PQgetvalue(res, 0, 0), wxConvUTF8) == wxT("0"))
		{
			PQclear(res);
			m_debuggerApiVersion = DEBUGGER_V1_API;
			return DEBUGGER_V1_API;
		}

		PQclear(res);
	}
	else
	{
		wxLogError(wxT("%s"), wxString(PQerrorMessage(m_pgConn), wxConvUTF8).c_str());
		return DEBUGGER_UNKNOWN_API;
	}

	// We have pldbg_get_proxy_info, so use it to get the API version
	res = waitForCommand(wxT( "SELECT proxyapiver FROM pldbg_get_proxy_info();"));

	if (PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		// Retrieve the query result and return it.
		m_debuggerApiVersion = (DebuggerApiVersions)atoi(wxString(PQgetvalue(res, 0, 0), wxConvUTF8).ToAscii());
		PQclear(res);
	}
	else
	{
		wxLogError(wxT("%s"), wxString(PQerrorMessage(m_pgConn), wxConvUTF8).c_str());
		return DEBUGGER_UNKNOWN_API;
	}

	return m_debuggerApiVersion;
}


wxString dbgPgConn::GetVersionString()
{
	PGresult *res;
	wxString result;

	res = waitForCommand(wxT( "SELECT version();"));

	if (PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		// Retrieve the query result and return it.
		result = wxString(PQgetvalue(res, 0, 0), wxConvUTF8);

		// Cleanup & exit
		PQclear(res);
	}

	return result;
}

bool dbgPgConn::GetIsEdb()
{
	// to retrieve edb flag
	BackendMinimumVersion(0, 0);
	return m_isEdb;
}

bool dbgPgConn::GetIsGreenplum()
{
	// to retrieve edb flag
	BackendMinimumVersion(0, 0);
	return m_isGreenplum;
}



