//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCollation.h PostgreSQL Collation
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCOLLATION_H
#define PGCOLLATION_H

#include "pgSchema.h"

class pgCollection;

class pgCollationFactory : public pgSchemaObjFactory
{
public:
	pgCollationFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgCollationFactory collationFactory;


class pgCollation : public pgSchemaObject
{
public:
	pgCollation(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgCollation();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetLcCollate() const
	{
		return lccollate;
	}
	void iSetLcCollate(const wxString &s)
	{
		lccollate = s;
	}
	wxString GetLcCtype() const
	{
		return lcctype;
	}
	void iSetLcCtype(const wxString &s)
	{
		lcctype = s;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
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

private:
	wxString lccollate, lcctype;
};

class pgCollationCollection : public pgSchemaObjCollection
{
public:
	pgCollationCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
