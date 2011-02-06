//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
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
};
extern pgForeignDataWrapperFactory foreignDataWrapperFactory;

class pgForeignDataWrapper : public pgDatabaseObject
{
public:
	pgForeignDataWrapper(const wxString &newName = wxT(""));

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return true;
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
	wxString validatorProc, options;
};

#endif
