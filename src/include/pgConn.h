//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConn.h - PostgreSQL Connection class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCONN_H
#define PGCONN_H

#include "base/pgConnBase.h"

// Class declarations
class pgSet;

class pgConn : public pgConnBase
{
public:
    pgConn(const wxString& server = wxT(""), const wxString& database = wxT(""), const wxString& username = wxT(""), const wxString& password = wxT(""), int port = 5432, int sslmode=0, OID oid=0);

    pgSet *ExecuteSet(const wxString& sql);

    bool HasPrivilege(const wxString &objTyp, const wxString &objName, const wxString &priv);
    bool HasFeature(int feature=0);
    bool BackendMinimumVersion(int major, int minor);
    bool EdbMinimumVersion(int major, int minor);
    wxString SystemNamespaceRestriction(const wxString &nsp);
    int GetMajorVersion() const { return majorVersion; }
    int GetMinorVersion() const { return minorVersion; }
    bool GetIsEdb();
    wxString EncryptPassword(const wxString &user, const wxString &password);
	wxString qtDbString(const wxString& value);
	pgConn *Duplicate();

private:
    bool features[32];
    int minorVersion, majorVersion;
    bool isEdb;

    wxString reservedNamespaces;

	wxString save_server, save_database, save_username, save_password;
	int save_port, save_sslmode;
	OID save_oid;
};

#endif

