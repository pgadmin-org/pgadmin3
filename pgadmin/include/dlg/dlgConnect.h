//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgConnect.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef DLGCONNECT_H
#define DLGCONNECT_H

#include "dlg/dlgClasses.h"

// Class declarations
class dlgConnect : public DialogWithHelp
{
public:
	dlgConnect(frmMain *form, const wxString &description, bool needPwd);
	~dlgConnect();
	wxString GetHelpPage() const;


	wxString GetPassword();
	bool GetStorePwd();
	int Go();

private:
	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	DECLARE_EVENT_TABLE()
};

#endif
