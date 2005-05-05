//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgMainConfig.cpp - Configure setting
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlgMainConfig.h"


// Icons
#include "images/property.xpm"





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


static const wxChar* contextStrings[]=
{
    _("Internal - not externally settable"),
    _("Postmaster - set on server start"),
    _("SIGHUP - reloaded on SIGHUP signal"),
    _("Backend - overridable in individual backend"),
    _("Suset - may be overridden by superuser"),
    _("Userlimit - may be set by user"),
    _("Userset - may be set by user"),
    _("Unknown")
};


static const wxChar* sourceStrings[]=
{
    _("Variable has still its initial default value"),
    _("Set via environment variable"),
    _("Set in configuration file"),
    _("Set on command line"),
    _("Set by unprivileged command"),
    _("Set in database variables"),
    _("Set in user variables"),
    _("Set in client parameters"),
    _("set by Override"),
    _("Set interactively"),
    _("Set by test"),
    _("Set by session parameters")
};


dlgMainConfig::dlgMainConfig(pgFrame *parent, pgSettingItem *_item) : 
DialogWithHelp((frmMain*)parent)
{
    wxLogInfo(wxT("Creating a main config dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource((wxWindow*)parent, wxT("dlgMainConfig"));

    // Icon
    SetIcon(wxIcon(property_xpm));
    CenterOnParent();

    item=_item;

    SetTitle(wxString::Format(_("Configuration setting \"%s\""), item->name.c_str()));

    // Setup the default values

    cbValue->Hide();
    chkValue->Hide();

    if (!item->newLine)
    {
        if (item->orgLine)
            item->newLine = new pgConfigLine(item->orgLine);
        else
            item->newLine = new pgConfigLine();
    }    

    chkEnabled->SetValue(!item->newLine->isComment);
    txtValue->SetValue(item->newLine->value);
    txtComment->SetValue(item->newLine->comment);

    wxFont fntLabel = stName->GetFont();
    fntLabel.SetWeight(wxBOLD);
    stName->SetFont(fntLabel);
    stName->SetLabel(item->name);

    wxString str;
    str += _("Category: ") + item->category + END_OF_LINE;
    str += _("Context: ");
    str += contextStrings[item->context];
    str += END_OF_LINE;

    if (item->source != pgSettingItem::PGC_UNKNOWNSOURCE)
    {
        str += _("Current value");
        str += wxT(": ");

        if (item->value == wxT("unset") && item->source == pgSettingItem::PGC_DEFAULT)
            str += _("unset");
        else
            str += item->value + END_OF_LINE + sourceStrings[item->source];

        str += END_OF_LINE;
    }

    stDescription->SetLabel(str + END_OF_LINE + item->short_desc + END_OF_LINE + item->extra_desc);
    btnOK->Enable();
}


dlgMainConfig::~dlgMainConfig()
{
    wxLogInfo(wxT("Destroying a main config dialogue"));
}


wxString dlgMainConfig::GetValue()
{
    return txtValue->GetValue();
}


wxString dlgMainConfig::GetHelpPage() const
{
    return wxT("pg/runtime-config");
}


void dlgMainConfig::OnChange(wxCommandEvent& ev)
{
}


void dlgMainConfig::OnOK(wxCommandEvent& ev)
{
    item->newLine->value = GetValue();
    item->newLine->comment = txtComment->GetValue();
    item->newLine->isComment = !chkEnabled->GetValue();

    EndModal(wxID_OK);
}


void dlgMainConfig::OnCancel(wxCommandEvent& ev)
{
    EndModal(wxID_CANCEL);
}

int dlgMainConfig::Go()
{
    // Set focus on the Password textbox and show modal
    return ShowModal();
}
