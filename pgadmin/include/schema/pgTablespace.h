//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTablespace.h - PostgreSQL Tablespace
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTABLESPACE_H
#define PGTABLESPACE_H


#include "pgServer.h"

class pgTablespaceFactory : public pgServerObjFactory
{
public:
	pgTablespaceFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgTablespaceFactory tablespaceFactory;


class pgTablespace : public pgServerObject
{
public:
	pgTablespace(const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
	void ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where = wxEmptyString);

	wxString GetLocation() const
	{
		return location;
	};
	void iSetLocation(const wxString &newVal)
	{
		location = newVal;
	}
	wxArrayString &GetVariables()
	{
		return variables;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	bool RequireDropConfirm()
	{
		return true;
	}
	pgConn *connection();

	wxString GetSql(ctlTree *browser);
	void MoveTablespace(frmMain *form);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	bool HasStats()
	{
		return true;
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
	wxString location;
	wxArrayString variables;
};


class pgTablespaceCollection : public pgServerObjCollection
{
public:
	pgTablespaceCollection(pgaFactory *factory, pgServer *sv);
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowStatistics(frmMain *form, ctlListView *statistics);
};

class moveTablespaceFactory : public contextActionFactory
{
public:
	moveTablespaceFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


#endif
