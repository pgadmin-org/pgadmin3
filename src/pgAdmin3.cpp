//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAdmin3.cpp - The application
//
//////////////////////////////////////////////////////////////////////////



// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/dir.h>
#include <wx/xrc/xmlres.h>
#include <wx/imagjpeg.h>
#include <wx/imaggif.h>
#include <wx/imagpng.h>
#include <wx/fs_zip.h>

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
wxArrayInt existingLangs;
wxLocale locale;

wxString loadPath;      // Where the program is loaded from
wxString docPath;       // Where docs are stored
wxString uiPath;        // Where ui data is stored


#define DOC_DIR     wxT("/docs")
#define UI_DIR      wxT("/ui")
#define COMMON_DIR  wxT("/common")



IMPLEMENT_APP(pgAdmin3)


// This is for an unresolved external from libpq 7.4
extern "C"
int is_absolute_path()
{ 
    return 0;
}




// The Application!
bool pgAdmin3::OnInit()
{
    // we are here
    loadPath=wxPathOnly(argv[0]);

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


#ifdef __WIN32__

    if (wxDir::Exists(loadPath + UI_DIR))
        uiPath = loadPath + UI_DIR;
    else
        uiPath = loadPath + wxT("/..") UI_DIR;

    if (wxDir::Exists(loadPath + DOC_DIR))
        docPath = loadPath + DOC_DIR;
    else
        docPath = loadPath + wxT("/../..") DOC_DIR;
    
#else

    if (wxDir::Exists(DATA_DIR UI_DIR))
        uiPath = DATA_DIR UI_DIR;
    else
        uiPath = loadPath + UI_DIR;

    if (wxDir::Exists(DATA_DIR DOC_DIR))
        docPath = DATA_DIR DOC_DIR;
    else
        docPath = loadPath + wxT("/..") DOC_DIR;
#endif


    locale.AddCatalogLookupPathPrefix(uiPath);
    
    long langCount=0;
    const wxLanguageInfo *langInfo;
    int langNo=2;       // skipping default, unknown

    while (true)
    {
        langInfo=wxLocale::GetLanguageInfo(langNo);
        if (!langInfo)
            break;

        if (langInfo->CanonicalName == wxT("en_US") || 
            (!langInfo->CanonicalName.IsEmpty() && 
             wxDir::Exists(uiPath + wxT("/") + langInfo->CanonicalName)))
        {
            existingLangs.Add(langNo);
            langCount++;
        }
        langNo++;
    }


    wxLanguage langId = (wxLanguage)settings->Read(wxT("LanguageId"), wxLANGUAGE_UNKNOWN);

    if (langId == wxLANGUAGE_UNKNOWN)
    {
        locale.Init(wxLANGUAGE_DEFAULT);
        locale.AddCatalog(wxT("pgadmin3"));
#ifdef __LINUX__
        {
            wxLogNull noLog;
            locale.AddCatalog(wxT("fileutils"));
        }
#endif

        if (langCount)
        {
            wxString *langNames=new wxString[langCount+1];
            langNames[0] = _("Default");

            for (langNo = 0; langNo < langCount ; langNo++)
            {
                langInfo = wxLocale::GetLanguageInfo(existingLangs.Item(langNo));
                langNames[langNo+1] = wxString(wxGetTranslation(langInfo->Description)) + wxT(" (")
                    + langInfo->CanonicalName + wxT(")");
            }


            langNo = wxGetSingleChoiceIndex(_("Please choose user language:"), _("User language"), 
                langCount+1, langNames);
            if (langNo > 0)
                langId = (wxLanguage)wxLocale::GetLanguageInfo(existingLangs.Item(langNo-1))->Language;

            delete[] langNames;
        }
    }

    if (langId != wxLANGUAGE_UNKNOWN)
    {
        if (locale.Init(langId))
        {
#ifdef __LINUX__
            {
                wxLogNull noLog;
                locale.AddCatalog(wxT("fileutils"));
            }
#endif
            locale.AddCatalog(wxT("pgadmin3"));
            settings->Write(wxT("LanguageId"), (long)langId);
        }
    }

    // Show the splash screen
    frmSplash* winSplash = new frmSplash((wxFrame *)NULL);
    if (!winSplash) 
        wxLogError(_("Couldn't create the splash screen!"));
    else
    {
        SetTopWindow(winSplash);
        winSplash->Show(TRUE);
	    winSplash->Update();
        wxYield();
    }

	
    // Startup the windows sockets if required
#ifdef __WXMSW__
    WSADATA	wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        wxLogFatalError(__("Cannot initialise the networking subsystem!"));   
    }
#endif

    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());
    wxImage::AddHandler(new wxGIFHandler());

    wxFileSystem::AddHandler(new wxZipFSHandler);

    // Setup the XML resources
    wxXmlResource::Get()->InitAllHandlers();


    LoadAllXrc(uiPath + COMMON_DIR);

    // Set some defaults
    SetAppName(APPNAME_L);

#ifndef __WXDEBUG__
    wxYield();
    wxSleep(2);
#endif

    // Create & show the main form
    wxPoint pos(settings->Read(wxT("frmMain/Left"), 50), settings->Read(wxT("frmMain/Top"), 50));
    wxSize size(settings->Read(wxT("frmMain/Width"), 750), settings->Read(wxT("frmMain/Height"), 550));
    CheckOnScreen(pos, size, 300, 200);

    winMain = new frmMain(APPNAME_L, pos, size);

    if (!winMain) 
        wxLogFatalError(__("Couldn't create the main window!"));

    winMain->Show(TRUE);
    SetTopWindow(winMain);
    SetExitOnFrameDelete(TRUE);

    if (winSplash) {
        winSplash->Close();
        delete winSplash;
    }

    // Display a Tip if required.
    extern sysSettings *settings;
    wxCommandEvent evt = wxCommandEvent();
    if (settings->GetShowTipOfTheDay()) winMain->OnTipOfTheDay(evt);

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

