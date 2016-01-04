//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmPassword.h - Change password
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMPASSWORD_H
#define FRMPASSWORD_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class pgServer;
// Class declarations
class frmPassword : public pgDialog
{
public:
	frmPassword(wxFrame *parent, pgObject *obj);
	~frmPassword();

private:
	pgServer *server;
	void OnHelp(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &event);
	void OnCancel(wxCommandEvent &event);
	DECLARE_EVENT_TABLE()
};


class passwordFactory : public actionFactory
{
public:
	passwordFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
