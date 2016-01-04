//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmPassword.cpp - Change password
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frm/frmPassword.h"
#include "schema/pgServer.h"

#include "images/connect.pngc"

#define txtCurrent      CTRL_TEXT("txtCurrent")
#define txtNew          CTRL_TEXT("txtNew")
#define txtConfirm      CTRL_TEXT("txtConfirm")


BEGIN_EVENT_TABLE(frmPassword, pgDialog)
	EVT_BUTTON (wxID_HELP,            frmPassword::OnHelp)
	EVT_BUTTON (wxID_OK,              frmPassword::OnOK)
	EVT_BUTTON (wxID_CANCEL,          frmPassword::OnCancel)
END_EVENT_TABLE()


frmPassword::frmPassword(wxFrame *parent, pgObject *obj)
{
	SetFont(settings->GetSystemFont());
	LoadResource(parent, wxT("frmPassword"));
	RestorePosition();

	server = obj->GetServer();
	// Icon
	SetIcon(*connect_png_ico);
}

frmPassword::~frmPassword()
{
	SavePosition();
}


void frmPassword::OnHelp(wxCommandEvent &ev)
{
	DisplayHelp(wxT("password"), HELP_PGADMIN);
}


void frmPassword::OnOK(wxCommandEvent &event)
{

	// Is the old password right?
	if (txtCurrent->GetValue() != server->GetPassword())
	{
		wxLogError(__("Incorrect password!"));
		return;
	}

	// Did we confirm the password OK?
	if (txtNew->GetValue() != txtConfirm->GetValue())
	{
		wxLogError(__("Passwords do not match!"));
		return;
	}

	// Set the new password
	if (!server->SetPassword(txtNew->GetValue()))
	{
		wxLogError(__("The password could not be changed!"));
		return;
	}

	// All must have gone well!
	wxLogMessage(__("Password successfully changed!"));
	this->Destroy();
}


void frmPassword::OnCancel(wxCommandEvent &event)
{
	Destroy();
}


passwordFactory::passwordFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("C&hange Password..."), _("Change your password."));
}


wxWindow *passwordFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmPassword *frm = new frmPassword((pgFrame *)form, obj);
	frm->Show();
	return 0;
}


bool passwordFactory::CheckEnable(pgObject *obj)
{
	if (obj)
	{
		pgServer *server = obj->GetServer();
		return server && server->GetConnected();
	}
	return false;
}
