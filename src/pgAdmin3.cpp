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
#include <wx/dir.h>
#include <wx/xrc/xmlres.h>

// Windows headers
#ifdef __WXMSW__
  #include <winsock.h>
#endif

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "sysLogger.h"
#include "sysSettings.h"
#include "frmMain.h"
#include "frmSplash.h"

// Globals
frmMain *winMain;
wxLog *logger;
sysSettings *settings;

IMPLEMENT_APP(pgAdmin3)

// The Application!
bool pgAdmin3::OnInit()
{

    // Load the Settings
#ifdef __WXMSW__
    settings = new sysSettings(APPNAME_L);
#else
    settings = new sysSettings(APPNAME_S);
#endif

	// Setup logging
    logger = new sysLogger();
    wxLog::SetActiveTarget(logger);

    wxString msg;
    msg << wxT("# ") << APPNAME_L << wxT(" Version ") << VERSION << wxT(" Startup");
    wxLogInfo(wxT("##############################################################"));
    wxLogInfo(msg);
    wxLogInfo(wxT("##############################################################"));

    // Show the splash screen
    frmSplash* winSplash = new frmSplash((wxFrame *)NULL);
    if (!winSplash) 
        wxLogError(wxT("Couldn't create the splash screen!"));
    else {
      SetTopWindow(winSplash);
      winSplash->Show(TRUE);
	  winSplash->Refresh();
      wxYield();
    }
	
    // Startup the windows sockets if required
#ifdef __WXMSW__
    WSADATA	wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        wxLogFatalError("Cannot initialise the networking subsystem!");   
    }
#endif

    // Setup the XML resources
    wxXmlResource::Get()->InitAllHandlers();

    bool done;
    wxString loadPath=wxPathOnly(argv[0]);

    done=LoadAllXrc(loadPath + wxT("/") + XRC_PATH);
    if (!done)
        done=LoadAllXrc(loadPath + wxT("/../") + XRC_PATH);

    done=LoadAllXrc(loadPath + wxT("/ui/common"));
    if (!done)
        done=LoadAllXrc(loadPath + wxT("/../ui/common"));


    // Set some defaults
#ifdef __WXMSW__
    SetAuto3D(TRUE);
#endif
    SetAppName(APPNAME_L);

#ifndef _DEBUG
	#ifndef __WXMSW__
		wxYield();
	#endif
    wxSleep(2);
#endif

    // Create & show the main form
    wxPoint pos(settings->Read(wxT("frmMain/Left"), 50), settings->Read(wxT("frmMain/Top"), 50));
    wxSize size(settings->Read(wxT("frmMain/Width"), 750), settings->Read(wxT("frmMain/Height"), 550));
    CheckOnScreen(pos, size, 300, 200);

    winMain = new frmMain(APPNAME_L, pos, size);

    if (!winMain) 
        wxLogFatalError(wxT("Couldn't create the main window!"));

    winMain->Show(TRUE);
    SetTopWindow(winMain);
    SetExitOnFrameDelete(TRUE);

    if (winSplash) {
        winSplash->Close();
        delete winSplash;
    }

    // Display a Tip if required.
    extern sysSettings *settings;
    if (settings->GetShowTipOfTheDay()) winMain->OnTipOfTheDay();

    return TRUE;
}

// Not the Application!
int pgAdmin3::OnExit()
{
    // Delete the settings object to ensure settings are saved.
    delete settings;

#ifdef __WXMSW__
	WSACleanup();
#endif
    return 1;

	// Keith 2003.03.05
	// We must delete this after cleanup to prevent memory leaks
    delete logger;
}


bool pgAdmin3::LoadAllXrc(const wxString dir)
{
    if (!wxDir::Exists(dir))
        return false;

    wxArrayString files;
    int count=wxDir::GetAllFiles(dir, &files, wxT("*.xrc"), wxDIR_FILES);
    if (!count)
        return false;
    int i;

    for (i=0 ; i < count ; i++)
    {
        wxLogInfo(wxT("Loading %s"), files.Item(i).c_str());
        wxXmlResource::Get()->Load(files.Item(i));
    }
    return true;
}

