//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgForeignTable.h PostgreSQL Foreign Table
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGFOREIGNTABLE_H
#define PGFOREIGNTABLE_H

#include "pgSchema.h"

class pgForeignTableFactory : public pgSchemaObjFactory
{
public:
	pgForeignTableFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgForeignTableFactory foreignTableFactory;

class pgForeignTable : public pgSchemaObject
{
public:
	pgForeignTable(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgForeignTable();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return !GetSystemObject() && pgSchemaObject::CanDrop();
	}

	wxString GetForeignServer() const
	{
		return foreignserver;
	}
	void iSetForeignServer(const wxString &s)
	{
		foreignserver = s;
	}
	const wxArrayString &GetOptionsArray()
	{
		return optionsArray;
	}
	wxString GetOptionsList() const
	{
		return options;
	}
	void iSetOptions(const wxString &s);
	const wxArrayString &GetTypesArray()
	{
		return typesArray;
	}
	wxString GetTypesList() const
	{
		return typesList;
	}
	wxString GetQuotedTypesList() const
	{
		return quotedTypesList;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	wxString GetSelectSql(ctlTree *browser);
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
	bool CanView()
	{
		return true;
	}

private:
	wxString foreignserver, options, typesList, quotedTypesList;
	wxArrayString typesArray;
	wxArrayString optionsArray;
};

class pgForeignTableCollection : public pgSchemaObjCollection
{
public:
	pgForeignTableCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
