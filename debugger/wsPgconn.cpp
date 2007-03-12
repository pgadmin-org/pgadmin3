//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wspgconn.cpp 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wspgconn.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsPgconn.h"
#include "debugger.h"

#include <wx/wx.h>
#include <wx/log.h>

#include <stdexcept>

#ifdef __WXMSW__
#include <winsock.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// wsPgConn constructors
//
//	A wsPgConn object encapsulates the connection to a PostgreSQL server.  This
//  class is a wrapper around a Pgconn that provides a convenient constructor 
//  and a few member functions.  
//
//	The constructor creates a new thread and connects to the specified server

wsPgConn::wsPgConn( const wxString & server, const wxString & database, const wxString & userName, const wxString & password, const wxString & port, int sslmode )
{
	Init( server, database, userName, password, port, sslmode, true );
}

wsPgConn::wsPgConn( const wsConnProp & props, bool startThread )
{
	Init(  props.m_host, props.m_database, props.m_userName, props.m_password, props.m_port, props.m_sslMode, startThread );
}

void wsPgConn::Init( const wxString & server, const wxString & database, const wxString & username, const wxString & password, const wxString & port, int sslmode, bool startThread )
{
	m_pgConn       = NULL;

	if( startThread )
		m_workerThread = new wsPgThread( *this );
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

	// Build up a connection string
    wxString connectParams;

    if( server.Length()) 
    {
		connectParams.Append( wxT( "host=" ));
		connectParams.Append( server );

		msg += delimiter + server; delimiter = _( ":" );
    }

    if( port.Length()) 
    {
		connectParams += wxT( " port=" );
		connectParams += port;

		msg += delimiter + port; delimiter = _( ":" );
    }


    if( database.Length()) 
    {
		connectParams.Append( wxT( " dbname=" ));
		connectParams.Append( database );

		msg += delimiter + database; delimiter = _( ":" );
    }

    if( username.Length()) 
    {
		connectParams.Append( wxT( " user=" ));
		connectParams.Append( username );

		msg += delimiter + username; delimiter =  _( ":" );
    }

    if( password.Length()) 
    {
		connectParams.Append( wxT( " password=" ));
		connectParams.Append( password );
    }

    connectParams.Trim( true );
    connectParams.Trim( false );

	glApp->getStatusBar()->SetStatusText( wxString(_( "Connecting to " )) + msg, 1 );	
    m_pgConn = PQconnectdb( connectParams.ToAscii());

    if( PQstatus( m_pgConn ) == CONNECTION_OK )
    {
		glApp->getStatusBar()->SetStatusText( wxString(_( "Connected to " )) + msg, 1 );	
#if wxUSE_UNICODE
        PQsetClientEncoding( m_pgConn, "UNICODE" );
#else
        PQsetClientEncoding( m_pgConn, "SQL_ASCII" );
#endif
    }
	else
	{
		throw( std::runtime_error( PQerrorMessage( m_pgConn )));
	}
}

wsPgConn::~wsPgConn()
{
	close();
}

////////////////////////////////////////////////////////////////////////////////
// isConnected()
//
//	Returns TRUE if the connection to the PostgreSQL server is alive and well,
//  otherwise, returns FALSE.

bool wsPgConn::isConnected( void ) const
{
    if( m_pgConn && PQstatus( m_pgConn ) == CONNECTION_OK )
		return( true );
    else
		return( false );
}

////////////////////////////////////////////////////////////////////////////////
// getName()
//
// 	This function returns a user-friendly descriptive name for this connection.
// 	The result is "hostname/database".

const wxString wsPgConn::getName() const
{
    wxString 	result = wxString( PQhost( m_pgConn ), wxConvUTF8 );

    result += wxT( "/" );
    result.Append( wxString( PQdb( m_pgConn ), wxConvUTF8 ));

    return( result );

}

////////////////////////////////////////////////////////////////////////////////
// getHost()
//
//	Returns the name of the host (or the IP address) that we're connected to.

const wxString wsPgConn::getHost() const
{
	return( wxString( PQhost( m_pgConn ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getDatabase()
//
//	Returns the name of the database that we're connected to

const wxString wsPgConn::getDatabase() const
{
	return( wxString( PQdb( m_pgConn ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getConnection()
//
//	Returns the libpq connection handle for this wsPgConn

PGconn * wsPgConn::getConnection()
{
	return( m_pgConn );
}

////////////////////////////////////////////////////////////////////////////////
// startCommand()
//
// 	The GUI thread invokes startCommand() when the user asks us to execute a
//	command.  We pass off the real work to the worker thread.

void wsPgConn::startCommand( const wxString & command, wxEvtHandler * caller, wxEventType eventType )
{
    wxLogInfo( command );

    m_workerThread->startCommand( command, caller, eventType );
}

PGresult * wsPgConn::waitForCommand( const wxString & command )
{
	::wxLogDebug( _( "waiting for %s" ), command.c_str());

	PGresult * result = PQexec( m_pgConn, command.mb_str( wxConvUTF8 ));

	::wxLogDebug( _( "complete" ));

	return( result );
}

////////////////////////////////////////////////////////////////////////////////
// setNoticeHandler()
//
// 	Register a NOTICE handler with the libpq library - libpq will invoke the
//  given handler whenever a NOTICE arrives on this connection. libpq will
//	pass 'arg' to the handler. 'handler' is typically a static function and 
//	'arg' is often a pointer to an object.  That lets you use a regular member 
//	function as a callback (because 'arg' is mapped into a 'this' pointer by the 
// 	callback function).

void wsPgConn::setNoticeHandler( PQnoticeProcessor handler, void * arg )
{
    PQnoticeProcessor p = PQsetNoticeProcessor( m_pgConn, handler, arg );
}

void wsPgConn::close()
{
	if( m_pgConn )
		PQfinish( m_pgConn );

	m_pgConn = NULL;
}
