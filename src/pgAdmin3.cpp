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
    
    // Setup logging first
    objLogger = new sysLogger();
    wxLog::SetActiveTarget(objLogger);
    
    // Show the splash screen
    frmSplash* winSplash = new frmSplash((wxFrame *)NULL);
    SetTopWindow(winSplash);
    winSplash->Show(TRUE);
    
    // Set some defaults
    SetAuto3D(TRUE);
    SetAppName(APPNAME);

    // Load the Settings
    objSettings = new sysSettings();

#ifndef _DEBUG
    wxSleep(2);
#endif
    
    // Create & show the main form
    winMain = new frmMain(wxT("pgAdmin III"), wxPoint(objSettings->GetFrmMainLeft(), objSettings->GetFrmMainTop()), wxSize(objSettings->GetFrmMainWidth(), objSettings->GetFrmMainHeight()));
    winMain->Show(TRUE);
    SetTopWindow(winMain);
    SetExitOnFrameDelete(TRUE);
    
    winSplash->Close();
    
    return TRUE;
}

// Not the Application!
int pgAdmin3::OnExit()
{
    // Delete the settings object to ensure settings are saved.
    delete objSettings;
    return 1;
}

