//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgSchema.h PostgreSQL Schema
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSCHEMA_H
#define PGSCHEMA_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgSchema : public pgDatabaseObject
{
public:
    pgSchema(const wxString& newName = wxT(""));
    ~pgSchema();

    int GetIcon() { return PGICON_SCHEMA; }
    wxString GetPrefix() const { return database->GetSchemaPrefix(GetName()); }
    wxString GetQuotedPrefix() const { return database->GetQuotedSchemaPrefix(GetName()); }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser);

    long GetSchemaTyp() const { return schemaTyp; }
    void iSetSchemaTyp(const long l) { schemaTyp=l; }
    bool GetCreatePrivilege() const { return createPrivilege; }
    void iSetCreatePrivilege(const bool b) { createPrivilege=b; }
    wxString GetTablespace() const { return tablespace; };
    void iSetTablespace(const wxString& newVal) { tablespace = newVal; }
    bool GetSystemObject() const { return schemaTyp < 1; }

    bool CanBackup() { return true; }
    bool RequireDropConfirm() { return true; }
    bool WantDummyChild() { return true; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxMenu *GetNewMenu();
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    long schemaTyp; // 0: System 1: temporär 2: normal
    bool createPrivilege;
    wxString tablespace;
};

#endif
