//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmOptions.cpp - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmOptions.h"
#include "sysSettings.h"
#include "sysLogger.h"
#include "misc.h"

// Icons
#include "images/pgAdmin3.xpm"

BEGIN_EVENT_TABLE(frmOptions, wxDialog)
    EVT_BUTTON (XRCID("btnOK"),               frmOptions::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),           frmOptions::OnCancel)
    EVT_BUTTON (XRCID("btnBrowseLogfile"),    frmOptions::OnBrowseLogFile)
END_EVENT_TABLE()

frmOptions::frmOptions(wxFrame *parent)
{

    wxLogInfo(wxT("Creating an options dialogue"));

    wxXmlResource::Get()->LoadDialog(this, parent, "frmOptions"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

    CTRL("txtLogfile", wxTextCtrl)->SetValue(settings->GetLogFile());
    CTRL("radLoglevel", wxRadioBox)->SetSelection(settings->GetLogLevel());
    CTRL("txtMaxRows", wxTextCtrl)->SetValue(NumToStr(settings->GetMaxRows()));
    CTRL("chkAskSaveConfirm", wxCheckBox)->SetValue(!settings->GetAskSaveConfirmation());
    CTRL("chkAskDelete", wxCheckBox)->SetValue(settings->GetConfirmDelete());
    CTRL("chkShowUsersForPrivileges", wxCheckBox)->SetValue(settings->GetShowUsersForPrivileges());
}


frmOptions::~frmOptions()
{
    wxLogInfo(wxT("Destroying an options dialogue"));
}



void frmOptions::OnOK(wxCommandEvent &ev)
{

    // Logfile
    wxString logFile = CTRL("txtLogfile", wxTextCtrl)->GetValue();
    wxLogInfo(wxT("Setting logfile to: %s"), logFile.c_str());
    settings->SetLogFile(logFile);

    // Loglevel
    wxString logInfo = CTRL("radLoglevel", wxRadioBox)->GetStringSelection();
    wxLogInfo(wxT("Setting loglevel to: %s"),logInfo.c_str());
    int sel = CTRL("radLoglevel", wxRadioBox)->GetSelection();

    switch(sel) {
        case(0):
          settings->SetLogLevel(LOG_NONE);
          break;
        case(1):
          settings->SetLogLevel(LOG_ERRORS);
          break;
        case(2):
          settings->SetLogLevel(LOG_SQL);
          break;
        case(3):
          settings->SetLogLevel(LOG_DEBUG);
          break;
        default:
          settings->SetLogLevel(LOG_ERRORS);
          break;
    }

    // Query parameter
    wxString maxRows=CTRL("txtMaxRows", wxTextCtrl)->GetValue();
    settings->SetMaxRows(StrToLong(maxRows));

    settings->SetAskSaveConfirmation(!(CTRL("chkAskSaveConfirm", wxCheckBox)->IsChecked()));
    settings->SetConfirmDelete((CTRL("chkAskDelete", wxCheckBox)->IsChecked()));
    settings->SetShowUsersForPrivileges((CTRL("chkShowUsersForPrivileges", wxCheckBox)->IsChecked()));
    Destroy();
}


void frmOptions::OnCancel(wxCommandEvent &ev)
{
    Destroy();
}

void frmOptions::OnBrowseLogFile(wxCommandEvent &ev)
{
    wxFileDialog logFile(this, wxT("Select log file"), wxT(""), wxT(""), wxT("Log files (*.log)|*.log|All files (*.*)|*.*"));
    logFile.SetDirectory(wxGetHomeDir());
    if (logFile.ShowModal() == wxID_OK) CTRL("txtLogfile", wxTextCtrl)->SetValue(logFile.GetPath());
}
