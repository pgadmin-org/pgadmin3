//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool DropObject(wxFrame *frame, ctlTree *browse, bool cascadedr);
	wxString GetDefinition();
	wxString GetCreate();
	wxString GetSql(ctlTree *browser);
	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-altertable");
	}
	OID GetConstraintOid()
	{
		return constraintOid;
	}
	void iSetConstraintOid(const OID o)
	{
		constraintOid = o;
	}

protected:
	pgIndexConstraint(pgSchema *newSchema, pgaFactory &factory, const wxString &newName)
		: pgIndexBase(newSchema, factory, newName) {}

private:
	OID constraintOid;
};


class pgPrimaryKeyFactory : public pgIndexBaseFactory
{
public:
	pgPrimaryKeyFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgPrimaryKeyFactory primaryKeyFactory;

class pgPrimaryKey : public pgIndexConstraint
{
public:
	pgPrimaryKey(pgSchema *newSchema, const wxString &newName = wxT(""))
		: pgIndexConstraint(newSchema, primaryKeyFactory, newName) {}

	wxString GetTranslatedMessage(int kindOfMessage) const;
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	bool CanCreate()
	{
		return false;
	}
};


class pgUniqueFactory : public pgIndexBaseFactory
{
public:
	pgUniqueFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgUniqueFactory uniqueFactory;

class pgUnique : public pgIndexConstraint
{
public:
	pgUnique(pgSchema *newSchema, const wxString &newName = wxT(""))
		: pgIndexConstraint(newSchema, uniqueFactory, newName) {}

	wxString GetTranslatedMessage(int kindOfMessage) const;
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
};


class pgExcludeFactory : public pgIndexBaseFactory
{
public:
	pgExcludeFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgExcludeFactory excludeFactory;

class pgExclude : public pgIndexConstraint
{
public:
	pgExclude(pgSchema *newSchema, const wxString &newName = wxT(""))
		: pgIndexConstraint(newSchema, excludeFactory, newName) {}

	wxString GetTranslatedMessage(int kindOfMessage) const;
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
};


#endif
