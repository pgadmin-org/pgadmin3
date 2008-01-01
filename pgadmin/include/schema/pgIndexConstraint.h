//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgIndexConstraint.h PostgreSQL Index Constraint: PK, Unique
//
//////////////////////////////////////////////////////////////////////////


#ifndef __PG_INDEXCONSTRAINT
#define __PG_INDEXCONSTRAINT

#include "pgIndex.h"

class pgIndexConstraint : public pgIndexBase
{
public:
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool DropObject(wxFrame *frame, ctlTree *browse, bool cascadedr);
    wxString GetDefinition();
    wxString GetCreate();
    wxString GetSql(ctlTree *browser);
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-altertable"); }
    OID GetConstraintOid() { return constraintOid; }
    void iSetConstraintOid(const OID o) { constraintOid = o; }

protected:
    pgIndexConstraint(pgTable *newTable, pgaFactory &factory, const wxString& newName)
        : pgIndexBase(newTable, factory, newName) {}

private:
    OID constraintOid;
};


class pgPrimaryKeyFactory : public pgIndexBaseFactory
{
public:
    pgPrimaryKeyFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgPrimaryKeyFactory primaryKeyFactory;

class pgPrimaryKey : public pgIndexConstraint
{
public:
    pgPrimaryKey(pgTable *newTable, const wxString& newName = wxT(""))
        : pgIndexConstraint(newTable, primaryKeyFactory, newName) {}

    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
    bool CanCreate() { return false; }
};


class pgUniqueFactory : public pgIndexBaseFactory
{
public:
    pgUniqueFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgUniqueFactory uniqueFactory;

class pgUnique : public pgIndexConstraint
{
public:
    pgUnique(pgTable *newTable, const wxString& newName = wxT(""))
        : pgIndexConstraint(newTable, uniqueFactory, newName) {}

    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
};


#endif
