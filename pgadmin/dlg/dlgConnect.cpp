//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgConnect.cpp - Connect to a database
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlg/dlgConnect.h"
#include "db/pgConn.h"
#include "frm/frmHint.h"
#include "schema/pgServer.h"
#include "utils/sysLogger.h"

#include "images/connect.pngc"



BEGIN_EVENT_TABLE(dlgConnect, DialogWithHelp)
	EVT_BUTTON (wxID_OK,               dlgConnect::OnOK)
	EVT_BUTTON (wxID_CANCEL,           dlgConnect::OnCancel)
END_EVENT_TABLE()


#define stDescription   CTRL_STATIC("stDescription")
#define chkStorePwd     CTRL_CHECKBOX("chkStorePwd")
#define txtPassword     CTRL_TEXT("txtPassword")



dlgConnect::dlgConnect(frmMain *form, const wxString &description, bool storePwd) :
	DialogWithHelp(form)
{
	SetFont(settings->GetSystemFont());
	LoadResource((wxWindow *)form, wxT("dlgConnect"));

	SetIcon(*connect_png_ico);
	RestorePosition();

	// Setup the default values
	stDescription->SetLabel(description);
	chkStorePwd->SetValue(storePwd);
	txtPassword->Enable(true);

	if (form == NULL)
		chkStorePwd->Hide();
}

dlgConnect::~dlgConnect()
{
	SavePosition();
}


wxString dlgConnect::GetHelpPage() const
{
	return wxT("connect");
}


void dlgConnect::OnOK(wxCommandEvent &ev)
{
	// Display the 'save password' hint if required
	if(chkStorePwd->GetValue())
	{
		if (frmHint::ShowHint(this, HINT_SAVING_PASSWORDS) == wxID_CANCEL)
			return;
	}

	EndModal(wxID_OK);
}


void dlgConnect::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

int dlgConnect::Go()
{
	// Set focus on the Password textbox and show modal
	txtPassword->SetFocus();
	return ShowModal();
}

wxString dlgConnect::GetPassword()
{
	return txtPassword->GetValue();
}

bool dlgConnect::GetStorePwd()
{
	return chkStorePwd->GetValue();
}
