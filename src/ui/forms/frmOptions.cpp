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
#include "../../pgAdmin3.h"
#include "frmOptions.h"
#include "../../utils/sysSettings.h"
#include "../../utils/sysLogger.h"

// Icons
#include "../../images/pgAdmin3.xpm"

BEGIN_EVENT_TABLE(frmOptions, wxDialog)
  EVT_BUTTON (XRCID("btnOK"), frmOptions::OnOK)
  EVT_BUTTON (XRCID("btnCancel"), frmOptions::OnCancel)
  EVT_BUTTON (XRCID("btnBrowseLogfile"), frmOptions::OnBrowseLogFile)
END_EVENT_TABLE()

frmOptions::frmOptions(wxFrame *parent)
{

    wxLogInfo(wxT("Creating an options dialogue"));
    extern sysSettings *objSettings;

    wxXmlResource::Get()->LoadDialog(this, parent, "frmOptions"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    Center();

    XRCCTRL(*this, "txtLogfile", wxTextCtrl)->SetValue(objSettings->GetLogFile());
    XRCCTRL(*this, "radLoglevel", wxRadioBox)->SetSelection(objSettings->GetLogLevel());
}

frmOptions::~frmOptions()
{
    wxLogInfo(wxT("Destroying an options dialogue"));
}

void frmOptions::OnOK()
{
    extern sysSettings *objSettings;

    // Logfile
    wxString szLogFile = XRCCTRL(*this, "txtLogfile", wxTextCtrl)->GetValue();
    wxLogInfo(wxT("Setting logfile to: %s"), szLogFile.c_str());
    objSettings->SetLogFile(szLogFile);

    // Loglevel
    wxString szLogInfo = XRCCTRL(*this, "radLoglevel", wxRadioBox)->GetStringSelection();
    wxLogInfo(wxT("Setting loglevel to: %s"),szLogInfo.c_str());
    int iSel = XRCCTRL(*this, "radLoglevel", wxRadioBox)->GetSelection();

    switch(iSel) {
        case(0):
          objSettings->SetLogLevel(LOG_NONE);
          break;
        case(1):
          objSettings->SetLogLevel(LOG_ERRORS);
          break;
        case(2):
          objSettings->SetLogLevel(LOG_SQL);
          break;
        case(3):
          objSettings->SetLogLevel(LOG_DEBUG);
          break;
        default:
          objSettings->SetLogLevel(LOG_ERRORS);
          break;
    }
    this->Destroy();
}

void frmOptions::OnCancel()
{
    this->Destroy();
}

void frmOptions::OnBrowseLogFile()
{
    wxFileDialog dlgLogFile(this, wxT("Select log file"), wxT(""), wxT(""), wxT("Log files (*.log)|*.log|All files (*.*)|*.*"));
    dlgLogFile.SetDirectory(wxGetHomeDir());
    if (dlgLogFile.ShowModal() == wxID_OK) XRCCTRL(*this, "txtLogfile", wxTextCtrl)->SetValue(dlgLogFile.GetPath());
}
