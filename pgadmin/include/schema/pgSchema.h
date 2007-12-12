//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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

class pgSchemaBaseFactory : public pgDatabaseObjFactory
{
public:
	pgSchemaBaseFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img, char **imgSm=0);
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};

class pgSchemaFactory : public pgSchemaBaseFactory
{
public:
    pgSchemaFactory();
};

class pgCatalogFactory : public pgSchemaBaseFactory
{
public:
    pgCatalogFactory();
    bool CanCreate() { return false; }
    bool CanEdit() { return true; }
};

extern pgSchemaFactory schemaFactory;
extern pgCatalogFactory catalogFactory;


class pgSchemaBase : public pgDatabaseObject
{
public:
    pgSchemaBase(pgaFactory &factory, const wxString& newName = wxT(""));

    wxString GetPrefix() const { return database->GetSchemaPrefix(GetName()); }
    wxString GetQuotedPrefix() const { return database->GetQuotedSchemaPrefix(GetName()); }
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, ctlTree *browser);
    bool CanDropCascaded() { return GetMetaType() != PGM_CATALOG; }

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

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }
private:
    long schemaTyp;
    bool createPrivilege;
};

class pgSchema : public pgSchemaBase
{
public:
	pgSchema(const wxString& newName = wxT(""));
};

class pgCatalog : public pgSchemaBase
{
public:
	pgCatalog(const wxString& newName = wxT(""));
	virtual wxString GetDisplayName();
    bool CanCreate() { return false; }
    bool CanEdit() { return true; }
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
        { SetSchema(newSchema); }
    pgSchemaObject(pgSchema *newSchema, int newType, const wxString& newName = wxT("")) : pgDatabaseObject(newType, newName)
        { SetSchema(newSchema); }

    bool GetSystemObject() const;

    bool CanDrop(); 
    bool CanEdit() { return schema->GetMetaType() != PGM_CATALOG; }
    bool CanCreate();

    void SetSchema(pgSchema *newSchema);
    void UpdateSchema(ctlTree *browser, OID schemaOid);
    virtual pgSchema *GetSchema() const {return schema; }
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
    virtual bool CanCreate();
};


#endif
