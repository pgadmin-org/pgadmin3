//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgConstraints.h - Constraint collection
//
//////////////////////////////////////////////////////////////////////////


#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

#include "pgTable.h"
#include "pgDomain.h"


class pgConstraintCollection : public pgSchemaObjCollection
{
public:
	pgConstraintCollection(pgaFactory *factory, pgSchema *schema);

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-altertable");
	}
	bool CanCreate()
	{
		return true;
	}
	wxMenu *GetNewMenu();

	void ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane);
	wxString GetTranslatedMessage(int kindOfMessage) const;

	pgTable *table;
	pgDomain *domain;
};


class pgConstraintFactory : public pgSchemaObjFactory
{
public:
	pgConstraintFactory();
	virtual dlgProperty *CreateDialog(class frmMain *, class pgObject *, class pgObject *)
	{
		return 0;
	}
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgConstraintFactory constraintFactory;
extern pgaCollectionFactory constraintCollectionFactory;

#endif
