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

// App headers
#include "pgConn.h"
#include "../../pgAdmin3.h"

pgConn::pgConn(wxString& szServer, wxString& szDatabase, wxString& szUsername, wxString& szPassword, long lPort) : objConn()
{
    wxLogDebug(wxT("Creating pgConn object"));

    // Create the connection string
    wxString szConn;
    if (!szServer.IsEmpty()) {
      szConn.Append(wxT(" host="));
      szConn.Append(szServer);
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
    if (lPort > 0) {
      szConn.Append(wxT(" port="));
      szConn.Append(lPort);
    }
    szConn.Trim(FALSE);

    // Open the connection
    objConn = PQconnectdb(szConn.c_str());

    // Now check the status
    switch(PQstatus(objConn))
    {
        case CONNECTION_STARTED:
            wxLogInfo(wxT("Connecting..."));
            break;

        case CONNECTION_MADE:
            wxLogInfo(wxT("Connected to server..."));
            break;

        case CONNECTION_AWAITING_RESPONSE:
            wxLogInfo(wxT("Awaiting response from server..."));
            break;

        case CONNECTION_AUTH_OK:
            wxLogInfo(wxT("Connection authenticated OK..."));
            break;

        case CONNECTION_SETENV:
            wxLogInfo(wxT("Negotiating environment..."));
            break;

        default:
            wxLogInfo(wxT("Connecting..."));
    }
}


pgConn::~pgConn()
{
    wxLogDebug(wxT("Destroying pgConn object"));
}

