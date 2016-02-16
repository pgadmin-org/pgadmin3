//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include "pgSet.h"

// status enums
enum
{
	PGCONN_OK = CONNECTION_OK,
	PGCONN_BAD = CONNECTION_BAD,
	PGCONN_REFUSED,
	PGCONN_DNSERR,
	PGCONN_ABORTED,     // connect user aborted
	PGCONN_BROKEN,       // tcp/pipe broken
	PGCONN_SSHTUNNEL_ERROR
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
typedef struct pgNotification
{
	wxString name;
	int pid;
	wxString data;
} pgNotification;

// An error record
typedef struct pgError
{
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

	void SetError(PGresult *_res = NULL, wxMBConv *_conv = NULL);
} pgError;

class pgConn
{
public:
	pgConn(const wxString &server = wxT(""), const wxString &service = wxT(""), const wxString &hostaddr = wxT(""),
	       const wxString &database = wxT(""), const wxString &username = wxT(""), const wxString &password = wxT(""),
	       int port = 5432, const wxString &rolename = wxT(""), int sslmode = 0, OID oid = 0,
	       const wxString &applicationname = wxT("pgAdmin"),
	       const wxString &sslcert = wxT(""), const wxString &sslkey = wxT(""), const wxString &sslrootcert = wxT(""), const wxString &sslcrl = wxT(""),
	       const bool sslcompression = true);
	~pgConn();

	bool IsSuperuser();
	bool HasPrivilege(const wxString &objTyp, const wxString &objName, const wxString &priv);
	bool HasFeature(int feature = 0, bool forceCheck = false);
	bool BackendMinimumVersion(int major, int minor);
	bool BackendMinimumVersion(int major, int minor, int patch);
	bool EdbMinimumVersion(int major, int minor);
	wxString SystemNamespaceRestriction(const wxString &nsp);
	int GetMajorVersion() const
	{
		return majorVersion;
	}
	int GetMinorVersion() const
	{
		return minorVersion;
	}
	bool GetIsEdb();
	bool GetIsGreenplum();
	bool GetIsHawq();
	wxString EncryptPassword(const wxString &user, const wxString &password);
	wxString qtDbString(const wxString &value);
	pgConn *Duplicate(const wxString &_appName = wxT(""));

	static void ExamineLibpqVersion();
	static double GetLibpqVersion()
	{
		return libpqVersion;
	}

	static bool IsValidServerEncoding(int encid)
	{
		return pg_valid_server_encoding_id(encid) == 0 ? false : true;
	}

	void Close();
	bool Reconnect();
	bool ExecuteVoid(const wxString &sql, bool reportError = true);
	wxString ExecuteScalar(const wxString &sql, bool reportError = true);
	pgSet *ExecuteSet(const wxString &sql, bool reportError = true);
	void CancelExecution(void);

	wxString GetHostAddr() const
	{
		return save_hostaddr;
	}
	wxString GetService() const
	{
		return save_service;
	}
	wxString GetUser() const
	{
		return conn ? wxString(PQuser(conn), *conv) : wxT("");
	}
	wxString GetPassword() const
	{
		return conn ? wxString(PQpass(conn), *conv) : wxT("");
	}
	wxString GetRole() const
	{
		return dbRole;
	}
	wxString GetHost() const
	{
		return dbHost;
	}
	wxString GetHostName() const
	{
		return dbHostName;
	}
	wxString GetDbname() const
	{
		return save_database;
	}
	wxString GetApplicationName() const
	{
		return save_applicationname;
	}
	wxString GetSSLCert() const
	{
		return save_sslcert;
	}
	wxString GetSSLKey() const
	{
		return save_sslkey;
	}
	wxString GetSSLRootCert() const
	{
		return save_sslrootcert;
	}
	wxString GetSSLCrl() const
	{
		return save_sslcrl;
	}
	bool GetSSLCompression() const
	{
		return save_sslcompression;
	}
	wxString GetName() const;
	bool GetNeedUtfConnectString()
	{
		return utfConnectString;
	}
	int GetPort() const
	{
		return conn ? atoi(PQport(conn)) : 0;
	};
	wxString GetTTY() const
	{
		return conn ? wxString(PQtty(conn), *conv) : wxT("");
	}
	wxString GetOptions() const
	{
		return conn ? wxString(PQoptions(conn), *conv) : wxT("");
	}
	int GetSslMode() const
	{
		return save_sslmode;
	}
	wxString GetSslModeName();
	int GetBackendPID() const
	{
		return conn ? PQbackendPID(conn) : 0;
	}
	int GetStatus() const;
	int GetLastResultStatus() const
	{
		return lastResultStatus;
	}
	bool IsAlive();
	wxString GetLastError() const;
	pgError GetLastResultError() const
	{
		return lastResultError;
	}
	wxString GetVersionString();
	OID GetLastSystemOID() const
	{
		return lastSystemOID;
	}
	OID GetDbOid() const
	{
		return dbOid;
	}
	void RegisterNoticeProcessor(PQnoticeProcessor proc, void *arg);
	wxMBConv *GetConv()
	{
		return conv;
	};

	void LogError(const bool quiet = false);

	bool IsSSLconnected();
	PGconn *connection()
	{
		return conn;
	}
	void Notice(const char *msg);
	pgNotification *GetNotification();
	int GetTxStatus();

	void Reset();

	bool StartCopy(const wxString query);
	bool PutCopyData(const char *data, long count);
	bool EndPutCopy(const wxString errormsg);
	bool GetCopyFinalStatus(void);

	bool TableHasColumn(wxString schemaname, wxString tblname, const wxString &colname);

protected:
	PGconn   *conn;
	PGcancel *m_cancelConn;
	wxMutex   m_cancelConnMutex;
	int lastResultStatus;

	int connStatus;

	void SetLastResultError(PGresult *res, const wxString &msg = wxEmptyString);
	void SetConnCancel(void);
	void ResetConnCancel(void);
	pgError lastResultError;

	wxMBConv *conv;
	bool needColQuoting, utfConnectString;
	wxString dbRole, dbHost, dbHostName;
	OID lastSystemOID;
	OID dbOid;

	void *noticeArg;
	PQnoticeProcessor noticeProc;
	static double libpqVersion;

	friend class pgQueryThread;

private:
	bool DoConnect();
	bool Initialize();

	wxString qtString(const wxString &value);

	bool features[32];
	int minorVersion, majorVersion, patchVersion;
	bool isEdb;
	bool isGreenplum;
	bool isHawq;

	wxString reservedNamespaces;
	wxString connstr;

	wxString save_server, save_service, save_hostaddr, save_database, save_username, save_password, save_rolename, save_applicationname;
	wxString save_sslcert, save_sslkey, save_sslrootcert, save_sslcrl;
	int save_port, save_sslmode;
	bool save_sslcompression;
	OID save_oid;
};

#endif


