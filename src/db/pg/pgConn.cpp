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
#endif

// App headers
#include "pgConn.h"
#include "../../pgAdmin3.h"
#include "pgSet.h"

pgConn::pgConn(const wxString& szServer, const wxString& szDatabase, const wxString& szUsername, const wxString& szPassword, int iPort)
{
    wxLogInfo(wxT("Creating pgConn object"));
    wxString szMsg, szHost;

    // Check the hostname/ipaddress
    struct hostent *heHost;
    unsigned long lIPAddr;
    
#ifdef __WXMSW__
    struct in_addr saAddr;
#else
    unsigned long saAddr;
#endif
    
    
    lIPAddr = inet_addr(szServer.c_str());
	if (lIPAddr == INADDR_NONE) // szServer is not an IP address
	{
		heHost = gethostbyname(szServer.c_str());
		if (heHost == NULL)
		{
            bResolvedIP = FALSE;
            szMsg.Printf("Could not resolve hostname: %s", szServer.c_str());
			wxLogError(szMsg);
			return;
		}

        memcpy(&(saAddr),heHost->h_addr,heHost->h_length); 
	    szHost.Printf("%s", inet_ntoa(*((struct in_addr*) heHost->h_addr_list[0])));

    } else {
        szHost = szServer;
    }

    bResolvedIP = TRUE;
    szMsg.Printf(wxT("Server name: %s (resolved to: %s)"), szServer.c_str(), szHost.c_str());
    wxLogInfo(szMsg);

    // Create the connection string
    wxString szConn;
    if (!szServer.IsEmpty()) {
      szConn.Append(wxT(" hostaddr="));
      szConn.Append(szHost);
    }
    if (!szDatabase.IsEmpty()) {
      szConn.Append(wxT(" dbname="));
      szConn.Append(szDatabase);
    }
    if (!szUsername.IsEmpty()) {
      szConn.Append(wxT(" user="));
      szConn.Append(szUsername);
    }
    if (!szPassword.IsEmpty()) {
      szConn.Append(wxT(" password="));
      szConn.Append(szPassword);
    }
    if (iPort > 0) {
      szConn.Append(wxT(" port="));
      szConn.Printf("%s%d", szConn.c_str(), iPort);
    }
    szConn.Trim(FALSE);

    // Open the connection
    szMsg.Clear();
    szMsg.Printf(wxT("Opening connection with connection string: %s"), szConn.c_str());
    wxLogInfo(szMsg);

    objConn = PQconnectdb(szConn.c_str());
    szDBHost = szServer;
}

pgConn::~pgConn()
{
    wxLogInfo(wxT("Destroying pgConn object"));
    PQfinish(objConn);
}

//////////////////////////////////////////////////////////////////////////
// Execute SQL
//////////////////////////////////////////////////////////////////////////

int pgConn::ExecuteVoid(const wxString& szSQL)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString szMsg;
    szMsg.Printf(wxT("Void query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), szSQL.c_str());
    wxLogStatus(szMsg);
    qryRes = PQexec(objConn, szSQL.c_str());
    int iRes = PQresultStatus(qryRes);

    // Cleanup & exit
    PQclear(qryRes);
    return iRes;
}

wxString pgConn::ExecuteScalar(const wxString& szSQL) const
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString szMsg;
    int iPort = this->GetPort();
    szMsg.Printf(wxT("Scalar query (%s:%d): %s"), this->GetHost().c_str(), iPort, szSQL.c_str());
    wxLogInfo(szMsg);
    qryRes = PQexec(objConn, szSQL.c_str());
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK) {
        return wxString("");
    }

    // Retrieve the query result and return it.
    wxString szResult;
    szResult.Printf("%s", PQgetvalue(qryRes, 0, 0));
    szMsg.Printf(wxT("Query result: %s"), szResult.c_str());
    wxLogInfo(szMsg);

    // Cleanup & exit
    PQclear(qryRes);
    return szResult;
}

pgSet *pgConn::ExecuteSet(const wxString& szSQL)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString szMsg;
    szMsg.Printf(wxT("Set query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), szSQL.c_str());
    wxLogInfo(szMsg);
    qryRes = PQexec(objConn, szSQL.c_str());
    pgSet *objSet = new pgSet(qryRes, objConn);

    // Don't cleanup here, let the pgSet do that itself.
    return objSet;
}

//////////////////////////////////////////////////////////////////////////
// Info
//////////////////////////////////////////////////////////////////////////

wxString pgConn::GetUser() const
{
  return wxString(PQuser(objConn));
}

wxString pgConn::GetPassword() const
{
  return wxString(PQpass(objConn));
}

wxString pgConn::GetHost() const
{
  return szDBHost;
}

int pgConn::GetPort() const
{
  return atoi(PQport(objConn));
}

wxString pgConn::GetTTY() const
{
  return wxString(PQtty(objConn));
}

wxString pgConn::GetOptions() const
{
  return wxString(PQoptions(objConn));
}

int pgConn::GetBackendPID()
{
    return PQbackendPID(objConn);
}

int pgConn::GetStatus()
{
    if(bResolvedIP) {
	    return PQstatus(objConn);
    } else {
        return PGCONN_DNSERR;
    }
}

wxString pgConn::GetLastError() const
{
	return wxString(PQerrorMessage(objConn));
}

wxString pgConn::GetServerVersion() const
{
	wxString szSQL;
    szSQL.Printf("SELECT version();");
	return ExecuteScalar(szSQL);
}

wxString pgConn::GetLastSystemOID() const
{
	wxString szSQL;
    szSQL.Printf("SELECT datlastsysoid FROM pg_database LIMIT 1;");
	return ExecuteScalar(szSQL);
}