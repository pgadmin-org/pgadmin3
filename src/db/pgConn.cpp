//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConn.cpp - PostgreSQL Connection class
//
/////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>
#include "pgfeatures.h"

// Network  headers
#ifdef __WXMSW__
#include <winsock.h>
#else

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif

#endif

// App headers
#include "pgConn.h"
#include "misc.h"
#include "md5.h"
#include "pgSet.h"
#include "sysLogger.h"



pgConn::pgConn(const wxString& server, const wxString& database, const wxString& username, const wxString& password, int port, int sslmode, OID oid)
: pgConnBase(server, database, username, password, port, sslmode, oid)
{
	save_server = server;
	save_database = database;
	save_username = username;
	save_password = password;
	save_port = port;
	save_sslmode = sslmode;
	save_oid = oid;

    memset(features, 0, sizeof(features));
    majorVersion=0;
}

pgConn *pgConn::Duplicate()
{
	return new pgConn(wxString(save_server), wxString(save_database), wxString(save_username), wxString(save_password), save_port, save_sslmode, save_oid);
}

bool pgConn::GetIsEdb()
{
    // to retrieve edb flag
    BackendMinimumVersion(0,0);
    return isEdb; 
}


wxString pgConn::SystemNamespaceRestriction(const wxString &nsp)
{
    if (reservedNamespaces.IsEmpty())
    {
        reservedNamespaces = wxT("'information_schema'");

        if (GetIsEdb())
            reservedNamespaces += wxT(", 'sys'");

        pgSet *set=ExecuteSet(
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



pgSet *pgConn::ExecuteSet(const wxString& sql)
{
    return (pgSet*)pgConnBase::ExecuteSet(sql);
}



bool pgConn::HasPrivilege(const wxString &objTyp, const wxString &objName, const wxString &priv)
{
    wxString res=ExecuteScalar(
        wxT("SELECT has_") + objTyp.Lower() 
        + wxT("_privilege(") + qtDbString(objName)
        + wxT(", ") + qtDbString(priv) + wxT(")"));

    return StrToBool(res);
}




bool pgConn::BackendMinimumVersion(int major, int minor)
{
    if (!majorVersion)
    {
        wxString version=GetVersionString();
	    sscanf(version.ToAscii(), "%*s %d.%d", &majorVersion, &minorVersion);
        isEdb = version.Upper().Matches(wxT("ENTERPRISEDB*"));
    }
	return majorVersion > major || (majorVersion == major && minorVersion >= minor);
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

        wxString sql=
            wxT("SELECT proname, pronargs, proargtypes[0] AS arg0, proargtypes[1] AS arg1, proargtypes[2] AS arg2\n")
            wxT("  FROM pg_proc\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE proname IN ('pg_tablespace_size', 'pg_file_read', 'pg_logfile_rotate',")
            wxT(                  " 'pg_postmaster_starttime', 'pg_terminate_backend', 'pg_reload_conf')\n");

        if (BackendMinimumVersion(8, 1))
            sql += wxT("   AND nspname = 'pg_catalog'");
        else
            sql += wxT("   AND nspname IN ('pg_catalog', 'public')");

        pgSet *set=ExecuteSet(sql);

        if (set)
        {
            while (!set->Eof())
            {
                wxString proname=set->GetVal(wxT("proname"));
                long pronargs = set->GetLong(wxT("pronargs"));

                if (proname == wxT("pg_tablespace_size") && pronargs == 1 && set->GetLong(wxT("arg0")) == 26)
                    features[FEATURE_SIZE]= true;
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

                set->MoveNext();
            }
            delete set;
        }
    }

    if (featureNo <= FEATURE_INITIALIZED || featureNo >= FEATURE_LAST)
        return false;
    return features[featureNo];
}


// Encrypt a password using the appropriate encoding conversion
wxString pgConn::EncryptPassword(const wxString &user, const wxString &password)
{
    char hash[MD5_PASSWD_LEN+1];

    pg_md5_encrypt(password.mb_str(*conv), user.mb_str(*conv), strlen(user.mb_str(*conv)), hash);

    return wxString::FromAscii(hash);
}

wxString pgConn::qtDbString(const wxString& value)
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
