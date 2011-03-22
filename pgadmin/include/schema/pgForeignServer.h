//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgForeignServer.h PostgreSQL Foreign Server
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGFOREIGNSERVER_H
#define PGFOREIGNSERVER_H

#include "pgForeignDataWrapper.h"

class pgCollection;

class pgForeignServerFactory : public pgForeignDataWrapperObjFactory
{
public:
	pgForeignServerFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgForeignServerFactory foreignServerFactory;


class pgForeignServer : public pgForeignDataWrapperObject
{
public:
	pgForeignServer(pgForeignDataWrapper *newForeignDataWrapper, const wxString &newName = wxT(""));
	wxString GetTranslatedMessage(int kindOfMessage) const;

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return true;
	}
	bool CanCreate()
	{
		return true;
	}

	wxString GetFdw() const
	{
		return fdw;
	}
	void iSetFdw(const wxString &s)
	{
		fdw = s;
	}
	wxString GetType() const
	{
		return type;
	}
	void iSetType(const wxString &s)
	{
		type = s;
	}
	wxString GetVersion() const
	{
		return version;
	}
	void iSetVersion(const wxString &s)
	{
		version = s;
	}
	wxString GetOptions() const
	{
		return options;
	}
	wxString GetCreateOptions();
	void iSetOptions(const wxString &s)
	{
		options = s;
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
	wxString fdw, type, version, options;
};

class pgForeignServerCollection : public pgForeignDataWrapperObjCollection
{
public:
	pgForeignServerCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
