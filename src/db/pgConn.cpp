//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConn.cpp - PostgreSQL Connection class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// Network  headers
#ifdef __WXMSW__
    #include <winsock.h>
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
#endif

// App headers
#include "pgConn.h"
#include "misc.h"
#include "pgSet.h"
#include "sysLogger.h"


extern double libpqVersion;

static void pgNoticeProcessor(void *arg, const char *message)
{
    ((pgConn*)arg)->Notice(message);
}


pgConn::pgConn(const wxString& server, const wxString& database, const wxString& username, const wxString& password, int port, int sslmode)
{
    wxLogInfo(wxT("Creating pgConn object"));
    wxString msg, hostip;

    conv = &wxConvLibc;
    needColQuoting = false;

    // Check the hostname/ipaddress
    struct hostent *host;
    unsigned long addr;
    conn=0;
    majorVersion=0;
    noticeArg=0;
    
#ifdef __WXMSW__
    struct in_addr ipaddr;
#else
    unsigned long ipaddr;
#endif
    
    
    addr = inet_addr(server.ToAscii());
	if (addr == INADDR_NONE) // szServer is not an IP address
	{
		host = gethostbyname(server.ToAscii());
		if (host == NULL)
		{
            resolvedIP = FALSE;
            wxLogError(__("Could not resolve hostname %s"), server.c_str());
			return;
		}

        memcpy(&(ipaddr),host->h_addr,host->h_length); 
	    hostip = wxString::FromAscii(inet_ntoa(*((struct in_addr*) host->h_addr_list[0])));

    }
    else
        hostip = server;

    resolvedIP = TRUE;
    wxLogInfo(wxT("Server name: %s (resolved to: %s)"), server.c_str(), hostip.c_str());

    // Create the connection string
    wxString connstr;
    if (!server.IsEmpty()) {
      connstr.Append(wxT(" hostaddr="));
      connstr.Append(qtString(hostip));
    }
    if (!database.IsEmpty()) {
      connstr.Append(wxT(" dbname="));
      connstr.Append(qtString(database));
    }
    if (!username.IsEmpty()) {
      connstr.Append(wxT(" user="));
      connstr.Append(qtString(username));
    }
    if (!password.IsEmpty()) {
      connstr.Append(wxT(" password="));
      connstr.Append(qtString(password));
    }
    if (port > 0) {
      connstr.Append(wxT(" port="));
      connstr.Append(NumToStr((long)port));
    }

    if (libpqVersion > 7.3)
    {
        switch (sslmode)
        {
            case 1: connstr.Append(wxT(" sslmode=require"));   break;
            case 2: connstr.Append(wxT(" sslmode=prefer"));    break;
            case 3: connstr.Append(wxT(" sslmode=allow"));     break;
            case 4: connstr.Append(wxT(" sslmode=disable"));   break;
        }
    }
    else
    {
        switch (sslmode)
        {
            case 1: connstr.Append(wxT(" requiressl=1"));   break;
            case 2: connstr.Append(wxT(" requiressl=0"));   break;
        }
    }
    connstr.Trim(FALSE);

    // Open the connection
    wxLogInfo(wxT("Opening connection with connection string: %s"), connstr.c_str());

    conn = PQconnectdb(connstr.ToAscii());
    dbHost = server;

    // Set client encoding to Unicode/Ascii
    if (PQstatus(conn) == CONNECTION_OK)
    {

        PQsetNoticeProcessor(conn, pgNoticeProcessor, this);

        pgSet *set=ExecuteSet(
            wxT("SELECT pg_encoding_to_char(encoding) AS encoding, datlastsysoid\n")
            wxT("  FROM pg_database WHERE datname=") + qtString(database));
        if (set)
        {
            if (set->ColNumber(wxT("\"datlastsysoid\"")) >= 0)
                needColQuoting = true;

            lastSystemOID = set->GetLong(wxT("datlastsysoid"));
            wxString encoding = set->GetVal(wxT("encoding"));

#if wxUSE_UNICODE
            if (encoding != wxT("SQL_ASCII") && encoding != wxT("MULE_INTERNAL"))
            {
                encoding = wxT("UNICODE");
                conv = &wxConvUTF8;
            }
#endif

            wxLogInfo(wxT("Setting client_encoding to '%s'"), encoding.c_str());
            if (PQsetClientEncoding(conn, encoding.ToAscii()))
				wxLogError(wxT("%s"), wxString(PQerrorMessage(conn), *conv).c_str());

            delete set;
        }
    }
}


pgConn::~pgConn()
{
    wxLogInfo(wxT("Destroying pgConn object"));
    if (conn)
        PQfinish(conn);
}


#ifdef SSL
// we don't define USE_SSL so we don't get ssl.h included
extern "C"
{
extern void *PQgetssl(PGconn *conn);
}

bool pgConn::IsSSLconnected()
{
    return (PQstatus(conn) == CONNECTION_OK && PQgetssl(conn) != NULL);
}
#endif


void pgConn::RegisterNoticeProcessor(PQnoticeProcessor proc, void *arg)
{
    noticeArg=arg;
    noticeProc=proc;
}


void pgConn::Notice(const char *msg)
{
    wxString str(msg, *conv);
    wxLogNotice(wxT("%s"), str.c_str());

    if (noticeArg && noticeProc)
        (*noticeProc)(noticeArg, msg);
}


//////////////////////////////////////////////////////////////////////////
// Execute SQL
//////////////////////////////////////////////////////////////////////////

bool pgConn::ExecuteVoid(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;

    wxLogSql(wxT("Void query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    qryRes = PQexec(conn, sql.mb_str(*conv));
    int res = PQresultStatus(qryRes);

    // Check for errors
    if (res != PGRES_TUPLES_OK &&
        res != PGRES_COMMAND_OK)
    {
        wxLogError(wxT("%s"), wxString(PQerrorMessage(conn), *conv).c_str());
    }

    // Cleanup & exit
    PQclear(qryRes);
    return res == PGRES_TUPLES_OK || res == PGRES_COMMAND_OK;
}


bool pgConn::HasPrivilege(const wxString &objTyp, const wxString &objName, const wxString &priv)
{
    wxString res=ExecuteScalar(
        wxT("SELECT has_") + objTyp.Lower() 
        + wxT("_privilege(") + qtString(objName)
        + wxT(", ") + qtString(priv) + wxT(")"));

    return StrToBool(res);
}


wxString pgConn::ExecuteScalar(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxLogSql(wxT("Scalar query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    qryRes = PQexec(conn, sql.mb_str(*conv));
        
    // Check for errors
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK)
    {
        wxLogError(wxT("%s"), wxString(PQerrorMessage(conn), *conv).c_str());
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
    wxString result;
    result=wxString(PQgetvalue(qryRes, 0, 0), *conv);

    wxLogSql(wxT("Query result: %s"), result.c_str());

    // Cleanup & exit
    PQclear(qryRes);
    return result;
}

pgSet *pgConn::ExecuteSet(const wxString& sql)
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxLogSql(wxT("Set query (%s:%d): %s"), this->GetHost().c_str(), this->GetPort(), sql.c_str());
    qryRes = PQexec(conn, sql.mb_str(*conv));

    int status= PQresultStatus(qryRes);

    if (status == PGRES_TUPLES_OK || status == PGRES_COMMAND_OK)
    {
        pgSet *set = new pgSet(qryRes, conn, *conv, needColQuoting);
        if (!set)
        {
            wxLogError(__("Couldn't create a pgSet object!"));
            PQclear(qryRes);
        }
    	return set;
    }
    else
    {
        wxLogError(wxT("%s"), wxString(PQerrorMessage(conn), *conv).c_str());
        PQclear(qryRes);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
// Info
//////////////////////////////////////////////////////////////////////////

int pgConn::GetStatus() const
{
    if(resolvedIP) {
	    return PQstatus(conn);
    } else {
        return PGCONN_DNSERR;
    }
}


wxString pgConn::GetVersionString()
{
	return ExecuteScalar(wxT("SELECT version();"));
}


bool pgConn::BackendMinimumVersion(int major, int minor)
{
    if (!majorVersion)
    {
	    sscanf(GetVersionString().ToAscii(), "%*s %d.%d", &majorVersion, &minorVersion);
    }
	return majorVersion > major || (majorVersion == major && minorVersion >= minor);
}
