//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgOperatorFamily.h PostgreSQL OperatorFamily
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGOPERATORFAMILY_H
#define PGOPERATORFAMILY_H

#include "pgSchema.h"

class pgCollection;
class pgOperatorFamilyFactory : public pgSchemaObjFactory
{
public:
	pgOperatorFamilyFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgOperatorFamilyFactory operatorFamilyFactory;


class pgOperatorFamily : public pgSchemaObject
{
public:
	pgOperatorFamily(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgOperatorFamily();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetFullName()
	{
		return GetName() + wxT("(") + GetAccessMethod() + wxT(")");
	}
	wxString GetAccessMethod() const
	{
		return accessMethod;
	}
	void iSetAccessMethod(const wxString &s)
	{
		accessMethod = s;
	}
	wxString GetSql(ctlTree *browser);

	bool CanCreate()
	{
		return false;
	}
	bool CanEdit()
	{
		return false;
	}
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createopfamily");
	}
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
	wxString accessMethod;
};

class pgOperatorFamilyCollection : public pgSchemaObjCollection
{
public:
	pgOperatorFamilyCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
