//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgMainConfig.cpp - Configure setting
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlg/dlgMainConfig.h"


// Icons
#include "images/property.pngc"





BEGIN_EVENT_TABLE(dlgMainConfig, DialogWithHelp)
	EVT_BUTTON (wxID_OK,              dlgMainConfig::OnOK)
	EVT_BUTTON (wxID_CANCEL,          dlgMainConfig::OnCancel)
	EVT_TEXT(XRCID("txtValue"),       dlgMainConfig::OnChange)
	EVT_TEXT(XRCID("cbValue"),        dlgMainConfig::OnChange)
	EVT_CHECKBOX(XRCID("chkValue"),   dlgMainConfig::OnChange)
END_EVENT_TABLE()


#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define cbValue             CTRL_COMBOBOX("cbValue")
#define txtValue            CTRL_TEXT("txtValue")
#define chkValue            CTRL_CHECKBOX("chkValue")
#define txtComment          CTRL_TEXT("txtComment")
#define stName              CTRL_STATIC("stName")
#define stDescription       CTRL_STATIC("stDescription")


static const wxChar *contextStrings[] =
{
	__("Internal - not externally settable"),
	__("Postmaster - set on server start"),
	__("SIGHUP - reloaded on SIGHUP signal"),
	__("Backend - overridable in individual backend"),
	__("Suset - may be overridden by superuser"),
	__("Userlimit - may be set by user"),
	__("Userset - may be set by user"),
	__("Unknown")
};


static const wxChar *sourceStrings[] =
{
	__("Variable has still its initial default value"),
	__("Set via environment variable"),
	__("Set in configuration file"),
	__("Set on command line"),
	__("Set by unprivileged command"),
	__("Set in database variables"),
	__("Set in user variables"),
	__("Set in client parameters"),
	__("set by Override"),
	__("Set interactively"),
	__("Set by test"),
	__("Set by session parameters")
};


dlgMainConfig::dlgMainConfig(pgFrame *parent, pgSettingItem *_item) :
	DialogWithHelp((frmMain *)parent)
{
	SetFont(settings->GetSystemFont());
	LoadResource((wxWindow *)parent, wxT("dlgMainConfig"));

	// Icon
	SetIcon(*property_png_ico);
	RestorePosition();

	item = _item;

	SetTitle(wxString::Format(_("Configuration setting \"%s\""), item->name.c_str()));

	// Setup the default values

	cbValue->Hide();
	chkValue->Hide();

	if (!item->newLine)
	{
		if (item->orgLine)
			item->newLine = new pgConfigLine(item->orgLine);
		else
		{
			item->newLine = new pgConfigLine();
			item->newLine->item = item;
		}
	}

	chkEnabled->SetValue(!item->newLine->isComment);
	txtValue->SetValue(item->newLine->value);
	txtComment->SetValue(item->newLine->comment);

	wxFont fntLabel = stName->GetFont();
	fntLabel.SetWeight(wxBOLD);
	stName->SetFont(fntLabel);
	stName->SetLabel(item->name);

	wxString str;
	str += _("Category") + wxString(wxT(": ")) + item->category + END_OF_LINE;
	str += _("Context") + wxString(wxT(": "));
	str += wxGetTranslation(contextStrings[item->context]);
	str += END_OF_LINE;

	if (item->source != pgSettingItem::PGC_UNKNOWNSOURCE)
	{
		str += _("Current value") + wxString(wxT(": "));

		if (item->value == wxT("unset") && item->source == pgSettingItem::PGC_DEFAULT)
			str += _("unset");
		else
			str += item->value + END_OF_LINE wxT("     ") + wxGetTranslation(sourceStrings[item->source]);

		str += END_OF_LINE;
	}

	stDescription->SetLabel(str + END_OF_LINE + item->short_desc + END_OF_LINE + item->extra_desc);
	btnOK->Enable();
}


dlgMainConfig::~dlgMainConfig()
{
	SavePosition();
}


wxString dlgMainConfig::GetValue()
{
	return txtValue->GetValue();
}


wxString dlgMainConfig::GetHelpPage() const
{
	return wxT("pg/runtime-config");
}


void dlgMainConfig::OnChange(wxCommandEvent &ev)
{
}


void dlgMainConfig::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	item->newLine->value = GetValue();
	item->newLine->comment = txtComment->GetValue();
	item->newLine->isComment = !chkEnabled->GetValue();

	EndModal(wxID_OK);
}


void dlgMainConfig::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

int dlgMainConfig::Go()
{
	// Set focus on the Password textbox and show modal
	return ShowModal();
}
