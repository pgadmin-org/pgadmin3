//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#include "pgCollection.h"
#include "pgDatabase.h"


enum
{
    SCHEMATYP_SYSTEM=0,
    SCHEMATYP_TEMP,
    SCHEMATYP_USERSYS,
    SCHEMATYP_NORMAL
};


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
    bool CanDropCascaded() { return true; }

    long GetSchemaTyp() const { return schemaTyp; }
    void iSetSchemaTyp(const long l) { schemaTyp=l; }
    bool GetCreatePrivilege() const { return createPrivilege; }
    void iSetCreatePrivilege(const bool b) { createPrivilege=b; }
    bool GetSystemObject() const { return schemaTyp <= SCHEMATYP_TEMP; }

    bool CanBackup() { return true; }
    bool RequireDropConfirm() { return true; }
    bool WantDummyChild() { return true; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    wxMenu *GetNewMenu();
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    long schemaTyp;
    bool createPrivilege;
};


/////////////////////////////////////////////////////

class pgSchemaCollection : public pgCollection
{
public:
    pgSchemaCollection(pgaFactory &factory, pgSchema *sch);
    bool CanCreate();
};


#endif
