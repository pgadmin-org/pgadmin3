//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAdmin3.cpp - The application
//
//////////////////////////////////////////////////////////////////////////



// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/xrc/xmlres.h>
#include <wx/imagjpeg.h>
#include <wx/imaggif.h>
#include <wx/imagpng.h>


// Windows headers
#ifdef __WXMSW__
  #include <winsock.h>
#endif

// Linux headers
#ifdef __LINUX__
#include <signal.h>
#endif

#if wxCHECK_VERSION(2,5,1)
#ifdef __WXGTK__
#include <wx/renderer.h>
#endif
#endif 



// App headers
#include "pgAdmin3.h"
#include "copyright.h"
#include "version.h"
#include "misc.h"
#include "sysLogger.h"
#include "sysSettings.h"
#include "update.h"
#include "frmMain.h"
#include "frmConfig.h"
#include "frmSplash.h"
#include <wx/socket.h>
#include <wx/dir.h>
#include <wx/fs_zip.h>
#include "xh_calb.h"
#include "xh_timespin.h"
#include "xh_sqlbox.h"
#include "xh_ctlcombo.h"

// Globals
frmMain *winMain=0;
wxThread *updateThread=0;

wxLog *logger;
sysSettings *settings;
wxArrayInt existingLangs;
wxArrayString existingLangNames;
wxLocale *locale=0;

wxString loadPath;              // Where the program is loaded from
wxString docPath;               // Where docs are stored
wxString uiPath;                // Where ui data is stored
wxString backupExecutable;      // complete filename of pg_dump and pg_restore, if available
wxString restoreExecutable;

wxString slony1BaseScript;
wxString slony1FunctionScript;
wxString slony1XxidScript;

double libpqVersion=0.0;

bool dialogTestMode=false;

#define DOC_DIR     wxT("/docs")
#define UI_DIR      wxT("/ui")
#define COMMON_DIR  wxT("/common")
#define LANG_FILE   wxT("pgadmin3.lng")


IMPLEMENT_APP(pgAdmin3)


#if wxCHECK_VERSION(2,5,1)
#ifdef __WXGTK__

class pgRendererNative : public wxDelegateRendererNative
{
public:
	void DrawTreeItemButton(wxWindow* win,wxDC& dc, const wxRect& rect, int flags)
	{
		GetGeneric().DrawTreeItemButton(win, dc, rect, flags);
	}
};

#endif
#endif



// The Application!
bool pgAdmin3::OnInit()
{
    // we are here
    loadPath=wxPathOnly(argv[0]);
	if (loadPath.IsEmpty())
		loadPath = wxT(".");

    frmConfig::tryMode configMode=frmConfig::NONE;

    if (argc > 1 && *argv[1] == '-')
    {
        switch (argv[1][1])
        {
            case 'c':
            {
                // file configurator mode
                if (argv[1][2]== 'm')
                    configMode = frmConfig::MAINFILE;
                else if (argv[1][2]== 'h')
                    configMode = frmConfig::HBAFILE;
                else
                    configMode=frmConfig::ANYFILE;

                break;
            }
            case 't':
            {
                dialogTestMode = true;
                break;
            }
        }
    }


    wxPathList path;

    path.Add(loadPath);

#ifdef __WXMSW__

	// Look for a path 'hint' on Windows. This registry setting may
	// be set by the Win32 PostgreSQL installer which will generally
	// install pg_dump et al. in the PostgreSQL bindir rather than
	// the pgAdmin directory.

    wxRegKey hintKey(wxT("HKEY_LOCAL_MACHINE\\Software\\") APPNAME_L);

	if (hintKey.HasValue(wxT("Helper Path")))
	{
		wxString hintPath;
	    hintKey.QueryValue(wxT("Helper Path"), hintPath);
		path.Add(hintPath);
	}

#endif 

    path.AddEnvList(wxT("PATH"));

    // evaluate all working paths

#ifdef __WXMSW__

    backupExecutable  = path.FindValidPath(wxT("pg_dump.exe"));
    restoreExecutable = path.FindValidPath(wxT("pg_restore.exe"));

    if (wxDir::Exists(loadPath + UI_DIR))
        uiPath = loadPath + UI_DIR;
    else
        uiPath = loadPath + wxT("/..") UI_DIR;

    if (wxDir::Exists(loadPath + DOC_DIR))
        docPath = loadPath + DOC_DIR;
    else
        docPath = loadPath + wxT("/../..") DOC_DIR;
    
#else

    backupExecutable  = path.FindValidPath(wxT("pg_dump"));
    restoreExecutable = path.FindValidPath(wxT("pg_restore"));

    if (wxDir::Exists(DATA_DIR UI_DIR))
        uiPath = DATA_DIR UI_DIR;
    else
        uiPath = loadPath + UI_DIR;

    if (wxDir::Exists(DATA_DIR DOC_DIR))
        docPath = DATA_DIR DOC_DIR;
    else
        docPath = loadPath + wxT("/..") DOC_DIR;
#endif


    slony1BaseScript=path.FindValidPath(wxT("slony1_base.sql"));
    slony1FunctionScript=path.FindValidPath(wxT("slony1_funcs.sql"));
    slony1XxidScript=path.FindValidPath(wxT("slony1_xxid.sql"));


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
    msg << wxT("# ") << APPNAME_L << wxT(" Version ") << VERSION_STR << wxT(" Startup");
    wxLogInfo(wxT("##############################################################"));
    wxLogInfo(msg);
    wxLogInfo(wxT("##############################################################"));

#if wxCHECK_VERSION(2,5,0)
    // that's what we expect
#else
    wxLogInfo(wxT("Not compiled against wxWindows 2.5 or above: using ") wxVERSION_STRING);
#endif

#ifdef SSL
    wxLogInfo(wxT("Compiled with dynamically linked SSL support"));
#endif

#if wxCHECK_VERSION(2,5,1)
#ifdef __WXGTK__
	static pgRendererNative *renderer=new pgRendererNative();
	wxRendererNative::Get();
	wxRendererNative::Set(renderer);
#endif
#endif

#ifdef __LINUX__
	signal(SIGPIPE, SIG_IGN);
#endif

    locale = new wxLocale();
    locale->AddCatalogLookupPathPrefix(uiPath);

    wxLanguage langId = (wxLanguage)settings->Read(wxT("LanguageId"), wxLANGUAGE_DEFAULT);
    if (locale->Init(langId))
    {
#ifdef __LINUX__
        {
            wxLogNull noLog;
            locale->AddCatalog(wxT("fileutils"));
        }
#endif
        locale->AddCatalog(wxT("pgadmin3"));
    }


    long langCount=0;
    const wxLanguageInfo *langInfo;
    int langNo;

    wxString langfile=FileRead(uiPath + wxT("/") LANG_FILE, 1);

    if (!langfile.IsEmpty())
    {
        wxStringTokenizer tk(langfile, wxT("\n\r"));

        while (tk.HasMoreTokens())
        {
            wxString line=tk.GetNextToken().Strip(wxString::both);
            if (line.IsEmpty() || line.StartsWith(wxT("#")))
                continue;

            wxString englishName=line.BeforeFirst(',').Trim(true);
            wxString translatedName=line.AfterFirst(',').Trim(false);

            langNo=2;       // skipping default, unknown

            while (true)
            {
                langInfo=wxLocale::GetLanguageInfo(langNo);
                if (!langInfo)
                    break;

                if (englishName == langInfo->Description && 
                    (langInfo->CanonicalName == wxT("en_US") || 
                    (!langInfo->CanonicalName.IsEmpty() && 
                     wxDir::Exists(uiPath + wxT("/") + langInfo->CanonicalName))))
                {
                    existingLangs.Add(langNo);
                    existingLangNames.Add(translatedName);
                    langCount++;
                }
                langNo++;
            }
        }
    }


    // Show the splash screen
    frmSplash* winSplash = new frmSplash((wxFrame *)NULL);
    if (!winSplash) 
        wxLogError(__("Couldn't create the splash screen!"));
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
    wxXmlResource::Get()->AddHandler(new wxCalendarBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxTimeSpinXmlHandler);
    wxXmlResource::Get()->AddHandler(new ctlSQLBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new ctlComboBoxXmlHandler);

#define chkXRC(id) XRCID(#id) == id
    wxASSERT_MSG(
        chkXRC(wxID_OK) &&
        chkXRC(wxID_CANCEL) && 
        chkXRC(wxID_HELP) &&
        chkXRC(wxID_APPLY) &&
        chkXRC(wxID_ADD) &&
        chkXRC(wxID_STOP) &&
        chkXRC(wxID_REMOVE)&&
        chkXRC(wxID_REFRESH) &&
        chkXRC(wxID_CLOSE), 
        wxT("XRC ID not correctly assigned."));
    // if this assert fires, some event table uses XRCID(...) instead of wxID_... directly
        

    // examine libpq version
    libpqVersion=7.3;
    PQconninfoOption *cio=PQconndefaults();

    if (cio)
    {
        PQconninfoOption *co=cio;
        while (co->keyword)
        {
            if (!strcmp(co->keyword, "sslmode"))
            {
                libpqVersion=7.4;
                break;
            }
            co++;
        }
        PQconninfoFree(cio);
    }


#ifdef EMBED_XRC

    // resources are loaded from memory
    extern void InitXmlResource();
    InitXmlResource();

#else

    // for debugging, dialog resources are read from file
    wxXmlResource::Get()->Load(uiPath+COMMON_DIR + wxT("/*.xrc"));
#endif


    // Set some defaults
    SetAppName(APPNAME_L);

#ifndef __WXDEBUG__
    wxYield();
    wxSleep(2);
#endif


    if (configMode)
    {
        int i;

        for (i=2 ; i < argc ; i++)
        {
            wxString str;
            if (*argv[i] == '"')
            {
                wxString str=argv[i]+1;
                str=str.Mid(0, str.Length()-1);
            }
            else
                str = argv[i];

            if (configMode == frmConfig::ANYFILE && wxDir::Exists(str))
            {
                frmConfig::Create(APPNAME_L, str + wxT("/pg_hba.conf"), frmConfig::HBAFILE);
                frmConfig::Create(APPNAME_L, str + wxT("/postgresql.conf"), frmConfig::MAINFILE);
            }
            else
            {
                frmConfig::Create(APPNAME_L, str, configMode);
            }
        }
        if (winSplash)
        {
            winSplash->Close();
            delete winSplash;
        }
    }

    else
    {
        wxSocketBase::Initialize();

        // Create & show the main form
        winMain = new frmMain(APPNAME_L);

        if (!winMain) 
            wxLogFatalError(__("Couldn't create the main window!"));

        // updateThread = BackgroundCheckUpdates(winMain);

        winMain->Show(TRUE);
        SetTopWindow(winMain);
        SetExitOnFrameDelete(TRUE);

        if (winSplash)
        {
            winSplash->Close();
            delete winSplash;
        }

        // Display a Tip if required.
        extern sysSettings *settings;
        wxCommandEvent evt = wxCommandEvent();
        if (settings->GetShowTipOfTheDay()) winMain->OnTipOfTheDay(evt);
    }

    return TRUE;
}

// Not the Application!
int pgAdmin3::OnExit()
{
    if (updateThread)
    {
        updateThread->Delete();
        delete updateThread;
    }

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
