//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgUser.h - User property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_USERPROP
#define __DLG_USERPROP

#include "dlg/dlgProperty.h"
#include "ctl/calbox.h"
#include "ctl/timespin.h"

class pgUser;

class dlgUser : public dlgProperty
{
public:
	dlgUser(pgaFactory *factory, frmMain *frame, pgUser *node = 0);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	wxString GetHelpPage() const;

	int Go(bool modal);

private:
	pgUser *user;
	wxArrayString varInfo;

	void OnChangeSuperuser(wxCommandEvent &ev);
	void OnChangeSpin(wxSpinEvent &ev);
	void OnChangeCal(wxCalendarEvent &ev);
	void OnChangeDate(wxDateEvent &ev);
	void OnGroupAdd(wxCommandEvent &ev);
	void OnGroupRemove(wxCommandEvent &ev);

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
