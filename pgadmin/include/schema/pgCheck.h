//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
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



class pgCheckFactory : public pgTableObjFactory
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


class pgCheck : public pgTableObject
{
public:
	pgCheck(pgTable *newTable, const wxString &newName = wxT(""));
	~pgCheck();

	int GetIconId();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetFkTable() const
	{
		return fkTable;
	}
	void iSetFkTable(const wxString &s)
	{
		fkTable = s;
	}
	wxString GetFkSchema() const
	{
		return fkSchema;
	}
	void iSetFkSchema(const wxString &s)
	{
		fkSchema = s;
	}
	wxString GetDefinition() const
	{
		return definition;
	}
	void iSetDefinition(const wxString &s)
	{
		definition = s;
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
	wxString definition, fkTable, fkSchema;
	bool valid;
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
