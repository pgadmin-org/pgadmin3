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
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/filedlg.h>

// App headers
#include "../../pgAdmin3.h"
#include "frmOptions.h"
#include "../../utils/sysSettings.h"
#include "../../utils/sysLogger.h"

// Icons
#include "../../images/pgAdmin3.xpm"

BEGIN_EVENT_TABLE(frmOptions, wxDialog)
  EVT_BUTTON (BTN_OK, frmOptions::OK)
  EVT_BUTTON (BTN_CANCEL, frmOptions::Cancel)
  EVT_BUTTON (BTN_BROWSELOGFILE, frmOptions::BrowseLogFile)
END_EVENT_TABLE()

frmOptions::frmOptions(wxFrame *parent)
: wxDialog(parent, -1, wxT("Options"), wxDefaultPosition, wxSize(370, 460), wxCAPTION | wxDIALOG_MODAL | wxSYSTEM_MENU | wxSTAY_ON_TOP)
{

    wxLogDebug(wxT("Creating an options dialogue"));
    extern sysSettings *objSettings;

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    Center();

    // Layout the form
    // First the notebook
    wxNotebook* nbOptions = new wxNotebook(this, -1, wxPoint(5, 5), wxSize(355, 390), wxNB_TOP);
    wxPanel *pnlLogging = new wxPanel(nbOptions, -1, wxDefaultPosition, wxDefaultSize);
    nbOptions->AddPage(pnlLogging, wxT("&Logging"));

    // Now the buttons
    wxButton *btnCancel = new wxButton(this, BTN_CANCEL, wxT("&Cancel"), wxPoint(280, 400), wxSize(80, 25));
    wxButton *btnOK = new wxButton(this, BTN_OK, wxT("&OK"), wxPoint(195, 400), wxSize(80, 25));
    btnOK->SetDefault();

    // Logfile
    (void)new wxStaticText(pnlLogging, -1, wxT("Logfile (%ID will be replaced with the Process ID)"), wxPoint(10, 60), wxDefaultSize);
    txtLogFile = new wxTextCtrl(pnlLogging, -1, objSettings->GetLogFile(), wxPoint(10, 75), wxSize(295, 23));
    wxButton *btnBrowseLogFile = new wxButton(pnlLogging, BTN_BROWSELOGFILE, wxT("..."), wxPoint(305, 75), wxSize(30, 23));

    // Log level
    wxString szLogLevels[] =
    {
        "No Logging",
        "Errors",
        "Info & SQL Queries",
        "Debug"
    };
    radLogLevel = new wxRadioBox(pnlLogging, -1, "Log Level", wxPoint(110,180), wxSize(-1,-1), WXSIZEOF(szLogLevels), szLogLevels, 1, wxRA_SPECIFY_COLS );
    radLogLevel->SetSelection(objSettings->GetLogLevel());
}

frmOptions::~frmOptions()
{
    wxLogDebug(wxT("Destroying an options dialogue"));
}

void 
frmOptions::OK()
{
    extern sysSettings *objSettings;

    // Logfile
    wxString szLogFile = txtLogFile->GetValue();
    wxLogInfo(wxT("Setting logfile to: %s"), szLogFile.c_str());
    objSettings->SetLogFile(szLogFile);

    // Loglevel
    wxString szLogInfo = radLogLevel->GetStringSelection();
    wxLogInfo(wxT("Setting loglevel to: %s"),szLogInfo.c_str());
    int iSel = radLogLevel->GetSelection();

    switch(iSel) {
        case(0):
          objSettings->SetLogLevel(LOG_NONE);
          break;
        case(1):
          objSettings->SetLogLevel(LOG_ERRORS);
          break;
        case(2):
          objSettings->SetLogLevel(LOG_INFO);
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

void 
frmOptions::Cancel()
{
    this->Destroy();
}

void 
frmOptions::BrowseLogFile()
{
    wxFileDialog dlgLogFile(this, wxT("Select log file"), wxT(""), wxT(""), wxT("Log files (*.log)|*.log|All files (*.*)|*.*"));
    dlgLogFile.SetDirectory(wxGetHomeDir());
    if (dlgLogFile.ShowModal() == wxID_OK) txtLogFile->SetValue(dlgLogFile.GetPath());
}
