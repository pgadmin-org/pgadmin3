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

class pgSchemaFactory : public pgDatabaseObjFactory
{
public:
    pgSchemaFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgSchemaFactory schemaFactory;


class pgSchema : public pgDatabaseObject
{
public:
    pgSchema(const wxString& newName = wxT(""));
    ~pgSchema();

    wxString GetPrefix() const { return database->GetSchemaPrefix(GetName()); }
    wxString GetQuotedPrefix() const { return database->GetQuotedSchemaPrefix(GetName()); }
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, ctlTree *browser);
    bool CanDropCascaded() { return true; }

    long GetSchemaTyp() const { return schemaTyp; }
    void iSetSchemaTyp(const long l) { schemaTyp=l; }
    bool GetCreatePrivilege() const { return createPrivilege; }
    void iSetCreatePrivilege(const bool b) { createPrivilege=b; }
    bool GetSystemObject() const { return schemaTyp <= SCHEMATYP_TEMP; }

    bool CanBackup() { return true; }
    bool RequireDropConfirm() { return true; }
    bool WantDummyChild() { return true; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxMenu *GetNewMenu();
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    long schemaTyp;
    bool createPrivilege;
};


/////////////////////////////////////////////////////

class pgSchemaObjCollection : public pgCollection
{
public:
    pgSchemaObjCollection(pgaFactory *factory, pgSchema *sch);
    bool CanCreate();
};

class pgSchemaObjFactory : public pgDatabaseObjFactory
{
public:
    pgSchemaObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img) : pgDatabaseObjFactory(tn, ns, nls, img) {}
    virtual pgCollection *CreateCollection(pgObject *obj);
};

#endif
