//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include <wx/fs_zip.h>
#include <wx/socket.h>
#include <wx/stdpaths.h>
#include <wx/clipbrd.h>
#include <wx/sysopt.h>

// wxOGL
#include <ogl/ogl.h>

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
#include "frm/frmStatus.h"
#ifdef DATABASEDESIGNER
#include "frm/frmDatabaseDesigner.h"
#endif
#include "frm/frmSplash.h"
#include "dlg/dlgSelectConnection.h"
#include "db/pgConn.h"
#include "utils/sysLogger.h"
#include "utils/registry.h"
#include "frm/frmHint.h"

#include "ctl/xh_calb.h"
#include "ctl/xh_timespin.h"
#include "ctl/xh_sqlbox.h"
#include "ctl/xh_ctlcombo.h"
#include "ctl/xh_ctltree.h"
#include "ctl/xh_ctlchecktreeview.h"
#include "ctl/xh_ctlcolourpicker.h"

#define DOC_DIR       wxT("/docs")
#define UI_DIR        wxT("/ui")
#define I18N_DIR      wxT("/i18n")
#define BRANDING_DIR  wxT("/branding")
#define PLUGINS_DIR   wxT("/plugins.d")
#define SETTINGS_INI  wxT("/settings.ini")

// Globals
frmMain *winMain = 0;
wxThread *updateThread = 0;

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
#include "utils/sshTunnel.h"
CSSHTunnelThread *pgadminTunnelThread = 0;
#endif

sysSettings *settings;
wxArrayInt existingLangs;
wxArrayString existingLangNames;
wxLocale *locale = 0;
pgAppearanceFactory *appearanceFactory = 0;

wxString pgBackupExecutable;       // complete filename of PostgreSQL's pg_dump, pg_dumpall and pg_restore, if available
wxString pgBackupAllExecutable;
wxString pgRestoreExecutable;

wxString edbBackupExecutable;      // complete filename of EnterpriseDB's pg_dump, pg_dumpall and pg_restore, if available
wxString edbBackupAllExecutable;
wxString edbRestoreExecutable;

wxString gpBackupExecutable;      // complete filename of Greenplum's pg_dump, pg_dumpall and pg_restore, if available
wxString gpBackupAllExecutable;
wxString gpRestoreExecutable;

wxString loadPath;              // Where the program is loaded from
wxString dataDir;               // The program data directory
wxString docPath;               // Where docs are stored
wxString uiPath;                // Where ui data is stored
wxString i18nPath;              // Where i18n data is stored
wxString brandingPath;          // Where branding data is stored
wxString pluginsDir;            // The plugins ini file directory
wxString settingsIni;           // The settings.ini file

wxLog *logger;

bool dialogTestMode = false;

#define LANG_FILE   wxT("pgadmin3.lng")

IMPLEMENT_APP(pgAdmin3)

#ifdef __WXMSW__
// Dynamically loaded EDB functions
PQGETOUTRESULT PQiGetOutResult;
PQPREPAREOUT PQiPrepareOut;
PQSENDQUERYPREPAREDOUT PQiSendQueryPreparedOut;
#endif

#ifdef __WXGTK__

class pgRendererNative : public wxDelegateRendererNative
{
public:
	pgRendererNative() : wxDelegateRendererNative(wxRendererNative::GetDefault()) {}

	void DrawTreeItemButton(wxWindow *win, wxDC &dc, const wxRect &rect, int flags)
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
	dlgList = new wxListBox(this, CTL_LB, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SORT);

	// unfortunately, the MemoryFS has no search functions implemented
	// so we can't extract the names in the EMBED_XRC case

	wxDir dir(uiPath);
	wxString filename;

	bool found = dir.GetFirst(&filename, wxT("*.xrc"));
	while (found)
	{
		dlgList->Append(filename.Left(filename.Length() - 4));
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
	wxString dlgName = dlgList->GetStringSelection();
	if (!dlgName.IsEmpty())
	{
		pgDialog *dlg = new pgDialog;
		dlg->SetFont(settings->GetSystemFont());
		dlg->LoadResource(this, dlgName);
		dlg->SetTitle(dlgName);
		dlg->Show();
	}
}


// The Application!
bool pgAdmin3::OnInit()
{
	// Force logging off until we're ready
	wxLog *seLog = new wxLogStderr();
	wxLog::SetActiveTarget(seLog);

	// Setup the basic paths for the app installation. Required by settings!
	InitAppPaths();

	// Load the Settings
#ifdef __WXMSW__
	settings = new sysSettings(wxT("pgAdmin III"));
#else
	settings = new sysSettings(wxT("pgadmin3"));
#endif

	// Setup additional helper paths etc. Requires settings!
	InitXtraPaths();

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

#ifdef DATABASEDESIGNER
	//Initialize Font
	hdFontAttribute::InitFont();
#endif

	long langCount = 0;
	const wxLanguageInfo *langInfo;

	wxString langfile = FileRead(i18nPath + wxT("/") LANG_FILE, 1);

	if (!langfile.IsEmpty())
	{
		wxStringTokenizer tk(langfile, wxT("\n\r"));

		while (tk.HasMoreTokens())
		{
			wxString line = tk.GetNextToken().Strip(wxString::both);
			if (line.IsEmpty() || line.StartsWith(wxT("#")))
				continue;

			wxString englishName = line.BeforeFirst(',').Trim(true);
			wxString translatedName = line.AfterFirst(',').Trim(false);

			langInfo = wxLocale::FindLanguageInfo(englishName);
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

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
#if wxCHECK_VERSION(2, 9, 0)
		// wxCmdLineEntryDesc is one of the few places in 2.9 where wxT()s have any effect...they break the build
		{wxCMD_LINE_SWITCH, "v", "version", _("show the version, and quit"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_SWITCH, "h", "help", _("show this help message, and quit"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{wxCMD_LINE_OPTION, "s", "server", _("auto-connect to specified server"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_SWITCH, "S", "serverstatus", _("open server status window"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, "Sc", "serverstatusconnect", _("connect server status window to database"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_SWITCH, "q", "query", _("open query tool"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, "qc", "queryconnect", _("connect query tool to database"), wxCMD_LINE_VAL_STRING},
#ifdef DATABASEDESIGNER
		{wxCMD_LINE_SWITCH, "d", "designer", _("open designer window"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, "dc", "designerconnectconnect", _("connect designer window to database"), wxCMD_LINE_VAL_STRING},
#endif
		{wxCMD_LINE_OPTION, "f", "file", _("file to load into the query tool in -q or -qc mode"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_OPTION, "cm", NULL, _("edit main configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, "ch", NULL, _("edit HBA configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, "cp", NULL, _("edit pgpass configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, "c", NULL, _("edit configuration files in cluster directory"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_SWITCH, "t", NULL, _("dialog translation test mode"), wxCMD_LINE_VAL_NONE},
#else
		{wxCMD_LINE_SWITCH, wxT("v"), wxT("version"), _("show the version, and quit"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), _("show the help message, and quit"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{wxCMD_LINE_OPTION, wxT("s"), wxT("server"), _("auto-connect to specified server"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_SWITCH, wxT("S"), wxT("serverstatus"), _("open server status window"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, wxT("Sc"), wxT("serverstatusconnect"), _("connect server status window to database"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_SWITCH, wxT("q"), wxT("query"), _("open query tool"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, wxT("qc"), wxT("queryconnect"), _("connect query tool to database"), wxCMD_LINE_VAL_STRING},
#ifdef DATABASEDESIGNER
		{wxCMD_LINE_SWITCH, wxT("d"), wxT("designer"), _("open designer window"), wxCMD_LINE_VAL_NONE},
		{wxCMD_LINE_OPTION, wxT("dc"), wxT("designerconnectconnect"), _("connect designer window to database"), wxCMD_LINE_VAL_STRING},
#endif
		{wxCMD_LINE_OPTION, wxT("f"), wxT("file"), _("file to load into the query tool in -q or -qc mode"), wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_OPTION, wxT("cm"), NULL, _("edit main configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, wxT("ch"), NULL, _("edit HBA configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, wxT("cp"), NULL, _("edit pgpass configuration file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_OPTION, wxT("c"), NULL, _("edit configuration files in cluster directory"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_SWITCH, wxT("t"), NULL, _("dialog translation test mode"), wxCMD_LINE_VAL_NONE},
#endif
		{wxCMD_LINE_NONE}

	};

	frmConfig::tryMode configMode = frmConfig::NONE;
	wxString configFile;

	wxCmdLineParser cmdParser(cmdLineDesc, argc, argv);
	if (cmdParser.Parse() != 0)
		return false;

	if (
	    (cmdParser.Found(wxT("q")) && cmdParser.Found(wxT("qc")))
	    || (cmdParser.Found(wxT("S")) && cmdParser.Found(wxT("Sc")))
#ifdef DATABASEDESIGNER
	    || (cmdParser.Found(wxT("d")) && cmdParser.Found(wxT("dc")))
#endif
	)
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
	::wxInitAllImageHandlers();

	appearanceFactory = new pgAppearanceFactory();

	if (cmdParser.Found(wxT("v")))
	{
		wxPrintf(wxT("%s %s\n"), appearanceFactory->GetLongAppName().c_str(), VERSION_STR);
		return false;
	}

	// Setup logging
	InitLogger();

	wxString msg;
	msg << wxT("# ") << appearanceFactory->GetLongAppName() << wxT(" Version ") << VERSION_STR << wxT(" Startup");
	wxLogInfo(wxT("##############################################################"));
	wxLogInfo(wxT("%s"), msg.c_str());
	wxLogInfo(wxT("##############################################################"));

#ifdef PG_SSL
	wxLogInfo(wxT("Compiled with dynamically linked SSL support"));
#endif

#ifdef __WXDEBUG__
	wxLogInfo(wxT("Running a DEBUG build."));
#else
	wxLogInfo(wxT("Running a RELEASE build."));
#endif

	// Log the path info
	wxLogInfo(wxT("i18n path     : %s"), i18nPath.c_str());
	wxLogInfo(wxT("UI path       : %s"), uiPath.c_str());
	wxLogInfo(wxT("Doc path      : %s"), docPath.c_str());
	wxLogInfo(wxT("Branding path : %s"), brandingPath.c_str());
	wxLogInfo(wxT("Plugins path  : %s"), pluginsDir.c_str());
	wxLogInfo(wxT("Settings INI  : %s"), settingsIni.c_str());

	wxLogInfo(wxT("PG pg_dump    : %s"), pgBackupExecutable.c_str());
	wxLogInfo(wxT("PG pg_dumpall : %s"), pgBackupAllExecutable.c_str());
	wxLogInfo(wxT("PG pg_restore : %s"), pgRestoreExecutable.c_str());

	wxLogInfo(wxT("EDB pg_dump   : %s"), edbBackupExecutable.c_str());
	wxLogInfo(wxT("EDB pg_dumpall: %s"), edbBackupAllExecutable.c_str());
	wxLogInfo(wxT("EDB pg_restore: %s"), edbRestoreExecutable.c_str());

	wxLogInfo(wxT("Greenplum pg_dump   : %s"), gpBackupExecutable.c_str());
	wxLogInfo(wxT("Greenplum pg_dumpall: %s"), gpBackupAllExecutable.c_str());
	wxLogInfo(wxT("Greenplum pg_restore: %s"), gpRestoreExecutable.c_str());

#ifdef __WXGTK__
	static pgRendererNative *renderer = new pgRendererNative();
	wxRendererNative::Get();
	wxRendererNative::Set(renderer);
#endif

#ifdef __LINUX__
	signal(SIGPIPE, SIG_IGN);
#endif

	// Show the splash screen
	// NOTE: We must *always* do this as in -q and -qc modes
	//       the splash screen becomes the top level window and
	//       allows the logon dialogs to be displayed!!
	frmSplash *winSplash = new frmSplash((wxFrame *)NULL);
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
	wxXmlResource::Get()->AddHandler(new ctlCheckTreeViewXmlHandler);
	wxXmlResource::Get()->AddHandler(new ctlColourPickerXmlHandler);

	InitXml();

	wxOGLInitialize();

	// Set some defaults
	SetAppName(appearanceFactory->GetLongAppName());

	// Setup the help paths
	InitHelp();
	wxLogInfo(wxT("PG Help       : %s"), settings->GetPgHelpPath().c_str());
	wxLogInfo(wxT("EDB Help      : %s"), settings->GetEdbHelpPath().c_str());
	wxLogInfo(wxT("Greenplum Help: %s"), settings->GetGpHelpPath().c_str());
	wxLogInfo(wxT("Slony Help    : %s"), settings->GetSlonyHelpPath().c_str());

#ifndef __WXDEBUG__
	wxTheApp->Yield(true);
	wxSleep(2);
#endif

#ifdef __WXMSW__
	// Attempt to dynamically load PGgetOutResult from libpq. this
	// is only present in EDB versions.
	InitLibpq();
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
			wxFrame *dtf = new frmDlgTest();
			dtf->Show();
			SetTopWindow(dtf);
		}

		else if ((cmdParser.Found(wxT("S")) || cmdParser.Found(wxT("Sc"))) && !cmdParser.Found(wxT("s")))
		{
			// -S specified, but not -s. Open the server status window but do *not* open the main window
			pgConn *conn = NULL;
			wxString connstr;
			wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Server Status");

			if (cmdParser.Found(wxT("S")))
			{
				wxLogInfo(wxT("Starting in server status mode (-S)."), configFile.c_str());

				winSplash->Show(false);
				dlgSelectConnection dlg(NULL, NULL);
				dlg.CenterOnParent();

				int rc = dlg.Go(conn, NULL);
				if (rc != wxID_OK)
					return false;
				bool dummyRes;
				conn = dlg.CreateConn(applicationname, dummyRes);
			}
			else if (cmdParser.Found(wxT("Sc"), &connstr))
			{
				wxLogInfo(wxT("Starting in server status connect mode (-Sc)."), configFile.c_str());
				wxString host, database, username, rolename, tmps;
				int sslmode = 0, port = 0;
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
					if (str.StartsWith(wxT("role="), &rolename))
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
						else if (!tmps.Cmp(wxT("verify-ca")))
							sslmode = 5;
						else if (!tmps.Cmp(wxT("verify-full")))
							sslmode = 6;
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
				conn = dlg.CreateConn(host, database, username, port, rolename, sslmode, applicationname);
			}
			else
			{
				/* Can't happen.. */
				return false;
			}
			if (!conn)
				return false;

			wxString txt = _("Server Status - ") + conn->GetName();
			frmStatus *fq = new frmStatus(NULL, txt, conn);
			fq->Go();
		}

#ifdef DATABASEDESIGNER
		else if ((cmdParser.Found(wxT("d")) || cmdParser.Found(wxT("dc"))) && !cmdParser.Found(wxT("s")))
		{
			// -d specified, but not -s. Open the designer window but do *not* open the main window
			pgConn *conn = NULL;
			wxString connstr;
			wxString applicationname = _("pgAdmin - Database designer");

			if (cmdParser.Found(wxT("d")))
			{
				wxLogInfo(wxT("Starting in designer mode (-d)."), configFile.c_str());

				winSplash->Show(false);
				dlgSelectConnection dlg(NULL, NULL);
				dlg.CenterOnParent();

				int rc = dlg.Go(conn, NULL);
				if (rc != wxID_OK)
					return false;
				bool dummyRes;
				conn = dlg.CreateConn(applicationname, dummyRes);
			}
			else if (cmdParser.Found(wxT("dc"), &connstr))
			{
				wxLogInfo(wxT("Starting in database designer connect mode (-dc)."), configFile.c_str());
				wxString host, database, username, rolename, tmps;
				int sslmode = 0, port = 0;
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
					if (str.StartsWith(wxT("role="), &rolename))
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
						else if (!tmps.Cmp(wxT("verify-ca")))
							sslmode = 5;
						else if (!tmps.Cmp(wxT("verify-full")))
							sslmode = 6;
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
				conn = dlg.CreateConn(host, database, username, port, rolename, sslmode, applicationname);
			}
			else
			{
				/* Can't happen.. */
				return false;
			}
			if (!conn)
				return false;

			frmDatabaseDesigner *fq = new frmDatabaseDesigner(NULL, wxEmptyString, conn);
			fq->Go();
		}
#endif

#ifdef __WXMAC__
		else if (((cmdParser.Found(wxT("q")) || cmdParser.Found(wxT("qc"))) && !cmdParser.Found(wxT("s"))) || !macFileToOpen.IsEmpty())
#else
		else if ((cmdParser.Found(wxT("q")) || cmdParser.Found(wxT("qc"))) && !cmdParser.Found(wxT("s")))
#endif
		{
			// -q specified, but not -s. Open a query tool but do *not* open the main window
			pgConn *conn = NULL;
			wxString connstr;
			wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Query Tool");

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

				int rc = dlg.Go(conn, NULL);
				if (rc != wxID_OK)
					return false;
				bool dummyRes;
				conn = dlg.CreateConn(applicationname, dummyRes);
			}
			else if (cmdParser.Found(wxT("qc"), &connstr))
			{
				wxLogInfo(wxT("Starting in query tool connect mode (-qc)."), configFile.c_str());
				wxString host, database, username, rolename, tmps;
				int sslmode = 0, port = 0;
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
					if (str.StartsWith(wxT("role="), &rolename))
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
						else if (!tmps.Cmp(wxT("verify-ca")))
							sslmode = 5;
						else if (!tmps.Cmp(wxT("verify-full")))
							sslmode = 6;
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
				conn = dlg.CreateConn(host, database, username, port, rolename, sslmode, applicationname);
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
			{
				wxLogInfo(wxT("Auto-loading file: %s"), fn.c_str());
			}
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

			wxString str;
			if (cmdParser.Found(wxT("s"), &str))
			{
				pgServer *srv = winMain->ConnectToServer(str, !cmdParser.Found(wxT("q")));
				if (srv && cmdParser.Found(wxT("q")))
				{
					pgConn *conn;
					wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Query Tool");
					conn = srv->CreateConn(wxEmptyString, 0, applicationname);
					if (conn)
					{
						wxString fn;
						cmdParser.Found(wxT("f"), &fn);
						if (!fn.IsEmpty())
						{
							wxLogInfo(wxT("Auto-loading file: %s"), fn.c_str());
						}
						frmQuery *fq = new frmQuery(winMain, wxEmptyString, conn, wxEmptyString, fn);
						fq->Go();
						winMain->AddFrame(fq);
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

// Setup the paths for the application itself
void pgAdmin3::InitAppPaths()
{
	i18nPath = LocatePath(I18N_DIR, false);
	docPath = LocatePath(DOC_DIR, false);
	uiPath = LocatePath(UI_DIR, false);
	brandingPath = LocatePath(BRANDING_DIR, false);
	pluginsDir = LocatePath(PLUGINS_DIR, false);
	settingsIni = LocatePath(SETTINGS_INI, true);
}

// Setup the paths for the helper apps etc.
void pgAdmin3::InitXtraPaths()
{
	//////////////////////////////////
	// Now setup the app helper paths
	//////////////////////////////////

	// Windows-only path prefixes
#ifdef __WXMSW__
	wxString programFiles = wxGetenv(wxT("ProgramFiles"));
	wxString programFilesX86 = wxGetenv(wxT("ProgramFiles(x86)"));

	// If we install a 32-bit pgAdmin in a 64-bit machine,
	// We will be getting the both values as "Drive:\Program Files(x86)".
	// Since, all 32-bit applications return "ProgramFiles" as ProgramFiles(x86).
	// In that case, we need can get the actual programFiles value,
	// by reading the key "ProgramFilesDir" in location "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion".
	//
	if (::wxIsPlatform64Bit() && programFiles == programFilesX86)
	{
		// Don't want to lost the exisisting behaviour.
		//
		wxString tmp = programFiles;
		programFiles = wxEmptyString;

		pgRegKey::PGREGWOWMODE wowMode = pgRegKey::PGREG_WOW64;
		pgRegKey *pgKey = pgRegKey::OpenRegKey(HKEY_LOCAL_MACHINE, wxT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), pgRegKey::PGREG_READ, wowMode);

		if (!(pgKey && pgKey->QueryValue(wxT("ProgramFilesDir"), programFiles)))
		{
			programFiles = tmp;
		}

		if(pgKey)
		{
			delete pgKey;
			pgKey = NULL;
		}
	}
#endif

	// First, check and invalidate the paths if they're no good.
#ifdef __WXMSW__
	if (!isPgApp(settings->GetPostgresqlPath() + wxT("\\pg_dump.exe")))
		settings->SetPostgresqlPath(wxEmptyString);

	if (!isEdbApp(settings->GetEnterprisedbPath() + wxT("\\pg_dump.exe")))
		settings->SetEnterprisedbPath(wxEmptyString);

	if (!isGpApp(settings->GetGPDBPath() + wxT("\\pg_dump.exe")))
		settings->SetGPDBPath(wxEmptyString);
#else
	if (!isPgApp(settings->GetPostgresqlPath() + wxT("/pg_dump")))
		settings->SetPostgresqlPath(wxEmptyString);

	if (!isEdbApp(settings->GetEnterprisedbPath() + wxT("/pg_dump")))
		settings->SetEnterprisedbPath(wxEmptyString);

	if (!isGpApp(settings->GetGPDBPath() + wxT("/pg_dump")))
		settings->SetGPDBPath(wxEmptyString);
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

	if (settings->GetGPDBPath().IsEmpty())
	{
		wxPathList path;

#ifdef __WXMSW__

		wxString GPhint = wxString(getenv( "GPHOME_CLIENTS" ), wxConvUTF8 );

		if (GPhint.Length() > 1)
		{
			path.Add(GPhint + wxT("\\bin"));
			path.Add(GPhint + wxT("\\lib"));
			path.Add(GPhint);
		}

		GPhint = wxString(getenv( "GPHOME" ), wxConvUTF8 );

		if (GPhint.Length() > 1)
		{
			path.Add(GPhint + wxT("\\bin"));
			path.Add(GPhint + wxT("\\lib"));
			path.Add(GPhint);
		}

#endif
		// Look in the app directory for things
		path.Add(loadPath);

#ifdef __WXMAC__

		if (wxDir::Exists(dataDir))
			path.Add(dataDir) ;
#endif
		path.AddEnvList(wxT("PATH"));

#ifdef __WXMSW__
		wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
		wxFileName tmp = path.FindValidPath(wxT("pg_dump"));
#endif

		if (tmp.FileExists())
		{
			if (isGpApp(tmp.GetFullPath()) && settings->GetGPDBPath().IsEmpty())
				settings->SetGPDBPath(tmp.GetPath());
		}
	}

	// Now, if either path is still empty, try a less intelligent search for each

	// PostgreSQL
	if (settings->GetPostgresqlPath().IsEmpty())
	{
		wxPathList path;

#ifdef __WXMSW__
		path.Add(wxT("C:\\PostgresPlus\\8.3\\bin"));

		if (!programFiles.IsEmpty())
		{
			path.Add(programFiles + wxT("\\PostgreSQL\\9.5\\bin"));
			path.Add(programFiles + wxT("\\PostgreSQL\\9.4\\bin"));
			path.Add(programFiles + wxT("\\PostgreSQL\\9.3\\bin"));
			path.Add(programFiles + wxT("\\PostgreSQL\\9.2\\bin"));
			path.Add(programFiles + wxT("\\PostgreSQL\\9.1\\bin"));
			path.Add(programFiles + wxT("\\PostgreSQL\\9.0\\bin"));
			path.Add(programFiles + wxT("\\PostgreSQL\\8.4\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\9.0SS\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\8.4SS\\bin"));
		}

		if (!programFilesX86.IsEmpty())
		{
			path.Add(programFilesX86 + wxT("\\PostgreSQL\\9.5\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgreSQL\\9.4\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgreSQL\\9.3\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgreSQL\\9.2\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgreSQL\\9.1\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgreSQL\\9.0\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgreSQL\\8.4\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\9.0SS\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\8.4SS\\bin"));
		}

		wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
		// Mac paths
		path.Add(wxT("/Library/PostgreSQL/9.5/bin"));
		path.Add(wxT("/Library/PostgreSQL/9.4/bin"));
		path.Add(wxT("/Library/PostgreSQL/9.3/bin"));
		path.Add(wxT("/Library/PostgreSQL/9.2/bin"));
		path.Add(wxT("/Library/PostgreSQL/9.1/bin"));
		path.Add(wxT("/Library/PostgreSQL/9.0/bin"));
		path.Add(wxT("/Library/PostgreSQL/8.4/bin"));
		path.Add(wxT("/Library/PostgresPlus/9.0SS/bin"));
		path.Add(wxT("/Library/PostgresPlus/8.4SS/bin"));

		// Generic Unix paths
		path.Add(wxT("/opt/PostgreSQL/9.5/bin"));
		path.Add(wxT("/opt/PostgreSQL/9.4/bin"));
		path.Add(wxT("/opt/PostgreSQL/9.3/bin"));
		path.Add(wxT("/opt/PostgreSQL/9.2/bin"));
		path.Add(wxT("/opt/PostgreSQL/9.1/bin"));
		path.Add(wxT("/opt/PostgreSQL/9.0/bin"));
		path.Add(wxT("/opt/PostgreSQL/8.4/bin"));
		path.Add(wxT("/opt/PostgresPlus/9.0SS/bin"));
		path.Add(wxT("/opt/PostgresPlus/8.4SS/bin"));
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
		if (!programFiles.IsEmpty())
		{
			path.Add(programFiles + wxT("\\PostgresPlus\\9.5AS\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\9.4AS\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\9.3AS\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\9.2AS\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\9.1AS\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\9.0AS\\bin"));
			path.Add(programFiles + wxT("\\PostgresPlus\\8.4AS\\bin"));
		}

		if (!programFilesX86.IsEmpty())
		{
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\9.5AS\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\9.4AS\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\9.3AS\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\9.2AS\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\9.1AS\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\9.0AS\\bin"));
			path.Add(programFilesX86 + wxT("\\PostgresPlus\\8.4AS\\bin"));
		}

		wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
		// Mac paths
		path.Add(wxT("/Library/PostgresPlus/9.5AS/bin"));
		path.Add(wxT("/Library/PostgresPlus/9.4AS/bin"));
		path.Add(wxT("/Library/PostgresPlus/9.3AS/bin"));
		path.Add(wxT("/Library/PostgresPlus/9.2AS/bin"));
		path.Add(wxT("/Library/PostgresPlus/9.1AS/bin"));
		path.Add(wxT("/Library/PostgresPlus/9.0AS/bin"));
		path.Add(wxT("/Library/PostgresPlus/8.4AS/bin"));

		// Generic Unix paths
		path.Add(wxT("/opt/PostgresPlus/9.5AS/bin"));
		path.Add(wxT("/opt/PostgresPlus/9.4AS/bin"));
		path.Add(wxT("/opt/PostgresPlus/9.3AS/bin"));
		path.Add(wxT("/opt/PostgresPlus/9.2AS/bin"));
		path.Add(wxT("/opt/PostgresPlus/9.1AS/bin"));
		path.Add(wxT("/opt/PostgresPlus/9.0AS/bin"));
		path.Add(wxT("/opt/PostgresPlus/8.4AS/bin"));
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

	// Greenplum
	if (settings->GetGPDBPath().IsEmpty())
	{
		wxPathList path;

#ifdef __WXMSW__
		// Ugly... Greenplum client releases have no predictable numbers, because the path is the server version
		if (!programFiles.IsEmpty())
		{
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-5.0\\bin"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.4\\bin"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.3\\bin"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.2\\bin"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.1\\bin"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.0\\bin"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-3.3\\bin"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-3.2\\bin"));

			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-5.0\\lib"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.4\\lib"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.3\\lib"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.2\\lib"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.1\\lib"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-4.0\\lib"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-3.3\\lib"));
			path.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-3.2\\lib"));
		}

		if (!programFilesX86.IsEmpty())
		{
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-5.0\\bin"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.4\\bin"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.3\\bin"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.2\\bin"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.1\\bin"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.0\\bin"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-3.3\\bin"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-3.2\\bin"));

			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-5.0\\lib"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.4\\lib"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.3\\lib"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.2\\lib"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.1\\lib"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-4.0\\lib"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-3.3\\lib"));
			path.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-3.2\\lib"));
		}


		wxFileName tmp = path.FindValidPath(wxT("pg_dump.exe"));
#else
		// Mac paths

		// Generic Unix paths

		path.Add(wxT("/usr/local/greenplum-clients-5.0/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-5.0/bin"));
		path.Add(wxT("/usr/local/greenplum-clients-4.4/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-4.4/bin"));
		path.Add(wxT("/usr/local/greenplum-clients-4.3/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-4.3/bin"));
		path.Add(wxT("/usr/local/greenplum-clients-4.2/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-4.2/bin"));
		path.Add(wxT("/usr/local/greenplum-clients-4.1/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-4.1/bin"));
		path.Add(wxT("/usr/local/greenplum-clients-4.0/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-4.0/bin"));
		path.Add(wxT("/usr/local/greenplum-clients-3.3/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-3.3/bin"));
		path.Add(wxT("/usr/local/greenplum-clients-3.2/bin"));
		path.Add(wxT("/opt/local/greenplum-clients-3.2/bin"));

		path.Add(wxT("/usr/local/greenplum-clients-5.0/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-5.0/lib"));
		path.Add(wxT("/usr/local/greenplum-clients-4.4/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-4.4/lib"));
		path.Add(wxT("/usr/local/greenplum-clients-4.3/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-4.3/lib"));
		path.Add(wxT("/usr/local/greenplum-clients-4.2/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-4.2/lib"));
		path.Add(wxT("/usr/local/greenplum-clients-4.1/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-4.1/lib"));
		path.Add(wxT("/usr/local/greenplum-clients-4.0/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-4.0/lib"));
		path.Add(wxT("/usr/local/greenplum-clients-3.3/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-3.3/lib"));
		path.Add(wxT("/usr/local/greenplum-clients-3.2/lib"));
		path.Add(wxT("/opt/local/greenplum-clients-3.2/lib"));


		wxFileName tmp = path.FindValidPath(wxT("pg_dump"));
#endif

		if (tmp.FileExists())
		{
			if (isGpApp(tmp.GetFullPath()))
				settings->SetGPDBPath(tmp.GetPath());
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

	gpBackupExecutable  = settings->GetGPDBPath() + wxT("\\pg_dump.exe");
	gpBackupAllExecutable  = settings->GetGPDBPath() + wxT("\\pg_dumpall.exe");
	gpRestoreExecutable = settings->GetGPDBPath() + wxT("\\pg_restore.exe");
#else
	pgBackupExecutable  = settings->GetPostgresqlPath() + wxT("/pg_dump");
	pgBackupAllExecutable  = settings->GetPostgresqlPath() + wxT("/pg_dumpall");
	pgRestoreExecutable = settings->GetPostgresqlPath() + wxT("/pg_restore");

	edbBackupExecutable  = settings->GetEnterprisedbPath() + wxT("/pg_dump");
	edbBackupAllExecutable  = settings->GetEnterprisedbPath() + wxT("/pg_dumpall");
	edbRestoreExecutable = settings->GetEnterprisedbPath() + wxT("/pg_restore");

	gpBackupExecutable  = settings->GetGPDBPath() + wxT("/pg_dump");
	gpBackupAllExecutable  = settings->GetGPDBPath() + wxT("/pg_dumpall");
	gpRestoreExecutable = settings->GetGPDBPath() + wxT("/pg_restore");
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

	if (!isGpApp(gpBackupExecutable))
		gpBackupExecutable = wxEmptyString;
	if (!isGpApp(gpBackupAllExecutable))
		gpBackupAllExecutable = wxEmptyString;
	if (!isGpApp(gpRestoreExecutable))
		gpRestoreExecutable = wxEmptyString;
}

wxString pgAdmin3::LocatePath(const wxString &pathToFind, const bool isFile)
{
	loadPath = wxPathOnly(argv[0]);

	if (loadPath.IsEmpty())
		loadPath = wxT(".");

	loadPath = sanitizePath(loadPath);

#if defined(__WXMSW__)

	// Search for the right paths. We check the following locations:
	//
	// 1) ./xxx               - Running as a standalone install
	// 2) ../pgAdmin/xxx      - Running in a pgInstaller 8.1 installation
	//                          (with the .exe and dlls in the main bin dir)
	// 3) ../xxx or ../../xxx - Running in a development environment

	if (!isFile)
	{
		if (wxDir::Exists(loadPath + pathToFind))
			return sanitizePath(loadPath + pathToFind);
		else if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") + pathToFind))
			return sanitizePath(loadPath + wxT("/../pgAdmin III") + pathToFind);
		else if (wxDir::Exists(loadPath + wxT("/..") + pathToFind))
			return sanitizePath(loadPath + wxT("/..") + pathToFind);
		else if (wxDir::Exists(loadPath + wxT("/../..") + pathToFind))
			return sanitizePath(loadPath + wxT("/../..") + pathToFind);
		else
			return wxEmptyString;
	}
	else
	{
		if (wxFile::Exists(loadPath + pathToFind))
			return sanitizePath(loadPath + pathToFind);
		else if (wxFile::Exists(loadPath + wxT("/../pgAdmin III") + pathToFind))
			return sanitizePath(loadPath + wxT("/../pgAdmin III") + pathToFind);
		else if (wxFile::Exists(loadPath + wxT("/..") + pathToFind))
			return sanitizePath(loadPath + wxT("/..") + pathToFind);
		else if (wxFile::Exists(loadPath + wxT("/../..") + pathToFind))
			return sanitizePath(loadPath + wxT("/../..") + pathToFind);
		else
			return wxEmptyString;
	}

#else

#ifdef __WXMAC__
#if wxCHECK_VERSION(2, 9, 5)
	wxStandardPaths &stdPaths = wxStandardPaths::Get();
#else
	// When using wxStandardPaths on OSX, wx default to the unix,
	// not to the mac variants. Therefore, we request wxStandardPathsCF
	// directly.
	wxStandardPathsCF stdPaths;
#endif
	dataDir = stdPaths.GetDataDir();

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

	if (!isFile)
	{
		if (wxDir::Exists(dataDir + pathToFind))
			return sanitizePath(dataDir + pathToFind);
		else if (wxDir::Exists(loadPath + wxT("/../share/pgadmin3") + pathToFind))
			return sanitizePath(loadPath + wxT("/../share/pgadmin3") + pathToFind);
		else if (wxDir::Exists(loadPath + pathToFind))
			return sanitizePath(loadPath + pathToFind);
		else if (wxDir::Exists(loadPath + wxT("/..") + pathToFind))
			return sanitizePath(loadPath + wxT("/..") + pathToFind);
		else
			return wxEmptyString;
	}
	else
	{
		if (wxFile::Exists(dataDir + pathToFind))
			return sanitizePath(dataDir + pathToFind);
		else if (wxFile::Exists(loadPath + wxT("/../share/pgadmin3") + pathToFind))
			return sanitizePath(loadPath + wxT("/../share/pgadmin3") + pathToFind);
		else if (wxFile::Exists(loadPath + pathToFind))
			return sanitizePath(loadPath + pathToFind);
		else if (wxFile::Exists(loadPath + wxT("/..") + pathToFind))
			return sanitizePath(loadPath + wxT("/..") + pathToFind);
		else
			return wxEmptyString;
	}

#endif
}

void pgAdmin3::InitHelp()
{
	// Windows-only path prefixes
#ifdef __WXMSW__
	wxString programFiles = wxGetenv(wxT("ProgramFiles"));
	wxString programFilesX86 = wxGetenv(wxT("ProgramFiles(x86)"));
#endif

	// Search for external docs. As Windows and *nix etc
	// are likely to be very different, we'll #ifdef them all.
	wxPathList stdPaths, noPaths, pgPaths, edbPaths, gpPaths, slonyPaths;
	wxString sep = wxFileName::GetPathSeparator();

	stdPaths.Add(docPath + sep + settings->GetCanonicalLanguageName());
	stdPaths.Add(docPath + sep + wxT("en_US"));
	stdPaths.Add(docPath);

#ifdef __WXMSW__
	if (!programFiles.IsEmpty())
	{
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.4\\doc"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.4\\doc\\html"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.3\\doc"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.3\\doc\\html"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.2\\doc"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.2\\doc\\html"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.1\\doc"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.1\\doc\\html"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.0\\doc"));
		pgPaths.Add(programFiles + wxT("\\PostgreSQL\\8.0\\doc\\html"));
	}

	if (!programFilesX86.IsEmpty())
	{
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.4\\doc"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.4\\doc\\html"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.3\\doc"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.3\\doc\\html"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.2\\doc"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.2\\doc\\html"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.1\\doc"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.1\\doc\\html"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.0\\doc"));
		pgPaths.Add(programFilesX86 + wxT("\\PostgreSQL\\8.0\\doc\\html"));
	}

	// Note that EDB docs are online from 8.3.
	edbPaths.Add(wxT("C:\\EnterpriseDB\\8.2\\dbserver\\doc"));
	edbPaths.Add(wxT("C:\\EnterpriseDB\\8.2\\dbserver\\doc\\html"));
	edbPaths.Add(wxT("C:\\EnterpriseDB\\8.1\\dbserver\\doc"));
	edbPaths.Add(wxT("C:\\EnterpriseDB\\8.1\\dbserver\\doc\\html"));
	edbPaths.Add(wxT("C:\\EnterpriseDB\\8.0\\dbserver\\doc"));
	edbPaths.Add(wxT("C:\\EnterpriseDB\\8.0\\dbserver\\doc\\html"));

	wxString GPhint = wxString(getenv( "GPHOME_CLIENTS" ), wxConvUTF8 );

	if (GPhint.Length() > 1)
	{
		gpPaths.Add(GPhint + wxT("\\docs"));
		gpPaths.Add(GPhint);
	}

	GPhint = wxString(getenv( "GPHOME" ), wxConvUTF8 );

	if (GPhint.Length() > 1)
	{
		gpPaths.Add(GPhint + wxT("\\docs"));
		gpPaths.Add(GPhint);
	}

	if (!programFiles.IsEmpty())
	{
		gpPaths.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-3.3\\docs"));
		gpPaths.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-3.2\\docs"));
		gpPaths.Add(programFiles + wxT("\\Greenplum\\greenplum-clients-3.1.1.1\\docs"));
	}

	if (!programFilesX86.IsEmpty())
	{
		gpPaths.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-3.3\\docs"));
		gpPaths.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-3.2\\docs"));
		gpPaths.Add(programFilesX86 + wxT("\\Greenplum\\greenplum-clients-3.1.1.1\\docs"));
	}
	;


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

	wxArrayString gpFoundDirs;
	wxString pgDirname1 = wxString(wxT("/usr/local"));
	wxDir gpDir1(pgDirname1);
	if ( gpDir1.IsOpened() )
	{
		wxString gpfilename;
		bool pgcont = gpDir1.GetFirst(&gpfilename, wxT("greenplum-clients*"), wxDIR_DIRS);
		while ( pgcont )
		{
			gpFoundDirs.Add(wxString(pgDirname1 + wxT("/") + gpfilename));
			pgcont = gpDir1.GetNext(&gpfilename);
		}
	}

	wxString pgDirname2 = wxString(wxT("/opt/local"));
	wxDir gpDir2(pgDirname2);
	if ( gpDir2.IsOpened() )
	{
		wxString gpfilename;
		bool pgcont = gpDir2.GetFirst(&gpfilename, wxT("greenplum-clients*"), wxDIR_DIRS);
		while ( pgcont )
		{
			gpFoundDirs.Add(wxString(pgDirname2 + wxT("/") + gpfilename));
			pgcont = gpDir2.GetNext(&gpfilename);
		}
	}

	// make sure that the highest version number comes first
	gpFoundDirs.Sort(true);
	for (wxArrayString::iterator iter = gpFoundDirs.begin(); iter != gpFoundDirs.end(); ++iter)
	{
		wxLogMessage(*iter);
		pgPaths.Add(wxString(*iter));
		pgPaths.Add(wxString(*iter) + wxT("/html"));
		pgPaths.Add(wxString(*iter) + wxT("/docs"));
	}

#endif

	// Slony will be installed into one of the DBMS directories
	slonyPaths.Add(pgPaths);
	slonyPaths.Add(edbPaths);
	slonyPaths.Add(gpPaths);

	// First look for a chm, then a zip, then an hhp file. For PostgreSQL
	// and EnterpriseDB we'll then look for an index.html. No point for
	// Slony as we'd most likely find the DBMS's help.

	wxString pgHelpPath = settings->GetPgHelpPath();
	wxString edbHelpPath = settings->GetEdbHelpPath();
	wxString gpHelpPath = settings->GetGpHelpPath();
	wxString slonyHelpPath = settings->GetSlonyHelpPath();

#if defined (__WXMSW__) || wxUSE_LIBMSPACK
	pgHelpPath = GenerateHelpPath(wxT("PostgreSQL.chm"), pgHelpPath, stdPaths, pgPaths);
	pgHelpPath = GenerateHelpPath(wxT("postgresql.chm"), pgHelpPath, stdPaths, pgPaths);
	pgHelpPath = GenerateHelpPath(wxT("postgres.chm"), pgHelpPath, stdPaths, pgPaths);
	pgHelpPath = GenerateHelpPath(wxT("pgsql.chm"), pgHelpPath, stdPaths, pgPaths);

	edbHelpPath = GenerateHelpPath(wxT("EnterpriseDB.chm"), edbHelpPath, stdPaths, edbPaths);
	edbHelpPath = GenerateHelpPath(wxT("enterprisedb.chm"), edbHelpPath, stdPaths, edbPaths);
	edbHelpPath = GenerateHelpPath(wxT("edb.chm"), edbHelpPath, stdPaths, edbPaths);

	gpHelpPath = GenerateHelpPath(wxT("Greenplum.chm"), gpHelpPath, stdPaths, gpPaths);
	gpHelpPath = GenerateHelpPath(wxT("GPDB.chm"), gpHelpPath, stdPaths, gpPaths);
	gpHelpPath = GenerateHelpPath(wxT("GPSQL.zip"), gpHelpPath, stdPaths, gpPaths);
	gpHelpPath = GenerateHelpPath(wxT("GPClientToolsWin.pdf"), gpHelpPath, stdPaths, gpPaths);
	gpHelpPath = GenerateHelpPath(wxT("GPClientTools.pdf"), gpHelpPath, stdPaths, gpPaths);
	gpHelpPath = GenerateHelpPath(wxT("GPUserGuide.pdf"), gpHelpPath, stdPaths, gpPaths);

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

	gpHelpPath = GenerateHelpPath(wxT("Greenplum.zip"), gpHelpPath, stdPaths, gpPaths);
	gpHelpPath = GenerateHelpPath(wxT("GPSQL.zip"), gpHelpPath, stdPaths, gpPaths);

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

	gpHelpPath = GenerateHelpPath(wxT("Greenplum.hhp"), gpHelpPath, stdPaths, gpPaths);

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

	gpHelpPath = GenerateHelpPath(wxT("index.html"), gpHelpPath, noPaths, gpPaths);
	gpHelpPath = GenerateHelpPath(wxT("GPUserGuide.pdf"), gpHelpPath, stdPaths, gpPaths);

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

	if (gpHelpPath.EndsWith(wxT("index.html")))
	{
		gpHelpPath = gpHelpPath.Left(gpHelpPath.Length() - 10);
#ifndef __WXMSW__
		gpHelpPath = wxT("file:///") + gpHelpPath;
#endif
	}

	// Last resorts - if we still have no help by now, use the websites!
	if (pgHelpPath.IsEmpty())
		pgHelpPath = wxT("http://www.postgresql.org/docs/current/static/");
	if (edbHelpPath.IsEmpty())
		edbHelpPath = wxT("http://www.enterprisedb.com/docs/en/current/server/");
	if (gpHelpPath.IsEmpty())
		gpHelpPath = wxT("http://gpdb.docs.pivotal.io/");
	if (slonyHelpPath.IsEmpty())
		slonyHelpPath = wxT("http://www.slony.info/documentation/");

	// OK, so set the values
	if (settings->GetPgHelpPath().IsEmpty())
		settings->SetPgHelpPath(pgHelpPath);
	if (settings->GetEdbHelpPath().IsEmpty())
		settings->SetEdbHelpPath(edbHelpPath);
	if (settings->GetGpHelpPath().IsEmpty())
		settings->SetGpHelpPath(gpHelpPath);
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
	    chkXRC(wxID_REMOVE) &&
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
	sysLogger::logFile = settings->GetLogFile();
	sysLogger::logLevel = settings->GetLogLevel();

	logger = new sysLogger();
	wxLog::SetVerbose(true);
	wxLog::SetActiveTarget(logger);
	wxLog::Resume();
}


void pgAdmin3::InitNetwork()
{
	// Startup the windows sockets if required
#ifdef __WXMSW__
	WSADATA    wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		wxLogFatalError(__("Cannot initialise the networking subsystem!"));
	}
#endif
	wxSocketBase::Initialize();

	pgConn::ExamineLibpqVersion();
}

#include "images/pgAdmin3-16.pngc"
#include "images/pgAdmin3-32.pngc"
#include "images/splash.pngc"

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

	large_icon = *pgAdmin3_32_png_img;
	small_icon = *pgAdmin3_16_png_img;
	splash_image = *splash_png_img;

	splash_text_colour = wxColour(255, 255, 255);
	report_key_colour = wxColour(0, 154, 206);

	long_appname = wxT("pgAdmin III");
	short_appname = wxT("pgadmin3");
	website_url = wxT("http://www.pgadmin.org/");

	hide_enterprisedb_help = false;
	hide_greenplum_help = false;

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
					large_icon = *pgAdmin3_32_png_img;
				else
					is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("smallicon=")))
			{
				small_icon = wxImage(brandingPath + wxT("/") + token.AfterFirst('=').Trim());
				if (!small_icon.IsOk())
					small_icon = *pgAdmin3_16_png_img;
				else
					is_branded = true;
			}
			else if (token.Lower().StartsWith(wxT("splashimage=")))
			{
				splash_image = wxImage(brandingPath + wxT("/") + token.AfterFirst('=').Trim());
				if (!splash_image.IsOk())
					splash_image = *splash_png_img;
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
			else if (token.Lower().StartsWith(wxT("hideenterprisedbhelp=")))
			{
				if (token.AfterFirst('=').Trim() == wxT("1"))
				{
					hide_enterprisedb_help = true;
					is_branded = true;
				}
			}
			else if (token.Lower().StartsWith(wxT("hidegreenplumhelp=")))
			{
				if (token.AfterFirst('=').Trim() == wxT("1"))
				{
					hide_greenplum_help = true;
					is_branded = true;
				}
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

#if wxCHECK_VERSION(2, 9, 5)
	wxStandardPaths &paths = wxStandardPaths::Get();
#else
	wxStandardPaths paths;
#endif
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

////////////////////////////////////////////////////////////////////////////////
// InitLibpq()
//
//    Dynamically load EDB-specific functions from libpq

#ifdef __WXMSW__
void pgAdmin3::InitLibpq()
{
	HINSTANCE hinstLib;

	// Get a handle to the DLL module.
	hinstLib = LoadLibrary(TEXT("libpq"));

	// If the handle is valid, try to get the function address.
	if (hinstLib != NULL)
	{
		PQiGetOutResult = (PQGETOUTRESULT) GetProcAddress(hinstLib, "PQgetOutResult");
		PQiPrepareOut = (PQPREPAREOUT) GetProcAddress(hinstLib, "PQprepareOut");
		PQiSendQueryPreparedOut = (PQSENDQUERYPREPAREDOUT) GetProcAddress(hinstLib, "PQsendQueryPreparedOut");

		// If the function address is valid, call the function.
		if (PQiGetOutResult != NULL)
			wxLogInfo(wxT("Using runtime dynamically linked EDB libpq functions."));
		else
			wxLogInfo(wxT("EDB libpq functions are not available."));
	}
}
#endif

