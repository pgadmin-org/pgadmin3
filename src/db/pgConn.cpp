//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

pgConn::pgConn(const wxString& server, const wxString& database, const wxString& username, const wxString& password, int port)
{
    wxLogInfo(wxT("Creating pgConn object"));
    wxString msg, hostip;

    // Check the hostname/ipaddress
    struct hostent *host;
    unsigned long addr;
    
#ifdef __WXMSW__
    struct in_addr ipaddr;
#else
    unsigned long ipaddr;
#endif
    
    
    addr = inet_addr(server.c_str());
	if (addr == INADDR_NONE) // szServer is not an IP address
	{
		host = gethostbyname(server.c_str());
		if (host == NULL)
		{
            resolvedIP = FALSE;
            msg.Printf("Could not resolve hostname: %s", server.c_str());
			wxLogError(msg);
			return;
		}

        memcpy(&(ipaddr),host->h_addr,host->h_length); 
	    hostip.Printf("%s", inet_ntoa(*((struct in_addr*) host->h_addr_list[0])));

    } else {
        hostip = server;
    }

    resolvedIP = TRUE;
    msg.Printf(wxT("Server name: %s (resolved to: %s)"), server.c_str(), hostip.c_str());
    wxLogInfo(msg);

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
      connstr.Append(NumToStr((double)port));
    }
    connstr.Trim(FALSE);

    // Open the connection
    msg.Printf(wxT("Opening connection with connection string: %s"), connstr.c_str());
    wxLogInfo(msg);

    conn = PQconnectdb(connstr.c_str());
    dbHost = server;
}

pgConn::~pgConn()
{
    wxLogInfo(wxT("Destroying pgConn object"));
    PQfinish(conn);
}

//////////////////////////////////////////////////////////////////////////
// Execute SQL
//////////////////////////////////////////////////////////////////////////

int pgConn::ExecuteVoid(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString msg;
    msg.Printf(wxT("Void query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    wxLogStatus(msg);
    qryRes = PQexec(conn, sql.c_str());
    int res = PQresultStatus(qryRes);

    // Check for errors
    if (res != PGRES_TUPLES_OK &&
        res != PGRES_COMMAND_OK) {
        msg.Printf(wxT("%s"), PQerrorMessage(conn));
        wxLogError(msg);
    }

    // Cleanup & exit
    PQclear(qryRes);
    return res;
}

wxString pgConn::ExecuteScalar(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString msg;
    msg.Printf(wxT("Scalar query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    wxLogInfo(msg);
    qryRes = PQexec(conn, sql.c_str());
        
    // Check for errors
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK) {
        msg.Printf(wxT("%s"), PQerrorMessage(conn));
        wxLogError(msg);
        PQclear(qryRes);
        return wxString("");
    }

	// Check for a returned row
    if (PQntuples(qryRes) < 1) {
		msg.Printf(wxT("Query returned no tuples"));
        wxLogInfo(msg);
        PQclear(qryRes);
        return wxString("");
	}
	
	// Retrieve the query result and return it.
	wxString result;
    result.Printf("%s", PQgetvalue(qryRes, 0, 0));
    msg.Printf(wxT("Query result: %s"), result.c_str());
    wxLogInfo(msg);

    // Cleanup & exit
    PQclear(qryRes);
    return result;
}

pgSet *pgConn::ExecuteSet(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString msg;
    msg.Printf(wxT("Set query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    wxLogInfo(msg);
    qryRes = PQexec(conn, sql.c_str());
    pgSet *set = new pgSet(qryRes, conn);
    if (!set) {
        wxLogError(wxT("Couldn't create a pgSet object!"));
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
	wxString sql;
    sql.Printf("SELECT version();");
	return ExecuteScalar(sql);
}

float pgConn::GetVersionNumber()
{
    int major, minor;
    wxString version;

	if (sscanf(GetVersionString(), "%*s %d.%d", &major, &minor) >= 2)
	{
		version.Printf("%d.%d", major, minor);
	}
	return (float) atof(version.c_str());
}


long pgConn::GetLastSystemOID()
{
	wxString sql;
    sql.Printf("SELECT datlastsysoid FROM pg_database LIMIT 1;");
	return atol(ExecuteScalar(sql));
}
