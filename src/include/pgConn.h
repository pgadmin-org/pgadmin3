//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
    PGCONN_ABORTED,     // connect user aborted
    PGCONN_BROKEN       // tcp/pipe broken
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
    pgConn(const wxString& server = wxT(""), const wxString& database = wxT(""), const wxString& username = wxT(""), const wxString& password = wxT(""), int port = 5432, int sslmode=0, OID oid=0);
    ~pgConn();

    void Close();
    bool HasPrivilege(const wxString &objTyp, const wxString &objName, const wxString &priv);
    bool HasFeature(int feature=0);
    bool ExecuteVoid(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql);
    pgSet *ExecuteSet(const wxString& sql);
    wxString GetUser() const { return wxString(PQuser(conn), *conv); }
    wxString GetPassword() const { return wxString(PQpass(conn), *conv); }
    wxString GetHost() const { return dbHost; }
    int GetPort() const { return atoi(PQport(conn)); };
    wxString GetTTY() const { return wxString(PQtty(conn), *conv); }
    wxString GetOptions() const { return wxString(PQoptions(conn), *conv); }
    int GetBackendPID() const { return PQbackendPID(conn); }
    int GetStatus() const;
    int GetLastResultStatus() const { return lastResultStatus; }
    bool IsAlive();
    wxString GetLastError() const;
    wxString GetVersionString();
    OID GetLastSystemOID() const { return lastSystemOID; }
    OID GetDbOid() const { return dbOid; }
    bool BackendMinimumVersion(int major, int minor);
    void RegisterNoticeProcessor(PQnoticeProcessor proc, void *arg);
    wxString SystemNamespaceRestriction(const wxString &nsp);

    void LogError();

#ifdef SSL
    bool IsSSLconnected();
#endif
    PGconn *connection() { return conn; }
    void Notice(const char *msg);

private:
    PGconn *conn;
    int lastResultStatus;
    bool features[32];

    int connStatus;
    int minorVersion, majorVersion;
    wxMBConv *conv;
    bool needColQuoting;
    wxString dbHost;
    OID lastSystemOID;
    OID dbOid;

    void *noticeArg;
    PQnoticeProcessor noticeProc;

    friend class pgQueryThread;
};

#endif

