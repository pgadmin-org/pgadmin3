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
    wxString ExecuteScalar(const wxString& sql);
    pgSet *ExecuteSet(const wxString& sql);
    wxString GetUser() const { return wxString(PQuser(conn)); }
    wxString GetPassword() const { return wxString(PQpass(conn)); }
    wxString GetHost() const { return dbHost; }
    int GetPort() const { return atoi(PQport(conn)); };
    wxString GetTTY() const { return wxString(PQtty(conn)); }
    wxString GetOptions() const { return wxString(PQoptions(conn)); }
    int GetBackendPID() const { return PQbackendPID(conn); }
    int GetStatus() const;
    wxString GetLastError() const { return wxString(PQerrorMessage(conn)); }
    wxString GetVersionString();
    float GetVersionNumber();
    long GetLastSystemOID();

    PGconn *connection() { return conn; }
private:

    PGconn *conn;
    bool resolvedIP;
    wxString dbHost;
};

#endif

