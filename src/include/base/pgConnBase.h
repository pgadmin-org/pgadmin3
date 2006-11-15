//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConnBase.h - PostgreSQL basic Connection class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCONNBASE_H
#define PGCONNBASE_H

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// App headers
#include "base/pgSetBase.h"

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

enum
{
    PGCONN_TXSTATUS_IDLE = PQTRANS_IDLE,
    PGCONN_TXSTATUS_ACTIVE = PQTRANS_ACTIVE,
    PGCONN_TXSTATUS_INTRANS = PQTRANS_INTRANS,
    PGCONN_TXSTATUS_INERROR = PQTRANS_INERROR,
    PGCONN_TXSTATUS_UNKNOWN = PQTRANS_UNKNOWN
};


// Our version of a pgNotify
typedef struct pgNotification {
    wxString name;
    int pid;
    wxString data;
} pgNotification;

// An error record
typedef struct pgError {
    wxString severity;
    wxString sql_state;
    wxString msg_primary;
    wxString msg_detail;
    wxString msg_hint;
    wxString statement_pos;
    wxString internal_pos;
    wxString internal_query;
    wxString context;
    wxString source_file;
    wxString source_line;
    wxString source_function;
    wxString formatted_msg;
} pgError;

// Class declarations
class pgConnBase
{
public:
    pgConnBase(const wxString& server = wxT(""), const wxString& database = wxT(""), const wxString& username = wxT(""), const wxString& password = wxT(""), int port = 5432, int sslmode=0, OID oid=0);
    ~pgConnBase();

    static void ExamineLibpqVersion();
    static double GetLibpqVersion() { return libpqVersion; }

    void Close();
    bool ExecuteVoid(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql);
    pgSetBase *ExecuteSet(const wxString& sql);
    wxString GetUser() const { return wxString(PQuser(conn), *conv); }
    wxString GetPassword() const { return wxString(PQpass(conn), *conv); }
    wxString GetHost() const { return dbHost; }
    wxString GetDbname() const { return dbname; }
	wxString GetName() const;
    bool GetNeedUtfConnectString() { return utfConnectString; }
    int GetPort() const { return atoi(PQport(conn)); };
    wxString GetTTY() const { return wxString(PQtty(conn), *conv); }
    wxString GetOptions() const { return wxString(PQoptions(conn), *conv); }
    int GetBackendPID() const { return PQbackendPID(conn); }
    int GetStatus() const;
    int GetLastResultStatus() const { return lastResultStatus; }
    bool IsAlive();
    wxString GetLastError() const;
    pgError GetLastResultError() const { return lastResultError; }
    wxString GetVersionString();
    OID GetLastSystemOID() const { return lastSystemOID; }
    OID GetDbOid() const { return dbOid; }
    void RegisterNoticeProcessor(PQnoticeProcessor proc, void *arg);
    wxMBConv *GetConv() { return conv; };

    void LogError();

    bool IsSSLconnected();
    PGconn *connection() { return conn; }
    void Notice(const char *msg);
    pgNotification *GetNotification();
    int GetTxStatus();

protected:
    PGconn *conn;
    int lastResultStatus;

    int connStatus;

    void SetLastResultError(PGresult *res);
    pgError lastResultError;

    wxMBConv *conv;
    bool needColQuoting, utfConnectString;
    wxString dbHost, dbname;
    OID lastSystemOID;
    OID dbOid;

    void *noticeArg;
    PQnoticeProcessor noticeProc;
    static double libpqVersion;

    friend class pgQueryThreadBase;
};

#endif

