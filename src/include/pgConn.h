//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgConn.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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

private:
    bool features[32];
    int minorVersion, majorVersion;
    bool isEdb;

    wxString reservedNamespaces;
};

#endif

