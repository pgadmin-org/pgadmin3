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

// Windows headers
#include <winsock.h>

// App headers
#include "pgConn.h"
#include "../../pgAdmin3.h"

pgConn::pgConn(wxString& szServer = wxString(""), wxString& szDatabase = wxString(""), wxString& szUsername = wxString(""), wxString& szPassword = wxString(""), int iPort = 5432) : objConn()
{
    wxLogDebug(wxT("Creating pgConn object"));
    wxString szMsg, szHost;

    // Check the hostname/ipaddress
    struct hostent *heHost;
	unsigned long lIPAddr;
    struct in_addr saAddr;

    lIPAddr = inet_addr(szServer.c_str());
	if (lIPAddr == INADDR_NONE) // szServer is not an IP address
	{
		heHost = gethostbyname(szServer.c_str());
		if (heHost == NULL)
		{
            szMsg.Printf("Could not resolve hostname: %s", szServer);
			wxLogError(szMsg);
			return;
		}

        memcpy(&(saAddr),heHost->h_addr,heHost->h_length); 
        szHost.Printf("%s", inet_ntoa(saAddr));
    } else {
        szHost = szServer;
    }

    szMsg.Printf(wxT("Server name: %s resolved to: %s"), szServer, szHost);
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
      szConn.Printf("%s%d", szConn, iPort);
    }
    szConn.Trim(FALSE);

    // Open the connection
    szMsg.Clear();
    szMsg.Printf(wxT("Opening connection with connection string: %s"), szConn);
    wxLogInfo(szMsg);

    objConn = PQconnectdb(szConn.c_str());
}

pgConn::~pgConn()
{
    wxLogDebug(wxT("Destroying pgConn object"));
    PQfinish(objConn);
}

wxString pgConn::GetUser()
{
  return wxString(PQuser(objConn));
}

wxString pgConn::GetPassword()
{
  return wxString(PQpass(objConn));
}

wxString pgConn::GetHost()
{
  return wxString(PQhost(objConn));
}

wxString pgConn::GetPort()
{
  return wxString(PQport(objConn));
}

wxString pgConn::GetTTY()
{
  return wxString(PQtty(objConn));
}

wxString pgConn::GetOptions()
{
  return wxString(PQoptions(objConn));
}

int pgConn::GetBackendPID()
{
    return PQbackendPID(objConn);
}

int pgConn::GetStatus()
{
  return PQstatus(objConn);
}

wxString pgConn::GetLastError()
{
  return wxString(PQerrorMessage(objConn));
}

