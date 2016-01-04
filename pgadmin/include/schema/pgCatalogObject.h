//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCatalogObject.h - EnterpriseDB catalog class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCATALOGOBJECT_H
#define PGCATALOGOBJECT_H

#include "pgSchema.h"

class pgCatalogObjectFactory : public pgSchemaObjFactory
{
public:
	pgCatalogObjectFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
	{
		return NULL;
	};
	virtual pgObject *CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgCatalogObjectFactory catalogObjectFactory;


// Class declarations
class pgCatalogObject : public pgSchemaObject
{
public:
	pgCatalogObject(pgSchema *newSchema, const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}
	bool CanCreate()
	{
		return false;
	}
	bool CanEdit()
	{
		return false;
	}

private:

};

class pgCatalogObjectCollection : public pgSchemaObjCollection
{
public:
	pgCatalogObjectCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
