//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCheck.h PostgreSQL Check
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCHECK_H
#define PGCHECK_H

// App headers
#include "pgTable.h"
#include "pgConstraints.h"



class pgCheckFactory : public pgSchemaObjFactory
{
public:
	pgCheckFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);

	int GetClosedIconId()
	{
		return closedId;
	}

protected:
	int closedId;
};
extern pgCheckFactory checkFactory;


class pgCheck : public pgSchemaObject
{
public:
	pgCheck(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgCheck();

	int GetIconId();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetObjectName() const
	{
		return objectName;
	}
	void iSetObjectName(const wxString &s)
	{
		objectName = s;
	}
	wxString GetObjectSchema() const
	{
		return objectSchema;
	}
	void iSetObjectSchema(const wxString &s)
	{
		objectSchema = s;
	}
	wxString GetObjectKind() const
	{
		return objectKind;
	}
	void iSetObjectKind(const wxString &s)
	{
		objectKind = s;
	}
	wxString GetDefinition() const
	{
		return definition;
	}
	void iSetDefinition(const wxString &s)
	{
		definition = s;
	}
	bool GetNoInherit() const
	{
		return noinherit;
	}
	void iSetNoInherit(const bool b)
	{
		noinherit = b;
	}
	bool GetValid() const
	{
		return valid;
	}
	void iSetValid(const bool b)
	{
		valid = b;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetConstraint();
	wxString GetSql(ctlTree *browser);
	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-altertable");
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
	void Validate(frmMain *form);

private:
	wxString definition, objectKind, objectName, objectSchema;
	bool noinherit, valid;
};

class pgCheckCollection : public pgSchemaObjCollection
{
public:
	pgCheckCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

class validateCheckFactory : public contextActionFactory
{
public:
	validateCheckFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
