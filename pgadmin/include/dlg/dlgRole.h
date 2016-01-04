//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRole.h - Role property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_ROLEPROP
#define __DLG_ROLEPROP

#include "dlg/dlgProperty.h"
#include "ctl/calbox.h"
#include "ctl/timespin.h"
#include "ctl/ctlSeclabelPanel.h"

class pgRole;

class dlgRole : public dlgProperty
{
public:
	dlgRole(pgaFactory *factory, frmMain *frame, pgRole *node = 0, bool chkLogin = false);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	wxString GetHelpPage() const;

	int Go(bool modal);

private:
	pgRole *role;
	wxArrayString varInfo;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);

	void OnOK(wxCommandEvent &ev);
	void OnChangeSuperuser(wxCommandEvent &ev);
	void OnChangeSpin(wxSpinEvent &ev);
	void OnChangeCal(wxCalendarEvent &ev);
	void OnChangeDate(wxDateEvent &ev);
	void OnRoleAdd(wxCommandEvent &ev);
	void OnRoleRemove(wxCommandEvent &ev);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnVarAdd(wxCommandEvent &ev);
	void OnVarRemove(wxCommandEvent &ev);
	void OnVarSelChange(wxListEvent &ev);

	void OnVarnameSelChange(wxCommandEvent &ev);
	void OnChangePasswd(wxCommandEvent &ev);

	void SetupVarEditor(int var);

	wxArrayString groupsIn;

	DECLARE_EVENT_TABLE()
};


#endif
