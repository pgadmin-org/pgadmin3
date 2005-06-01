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
#include "base/pgConnBase.h"

// Class declarations
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
    bool GetIsEdb() const { return isEdb; }

private:
    bool features[32];
    int minorVersion, majorVersion;
    bool isEdb;

    wxString reservedNamespaces;
};

#endif

