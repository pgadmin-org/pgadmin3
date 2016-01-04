//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
	wxMenu *GetNewMenu();
	bool CanDropCascaded()
	{
		return true;
	}
	bool CanCreate()
	{
		return true;
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
	wxString type, version, options;
};


class pgForeignServerObjFactory : public pgForeignDataWrapperObjFactory
{
public:
	pgForeignServerObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
		: pgForeignDataWrapperObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};


// Object that lives in a foreign server
class pgForeignServerObject : public pgForeignDataWrapperObject
{
public:
	pgForeignServerObject(pgForeignServer *newForeignServer, pgaFactory &factory, const wxString &newName = wxEmptyString) : pgForeignDataWrapperObject(newForeignServer->GetForeignDataWrapper(), factory, newName)
	{
		SetForeignServer(newForeignServer);
	}
	pgForeignServerObject(pgForeignServer *newForeignServer, int newType, const wxString &newName = wxT("")) : pgForeignDataWrapperObject(newForeignServer->GetForeignDataWrapper(), newType, newName)
	{
		SetForeignServer(newForeignServer);
	}

	bool CanDrop();
	bool CanEdit()
	{
		return true;
	}
	bool CanCreate();

	void SetForeignServer(pgForeignServer *newForeignServer);
	virtual pgForeignServer *GetForeignServer() const
	{
		return srv;
	}
	pgSet *ExecuteSet(const wxString &sql);
	wxString ExecuteScalar(const wxString &sql);
	bool ExecuteVoid(const wxString &sql);


protected:
	virtual void SetContextInfo(frmMain *form);

	pgForeignServer *srv;
};


class pgForeignServerObjCollection : public pgCollection
{
public:
	pgForeignServerObjCollection(pgaFactory *factory, pgForeignServer *newsrv);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
