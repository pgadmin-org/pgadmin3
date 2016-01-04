//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgPgpassConfig.cpp - Configure setting
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlg/dlgPgpassConfig.h"
#include "db/pgConn.h"
#include "db/pgSet.h"

// Icons
#include "images/property.pngc"





BEGIN_EVENT_TABLE(dlgPgpassConfig, DialogWithHelp)
	EVT_BUTTON (wxID_OK,                dlgPgpassConfig::OnOK)
	EVT_BUTTON (wxID_CANCEL,            dlgPgpassConfig::OnCancel)
	EVT_CHECKBOX(XRCID("chkEnabled"),   dlgPgpassConfig::OnChange)
	EVT_TEXT(XRCID("txtHostname"),      dlgPgpassConfig::OnChange)
	EVT_TEXT(XRCID("txtPort"),          dlgPgpassConfig::OnChange)
	EVT_TEXT(XRCID("txtDatabase"),      dlgPgpassConfig::OnChange)
	EVT_TEXT(XRCID("txtUsername"),      dlgPgpassConfig::OnChange)
	EVT_TEXT(XRCID("txtPassword"),      dlgPgpassConfig::OnChange)
	EVT_TEXT(XRCID("txtRePassword"),    dlgPgpassConfig::OnChange)
END_EVENT_TABLE()


#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define txtHostname         CTRL_TEXT("txtHostname")
#define txtPort             CTRL_TEXT("txtPort")
#define txtDatabase         CTRL_TEXT("txtDatabase")
#define txtUsername         CTRL_TEXT("txtUsername")
#define txtPassword         CTRL_TEXT("txtPassword")
#define txtRePassword       CTRL_TEXT("txtRePassword")

dlgPgpassConfig::dlgPgpassConfig(pgFrame *parent, pgPassConfigLine *_line) :
	DialogWithHelp((frmMain *)parent)
{
	SetFont(settings->GetSystemFont());
	LoadResource((wxWindow *)parent, wxT("dlgPgpassConfig"));

	userAdding = databaseAdding = false;

	// Icon
	SetIcon(*property_png_ico);
	RestorePosition();
	line = _line;

	chkEnabled->SetValue(!line->isComment);
	txtHostname->SetValue(line->hostname);
	txtPort->SetValue(line->port);
	txtDatabase->SetValue(line->database);
	txtUsername->SetValue(line->username);
	txtPassword->SetValue(line->password);
	txtRePassword->SetValue(line->password);
	btnOK->Disable();

}


dlgPgpassConfig::~dlgPgpassConfig()
{
	SavePosition();
}


wxString dlgPgpassConfig::GetHelpPage() const
{
	return wxT("pg/libpq-pgpass");
}


void dlgPgpassConfig::OnChange(wxCommandEvent &ev)
{
	/* Add any required validation rules here */
	wxString passwd = txtPassword->GetValue();
	wxString repasswd = txtRePassword->GetValue();
	if (passwd.IsEmpty() || (passwd.Length() > 1))
	{
		if (!passwd.compare(repasswd))
			btnOK->Enable();
	}
}


void dlgPgpassConfig::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	line->isComment = !chkEnabled->GetValue();
	line->hostname = txtHostname->GetValue();
	line->port = txtPort->GetValue();
	line->database = txtDatabase->GetValue();
	line->username = txtUsername->GetValue();
	line->password = txtPassword->GetValue();
	EndModal(wxID_OK);
}


void dlgPgpassConfig::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}


int dlgPgpassConfig::Go()
{
	return ShowModal();
}
