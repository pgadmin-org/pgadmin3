//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgSchema.h PostgreSQL Schema
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSCHEMA_H
#define PGSCHEMA_H

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

class pgSchemaObjFactory : public pgDatabaseObjFactory
{
public:
    pgSchemaObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img, char **imgSm=0) 
        : pgDatabaseObjFactory(tn, ns, nls, img, imgSm) {}
    virtual pgCollection *CreateCollection(pgObject *obj);
};

// Object that lives in a schema
class pgSchemaObject : public pgDatabaseObject
{
public:
    pgSchemaObject(pgSchema *newSchema, pgaFactory &factory, const wxString& newName=wxEmptyString) : pgDatabaseObject(factory, newName) 
        { SetSchema(newSchema); wxLogInfo(wxT("Creating a pg") + GetTypeName() + wxT(" object")); }
    pgSchemaObject(pgSchema *newSchema, int newType, const wxString& newName = wxT("")) : pgDatabaseObject(newType, newName)
        { SetSchema(newSchema); wxLogInfo(wxT("Creating a pg") + GetTypeName() + wxT(" object")); }

    ~pgSchemaObject()
        { wxLogInfo(wxT("Destroying a pg") + GetTypeName() + wxT(" object")); }

    bool GetSystemObject() const;

    bool CanDrop();
    bool CanEdit() { return true; }
    bool CanCreate();

    void SetSchema(pgSchema *newSchema);
    void UpdateSchema(ctlTree *browser, OID schemaOid);
    pgSchema *GetSchema() const {return schema; }
    pgSet *ExecuteSet(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql);
    bool ExecuteVoid(const wxString& sql);
    virtual wxString GetFullIdentifier() const;
    virtual wxString GetQuotedFullIdentifier() const;


protected:
    virtual void SetContextInfo(frmMain *form);

    pgSchema *schema;
};


// collection of pgSchemaObject
class pgSchemaObjCollection : public pgCollection
{
public:
    pgSchemaObjCollection(pgaFactory *factory, pgSchema *sch);
    bool CanCreate();
};


#endif
