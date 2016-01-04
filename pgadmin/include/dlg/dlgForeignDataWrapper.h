//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgForeignDataWrapper.h - Foreign Data Wrapper property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_FOREIGNDATAWRAPPERPROP
#define __DLG_FOREIGNDATAWRAPPERPROP

#include "dlg/dlgProperty.h"

class pgForeignDataWrapper;

class dlgForeignDataWrapper : public dlgSecurityProperty
{
public:
	dlgForeignDataWrapper(pgaFactory *factory, frmMain *frame, pgForeignDataWrapper *db);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgForeignDataWrapper *fdw;

	void OnChange(wxCommandEvent &ev);
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnSelChangeOption(wxListEvent &ev);
	void OnChangeOptionName(wxCommandEvent &ev);
	void OnAddOption(wxCommandEvent &ev);
	void OnChangeOption(wxCommandEvent &ev);
	void OnRemoveOption(wxCommandEvent &ev);

	wxString GetOptionsSql();

	DECLARE_EVENT_TABLE()
};


#endif
