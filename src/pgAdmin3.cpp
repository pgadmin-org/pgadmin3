//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgAdmin3.cpp - The application
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>

// App headers
#include "pgAdmin3.h"
#include "utils/sysLogger.h"
#include "utils/sysSettings.h"
#include "ui/forms/frmMain.h"
#include "ui/forms/frmSplash.h"

// Globals
frmMain *winMain;
wxLog *objLogger;
sysSettings *objSettings;

IMPLEMENT_APP(pgAdmin3)

// The Application!
bool pgAdmin3::OnInit()
{

    // Load the Settings
    objSettings = new sysSettings();

    // Setup logging
    objLogger = new sysLogger();
    wxLog::SetActiveTarget(objLogger);

    wxString szMsg;
    szMsg << wxT("# ") << APPNAME_L << wxT(" Version ") << VERSION << wxT(" Startup");
    wxLogInfo(wxT("##############################################################"));
    wxLogInfo(szMsg);
    wxLogInfo(wxT("##############################################################"));

    // Show the splash screen
    frmSplash* winSplash = new frmSplash((wxFrame *)NULL);
    SetTopWindow(winSplash);
    winSplash->Show(TRUE);

    // Set some defaults
#ifdef __WXMSW__
    SetAuto3D(TRUE);
#endif
    SetAppName(APPNAME_L);

#ifndef _DEBUG
    wxSleep(2);
#endif

    // Create & show the main form
    winMain = new frmMain(APPNAME_L, wxPoint(objSettings->GetFrmMainLeft(), objSettings->GetFrmMainTop()), wxSize(objSettings->GetFrmMainWidth(), objSettings->GetFrmMainHeight()));
    winMain->Show(TRUE);
    SetTopWindow(winMain);
    SetExitOnFrameDelete(TRUE);

    winSplash->Close();

    // Display a Tip if required.
    extern sysSettings *objSettings;
    if (objSettings->GetShowTipOfTheDay()) winMain->TipOfTheDay();

    return TRUE;
}

// Not the Application!
int pgAdmin3::OnExit()
{
    // Delete the settings object to ensure settings are saved.
    delete objSettings;
    return 1;
}

