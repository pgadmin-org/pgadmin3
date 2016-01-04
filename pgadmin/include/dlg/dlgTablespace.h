//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTablespace.h - Tablespace property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TABLESPACEPROP
#define __DLG_TABLESPACEPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgTablespace;

class dlgTablespace : public dlgSecurityProperty
{
public:
	dlgTablespace(pgaFactory *factory, frmMain *frame, pgTablespace *node = 0);

	void CheckChange();
	wxString GetSql();
	wxString GetSql2();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	wxString GetHelpPage() const;

	int Go(bool modal);

private:
	pgTablespace *tablespace;
	wxArrayString varInfo;
	bool dirtyVars;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnVarAdd(wxCommandEvent &ev);
	void OnVarRemove(wxCommandEvent &ev);
	void OnVarSelChange(wxListEvent &ev);
	void OnVarnameSelChange(wxCommandEvent &ev);
	void SetupVarEditor(int var);

	DECLARE_EVENT_TABLE()
};


#endif
