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
    int ExecuteVoid(wxString& szSQL);
    wxString ExecuteScalar(wxString& szSQL);
    wxString GetUser();
    wxString GetPassword();
    wxString GetHost();
    wxString GetPort();
    wxString GetTTY();
    wxString GetOptions();
    int GetBackendPID();
    int GetStatus();
    wxString GetLastError();
    wxString GetServerVersion();

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

enum
{
    PGCONN_EMPTY_QUERY = PGRES_EMPTY_QUERY,
    PGCONN_COMMAND_OK = PGRES_COMMAND_OK,
    PGCONN_TUPLES_OK = PGRES_TUPLES_OK,
    PGCONN_COPY_OUT = PGRES_COPY_OUT,
    PGCONN_COPY_IN = PGRES_COPY_IN,
    PGCONN_BAD_RESPONSE = PGRES_BAD_RESPONSE,
    PGCONN_NONFATAL_ERROR = PGRES_NONFATAL_ERROR,
    PGCONN_FATAL_ERROR = PGRES_FATAL_ERROR
};

#endif

