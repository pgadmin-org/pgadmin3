//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConn.cpp - PostgreSQL Connection class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// Network  headers
#ifdef __WXMSW__
    #include <winsock.h>
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
#endif

// App headers
#include "pgConn.h"
#include "pgAdmin3.h"
#include "misc.h"
#include "pgSet.h"
#include "sysLogger.h"


pgConn::pgConn(const wxString& server, const wxString& database, const wxString& username, const wxString& password, int port)
{
    wxLogInfo(wxT("Creating pgConn object"));
    wxString msg, hostip;

    // Check the hostname/ipaddress
    struct hostent *host;
    unsigned long addr;
    conn=0;
    
#ifdef __WXMSW__
    struct in_addr ipaddr;
#else
    unsigned long ipaddr;
#endif
    
    
    addr = inet_addr(server.ToAscii());
	if (addr == INADDR_NONE) // szServer is not an IP address
	{
		host = gethostbyname(server.ToAscii());
		if (host == NULL)
		{
            resolvedIP = FALSE;
            wxLogError(__("Could not resolve hostname %s"), server.c_str());
			return;
		}

        memcpy(&(ipaddr),host->h_addr,host->h_length); 
	    hostip = wxString::FromAscii(inet_ntoa(*((struct in_addr*) host->h_addr_list[0])));

    }
    else
        hostip = server;

    resolvedIP = TRUE;
    wxLogInfo(wxT("Server name: %s (resolved to: %s)"), server.c_str(), hostip.c_str());

    // Create the connection string
    wxString connstr;
    if (!server.IsEmpty()) {
      connstr.Append(wxT(" hostaddr="));
      connstr.Append(hostip);
    }
    if (!database.IsEmpty()) {
      connstr.Append(wxT(" dbname="));
      connstr.Append(database);
    }
    if (!username.IsEmpty()) {
      connstr.Append(wxT(" user="));
      connstr.Append(username);
    }
    if (!password.IsEmpty()) {
      connstr.Append(wxT(" password="));
      connstr.Append(password);
    }
    if (port > 0) {
      connstr.Append(wxT(" port="));
      connstr.Append(NumToStr((long)port));
    }
    connstr.Trim(FALSE);

    // Open the connection
    wxLogInfo(wxT("Opening connection with connection string: %s"), connstr.c_str());

    conn = PQconnectdb(connstr.ToAscii());
    dbHost = server;
}

pgConn::~pgConn()
{
    wxLogInfo(wxT("Destroying pgConn object"));
    if (conn)
        PQfinish(conn);
}

//////////////////////////////////////////////////////////////////////////
// Execute SQL
//////////////////////////////////////////////////////////////////////////

bool pgConn::ExecuteVoid(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;

    wxLogSql(wxT("Void query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    qryRes = PQexec(conn, sql.ToAscii());
    int res = PQresultStatus(qryRes);

    // Check for errors
    if (res != PGRES_TUPLES_OK &&
        res != PGRES_COMMAND_OK)
        wxLogError(wxT("%s"), wxString::FromAscii(PQerrorMessage(conn)));


    // Cleanup & exit
    PQclear(qryRes);
    return res == PGRES_TUPLES_OK || res == PGRES_COMMAND_OK;
}

wxString pgConn::ExecuteScalar(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxLogSql(wxT("Scalar query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    qryRes = PQexec(conn, sql.ToAscii());
        
    // Check for errors
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK)
    {
        wxLogError(wxT("%s"), wxString::FromAscii(PQerrorMessage(conn)));
        PQclear(qryRes);
        return wxEmptyString;
    }

	// Check for a returned row
    if (PQntuples(qryRes) < 1)
    {
		wxLogInfo(wxT("Query returned no tuples"));
        PQclear(qryRes);
        return wxEmptyString;
	}
	
	// Retrieve the query result and return it.
    wxString result=wxString::FromAscii(PQgetvalue(qryRes, 0, 0));

    wxLogSql(wxT("Query result: %s"), result.c_str());

    // Cleanup & exit
    PQclear(qryRes);
    return result;
}

pgSet *pgConn::ExecuteSet(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxLogSql(wxT("Set query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    qryRes = PQexec(conn, sql.ToAscii());
    pgSet *set = new pgSet(qryRes, conn);
    if (!set) {
        wxLogError(__("Couldn't create a pgSet object!"));
        PQclear(qryRes);
        return NULL;
    }

	return set;
}

//////////////////////////////////////////////////////////////////////////
// Info
//////////////////////////////////////////////////////////////////////////

int pgConn::GetStatus() const
{
    if(resolvedIP) {
	    return PQstatus(conn);
    } else {
        return PGCONN_DNSERR;
    }
}

wxString pgConn::GetVersionString()
{
	return ExecuteScalar(wxT("SELECT version();"));
}

float pgConn::GetVersionNumber()
{
    int major, minor;
    wxString version;

    static wxString decsep;
    if (decsep.Length() == 0) {
        decsep.Printf(wxT("%lf"), 1.2);
        decsep = decsep[(unsigned int)1];
    }

	if (sscanf(GetVersionString().ToAscii(), "%*s %d.%d", &major, &minor) >= 2)
	{
		version.Printf(wxT("%d%s%d"), major, decsep.c_str(), minor);
	}
	return StrToDouble(version);
}


long pgConn::GetLastSystemOID()
{
	return StrToLong(ExecuteScalar(wxT("SELECT datlastsysoid FROM pg_database LIMIT 1;")));
}
