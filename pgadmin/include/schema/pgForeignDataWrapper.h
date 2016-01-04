//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgForeignDataWrapper.h PostgreSQL Foreign Data Wrapper
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGFOREIGNDATAWRAPPER_H
#define PGFOREIGNDATAWRAPPER_H

#include "pgDatabase.h"

class pgCollection;
class pgForeignDataWrapperFactory : public pgDatabaseObjFactory
{
public:
	pgForeignDataWrapperFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgForeignDataWrapperFactory foreignDataWrapperFactory;

class pgForeignDataWrapper : public pgDatabaseObject
{
public:
	pgForeignDataWrapper(const wxString &newName = wxT(""));
	wxString GetTranslatedMessage(int kindOfMessage) const;

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return true;
	}
	wxMenu *GetNewMenu();

	wxString GetHandlerProc() const
	{
		return handlerProc;
	}
	void iSetHandlerProc(const wxString &s)
	{
		handlerProc = s;
	}
	wxString GetValidatorProc() const
	{
		return validatorProc;
	}
	void iSetValidatorProc(const wxString &s)
	{
		validatorProc = s;
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
	wxString handlerProc, validatorProc, options;
};

class pgForeignDataWrapperObjFactory : public pgDatabaseObjFactory
{
public:
	pgForeignDataWrapperObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
		: pgDatabaseObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};


class pgForeignDataWrapperCollection : public pgDatabaseObjCollection
{
public:
	pgForeignDataWrapperCollection(pgaFactory *factory, pgDatabase *db);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


// Object that lives in a foreign data wrapper
class pgForeignDataWrapperObject : public pgDatabaseObject
{
public:
	pgForeignDataWrapperObject(pgForeignDataWrapper *newForeignDataWrapper, pgaFactory &factory, const wxString &newName = wxEmptyString) : pgDatabaseObject(factory, newName)
	{
		SetForeignDataWrapper(newForeignDataWrapper);
	}
	pgForeignDataWrapperObject(pgForeignDataWrapper *newForeignDataWrapper, int newType, const wxString &newName = wxT("")) : pgDatabaseObject(newType, newName)
	{
		SetForeignDataWrapper(newForeignDataWrapper);
	}

	bool CanDrop();
	bool CanEdit()
	{
		return true;
	}
	bool CanCreate();

	void SetForeignDataWrapper(pgForeignDataWrapper *newForeignDataWrapper);
	virtual pgForeignDataWrapper *GetForeignDataWrapper() const
	{
		return fdw;
	}
	pgSet *ExecuteSet(const wxString &sql);
	wxString ExecuteScalar(const wxString &sql);
	bool ExecuteVoid(const wxString &sql);


protected:
	virtual void SetContextInfo(frmMain *form);

	pgForeignDataWrapper *fdw;
};



// collection of pgForeignDataWrapperObject
class pgForeignDataWrapperObjCollection : public pgCollection
{
public:
	pgForeignDataWrapperObjCollection(pgaFactory *factory, pgForeignDataWrapper *newfdw);
	wxString GetTranslatedMessage(int kindOfMessage) const;
	virtual bool CanCreate();
};

#endif
