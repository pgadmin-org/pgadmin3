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
#include "pgAdmin3.h"
#include "pgSet.h"

// status enums
enum 
{
    PGCONN_OK = CONNECTION_OK,
    PGCONN_BAD = CONNECTION_BAD,
    PGCONN_REFUSED,
    PGCONN_DNSERR,
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

// Class declarations
class pgConn
{
public:
    pgConn(const wxString& server = wxString(""), const wxString& database = wxString(""), const wxString& username = wxString(""), const wxString& password = wxString(""), int port = 5432);
    ~pgConn();
    int ExecuteVoid(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql) const;
    pgSet *ExecuteSet(const wxString& sql);
    wxString GetUser() const;
    wxString GetPassword() const;
    wxString GetHost() const;
    int GetPort() const;
    wxString GetTTY() const;
    wxString GetOptions() const;
    int GetBackendPID();
    int GetStatus();
    wxString GetLastError() const;
    wxString GetVersionString() const;
    float GetVersionNumber();
    long GetLastSystemOID();

private:
    PGconn *conn;
    bool resolvedIP;
    wxString dbHost;
};

#endif

