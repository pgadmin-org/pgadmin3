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
#include <wx/timer.h>
#include <wx/xrc/xmlres.h>

// Windows headers
#ifdef __WXMSW__
  #include <winsock.h>
#endif
#include <stdlib.h>

// App headers
#include "pgAdmin3.h"
#include "sysLogger.h"
#include "sysSettings.h"
#include "frmMain.h"
#include "frmSplash.h"

// Globals
frmMain *winMain;
wxLog *objLogger;
sysSettings *objSettings;
wxStopWatch swTimer;
wxString szTimer;

IMPLEMENT_APP(pgAdmin3)

// The Application!
bool pgAdmin3::OnInit()
{

    // Load the Settings
#ifdef __WXMSW__
    objSettings = new sysSettings(APPNAME_L);
#else
    objSettings = new sysSettings(APPNAME_S);
#endif

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
    if (!winSplash) 
        wxLogError(wxT("Couldn't create the splash screen!"));
    else {
      SetTopWindow(winSplash);
      winSplash->Show(TRUE);
	  winSplash->Refresh();
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
    LoadXrc(wxString("frmConnect.xrc"));
    LoadXrc(wxString("frmOptions.xrc"));
    LoadXrc(wxString("frmPassword.xrc"));

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
    winMain = new frmMain(APPNAME_L, wxPoint(objSettings->GetFrmMainLeft(), objSettings->GetFrmMainTop()), wxSize(objSettings->GetFrmMainWidth(), objSettings->GetFrmMainHeight()));
    winMain->Show(TRUE);
    SetTopWindow(winMain);
    SetExitOnFrameDelete(TRUE);

    if (winSplash) {
        winSplash->Close();
        delete winSplash;
    }

    // Display a Tip if required.
    extern sysSettings *objSettings;
    if (objSettings->GetShowTipOfTheDay()) winMain->OnTipOfTheDay();

    return TRUE;
}

// Not the Application!
int pgAdmin3::OnExit()
{
    // Delete the settings object to ensure settings are saved.
    delete objSettings;
#ifdef __WXMSW__
	WSACleanup();
#endif
    return 1;
}

// Global Stuff

void StartMsg(const wxString& szNewMsg)
{
    if (!szTimer.IsEmpty()) return;
    szTimer.Printf("%s...", szNewMsg.c_str());
    wxBeginBusyCursor();
    swTimer.Start(0);
    wxLogStatus(szTimer);
    winMain->stBar->SetStatusText(szTimer, 1);
}

void EndMsg()
{

    if (szTimer.IsEmpty()) return;

    // Get the execution time & display it
    float fTime = swTimer.Time();
    wxString szTime, szMsg;
    szTime.Printf("%.2f Secs", (fTime/1000));
    winMain->stBar->SetStatusText(szTime, 2);

    // Display the 'Done' message
    szTimer.Append(" Done.");
    szMsg.Printf("%s (%s)", szTimer.c_str(), szTime.c_str());
    wxLogStatus(szMsg);
    winMain->stBar->SetStatusText(szTimer, 1);
    wxEndBusyCursor();
    szTimer.Empty();
    
}

int wxCALLBACK ListSort(long itm1, long itm2, long sortData)
{
    return itm1 < itm2;
}

void pgAdmin3::LoadXrc(const wxString szFile)
{
    wxString szMsg, szXRC;
    szMsg.Printf(wxT("Loading %s"), szFile.c_str());
    wxLogInfo(szMsg);
    szXRC.Printf("%s/%s", XRC_PATH, szFile.c_str());
    wxXmlResource::Get()->Load(szXRC);
}

// Conversions

wxString StrToYesNo(const wxString& szVal)
{
    wxString szResult;
    if (szVal.StartsWith(wxT("t"))) {
        szResult.Printf("Yes");
    } else if (szVal.StartsWith(wxT("T"))) {
        szResult.Printf("Yes");
    } else if (szVal.StartsWith(wxT("1"))) {
        szResult.Printf("Yes");
    } else if (szVal.StartsWith(wxT("Y"))) {
        szResult.Printf("Yes");
    } else if (szVal.StartsWith(wxT("y"))) {
        szResult.Printf("Yes");
    } else {
        szResult.Printf("No");
    }

    return szResult;
}

wxString BoolToYesNo(bool bVal)
{
    wxString szResult;
    if (bVal) {
        szResult.Printf("Yes");
    } else {
        szResult.Printf("No");
    }
    return szResult;
}

bool StrToBool(const wxString& szVal)
{
    if (szVal.StartsWith(wxT("t"))) {
        return TRUE;
    } else if (szVal.StartsWith(wxT("T"))) {
        return TRUE;
    } else if (szVal.StartsWith(wxT("1"))) {
        return TRUE;
    } else if (szVal.StartsWith(wxT("Y"))) {
        return TRUE;
    } else if (szVal.StartsWith(wxT("y"))) {
        return TRUE;
    } 

    return FALSE;
}

wxString NumToStr(long nVal)
{
    wxString szResult;
    szResult.Printf("%d", nVal);
    return szResult;
}

long StrToLong(const wxString& szVal)
{
    return atol(szVal.c_str());
}

wxString NumToStr(double nVal)
{
    wxString szResult;
    szResult.Printf("%lf", nVal);

    // Get rid of excessive decimal places
    if (szResult.Contains(wxT("."))) {
        while ((szResult.Right(1) == "0") || (szResult.Right(1) == ".")) {
            szResult.RemoveLast();
        }
    }

    return szResult;
}

double StrToDouble(const wxString& szVal)
{
    return strtod(szVal.c_str(), 0);
}

wxString qtString(const wxString& szVal)
{
    wxString szRes = szVal;	

    szRes.Replace("\\", "\\\\");
    szRes.Replace("'", "\\'");
    szRes.Append(wxT("'"));
    szRes.Prepend(wxT("'"));
	
    return szRes;
}

wxString qtIdent(const wxString& szVal)
{
    wxString szRes = szVal;	
    wxString szOutput;
	
    int iPos = 0;

    // Replace Double Quotes
    szRes.Replace("\"", "\"\"");
	
    // Is it a number?
    if (szRes.IsNumber()) {
        szRes.Append(wxT("\""));
        szRes.Prepend(wxT("\""));
        return szRes;
    } else {
        while (iPos < (int)szRes.length()) {
            if (!((szRes[iPos] >= '0') && (szRes[iPos] <= '9')) && 
                !((szRes[iPos]  >= 'a') && (szRes[iPos]  <= 'z')) && 
                !(szRes[iPos]  == '_')){
            
                szRes.Append(wxT("\""));
                szRes.Prepend(wxT("\""));
                return szRes;	
            }
            iPos++;
        }
    }	
    return szRes;
}
