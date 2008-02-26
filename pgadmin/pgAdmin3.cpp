//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAdmin3.cpp - The application
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/xrc/xmlres.h>
#include <wx/image.h>
#include <wx/imagjpeg.h>
#include <wx/imaggif.h>
#include <wx/imagpng.h>
#include <wx/dir.h>
#include <wx/fs_zip.h>
#include <wx/ogl/ogl.h>
#include <wx/socket.h>
#include <wx/stdpaths.h>
#include <wx/clipbrd.h>
#include <wx/sysopt.h>

// Windows headers
#ifdef __WXMSW__
  #include <winsock.h>
#endif

// Linux headers
#ifdef __LINUX__
#include <signal.h>
#endif

#ifdef __WXGTK__
#include <wx/renderer.h>
#endif

// App headers
#include "copyright.h"
#include "version.h"
#include "utils/misc.h"
#include "utils/sysSettings.h"
#include "schema/pgServer.h"
#include "frm/frmMain.h"
#include "frm/frmConfig.h"
#include "frm/frmQuery.h"
#include "frm/frmSplash.h"
#include "dlg/dlgSelectConnection.h"
#include "db/pgConn.h"
#include "utils/sysLogger.h"
#include "frm/frmHint.h"

#include "ctl/xh_calb.h"
#include "ctl/xh_timespin.h"
#include "ctl/xh_sqlbox.h"
#include "ctl/xh_ctlcombo.h"
#include "ctl/xh_ctltree.h"

#define DOC_DIR      wxT("/docs")
#define UI_DIR       wxT("/ui")
#define I18N_DIR     wxT("/i18n")
#define BRANDING_DIR wxT("/branding")

// Globals
frmMain *winMain=0;
wxThread *updateThread=0;

sysSettings *settings;
wxArrayInt existingLangs;
wxArrayString existingLangNames;
wxLocale *locale=0;
pgAppearanceFactory *appearanceFactory=0;

wxString pgBackupExecutable;       // complete filename of PostgreSQL's pg_dump, pg_dumpall and pg_restore, if available
wxString pgBackupAllExecutable;
wxString pgRestoreExecutable;

wxString edbBackupExecutable;      // complete filename of EnterpriseDB's pg_dump, pg_dumpall and pg_restore, if available
wxString edbBackupAllExecutable;
wxString edbRestoreExecutable;

wxString loadPath;              // Where the program is loaded from
wxString docPath;               // Where docs are stored
wxString uiPath;                // Where ui data is stored
wxString i18nPath;              // Where i18n data is stored
wxString brandingPath;			// Where branding data is stored
wxLog *logger;

bool dialogTestMode=false;

#define LANG_FILE   wxT("pgadmin3.lng")

IMPLEMENT_APP(pgAdmin3)

#ifdef __WXGTK__

class pgRendererNative : public wxDelegateRendererNative
{
public:
	pgRendererNative() : wxDelegateRendererNative(wxRendererNative::GetDefault()) {}

	void DrawTreeItemButton(wxWindow* win,wxDC& dc, const wxRect& rect, int flags)
	{
		GetGeneric().DrawTreeItemButton(win, dc, rect, flags);
	}
};

#endif
#define CTL_LB  100
class frmDlgTest : public wxFrame
{
public:
    frmDlgTest();
private:
    void OnSelect(wxCommandEvent &ev);
    wxListBox *dlgList;
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(frmDlgTest, wxFrame)
    EVT_LISTBOX_DCLICK(CTL_LB, frmDlgTest::OnSelect)
END_EVENT_TABLE();


frmDlgTest::frmDlgTest() : wxFrame(0, -1, wxT("pgAdmin III Translation test mode"))
{
    dlgList=new wxListBox(this, CTL_LB);

    // unfortunately, the MemoryFS has no search functions implemented 
    // so we can't extract the names in the EMBED_XRC case

    wxDir dir(uiPath);
    wxString filename;

    bool found=dir.GetFirst(&filename, wxT("*.xrc"));
    while (found)
    {
        dlgList->Append(filename.Left(filename.Length()-4));
        found = dir.GetNext(&filename);
    }
    if (!dlgList->GetCount())
    {
        dlgList->Append(wxT("No xrc files in directory"));
        dlgList->Append(uiPath);
        dlgList->Disable();
    }
}


void frmDlgTest::OnSelect(wxCommandEvent &ev)
{
    wxString dlgName=dlgList->GetStringSelection();
    if (!dlgName.IsEmpty())
    {
        pgDialog *dlg=new pgDialog;
        dlg->wxWindowBase::SetFont(settings->GetSystemFont());
        dlg->LoadResource(this, dlgName); 
        dlg->SetTitle(dlgName);
        dlg->Show();
    }
}


// The Application!
bool pgAdmin3::OnInit()
{
    // Setup wxWidgets runtime options
#ifdef __WXMAC__
    wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
#endif

    // Force logging off until we're ready
    wxLog *seLog=new wxLogStderr();
    wxLog::SetActiveTarget(seLog);
  	
	static const wxCmdLineEntryDesc cmdLineDesc[] = 
	{
		{wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{wxCMD_LINE_OPTION, wxT("s"), wxT("server"), _("auto-connect to specified server"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_SWITCH, wxT("q"), wxT("query"), _("open query tool"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, wxT("qc"), wxT("queryconnect"), _("connect query tool to database"), wxCMD_LINE_VAL_STRING},
        {wxCMD_LINE_OPTION, wxT("f"), wxT("file"), _("file to load into the query tool in -q or -qc mode"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_OPTION, wxT("cm"), NULL, _("edit main configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, wxT("ch"), NULL, _("edit HBA configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
        {wxCMD_LINE_OPTION, wxT("cp"), NULL, _("edit pgpass configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, wxT("c"), NULL, _("edit configuration files in cluster directory"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_SWITCH, wxT("t"), NULL, _("dialog translation test mode"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_NONE}
	};

    // Load the Settings
#ifdef __WXMSW__
    settings = new sysSettings(wxT("pgAdmin III"));
#else
    settings = new sysSettings(wxT("pgadmin3"));
#endif

    // we are here
    InitPaths();

    frmConfig::tryMode configMode=frmConfig::NONE;
	wxString configFile;

	wxCmdLineParser cmdParser(cmdLineDesc, argc, argv);
	if (cmdParser.Parse() != 0) 
		return false;

	if (cmdParser.Found(wxT("q")) && cmdParser.Found(wxT("qc")))
	{
		cmdParser.Usage();
		return false;
	}

	if (cmdParser.Found(wxT("cm"), &configFile)) 
		configMode = frmConfig::MAINFILE;
	else if (cmdParser.Found(wxT("ch"), &configFile))
		configMode = frmConfig::HBAFILE;
	else if (cmdParser.Found(wxT("cp"), &configFile))
		configMode = frmConfig::PGPASSFILE;
	else if (cmdParser.Found(wxT("c"), &configFile))
		configMode = frmConfig::ANYFILE;

	if (cmdParser.Found(wxT("t")))
		dialogTestMode = true;

	// Setup the image handlers and appearance factory before we do any GUI or config stuff
    wxImage::AddHandler(new wxJPEGHandler());
    wxImage::AddHandler(new wxPNGHandler());
    wxImage::AddHandler(new wxGIFHandler());

	appearanceFactory = new pgAppearanceFactory();

	// Setup logging
    InitLogger();

    wxString msg;
    msg << wxT("# ") << appearanceFactory->GetLongAppName() << wxT(" Version ") << VERSION_STR << wxT(" Startup");
    wxLogInfo(wxT("##############################################################"));
    wxLogInfo(wxT("%s"), msg.c_str());
    wxLogInfo(wxT("##############################################################"));

#ifdef SSL
    wxLogInfo(wxT("Compiled with dynamically linked SSL support"));
#endif

#ifdef __WXDEBUG__
    wxLogInfo(wxT("Running a DEBUG build."));
#else
    wxLogInfo(wxT("Running a RELEASE build."));
#endif

    // Log the path info
    wxLogInfo(wxT("i18n path    : %s"), i18nPath.c_str());
    wxLogInfo(wxT("UI path      : %s"), uiPath.c_str());
    wxLogInfo(wxT("Doc path     : %s"), docPath.c_str());
    wxLogInfo(wxT("Branding path: %s"), brandingPath.c_str());

    wxLogInfo(wxT("PG pg_dump    : %s"), pgBackupExecutable.c_str());
    wxLogInfo(wxT("PG pg_dumpall : %s"), pgBackupAllExecutable.c_str());
    wxLogInfo(wxT("PG pg_restore : %s"), pgRestoreExecutable.c_str());

    wxLogInfo(wxT("EDB pg_dump   : %s"), edbBackupExecutable.c_str());
    wxLogInfo(wxT("EDB pg_dumpall: %s"), edbBackupAllExecutable.c_str());
    wxLogInfo(wxT("EDB pg_restore: %s"), edbRestoreExecutable.c_str());

#ifdef __WXGTK__
	static pgRendererNative *renderer=new pgRendererNative();
	wxRendererNative::Get();
	wxRendererNative::Set(renderer);
#endif

#ifdef __LINUX__
	signal(SIGPIPE, SIG_IGN);
#endif

    locale = new wxLocale();
    locale->AddCatalogLookupPathPrefix(i18nPath);

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

    wxString langfile=FileRead(i18nPath + wxT("/") LANG_FILE, 1);

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

            langInfo=wxLocale::FindLanguageInfo(englishName);
            if (langInfo)
            {
                if (langInfo->CanonicalName == wxT("en_US") || 
                    (!langInfo->CanonicalName.IsEmpty() && 
                     wxDir::Exists(i18nPath + wxT("/") + langInfo->CanonicalName)))
                {
                    existingLangs.Add(langInfo->Language);
                    existingLangNames.Add(translatedName);
                    langCount++;
                }
            }
        }
    }

    // Show the splash screen
    // NOTE: We must *always* do this as in -q and -qc modes
    //       the splash screen becomes the top level window and
    //       allows the logon dialogs to be displayed!!
    frmSplash* winSplash = new frmSplash((wxFrame *)NULL);
    if (!winSplash) 
        wxLogError(__("Couldn't create the splash screen!"));
    else
    {
        SetTopWindow(winSplash);
        winSplash->Show();
	    winSplash->Update();
        wxTheApp->Yield(true);
    }
	
    // Startup the windows sockets if required
    InitNetwork();

    wxFileSystem::AddHandler(new wxZipFSHandler);

    // Setup the XML resources
    wxXmlResource::Get()->InitAllHandlers();
    wxXmlResource::Get()->AddHandler(new wxCalendarBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxTimeSpinXmlHandler);
    wxXmlResource::Get()->AddHandler(new ctlSQLBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new ctlComboBoxXmlHandler);
    wxXmlResource::Get()->AddHandler(new ctlTreeXmlHandler);

    InitXml();

    wxOGLInitialize();

    // Set some defaults
    SetAppName(appearanceFactory->GetLongAppName());

    // Setup the help paths
    InitHelp();
    wxLogInfo(wxT("PG Help       : %s"), settings->GetPgHelpPath().c_str());
    wxLogInfo(wxT("EDB Help      : %s"), settings->GetEdbHelpPath().c_str());
    wxLogInfo(wxT("Slony Help    : %s"), settings->GetSlonyHelpPath().c_str());

#ifndef __WXDEBUG__
    wxTheApp->Yield(true);
    wxSleep(2);
#endif

    if (configMode)
    {
		if (configMode == frmConfig::ANYFILE && wxDir::Exists(configFile))
		{
		    wxLogInfo(wxT("Starting in ANYFILE config editor mode, in directory: %s"), configFile.c_str());
			frmConfig::Create(appearanceFactory->GetLongAppName(), configFile + wxT("/pg_hba.conf"), frmConfig::HBAFILE);
			frmConfig::Create(appearanceFactory->GetLongAppName(), configFile + wxT("/postgresql.conf"), frmConfig::MAINFILE);
		}
		else
		{
		    wxLogInfo(wxT("Starting in config editor mode, file: %s"), configFile.c_str());
			frmConfig::Create(appearanceFactory->GetLongAppName(), configFile, configMode);
		}
        if (winSplash)
        {
            winSplash->Close();
            delete winSplash;
            winSplash = 0;
        }
    }

    else
    {
        if (dialogTestMode)
        {
		    wxLogInfo(wxT("Starting in dialog test mode."));
            wxFrame *dtf=new frmDlgTest();
            dtf->Show();
            SetTopWindow(dtf);
        }

#ifdef __WXMAC__
        else if (((cmdParser.Found(wxT("q")) || cmdParser.Found(wxT("qc"))) && !cmdParser.Found(wxT("s"))) || !macFileToOpen.IsEmpty())
#else		
        else if ((cmdParser.Found(wxT("q")) || cmdParser.Found(wxT("qc"))) && !cmdParser.Found(wxT("s")))
#endif
        {
			// -q specified, but not -s. Open a query tool but do *not* open the main window
			pgConn *conn = NULL;
			wxString connstr;

#ifdef __WXMAC__
			if (cmdParser.Found(wxT("q")) || !macFileToOpen.IsEmpty())
#else
			if (cmdParser.Found(wxT("q")))
#endif
			{
			    wxLogInfo(wxT("Starting in query tool mode (-q)."), configFile.c_str());

				winSplash->Show(false);
				dlgSelectConnection dlg(NULL, NULL);
				dlg.CenterOnParent();
				
		        int rc=dlg.Go(conn, NULL);
				if (rc != wxID_OK)
					return false;
				conn = dlg.CreateConn();
			}
			else if (cmdParser.Found(wxT("qc"), &connstr))
			{
			    wxLogInfo(wxT("Starting in query tool connect mode (-qc)."), configFile.c_str());
				wxString host, database, username, tmps;
				int sslmode=0,port=0;
				wxStringTokenizer tkn(connstr, wxT(" "), wxTOKEN_STRTOK);
				while (tkn.HasMoreTokens())
				{
					wxString str = tkn.GetNextToken();
					if (str.StartsWith(wxT("hostaddr="), &host))
						continue;
					if (str.StartsWith(wxT("host="), &host))
						continue;
					if (str.StartsWith(wxT("dbname="), &database))
						continue;
					if (str.StartsWith(wxT("user="), &username))
						continue;
					if (str.StartsWith(wxT("port="), &tmps))
					{
						port = StrToLong(tmps);
						continue;
					}
					if (str.StartsWith(wxT("sslmode="), &tmps))
					{
						if (!tmps.Cmp(wxT("require")))
							sslmode = 1;
						else if (!tmps.Cmp(wxT("prefer")))
							sslmode = 2;
						else if (!tmps.Cmp(wxT("allow")))
							sslmode = 3;
						else if (!tmps.Cmp(wxT("disable")))
							sslmode = 4;
						else
						{
							wxMessageBox(_("Unknown SSL mode: ") + tmps);
							return false;
						}
						continue;
					}
					wxMessageBox(_("Unknown token in connection string: ") + str);
					return false;
				}
				winSplash->Show(false);
				dlgSelectConnection dlg(NULL, NULL);
				dlg.CenterOnParent();
				conn = dlg.CreateConn(host, database, username, port, sslmode);
			}
			else
			{
				/* Can't happen.. */
				return false;
			}
			if (!conn)
				return false;

            wxString fn;
#ifdef __WXMAC__
            if (!macFileToOpen.IsEmpty())
			{
			    wxLogInfo(wxT("Mac file launch: %s."), macFileToOpen.c_str());
			    fn = macFileToOpen;
			}
			else
			    cmdParser.Found(wxT("f"), &fn);
#else
            cmdParser.Found(wxT("f"), &fn);
#endif
			if (!fn.IsEmpty())
			    wxLogInfo(wxT("Auto-loading file: %s"), fn.c_str());
			frmQuery *fq = new frmQuery(NULL, wxEmptyString, conn, wxEmptyString, fn);
			fq->Go();
		}
		else
		{
            // Create & show the main form
            winMain = new frmMain(appearanceFactory->GetLongAppName());

            if (!winMain) 
                wxLogFatalError(__("Couldn't create the main window!"));

            winMain->Show();
            SetTopWindow(winMain);

	        // Display a Tip if required.
			extern sysSettings *settings;
			wxCommandEvent evt = wxCommandEvent();
			if (winMain && settings->GetShowTipOfTheDay())
			{
                if (winSplash)
                {
                    winSplash->Close();
                    delete winSplash;
                    winSplash = 0;
                }
				tipOfDayFactory tip(0, 0, 0);
				tip.StartDialog(winMain, 0);
			}

			wxString str;
			if (cmdParser.Found(wxT("s"), &str))
			{
				pgServer *srv = winMain->ConnectToServer(str, !cmdParser.Found(wxT("q")));
				if (srv && cmdParser.Found(wxT("q")))
				{
					pgConn *conn;
					conn = srv->CreateConn();
					if (conn)
					{
                        wxString fn;
                        cmdParser.Found(wxT("f"), &fn);
						if (!fn.IsEmpty())
						    wxLogInfo(wxT("Auto-loading file: %s"), fn.c_str());
						frmQuery *fq = new frmQuery(winMain, wxEmptyString, conn, wxEmptyString, fn);
						fq->Go();
					}
				}
			}
		}

        SetExitOnFrameDelete(true);

        if (winSplash)
        {
            winSplash->Close();
            delete winSplash;
            winSplash = 0;
        }
    }

    return true;
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

    wxTheClipboard->Flush();

    return 1;
}

// On the Mac, this function is called before Init, if the finder launches the app
// via a registered filetype. Grab the filename here, and test for it in Init.
#ifdef __WXMAC__
void pgAdmin3::MacOpenFile(const wxString &fileName) 
{
	macFileToOpen = fileName;
}
#endif

void pgAdmin3::InitPaths()
{
    loadPath=wxPathOnly(argv[0]);
	if (loadPath.IsEmpty())
		loadPath = wxT(".");

#ifdef __WXMSW__

    // Search for the right paths. We check the following locations:
    //
    // 1) ./xxx               - Running as a standalone install
    // 2) ../pgAdmin/xxx      - Running in a pgInstaller 8.1 installation 
    //                          (with the .exe and dlls in the main bin dir)
    // 3) ../../xxx or ../xxx - Running in a development environment
    
    if (wxDir::Exists(loadPath + I18N_DIR))
        i18nPath = loadPath + I18N_DIR;
    else if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") + I18N_DIR))
        i18nPath = loadPath + wxT("/../pgAdmin III") + I18N_DIR;
    else 
        i18nPath = loadPath + wxT("/../..") + I18N_DIR;

    if (wxDir::Exists(loadPath + DOC_DIR))
        docPath = loadPath + DOC_DIR;
    else if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") DOC_DIR))
        docPath = loadPath + wxT("/../pgAdmin III") DOC_DIR;
    else
        docPath = loadPath + wxT("/../..") DOC_DIR;

    if (wxDir::Exists(loadPath + UI_DIR))
        uiPath = loadPath + UI_DIR;
    if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") + UI_DIR))
        uiPath = loadPath + wxT("/../pgAdmin III") + UI_DIR;
    else
        uiPath = loadPath + wxT("/..") UI_DIR;

    if (wxDir::Exists(loadPath + BRANDING_DIR))
        brandingPath = loadPath + BRANDING_DIR;
    else if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") + BRANDING_DIR))
        brandingPath = loadPath + wxT("/../pgAdmin III") + BRANDING_DIR;
    else 
        brandingPath = loadPath + wxT("/../..") + BRANDING_DIR;
#else

    wxString dataDir;

#ifdef __WXMAC__

    // When using wxStandardPaths on OSX, wx default to the unix,
    // not to the mac variants. Therefore, we request wxStandardPathsCF
    // directly.
    wxStandardPathsCF stdPaths ;
    dataDir = stdPaths.GetDataDir() ;

#else // other *ixes

// Data path (defined by configure under Unix).
#ifndef DATA_DIR
#define DATA_DIR "./"
#endif

    dataDir = wxString::FromAscii(DATA_DIR);
#endif
  
    // On unix systems, the search path is as follows:
    //
    // 1) DATADIR/xxx              - DATADIR being defined by configure
    // 2) ./../share/pgadmin3/xxx  - The default 'make install' layout, but allowing for relocation
    // 3) ./xxx                    - Windows-style standalone install
    // 4) ./../xxx                 - Unix-style standalone install (with binaries in a bin directory)

    if (wxDir::Exists(dataDir + I18N_DIR))
        i18nPath = dataDir + I18N_DIR;
    
    if (wxDir::Exists(dataDir + UI_DIR))
        uiPath = dataDir + UI_DIR;

    if (wxDir::Exists(dataDir + DOC_DIR))
        docPath = dataDir + DOC_DIR ;

    if (wxDir::Exists(dataDir + BRANDING_DIR))
        brandingPath = dataDir + BRANDING_DIR ;

    if (i18nPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + wxT("/../share/pgadmin3") I18N_DIR))
            i18nPath = loadPath + wxT("/../share/pgadmin3") I18N_DIR;
        else if (wxDir::Exists(loadPath + I18N_DIR))
            i18nPath = loadPath + I18N_DIR;
        else
            i18nPath = loadPath + wxT("/..") I18N_DIR;
    }
    if (uiPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + wxT("/../share/pgadmin3") UI_DIR))
            uiPath = loadPath + wxT("/../share/pgadmin3") UI_DIR;
        else if (wxDir::Exists(loadPath + UI_DIR))
            uiPath = loadPath + UI_DIR;
        else 
            uiPath = loadPath + wxT("/..") UI_DIR;
    }
    if (docPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + wxT("/../share/pgadmin3") DOC_DIR))
            docPath = loadPath + wxT("/../share/pgadmin3") DOC_DIR;
        else if (wxDir::Exists(loadPath + DOC_DIR))
            docPath = loadPath + DOC_DIR ;
        else
            docPath = loadPath + wxT("/..") DOC_DIR ;
    }
    if (brandingPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + wxT("/../share/pgadmin3") BRANDING_DIR))
            brandingPath = loadPath + wxT("/../share/pgadmin3") BRANDING_DIR;
        else if (wxDir::Exists(loadPath + BRANDING_DIR))
            brandingPath = loadPath + BRANDING_DIR ;
        else
            brandingPath = loadPath + wxT("/..") BRANDING_DIR ;
    }
#endif

    //////////////////////////////////
    // Now setup the app helper paths
    //////////////////////////////////

    // First, check and invalidate the paths if they're no good.
#ifdef __WXMSW__
    if (!isPgApp(settings->GetPostgresqlPath() + wxT("\\pg_dump.exe")))
        settings->SetPostgresqlPath(wxEmptyString);

    if (!isEdbApp(settings->GetEnterprisedbPath() + wxT("\\pg_dump.exe")))
        settings->SetEnterprisedbPath(wxEmptyString);
#else
    if (!isPgApp(settings->GetPostgresqlPath() + wxT("/pg_dump")))
        settings->SetPostgresqlPath(wxEmptyString);

    if (!isEdbApp(settings->GetEnterprisedbPath() + wxT("/pg_dump")))
        settings->SetEnterprisedbPath(wxEmptyString);
#endif

    // Now, if either path is empty, start a search for helpers
    // If we find apps, record the appropriate path *only* if it's 
    // not already set
    if (settings->GetPostgresqlPath().IsEmpty() || settings->GetEnterprisedbPath().IsEmpty())
    {
        wxPathList path;

        // Look in the app directory for things first
        path.Add(loadPath);

#ifdef __WXMSW__

        // Look for a path 'hint' on Windows. This registry setting may
        // be set by the Win32 PostgreSQL installer which will generally
        // install pg_dump et al. in the PostgreSQL bindir rather than
        // the pgAdmin directory.
        wxRegKey hintKey(wxT("HKEY_LOCAL_MACHINE\\Software\\pgAdmin III"));
        if (hintKey.HasValue(wxT("Helper Path")))
        {
            wxString hintPath;
            hintKey.QueryValue(wxT("Helper Path"), hintPath);
            path.Add(hintPath);
        }

#else
#ifdef __WXMAC__

            if (wxDir::Exists(dataDir))
            path.Add(dataDir) ;

#endif
#endif

        path.AddEnvList(wxT("PATH"));

#ifdef __WXMSW__
        wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
        wxFileName tmp = path.FindValidPath(wxT("pg_dump"));
#endif

        if (tmp.FileExists())
        {
            if (isPgApp(tmp.GetFullPath()) && settings->GetPostgresqlPath().IsEmpty())
                settings->SetPostgresqlPath(tmp.GetPath());
            else if (isEdbApp(tmp.GetFullPath()) && settings->GetEnterprisedbPath().IsEmpty())
                settings->SetEnterprisedbPath(tmp.GetPath());
        }
    }

    // Now, if either path is still empty, try a less intelligent search for each

    // PostgreSQL
    if (settings->GetPostgresqlPath().IsEmpty())
    {
        wxPathList path;

#ifdef __WXMSW__
        path.Add(wxT("C:\\Program Files\\PostgreSQL\\8.3\\bin"));
        path.Add(wxT("C:\\Program Files\\PostgreSQL\\8.2\\bin"));
        path.Add(wxT("C:\\Program Files\\PostgreSQL\\8.1\\bin"));
        path.Add(wxT("C:\\Program Files\\PostgreSQL\\8.0\\bin"));

        wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
        path.Add(wxT("/usr/local/pgsql/bin"));
        path.Add(wxT("/usr/local/bin"));
        path.Add(wxT("/usr/bin"));
        path.Add(wxT("/opt/local/pgsql/bin"));
        path.Add(wxT("/opt/local/bin"));
        path.Add(wxT("/opt/bin"));

        wxFileName tmp = path.FindValidPath(wxT("pg_dump"));
#endif 

        if (tmp.FileExists())
        {
            if (isPgApp(tmp.GetFullPath()))
                settings->SetPostgresqlPath(tmp.GetPath());
        }
    }

    // EnterpriseDB
    if (settings->GetEnterprisedbPath().IsEmpty())
    {
        wxPathList path;

#ifdef __WXMSW__
        path.Add(wxT("C:\\EnterpriseDB\\8.3\\dbserver\\bin"));
        path.Add(wxT("C:\\EnterpriseDB\\8.2\\dbserver\\bin"));
        path.Add(wxT("C:\\EnterpriseDB\\8.1\\dbserver\\bin"));
        path.Add(wxT("C:\\EnterpriseDB\\8.0\\dbserver\\bin"));

        wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
        path.Add(wxT("/opt/EnterpriseDB/8.3/dbserver/bin"));
        path.Add(wxT("/usr/local/enterpriseDB/bin"));
        path.Add(wxT("/usr/local/enterprisedb/bin"));
        path.Add(wxT("/usr/local/edb/bin"));
        path.Add(wxT("/usr/local/bin"));
        path.Add(wxT("/usr/bin"));
        path.Add(wxT("/opt/local/enterpriseDB/bin"));
        path.Add(wxT("/opt/local/enterprisedb/bin"));
        path.Add(wxT("/opt/local/edb/bin"));
        path.Add(wxT("/opt/local/bin"));

        wxFileName tmp = path.FindValidPath(wxT("pg_dump"));
#endif 

        if (tmp.FileExists())
        {
            if (isEdbApp(tmp.GetFullPath()))
                settings->SetEnterprisedbPath(tmp.GetPath());
        }
    }

    // Now setup and verify the paths for each individual helper
#if defined(__WXMSW__)
    pgBackupExecutable  = settings->GetPostgresqlPath() + wxT("\\pg_dump.exe");
    pgBackupAllExecutable  = settings->GetPostgresqlPath() + wxT("\\pg_dumpall.exe");
    pgRestoreExecutable = settings->GetPostgresqlPath() + wxT("\\pg_restore.exe");

    edbBackupExecutable  = settings->GetEnterprisedbPath() + wxT("\\pg_dump.exe");
    edbBackupAllExecutable  = settings->GetEnterprisedbPath() + wxT("\\pg_dumpall.exe");
    edbRestoreExecutable = settings->GetEnterprisedbPath() + wxT("\\pg_restore.exe");
#else
    pgBackupExecutable  = settings->GetPostgresqlPath() + wxT("/pg_dump");
	pgBackupAllExecutable  = settings->GetPostgresqlPath() + wxT("/pg_dumpall");
    pgRestoreExecutable = settings->GetPostgresqlPath() + wxT("/pg_restore");

    edbBackupExecutable  = settings->GetEnterprisedbPath() + wxT("/pg_dump");
	edbBackupAllExecutable  = settings->GetEnterprisedbPath() + wxT("/pg_dumpall");
    edbRestoreExecutable = settings->GetEnterprisedbPath() + wxT("/pg_restore");
#endif

    if (!isPgApp(pgBackupExecutable))
        pgBackupExecutable = wxEmptyString;
    if (!isPgApp(pgBackupAllExecutable))
        pgBackupAllExecutable = wxEmptyString;
    if (!isPgApp(pgRestoreExecutable))
        pgRestoreExecutable = wxEmptyString;

    if (!isEdbApp(edbBackupExecutable))
        edbBackupExecutable = wxEmptyString;
    if (!isEdbApp(edbBackupAllExecutable))
        edbBackupAllExecutable = wxEmptyString;
    if (!isEdbApp(edbRestoreExecutable))
        edbRestoreExecutable = wxEmptyString;
}

void pgAdmin3::InitHelp()
{
    // Search for external docs. As Windows and *nix etc 
    // are likely to be very different, we'll #ifdef them all.
    wxPathList stdPaths, noPaths, pgPaths, edbPaths, slonyPaths;
    wxString sep = wxFileName::GetPathSeparator();

    stdPaths.Add(docPath + sep + settings->GetCanonicalLanguage());
    stdPaths.Add(docPath + sep + wxT("en_US"));
    stdPaths.Add(docPath);

#ifdef __WXMSW__
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.3\\doc"));
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.3\\doc\\html"));
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.2\\doc"));
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.2\\doc\\html"));
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.1\\doc"));
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.1\\doc\\html"));
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.0\\doc"));
    pgPaths.Add(wxT("C:\\Program Files\\PostgreSQL\\8.0\\doc\\html"));

    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.3\\dbserver\\doc"));
    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.3\\dbserver\\doc\\html"));
    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.2\\dbserver\\doc"));
    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.2\\dbserver\\doc\\html"));
    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.1\\dbserver\\doc"));
    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.1\\dbserver\\doc\\html"));
    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.0\\dbserver\\doc"));
    edbPaths.Add(wxT("C:\\EnterpriseDB\\8.0\\dbserver\\doc\\html"));
#else
    pgPaths.Add(wxT("/usr/local/pgsql/doc"));
    pgPaths.Add(wxT("/usr/local/pgsql/doc/html"));
    pgPaths.Add(wxT("/usr/local/doc/postgresql"));
    pgPaths.Add(wxT("/usr/local/doc/postgresql/html"));
    pgPaths.Add(wxT("/usr/doc/postgresql"));
    pgPaths.Add(wxT("/usr/doc/postgresql/html"));
    pgPaths.Add(wxT("/opt/local/pgsql/doc"));
    pgPaths.Add(wxT("/opt/local/pgsql/doc/html"));
    pgPaths.Add(wxT("/opt/local/doc/postgresql"));
    pgPaths.Add(wxT("/opt/local/doc/postgresql/html"));
    pgPaths.Add(wxT("/opt/doc/postgresql"));
    pgPaths.Add(wxT("/opt/doc/postgresql/html"));

    edbPaths.Add(wxT("/usr/local/enterpriseDB/doc"));
    edbPaths.Add(wxT("/usr/local/enterpriseDB/doc/html"));
    edbPaths.Add(wxT("/usr/local/enterprisedb/doc"));
    edbPaths.Add(wxT("/usr/local/enterprisedb/doc/html"));
    edbPaths.Add(wxT("/usr/local/edb/doc"));
    edbPaths.Add(wxT("/usr/local/edb/doc/html"));
    edbPaths.Add(wxT("/opt/local/enterpriseDB/doc"));
    edbPaths.Add(wxT("/opt/local/enterpriseDB/doc/html"));
    edbPaths.Add(wxT("/opt/local/enterprisedb/doc"));
    edbPaths.Add(wxT("/opt/local/enterprisedb/doc/html"));
    edbPaths.Add(wxT("/opt/local/edb/doc"));
    edbPaths.Add(wxT("/opt/local/edb/doc/html"));
#endif 

    // Slony will be installed into one of the DBMS directories
    slonyPaths.Add(pgPaths);
    slonyPaths.Add(edbPaths);

    // First look for a chm, then a zip, then an hhp file. For PostgreSQL
    // and EnterpriseDB we'll then look for an index.html. No point for 
    // Slony as we'd most likely find the DBMS's help.

    wxString pgHelpPath = settings->GetPgHelpPath();
    wxString edbHelpPath = settings->GetEdbHelpPath();
    wxString slonyHelpPath = settings->GetSlonyHelpPath();

#if defined (__WXMSW__) || wxUSE_LIBMSPACK
    pgHelpPath = GenerateHelpPath(wxT("PostgreSQL.chm"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("postgresql.chm"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("postgres.chm"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("pgsql.chm"), pgHelpPath, stdPaths, pgPaths);

    edbHelpPath = GenerateHelpPath(wxT("EnterpriseDB.chm"), edbHelpPath, stdPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("enterprisedb.chm"), edbHelpPath, stdPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("edb.chm"), edbHelpPath, stdPaths, edbPaths);

    slonyHelpPath = GenerateHelpPath(wxT("Slony-I.chm"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony-i.chm"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony1.chm"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony.chm"), slonyHelpPath, stdPaths, slonyPaths);
#endif

    pgHelpPath = GenerateHelpPath(wxT("PostgreSQL.zip"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("postgresql.zip"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("postgres.zip"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("pgsql.zip"), pgHelpPath, stdPaths, pgPaths);

    edbHelpPath = GenerateHelpPath(wxT("EnterpriseDB.zip"), edbHelpPath, stdPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("enterprisedb.zip"), edbHelpPath, stdPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("edb.zip"), edbHelpPath, stdPaths, edbPaths);

    slonyHelpPath = GenerateHelpPath(wxT("Slony-I.zip"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony-i.zip"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony1.zip"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony.zip"), slonyHelpPath, stdPaths, slonyPaths);

    pgHelpPath = GenerateHelpPath(wxT("PostgreSQL.hhp"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("postgresql.hhp"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("postgres.hhp"), pgHelpPath, stdPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("pgsql.hhp"), pgHelpPath, stdPaths, pgPaths);

    edbHelpPath = GenerateHelpPath(wxT("EnterpriseDB.hhp"), edbHelpPath, stdPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("enterprisedb.hhp"), edbHelpPath, stdPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("edb.hhp"), edbHelpPath, stdPaths, edbPaths);

    slonyHelpPath = GenerateHelpPath(wxT("Slony-I.hhp"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony-i.hhp"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony1.hhp"), slonyHelpPath, stdPaths, slonyPaths);
    slonyHelpPath = GenerateHelpPath(wxT("slony.hhp"), slonyHelpPath, stdPaths, slonyPaths);

    pgHelpPath = GenerateHelpPath(wxT("index.html"), pgHelpPath, noPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("index.html"), pgHelpPath, noPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("index.html"), pgHelpPath, noPaths, pgPaths);
    pgHelpPath = GenerateHelpPath(wxT("index.html"), pgHelpPath, noPaths, pgPaths);

    edbHelpPath = GenerateHelpPath(wxT("index.html"), edbHelpPath, noPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("index.html"), edbHelpPath, noPaths, edbPaths);
    edbHelpPath = GenerateHelpPath(wxT("index.html"), edbHelpPath, noPaths, edbPaths);

    // If either path ends in index.html, remove the filename because we
    // just want the path. In this case, we should also add file:/// on
    // non-Windows platforms.
    if (pgHelpPath.EndsWith(wxT("index.html")))
    {
        pgHelpPath = pgHelpPath.Left(pgHelpPath.Length() - 10);
#ifndef __WXMSW__
        pgHelpPath = wxT("file:///") + pgHelpPath;
#endif
    }

    if (edbHelpPath.EndsWith(wxT("index.html")))
    {
        edbHelpPath = edbHelpPath.Left(edbHelpPath.Length() - 10);
#ifndef __WXMSW__
        edbHelpPath = wxT("file:///") + edbHelpPath;
#endif
    }

    // Last resorts - if we still have no help by now, use the websites!
    if (pgHelpPath.IsEmpty())
        pgHelpPath = wxT("http://www.postgresql.org/docs/current/static/");
    if (edbHelpPath.IsEmpty())
        edbHelpPath = wxT("http://www.enterprisedb.com/documentation/8.2/");
    if (slonyHelpPath.IsEmpty())
        slonyHelpPath = wxT("http://www.slony.info/documentation/");

    // OK, so set the values
    if (settings->GetPgHelpPath().IsEmpty())
        settings->SetPgHelpPath(pgHelpPath);
    if (settings->GetEdbHelpPath().IsEmpty())
        settings->SetEdbHelpPath(edbHelpPath);
    if (settings->GetSlonyHelpPath().IsEmpty())
        settings->SetSlonyHelpPath(slonyHelpPath);
}

wxString pgAdmin3::GenerateHelpPath(const wxString &file, const wxString &current, wxPathList stdPaths, wxPathList dbmsPaths)
{
    // If we already have a value, don't change it.
    if (!current.IsEmpty())
        return current;

    if (!stdPaths.FindValidPath(file).IsEmpty())
        return stdPaths.FindValidPath(file);

    if (!dbmsPaths.FindValidPath(file).IsEmpty())
        return dbmsPaths.FindValidPath(file);

    return wxEmptyString;
}

void pgAdmin3::InitXml()
{
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
        


#ifdef EMBED_XRC
    wxLogInfo(__("Using embedded XRC data."));   
	
    // resources are loaded from memory
    extern void InitXmlResource();
    InitXmlResource();

#else
    wxLogInfo(__("Using external XRC files."));

    // for debugging, dialog resources are read from file
    wxXmlResource::Get()->Load(uiPath + wxT("/*.xrc"));
#endif

}


void pgAdmin3::InitLogger()
{
    logger = new sysLogger();
    wxLog::SetActiveTarget(logger);
	wxLog::Resume();
}


void pgAdmin3::InitNetwork()
{
    // Startup the windows sockets if required
#ifdef __WXMSW__
    WSADATA    wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        wxLogFatalError(__("Cannot initialise the networking subsystem!"));   
    }
#endif
    wxSocketBase::Initialize();

    pgConn::ExamineLibpqVersion();
}

#include "images/pgAdmin3.xpm"
#include "images/elephant32.xpm"
#include "images/splash.xpm"

pgAppearanceFactory::pgAppearanceFactory()
{
	is_branded = false;

	// Setup the default branding options
#ifdef __WIN32__
	splash_font_size = 8;
#else
#ifdef __WXMAC__
	splash_font_size = 11;
#else
	splash_font_size = 9;
#endif
#endif

	splash_pos_x = 128;
	splash_pos_y = 281;
	splash_pos_offset = 15;

	large_icon = wxImage(elephant32_xpm);
	small_icon = wxImage(pgAdmin3_xpm);
	splash_image = wxImage(splash_xpm);

	splash_text_colour = wxColour(255, 255, 255);
	report_key_colour = wxColour(0, 154, 206);

	long_appname = wxT("pgAdmin III");
	short_appname = wxT("pgadmin3");
	website_url = wxT("http://www.pgadmin.org/");

	// Attempt to overload branding information
	wxFileName brIni(brandingPath + wxT("/branding.ini"));
	if (brIni.FileExists())
	{
		wxString brCfg = FileRead(brIni.GetFullPath());

		wxStringTokenizer tkz(brCfg, wxT("\r\n"));

		while(tkz.HasMoreTokens())
		{
			wxString token = tkz.GetNextToken();

			if (token.Trim() == wxEmptyString || token.StartsWith(wxT(";")))
				continue;

			if (token.Lower().StartsWith(wxT("largeicon=")))
			{
				large_icon = wxImage(brandingPath + wxT("/") + token.AfterFirst('=').Trim());
				if (!large_icon.IsOk())
					large_icon = wxImage(elephant32_xpm);
				else
					is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("smallicon=")))
			{
				small_icon = wxImage(brandingPath + wxT("/") + token.AfterFirst('=').Trim());
				if (!small_icon.IsOk())
					small_icon = wxImage(pgAdmin3_xpm);
				else
					is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("splashimage=")))
			{
				splash_image = wxImage(brandingPath + wxT("/") + token.AfterFirst('=').Trim());
				if (!splash_image.IsOk())
					splash_image = wxImage(splash_xpm);
				else
					is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("icon=")))
			{
				icon = token.AfterFirst('=').Trim();
				is_branded = true;
			}
#ifdef __WIN32__
			else if (token.Lower().StartsWith(wxT("splashfontsizewin=")))
#else
#ifdef __WXMAC__
			else if (token.Lower().StartsWith(wxT("splashfontsizemac=")))
#else
			else if (token.Lower().StartsWith(wxT("splashfontsizegtk=")))
#endif
#endif
			{
				token.AfterFirst('=').Trim().ToLong(&splash_font_size);
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("splashposx=")))
			{
				token.AfterFirst('=').Trim().ToLong(&splash_pos_x);
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("splashposy=")))
			{
				token.AfterFirst('=').Trim().ToLong(&splash_pos_y);
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("splashposoffset=")))
			{
				token.AfterFirst('=').Trim().ToLong(&splash_pos_offset);
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("splashtextcolour=")))
			{
				splash_text_colour = wxColor(token.AfterFirst('=').Trim());
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("shortappname=")))
			{
				short_appname = token.AfterFirst('=').Trim();
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("longappname=")))
			{
				long_appname = token.AfterFirst('=').Trim();
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("websiteurl=")))
			{
				website_url = token.AfterFirst('=').Trim();
				is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("reportkeycolour=")))
			{
				report_key_colour = wxColor(token.AfterFirst('=').Trim());
				is_branded = true;
			}
		}
	}

#ifdef __WXMSW__

	// Set the MUI cache value for the grouped task bar title, 
	// otherwise we get the value from the resources which is 
	// definitely not what we want in branded mode!
	wxRegKey *pRegKey = new wxRegKey(wxT("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\ShellNoRoam\\MUICache"));
	if(!pRegKey->Exists())
		pRegKey->Create();

	wxStandardPaths paths;
	//wxString tmp;
	//tmp.Printf(wxT("%s"), long_appname);
	pRegKey->SetValue(paths.GetExecutablePath(), GetLongAppName());
    delete pRegKey;

    // Reset the image for the task bar group. This can only by
    // set per-exe name unfortunately. If we don't find an icon,
    // remove the registry value.
    wxString icon_path = brandingPath + wxT("\\") + icon;


    pRegKey = new wxRegKey(wxT("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + wxFileName(paths.GetExecutablePath()).GetFullName());
	if(!pRegKey->Exists())
		pRegKey->Create();

    if (wxFile::Exists(icon_path))
    {
        pRegKey->SetValue(wxT("TaskbarGroupIcon"), icon_path);
    }
    else
    {
        if (pRegKey->HasValue(wxT("TaskbarGroupIcon")))
            pRegKey->DeleteValue(wxT("TaskbarGroupIcon"));
    }
    delete pRegKey;
#endif
}

void pgAppearanceFactory::SetIcons(wxDialog *dlg)
{
    wxIconBundle icons;
    icons.AddIcon(GetSmallIconImage());
    icons.AddIcon(GetBigIconImage());
    dlg->SetIcons(icons);
}

void pgAppearanceFactory::SetIcons(wxTopLevelWindow *dlg)
{
    wxIconBundle icons;
    icons.AddIcon(GetSmallIconImage());
    icons.AddIcon(GetBigIconImage());
    dlg->SetIcons(icons);
}

wxIcon pgAppearanceFactory::GetSmallIconImage()
{
	wxIcon icon;
	icon.CopyFromBitmap(wxBitmap(small_icon));
    return icon;
}

wxIcon pgAppearanceFactory::GetBigIconImage()
{
	wxIcon icon;
	icon.CopyFromBitmap(wxBitmap(large_icon));
    return icon;
}

wxFont pgAppearanceFactory::GetSplashTextFont()
{
    wxFont fnt(*wxNORMAL_FONT);
    fnt.SetPointSize(splash_font_size);
    return fnt;
}
