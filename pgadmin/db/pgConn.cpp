//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgConn.cpp - PostgreSQL Connection class
//
/////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>
#include "utils/pgfeatures.h"

// Network  headers
#ifdef __WXMSW__
#include <winsock.h>
typedef u_long in_addr_t;

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif

#endif

// App headers
#include "utils/misc.h"
#include "utils/sysLogger.h"
#include "db/pgConn.h"
#include "utils/misc.h"
#include "utils/md5.h"
#include "db/pgSet.h"

double pgConn::libpqVersion = 8.0;

static void pgNoticeProcessor(void *arg, const char *message)
{
	((pgConn *)arg)->Notice(message);
}

pgConn::pgConn(const wxString &server, const wxString &database, const wxString &username, const wxString &password, int port, int sslmode, OID oid, const wxString &applicationname)
{
	wxString msg;

	save_server = server;
	save_database = database;
	save_username = username;
	save_password = password;
	save_port = port;
	save_sslmode = sslmode;
	save_oid = oid;
	save_applicationname = applicationname;

	memset(features, 0, sizeof(features));
	majorVersion = 0;

	conv = &wxConvLibc;
	needColQuoting = false;
	utfConnectString = false;

	// Check the hostname/ipaddress
	conn = 0;
	noticeArg = 0;
	connStatus = PGCONN_BAD;

	// Create the connection string
	if (!server.IsEmpty())
	{
		connstr.Append(wxT(" host="));
		connstr.Append(qtConnString(server));
	}
	if (!database.IsEmpty())
	{
		connstr.Append(wxT(" dbname="));
		connstr.Append(qtConnString(database));
	}
	if (!username.IsEmpty())
	{
		connstr.Append(wxT(" user="));
		connstr.Append(qtConnString(username));
	}
	if (!password.IsEmpty())
	{
		connstr.Append(wxT(" password="));
		connstr.Append(qtConnString(password));
	}

	if (port > 0)
	{
		connstr.Append(wxT(" port="));
		connstr.Append(NumToStr((long)port));
	}

	if (libpqVersion > 7.3)
	{
		switch (sslmode)
		{
			case 1:
				connstr.Append(wxT(" sslmode=require"));
				break;
			case 2:
				connstr.Append(wxT(" sslmode=prefer"));
				break;
			case 3:
				connstr.Append(wxT(" sslmode=allow"));
				break;
			case 4:
				connstr.Append(wxT(" sslmode=disable"));
				break;
			case 5:
				connstr.Append(wxT(" sslmode=verify-ca"));
				break;
			case 6:
				connstr.Append(wxT(" sslmode=verify-full"));
				break;
		}
	}
	else
	{
		switch (sslmode)
		{
			case 1:
				connstr.Append(wxT(" requiressl=1"));
				break;
			case 2:
				connstr.Append(wxT(" requiressl=0"));
				break;
		}
	}

	connstr.Trim(false);

	dbHost = server;
	dbHostName = server;

#ifdef HAVE_CONNINFO_PARSE
	if (!applicationname.IsEmpty())
	{
		// Check connection string with application_name
		char *errmsg;
		wxString connstr_with_applicationname = connstr + wxT(" application_name='") + applicationname + wxT("'");
		if (PQconninfoParse(connstr_with_applicationname.mb_str(wxConvUTF8), &errmsg))
		{
			connstr = connstr_with_applicationname;
		}
		else if (PQconninfoParse(connstr_with_applicationname.mb_str(wxConvLibc), &errmsg))
		{
			connstr = connstr_with_applicationname;
		}
	}
#endif

	// Open the connection
	wxString cleanConnStr = connstr;
	cleanConnStr.Replace(qtConnString(password), wxT("'XXXXXX'"));
	wxLogInfo(wxT("Opening connection with connection string: %s"), cleanConnStr.c_str());

	DoConnect();
}


pgConn::~pgConn()
{
	Close();
}


bool pgConn::DoConnect()
{
	wxCharBuffer cstrUTF = connstr.mb_str(wxConvUTF8);
	conn = PQconnectdb(cstrUTF);
	if (PQstatus(conn) == CONNECTION_OK)
		utfConnectString = true;
	else
	{
		wxCharBuffer cstrLibc = connstr.mb_str(wxConvLibc);
		if (strcmp(cstrUTF, cstrLibc))
		{
			PQfinish(conn);
			conn = PQconnectdb(cstrLibc);
		}
	}

	// Set client encoding to Unicode/Ascii
	if (PQstatus(conn) == CONNECTION_OK)
	{
		connStatus = PGCONN_OK;
		PQsetNoticeProcessor(conn, pgNoticeProcessor, this);

		wxString sql = wxT("SET DateStyle=ISO;\n");
		if (BackendMinimumVersion(9, 0))
			sql += wxT("SET bytea_output=escape;\n");

		sql += wxT("SELECT oid, pg_encoding_to_char(encoding) AS encoding, datlastsysoid\n")
		       wxT("  FROM pg_database WHERE ");

		if (save_oid)
			sql += wxT("oid = ") + NumToStr(save_oid);
		else
		{
			// Note, can't use qtDbString here as we don't know the server version yet.
			wxString db = save_database;
			db.Replace(wxT("\\"), wxT("\\\\"));
			db.Replace(wxT("'"), wxT("''"));
			sql += wxT("datname=") + qtString(db);
		}


		pgSet *set = ExecuteSet(sql);


		if (set)
		{
			if (set->ColNumber(wxT("\"datlastsysoid\"")) >= 0)
				needColQuoting = true;

			lastSystemOID = set->GetOid(wxT("datlastsysoid"));
			dbOid = set->GetOid(wxT("oid"));
			wxString encoding = set->GetVal(wxT("encoding"));

			if (encoding != wxT("SQL_ASCII") && encoding != wxT("MULE_INTERNAL"))
			{
				encoding = wxT("UNICODE");
				conv = &wxConvUTF8;
			}
			else
				conv = &wxConvLibc;

			wxLogInfo(wxT("Setting client_encoding to '%s'"), encoding.c_str());
			if (PQsetClientEncoding(conn, encoding.ToAscii()))
				wxLogError(wxT("%s"), GetLastError().c_str());

			delete set;
		}
	}
	else
		return false;

	return true;
}


void pgConn::Close()
{
	if (conn)
		PQfinish(conn);
	conn = 0;
	connStatus = PGCONN_BAD;
}


// Reconnect to the server
bool pgConn::Reconnect()
{
	// Close the existing (possibly broken) connection
	Close();

	// Reset any vars that need to be in a defined state before connecting
	needColQuoting = false;

	// Attempt the reconnect
	if (!DoConnect())
	{
		wxLogError(_("Failed to re-establish the connection to the server %s"), GetName().c_str());
		return false;
	}

	return true;
}


pgConn *pgConn::Duplicate()
{
	return new pgConn(wxString(save_server), wxString(save_database), wxString(save_username), wxString(save_password), save_port, save_sslmode, save_oid);
}


// Return the SSL mode name
wxString pgConn::GetSslModeName()
{
	switch (save_sslmode)
	{
		case 1:
			return wxT("require");
		case 2:
			return wxT("prefer");
		case 3:
			return wxT("allow");
		case 4:
			return wxT("disable");
		case 5:
			return wxT("verify-ca");
		case 6:
			return wxT("verify-full");
		default:
			return wxT("prefer");
	}
}

bool pgConn::GetIsEdb()
{
	// to retrieve edb flag
	BackendMinimumVersion(0, 0);
	return isEdb;
}

bool pgConn::GetIsGreenplum()
{
	// to retrieve Greenplum flag
	BackendMinimumVersion(0, 0);
	return isGreenplum;
}

wxString pgConn::SystemNamespaceRestriction(const wxString &nsp)
{
	if (reservedNamespaces.IsEmpty())
	{
		reservedNamespaces = wxT("'information_schema'");

		if (GetIsEdb())
			reservedNamespaces += wxT(", 'sys'");

		pgSet *set = ExecuteSet(
		                 wxT("SELECT nspname FROM pg_namespace nsp\n")
		                 wxT("  JOIN pg_proc pr ON pronamespace=nsp.oid\n")
		                 wxT(" WHERE proname IN ('slonyversion')"));
		if (set)
		{
			while (!set->Eof())
			{
				reservedNamespaces += wxT(", ") + qtDbString(set->GetVal(wxT("nspname")));
				set->MoveNext();
			}
			delete set;
		}
	}

	if (BackendMinimumVersion(8, 1))
		return wxT("(") + nsp + wxT(" NOT LIKE E'pg\\_%' AND ") + nsp + wxT(" NOT in (") + reservedNamespaces + wxT("))");
	else
		return wxT("(") + nsp + wxT(" NOT LIKE 'pg\\_%' AND ") + nsp + wxT(" NOT in (") + reservedNamespaces + wxT("))");
}

bool pgConn::HasPrivilege(const wxString &objTyp, const wxString &objName, const wxString &priv)
{
	wxString res = ExecuteScalar(
	                   wxT("SELECT has_") + objTyp.Lower()
	                   + wxT("_privilege(") + qtDbString(objName)
	                   + wxT(", ") + qtDbString(priv) + wxT(")"));

	return StrToBool(res);
}




bool pgConn::BackendMinimumVersion(int major, int minor)
{
	if (!majorVersion)
	{
		wxString version = GetVersionString();
		sscanf(version.ToAscii(), "%*s %d.%d.%d", &majorVersion, &minorVersion, &patchVersion);
		isEdb = version.Upper().Matches(wxT("ENTERPRISEDB*"));

		// EnterpriseDB 8.3 beta 1 & 2 and possibly later actually have PostgreSQL 8.2 style
		// catalogs. This is expected to change either before GA, but in the meantime we
		// need to check the catalogue version in more detail, and if we don't see what looks
		// like a 8.3 catalog, force the version number back to 8.2. Yuck.
		if (isEdb && majorVersion == 8 && minorVersion == 3)
		{
			if (ExecuteScalar(wxT("SELECT count(*) FROM pg_attribute WHERE attname = 'proconfig' AND attrelid = 'pg_proc'::regclass")) == wxT("0"))
				minorVersion = 2;
		}

		isGreenplum = version.Upper().Matches(wxT("*GREENPLUM DATABASE*"));
	}

	return majorVersion > major || (majorVersion == major && minorVersion >= minor);
}


// Greenplum sometimes adds features in patch releases, because Greenplum
// releases are not coordinated with PostgreSQL minor releases.
bool pgConn::BackendMinimumVersion(int major, int minor, int patch)
{
	if (!majorVersion)
		BackendMinimumVersion(0, 0);

	return majorVersion > major || (majorVersion == major && minorVersion > minor) || (majorVersion == major && minorVersion == minor && patchVersion >= patch);
}


bool pgConn::EdbMinimumVersion(int major, int minor)
{
	return BackendMinimumVersion(major, minor) && GetIsEdb();
}


bool pgConn::HasFeature(int featureNo)
{
	if (!features[FEATURE_INITIALIZED])
	{
		features[FEATURE_INITIALIZED] = true;

		wxString sql =
		    wxT("SELECT proname, pronargs, proargtypes[0] AS arg0, proargtypes[1] AS arg1, proargtypes[2] AS arg2\n")
		    wxT("  FROM pg_proc\n")
		    wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
		    wxT(" WHERE proname IN ('pg_tablespace_size', 'pg_file_read', 'pg_logfile_rotate',")
		    wxT(                  " 'pg_postmaster_starttime', 'pg_terminate_backend', 'pg_reload_conf',")
		    wxT(                  " 'pgstattuple', 'pgstatindex')\n")
		    wxT("   AND nspname IN ('pg_catalog', 'public')");

		pgSet *set = ExecuteSet(sql);

		if (set)
		{
			while (!set->Eof())
			{
				wxString proname = set->GetVal(wxT("proname"));
				long pronargs = set->GetLong(wxT("pronargs"));

				if (proname == wxT("pg_tablespace_size") && pronargs == 1 && set->GetLong(wxT("arg0")) == 26)
					features[FEATURE_SIZE] = true;
				else if (proname == wxT("pg_file_read") && pronargs == 3 && set->GetLong(wxT("arg0")) == 25
				         && set->GetLong(wxT("arg1")) == 20 && set->GetLong(wxT("arg2")) == 20)
					features[FEATURE_FILEREAD] = true;
				else if (proname == wxT("pg_logfile_rotate") && pronargs == 0)
					features[FEATURE_ROTATELOG] = true;
				else if (proname == wxT("pg_postmaster_starttime") && pronargs == 0)
					features[FEATURE_POSTMASTER_STARTTIME] = true;
				else if (proname == wxT("pg_terminate_backend") && pronargs == 1 && set->GetLong(wxT("arg0")) == 23)
					features[FEATURE_TERMINATE_BACKEND] = true;
				else if (proname == wxT("pg_reload_conf") && pronargs == 0)
					features[FEATURE_RELOAD_CONF] = true;
				else if (proname == wxT("pgstattuple") && pronargs == 1 && set->GetLong(wxT("arg0")) == 25)
					features[FEATURE_PGSTATTUPLE] = true;
				else if (proname == wxT("pgstatindex") && pronargs == 1 && set->GetLong(wxT("arg0")) == 25)
					features[FEATURE_PGSTATINDEX] = true;

				set->MoveNext();
			}
			delete set;
		}

		// Check for EDB function parameter default support
		wxString hasFuncDefs = ExecuteScalar(wxT("SELECT count(*) FROM pg_attribute WHERE attrelid = 'pg_catalog.pg_proc'::regclass AND attname = 'proargdefvals'"));
		if (hasFuncDefs == wxT("1"))
			features[FEATURE_FUNCTION_DEFAULTS] = true;
		else
			features[FEATURE_FUNCTION_DEFAULTS] = false;
	}

	if (featureNo <= FEATURE_INITIALIZED || featureNo >= FEATURE_LAST)
		return false;
	return features[featureNo];
}


// Encrypt a password using the appropriate encoding conversion
wxString pgConn::EncryptPassword(const wxString &user, const wxString &password)
{
	char hash[MD5_PASSWD_LEN + 1];

	pg_md5_encrypt(password.mb_str(*conv), user.mb_str(*conv), strlen(user.mb_str(*conv)), hash);

	return wxString::FromAscii(hash);
}

wxString pgConn::qtDbString(const wxString &value)
{
	wxString result = value;

	result.Replace(wxT("\\"), wxT("\\\\"));
	result.Replace(wxT("'"), wxT("''"));
	result.Append(wxT("'"));

	if (BackendMinimumVersion(8, 1))
	{
		if (result.Contains(wxT("\\")))
			result.Prepend(wxT("E'"));
		else
			result.Prepend(wxT("'"));
	}
	else
		result.Prepend(wxT("'"));

	return result;
}

void pgConn::ExamineLibpqVersion()
{
	libpqVersion = 7.3;
	PQconninfoOption *cio = PQconndefaults();

	if (cio)
	{
		PQconninfoOption *co = cio;
		while (co->keyword)
		{
			if (!strcmp(co->keyword, "sslmode"))
			{
				if (libpqVersion < 7.4)
					libpqVersion = 7.4;
			}
			if (!strcmp(co->keyword, "sslrootcert"))
			{
				if (libpqVersion < 8.4)
					libpqVersion = 8.4;
			}
			co++;
		}
		PQconninfoFree(cio);
	}
}

wxString pgConn::GetName() const
{
	wxString str;
	if (dbHost.IsEmpty())
		str.Printf(_("%s on local socket"), save_database.c_str());
	else
		str.Printf(_("%s on %s@%s:%d"), save_database.c_str(), GetUser().c_str(), dbHost.c_str(), GetPort());
	return str;
}

#ifdef SSL
// we don't define USE_SSL so we don't get ssl.h included
extern "C"
{
	extern void *PQgetssl(PGconn *conn);
}

bool pgConn::IsSSLconnected()
{
	return (conn && PQstatus(conn) == CONNECTION_OK && PQgetssl(conn) != NULL);
}
#else

bool pgConn::IsSSLconnected()
{
	return false ;
}
#endif


void pgConn::RegisterNoticeProcessor(PQnoticeProcessor proc, void *arg)
{
	noticeArg = arg;
	noticeProc = proc;
}




void pgConn::Notice(const char *msg)
{
	if (noticeArg && noticeProc)
		(*noticeProc)(noticeArg, msg);
	else
	{
		wxString str(msg, *conv);
		wxLogNotice(wxT("%s"), str.Trim().c_str());
	}
}


pgNotification *pgConn::GetNotification()
{
	pgNotify *notify;

	notify = PQnotifies(conn);
	if (!notify)
		return NULL;

	pgNotification *ret = new pgNotification;
	ret->name = wxString(notify->relname, *conv);
	ret->pid = notify->be_pid;
	ret->data = wxString(notify->extra, *conv);

	return ret;
}

int pgConn::GetTxStatus()
{
	return PQtransactionStatus(conn);
}

//////////////////////////////////////////////////////////////////////////
// Execute SQL
//////////////////////////////////////////////////////////////////////////

bool pgConn::ExecuteVoid(const wxString &sql, bool reportError)
{
	if (GetStatus() != PGCONN_OK)
		return false;

	// Execute the query and get the status.
	PGresult *qryRes;

	wxLogSql(wxT("Void query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
	qryRes = PQexec(conn, sql.mb_str(*conv));
	lastResultStatus = PQresultStatus(qryRes);
	SetLastResultError(qryRes);

	// Check for errors
	if (lastResultStatus != PGRES_TUPLES_OK &&
	        lastResultStatus != PGRES_COMMAND_OK)
	{
		LogError(!reportError);
		PQclear(qryRes);
		return false;
	}

	// Cleanup & exit
	PQclear(qryRes);
	return  true;
}



wxString pgConn::ExecuteScalar(const wxString &sql)
{
	wxString result;

	if (GetStatus() == PGCONN_OK)
	{
		// Execute the query and get the status.
		PGresult *qryRes;
		wxLogSql(wxT("Scalar query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
		qryRes = PQexec(conn, sql.mb_str(*conv));
		lastResultStatus = PQresultStatus(qryRes);
		SetLastResultError(qryRes);

		// Check for errors
		if (lastResultStatus != PGRES_TUPLES_OK && lastResultStatus != PGRES_COMMAND_OK)
		{
			LogError();
			PQclear(qryRes);
			return wxEmptyString;
		}

		// Check for a returned row
		if (PQntuples(qryRes) < 1)
		{
			wxLogInfo(wxT("Query returned no tuples"));
			PQclear(qryRes);
			return wxEmptyString;
		}

		// Retrieve the query result and return it.
		result = wxString(PQgetvalue(qryRes, 0, 0), *conv);

		wxLogSql(wxT("Query result: %s"), result.c_str());

		// Cleanup & exit
		PQclear(qryRes);
	}

	return result;
}

pgSet *pgConn::ExecuteSet(const wxString &sql)
{
	// Execute the query and get the status.
	if (GetStatus() == PGCONN_OK)
	{
		PGresult *qryRes;
		wxLogSql(wxT("Set query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
		qryRes = PQexec(conn, sql.mb_str(*conv));

		lastResultStatus = PQresultStatus(qryRes);
		SetLastResultError(qryRes);

		if (lastResultStatus == PGRES_TUPLES_OK || lastResultStatus == PGRES_COMMAND_OK)
		{
			pgSet *set = new pgSet(qryRes, this, *conv, needColQuoting);
			if (!set)
			{
				wxLogError(__("Couldn't create a pgSet object!"));
				PQclear(qryRes);
			}
			return set;
		}
		else
		{
			LogError();
			PQclear(qryRes);
		}
	}
	return new pgSet();
}

//////////////////////////////////////////////////////////////////////////
// Info
//////////////////////////////////////////////////////////////////////////

wxString pgConn::GetLastError() const
{
	wxString errmsg;
	char *pqErr;
	if (conn && (pqErr = PQerrorMessage(conn)) != 0)
	{
		errmsg = wxString(pqErr, wxConvUTF8);
		if (errmsg.IsNull())
			errmsg = wxString(pqErr, wxConvLibc);
	}
	else
	{
		if (connStatus == PGCONN_BROKEN)
			errmsg = _("Connection to database broken.");
		else
			errmsg = _("No connection to database.");
	}
	return errmsg;
}



void pgConn::LogError(const bool quiet)
{
	if (conn)
	{
		if (quiet)
		{
			wxLogQuietError(wxT("%s"), GetLastError().Trim().c_str());
		}
		else
		{
			wxLogError(wxT("%s"), GetLastError().Trim().c_str());
			IsAlive();
		}
	}
}



bool pgConn::IsAlive()
{
	if (GetStatus() != PGCONN_OK)
		return false;

	PGresult *qryRes = PQexec(conn, "SELECT 1;");
	lastResultStatus = PQresultStatus(qryRes);
	if (lastResultStatus != PGRES_TUPLES_OK)
	{
		PQclear(qryRes);
		qryRes = PQexec(conn, "ROLLBACK TRANSACTION; SELECT 1;");
		lastResultStatus = PQresultStatus(qryRes);
		SetLastResultError(qryRes);
	}
	PQclear(qryRes);

	// Check for errors
	if (lastResultStatus != PGRES_TUPLES_OK)
	{
		PQfinish(conn);
		conn = 0;
		connStatus = PGCONN_BROKEN;
		return false;
	}

	return true;
}


int pgConn::GetStatus() const
{
	if (!this)
		return PGCONN_BAD;

	if (conn)
		((pgConn *)this)->connStatus = PQstatus(conn);

	return connStatus;
}


wxString pgConn::GetVersionString()
{
	return ExecuteScalar(wxT("SELECT version();"));
}

void pgConn::SetLastResultError(PGresult *res, const wxString &msg)
{
	if (res)
	{
		lastResultError.severity = wxString(PQresultErrorField(res, PG_DIAG_SEVERITY), *conv);
		lastResultError.sql_state = wxString(PQresultErrorField(res, PG_DIAG_SQLSTATE), *conv);
		lastResultError.msg_primary = wxString(PQresultErrorField(res, PG_DIAG_MESSAGE_PRIMARY), *conv);
		lastResultError.msg_detail = wxString(PQresultErrorField(res, PG_DIAG_MESSAGE_DETAIL), *conv);
		lastResultError.msg_hint = wxString(PQresultErrorField(res, PG_DIAG_MESSAGE_HINT), *conv);
		lastResultError.statement_pos = wxString(PQresultErrorField(res, PG_DIAG_STATEMENT_POSITION), *conv);
		lastResultError.internal_pos = wxString(PQresultErrorField(res, PG_DIAG_INTERNAL_POSITION), *conv);
		lastResultError.internal_query = wxString(PQresultErrorField(res, PG_DIAG_INTERNAL_QUERY), *conv);
		lastResultError.context = wxString(PQresultErrorField(res, PG_DIAG_CONTEXT), *conv);
		lastResultError.source_file = wxString(PQresultErrorField(res, PG_DIAG_SOURCE_FILE), *conv);
		lastResultError.source_line = wxString(PQresultErrorField(res, PG_DIAG_SOURCE_LINE), *conv);
		lastResultError.source_function = wxString(PQresultErrorField(res, PG_DIAG_SOURCE_FUNCTION), *conv);
	}
	else
	{
		lastResultError.severity = wxEmptyString;
		lastResultError.sql_state = wxEmptyString;
		if (msg.IsEmpty())
			lastResultError.msg_primary = GetLastError();
		else
			lastResultError.msg_primary = msg;
		lastResultError.msg_detail = wxEmptyString;
		lastResultError.msg_hint = wxEmptyString;
		lastResultError.statement_pos = wxEmptyString;
		lastResultError.internal_pos = wxEmptyString;
		lastResultError.internal_query = wxEmptyString;
		lastResultError.context = wxEmptyString;
		lastResultError.source_file = wxEmptyString;
		lastResultError.source_line = wxEmptyString;
		lastResultError.source_function = wxEmptyString;
	}

	wxString errMsg;

	if (lastResultError.severity != wxEmptyString && lastResultError.msg_primary != wxEmptyString)
		errMsg = lastResultError.severity + wxT(": ") + lastResultError.msg_primary;
	else if (lastResultError.msg_primary != wxEmptyString)
		errMsg = lastResultError.msg_primary;

	if (!lastResultError.sql_state.IsEmpty())
	{
		if (!errMsg.EndsWith(wxT("\n")))
			errMsg += wxT("\n");
		errMsg += _("SQL state: ");
		errMsg += lastResultError.sql_state;
	}

	if (!lastResultError.msg_detail.IsEmpty())
	{
		if (!errMsg.EndsWith(wxT("\n")))
			errMsg += wxT("\n");
		errMsg += _("Detail: ");
		errMsg += lastResultError.msg_detail;
	}

	if (!lastResultError.msg_hint.IsEmpty())
	{
		if (!errMsg.EndsWith(wxT("\n")))
			errMsg += wxT("\n");
		errMsg += _("Hint: ");
		errMsg += lastResultError.msg_hint;
	}

	if (!lastResultError.statement_pos.IsEmpty())
	{
		if (!errMsg.EndsWith(wxT("\n")))
			errMsg += wxT("\n");
		errMsg += _("Character: ");
		errMsg += lastResultError.statement_pos;
	}

	if (!lastResultError.context.IsEmpty())
	{
		if (!errMsg.EndsWith(wxT("\n")))
			errMsg += wxT("\n");
		errMsg += _("Context: ");
		errMsg += lastResultError.context;
	}
	lastResultError.formatted_msg = errMsg;
}

// String quoting - only for use during the connection phase!!
wxString pgConn::qtString(const wxString &value)
{
	wxString result = value;

	result.Replace(wxT("\\"), wxT("\\\\"));
	result.Replace(wxT("'"), wxT("\\'"));
	result.Append(wxT("'"));
	result.Prepend(wxT("'"));

	return result;
}

// Check if, TABLE (tblname) has column with name colname
bool pgConn::TableHasColumn(wxString schemaname, wxString tblname, const wxString &colname)
{
	//
	// SELECT a.attname
	// FROM pg_catalog.pg_attribute a
	// WHERE a.attrelid = (SELECT c.oid
	//                     FROM pg_catalog.pg_class c
	//                          LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
	//                     WHERE c.relname ~ '^(TABLENAME)$' AND
	//                           pg_catalog.pg_table_is_visible(c.oid) AND
	//                           n.nspname ~ '^(SCHEMANAME)$') AND
	//       a.attnum > 0 AND NOT a.attisdropped
	// ORDER BY a.attnum
	//

	if (tblname.IsEmpty() || colname.IsEmpty())
		return false;

	if (schemaname.IsEmpty())
		schemaname = wxT("public");

	if (this && GetStatus() == PGCONN_OK)
	{
		tblname.Replace(wxT("\\"), wxT("\\\\"));
		tblname.Replace(wxT("'"), wxT("''"));
		schemaname.Replace(wxT("\\"), wxT("\\\\"));
		schemaname.Replace(wxT("'"), wxT("''"));

		wxString sql
		= wxT("SELECT a.attname AS colname FROM pg_catalog.pg_attribute a ") \
		  wxT("WHERE a.attrelid = (SELECT c.oid FROM pg_catalog.pg_class c ") \
		  wxT("                    LEFT JOIN pg_catalog.pg_namespace n ON ") \
		  wxT("                                    n.oid = c.relnamespace ") \
		  wxT("                    WHERE c.relname ~ '^(") + tblname + wxT(")$' AND ") \
		  wxT("                          n.nspname ~ '^(") + schemaname + wxT(")$') AND ") \
		  wxT("      a.attnum > 0 AND NOT a.attisdropped ") \
		  wxT("ORDER BY a.attnum");

		pgSet *set = ExecuteSet(sql);
		if (set)
		{
			while (!set->Eof())
			{
				if (set->GetVal(wxT("colname")) == colname)
				{
					delete set;
					return true;
				}
				set->MoveNext();
			}
		}
		delete set;
	}

	return false;
}

