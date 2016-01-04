//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgIndex.h - Index property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_INDEXPROP
#define __DLG_INDEXPROP

#include "dlg/dlgProperty.h"


class pgIndex;
class pgIndexBase;

#define btnAddCol       CTRL_BUTTON("btnAddCol")
#define btnRemoveCol    CTRL_BUTTON("btnRemoveCol")

class dlgIndexBase : public dlgCollistProperty
{
public:
	dlgIndexBase(pgaFactory *factory, frmMain *frame, const wxString &resName, pgIndexBase *index, pgTable *parentNode);
	dlgIndexBase(pgaFactory *factory, frmMain *frame, const wxString &resName, ctlListView *colList);

	void OnSelectComboCol(wxCommandEvent &ev);
	void OnSelectListCol(wxListEvent &ev);
	void OnSelectCol();
	void CheckChange();
	pgObject *GetObject();
	int Go(bool modal);

protected:
	pgIndexBase *index;

private:
	DECLARE_EVENT_TABLE()
};


class dlgIndex : public dlgIndexBase
{
public:
	dlgIndex(pgaFactory *factory, frmMain *frame, pgIndex *index, pgTable *parentNode);

	int Go(bool modal);
	void CheckChange();
	wxString GetColumns();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);

private:
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnSelectType(wxCommandEvent &ev);
	void OnDescChange(wxCommandEvent &ev);
	void OnAddCol(wxCommandEvent &ev);
	void OnRemoveCol(wxCommandEvent &ev);

	wxString m_previousType;

	DECLARE_EVENT_TABLE()
};


#endif
