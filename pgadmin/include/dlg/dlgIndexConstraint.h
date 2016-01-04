//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgIndexConstraint.h - IndexConstraint property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_INDEXCONSTRAINTPROP
#define __DLG_INDEXCONSTRAINTPROP


#include "dlg/dlgIndex.h"


class pgTable;
class pgIndexBase;
class pgPrimaryKey;
class pgUnique;
class pgExclude;

class dlgIndexConstraint : public dlgIndexBase
{
protected:
	dlgIndexConstraint(pgaFactory *factory, frmMain *frame, const wxString &resName, pgIndexBase *index, pgTable *parentNode);
	dlgIndexConstraint(pgaFactory *factory, frmMain *frame, const wxString &resName, ctlListView *colList);

public:
	ctlListView *columns;


	int Go(bool modal);
	void CheckChange();
	wxString GetDefinition();
	wxString GetColumns();
	wxString GetSql();
	wxString GetHelpPage() const
	{
		return wxT("pg/sql-altertable");
	}

private:
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnAddCol(wxCommandEvent &ev);
	void OnRemoveCol(wxCommandEvent &ev);
	void OnSelectType(wxCommandEvent &ev);
	void OnSelectComboCol(wxCommandEvent &ev);
	void OnChangeIndex(wxCommandEvent &ev);

	wxString m_previousType;

	DECLARE_EVENT_TABLE()
};


class dlgPrimaryKey : public dlgIndexConstraint
{
public:
	dlgPrimaryKey(pgaFactory *factory, frmMain *frame, pgPrimaryKey *index, pgTable *parentNode);
	dlgPrimaryKey(pgaFactory *factory, frmMain *frame, ctlListView *colList);

	pgObject *CreateObject(pgCollection *collection);
};


class dlgUnique : public dlgIndexConstraint
{
public:
	dlgUnique(pgaFactory *factory, frmMain *frame, pgUnique *index, pgTable *parentNode);
	dlgUnique(pgaFactory *factory, frmMain *frame, ctlListView *colList);

	pgObject *CreateObject(pgCollection *collection);
};


class dlgExclude : public dlgIndexConstraint
{
public:
	dlgExclude(pgaFactory *factory, frmMain *frame, pgExclude *index, pgTable *parentNode);
	dlgExclude(pgaFactory *factory, frmMain *frame, ctlListView *colList);

	pgObject *CreateObject(pgCollection *collection);
};


#endif
