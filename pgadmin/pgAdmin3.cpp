//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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

#define HELPER_DIR  wxT("/helper")

// Globals
frmMain *winMain=0;
wxThread *updateThread=0;

sysSettings *settings;
wxArrayInt existingLangs;
wxArrayString existingLangNames;
wxLocale *locale=0;
pgAppearanceFactory *appearanceFactory=0;

wxString pgBackupExecutable;      // complete filename of PostgreSQL's pg_dump, pg_dumpall and pg_restore, if available
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
	static const wxCmdLineEntryDesc cmdLineDesc[] = 
	{
		{wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{wxCMD_LINE_OPTION, wxT("s"), wxT("server"), _("auto-connect to specified server"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_SWITCH, wxT("q"), wxT("query"), _("open query tool"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, wxT("qc"), wxT("queryconnect"), _("connect query tool to database"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_OPTION, wxT("cm"), NULL, _("edit main configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, wxT("ch"), NULL, _("edit HBA configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
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
    wxLogInfo(msg);
    wxLogInfo(wxT("##############################################################"));

#ifdef SSL
    wxLogInfo(wxT("Compiled with dynamically linked SSL support"));
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
    int langNo;

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

            langNo=2;       // skipping default, unknown

            while (true)
            {
                langInfo=wxLocale::GetLanguageInfo(langNo);
                if (!langInfo)
                    break;

                if (englishName == langInfo->Description && 
                    (langInfo->CanonicalName == wxT("en_US") || 
                    (!langInfo->CanonicalName.IsEmpty() && 
                     wxDir::Exists(i18nPath + wxT("/") + langInfo->CanonicalName))))
                {
                    existingLangs.Add(langNo);
                    existingLangNames.Add(translatedName);
                    langCount++;
                }
                langNo++;
            }
        }
    }

#ifdef __WXDEBUG__
	frmSplash *winSplash = NULL;
#else
    // Show the splash screen
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
#endif

	
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

#ifndef __WXDEBUG__
    wxTheApp->Yield(true);
    wxSleep(2);
#endif


    if (configMode)
    {
		if (configMode == frmConfig::ANYFILE && wxDir::Exists(configFile))
		{
			frmConfig::Create(appearanceFactory->GetLongAppName(), configFile + wxT("/pg_hba.conf"), frmConfig::HBAFILE);
			frmConfig::Create(appearanceFactory->GetLongAppName(), configFile + wxT("/postgresql.conf"), frmConfig::MAINFILE);
		}
		else
		{
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
            wxFrame *dtf=new frmDlgTest();
            dtf->Show();
            SetTopWindow(dtf);
        }
        else if ((cmdParser.Found(wxT("q")) || cmdParser.Found(wxT("qc"))) &&!cmdParser.Found(wxT("s")))
        {
			// -q specified, but not -s. Open a query tool but do *not* open the main window
			pgConn *conn = NULL;
			wxString connstr;

			if (cmdParser.Found(wxT("q")))
			{
				dlgSelectConnection dlg(NULL, NULL);
		        int rc=dlg.Go(conn, NULL);
				if (rc != wxID_OK)
					return false;
				conn = dlg.CreateConn();
			}
			else if (cmdParser.Found(wxT("qc"), &connstr))
			{
				wxString host, database, username, tmps;
				int sslmode=0,port=0;
				wxStringTokenizer tkn(connstr, wxT(" "), wxTOKEN_STRTOK);
				while (tkn.HasMoreTokens())
				{
					wxString str = tkn.GetNextToken();
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
				dlgSelectConnection dlg(NULL, NULL);
				conn = dlg.CreateConn(host, database, username, port, sslmode);
			}
			else
			{
				/* Can't happen.. */
				return false;
			}
			if (!conn)
				return false;
			frmQuery *fq = new frmQuery(NULL, wxEmptyString, conn, wxEmptyString);
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
						frmQuery *fq = new frmQuery(winMain, wxEmptyString, conn, wxEmptyString);
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

    //When using wxStandardPaths on OSX, wx defaults to the unix,
    //not to the mac variants. Therefor, we request wxStandardPathsCF
    //directly.
    wxStandardPathsCF stdPaths ;
    dataDir = stdPaths.GetDataDir() ;

#else // other *ixes

// Data path (defined by configure under Unix).
#ifndef DATA_DIR
#define DATA_DIR "./"
#endif

    dataDir = wxString::FromAscii(DATA_DIR);
#endif

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
        if (wxDir::Exists(loadPath + I18N_DIR))
            i18nPath = loadPath + I18N_DIR;
        else
            i18nPath = loadPath + wxT("/..") I18N_DIR;
    }
    if (uiPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + UI_DIR))
            uiPath = loadPath + UI_DIR;
        else 
            uiPath = loadPath + wxT("/..") UI_DIR;
    }
    if (docPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + DOC_DIR))
            docPath = loadPath + DOC_DIR ;
        else
            docPath = loadPath + wxT("/..") DOC_DIR ;
    }
    if (brandingPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + BRANDING_DIR))
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

            if (wxDir::Exists(dataDir + HELPER_DIR))
            path.Add(dataDir + HELPER_DIR) ;

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
        path.Add(wxT("C:\\Program Files\\PostgreSQL\\8.2\\bin"));
        path.Add(wxT("C:\\Program Files\\PostgreSQL\\8.1\\bin"));
        path.Add(wxT("C:\\Program Files\\PostgreSQL\\8.0\\bin"));

        wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
        path.Add(wxT("/usr/local/pgsql/bin"));
        path.Add(wxT("/usr/local/bin"));
        path.Add(wxT("/usr/bin"));

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
        path.Add(wxT("C:\\EnterpriseDB\\8.2\\dbserver\\bin"));
        path.Add(wxT("C:\\EnterpriseDB\\8.1\\dbserver\\bin"));
        path.Add(wxT("C:\\EnterpriseDB\\8.0\\dbserver\\bin"));

        wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
        path.Add(wxT("/usr/local/enterpriseDB/bin"));
        path.Add(wxT("/usr/local/bin"));
        path.Add(wxT("/usr/bin"));

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

    // resources are loaded from memory
    extern void InitXmlResource();
    InitXmlResource();

#else

    // for debugging, dialog resources are read from file
    wxXmlResource::Get()->Load(uiPath + wxT("/*.xrc"));
#endif

}


void pgAdmin3::InitLogger()
{
    logger = new sysLogger();
    wxLog::SetActiveTarget(logger);
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
