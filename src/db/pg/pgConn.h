//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgConn.h - PostgreSQL Connection class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCONN_H
#define PGCONN_H

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// App headers
#include "../../pgAdmin3.h"


// Class declarations
class pgConn
{
public:
    pgConn(wxString& szServer, wxString& szDatabase, wxString& szUsername, wxString& szPassword, int iPort);
    ~pgConn();
    wxString GetUser();
    wxString GetPassword();
    wxString GetHost();
    wxString GetPort();
    wxString GetTTY();
    wxString GetOptions();
    int GetBackendPID();
    int GetStatus();
    wxString GetLastError();

private:
    PGconn *objConn;
};

// status enums
enum
{
    PGCONN_OK = CONNECTION_OK,
    PGCONN_BAD = CONNECTION_BAD,
    PGCONN_ABORTED
};

#endif

