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
#include <wx/file.h>
#include <wx/xrc/xmlres.h>
#include <wx/imagjpeg.h>
#include <wx/imaggif.h>
#include <wx/imagpng.h>
#include <wx/fs_zip.h>
#include "xh_calb.h"
#include "xh_sqlbox.h"


// Windows headers
#ifdef __WXMSW__
  #include <winsock.h>
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
#include "frmMain.h"
#include "frmSplash.h"

// Globals
frmMain *winMain;
wxLog *logger;
sysSettings *settings;
wxArrayInt existingLangs;
wxArrayString existingLangNames;
wxLocale locale;

wxString loadPath;      // Where the program is loaded from
wxString docPath;       // Where docs are stored
wxString uiPath;        // Where ui data is stored
double libpqVersion=0.0;

#define DOC_DIR     wxT("/docs")
#define UI_DIR      wxT("/ui")
#define COMMON_DIR  wxT("/common")
#define LANG_FILE   wxT("pgadmin3.lng")


#ifdef SSL
extern "C"
{
extern const char *SSL_version_str;
}
#endif


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

    // evaluate all working paths
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
    wxLogInfo(wxT("Compiled with ") + wxString::FromAscii(SSL_version_str));
#endif

#if wxCHECK_VERSION(2,5,1)
#ifdef __WXGTK__
	static pgRendererNative *renderer=new pgRendererNative();
	wxRendererNative::Get();
	wxRendererNative::Set(renderer);
#endif
#endif

    locale.AddCatalogLookupPathPrefix(uiPath);
    
    long langCount=0;
    const wxLanguageInfo *langInfo;
    int langNo;

    wxString langfile=FileRead(uiPath + wxT("/") LANG_FILE, 0, 1);

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
                langNames[langNo+1] = wxT("(") + langInfo->CanonicalName + wxT(") ")
                        + existingLangNames.Item(langNo);
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
    wxXmlResource::Get()->AddHandler(new wxCalendarBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new ctlSQLBoxXmlHandler);


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
    wxArrayString files;
    int count=wxDir::GetAllFiles(uiPath+COMMON_DIR, &files, wxT("*.xrc"), wxDIR_FILES);
    if (!count)
        return false;
    int i;

    for (i=0 ; i < count ; i++)
    {
#if 0
        // pixel -> dlgunit translation
        FILE *f=fopen(files.Item(i).ToAscii(), "r+b");
        if (f)
        {
            int len=50000;
            char *buffer=new char[len+1];
            memset(buffer, 0, len);
            len = fread(buffer, 1, len, f);
            buffer[len]=0;
            fseek(f, 0, SEEK_SET);

            char *bp=buffer;

            char *pos, *size, *end;
            int x, y;
            
            do
            {
                pos = strstr(bp, "<pos>");
                size = strstr(bp, "<size>");

                if (pos && (!size || pos < size))
                {
                    pos += 5;
                    fwrite(bp, 1, pos-bp, f);

                    end = strstr(pos, "</pos>");
                    if (end[-1] == 'd')
                        bp = pos;
                    else
                    {
                        sscanf(pos, "%d,%d", &x, &y);
                        x = (x*4)/6;    y = (y*8)/13;
                        fprintf(f, "%d,%dd", x, y);

                        bp = end;
                    }
                }
                else if (size)
                {
                    size += 6;
                    fwrite(bp, 1, size-bp, f);

                    end = strstr(size, "</size>");
                    if (end[-1] == 'd')
                        bp = size;
                    else
                    {
                        sscanf(size, "%d,%d", &x, &y);
                        x = (x*4)/6;    y = (y*8)/13;
                        fprintf(f, "%d,%dd", x, y);
                       bp = end;
                    }
                }
            }
            while (pos || size);

            fwrite(bp, 1, strlen(bp), f);
            fclose(f);
            delete buffer;
        }

#endif

        
        wxLogInfo(wxT("Loading %s"), files.Item(i).c_str());
        wxXmlResource::Get()->Load(files.Item(i));
    }

#endif


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
