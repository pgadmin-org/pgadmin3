//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gpExtTable.h Greenplum External Table
//
//////////////////////////////////////////////////////////////////////////

#ifndef gpExtTable_H
#define gpExtTable_H

#include "pgSchema.h"

class pgCollection;

class gpExtTableFactory : public pgSchemaObjFactory
{
public:
	gpExtTableFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	pgCollection *CreateCollection(pgObject *obj);
};
extern gpExtTableFactory extTableFactory;


class gpExtTable : public pgSchemaObject
{
public:
	gpExtTable(pgSchema *newSchema, const wxString &newName = wxT(""));
	~gpExtTable();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return !GetSystemObject() && pgSchemaObject::CanDrop();
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	bool CanView()
	{
		return true;
	}
	bool WantDummyChild()
	{
		return false;
	}

	wxMenu *GetNewMenu();
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

	void ShowHint(frmMain *form, bool force);
	bool GetCanHint()
	{
		return true;
	};

	bool IsUpToDate();
	//wxString GetFormattedDefinition();

private:
	wxString GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM);
};

class gpExtTableCollection : public pgSchemaObjCollection
{
public:
	gpExtTableCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
