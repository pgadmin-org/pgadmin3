//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgAdmin3.h - The main application header
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGADMIN3_H
#define PGADMIN3_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/hashmap.h>
#include <wx/colordlg.h>
#include <wx/listctrl.h>
#include <wx/xrc/xmlres.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include "utils/misc.h"
#include <ctl/ctlTree.h>
#include "ctl/ctlSQLBox.h"
#include "ctl/ctlListView.h"
#include "ctl/ctlComboBox.h"
#include <ctl/ctlCheckTreeView.h>
#include <ctl/ctlColourPicker.h>
#include "dlg/dlgClasses.h"
#include "db/pgConn.h"
#include "db/pgSet.h"
#include "utils/factory.h"

#include "precomp.h"

// App headers
#include "utils/sysSettings.h"

#ifdef __WXMSW__
#else
#include "config.h"
#undef VERSION
#endif

// Check the wxWidgets config
#if !wxCHECK_VERSION(2, 8, 0)
#error wxWidgets 2.8.0 or higher is required to compile this version of pgAdmin.
#endif

#if !wxUSE_UNICODE
#error wxWidgets must be compiled with Unicode support to build pgAdmin.
#endif

// Supported server minimum and maximum values.
const short SERVER_MIN_VERSION_N = 0x0804;
const wxString SERVER_MIN_VERSION_T = wxT("8.4");
const short SERVER_MAX_VERSION_N = 0x0906;
const wxString SERVER_MAX_VERSION_T = wxT("9.6");

// Supported Greenplum Database and Greenplum HAWQ minimum and maximum values.
const short GP_MIN_VERSION_N = 0x0802;
const wxString GP_MIN_VERSION_T = wxT("8.2");
const short GP_MAX_VERSION_N = 0x0803;
const wxString GP_MAX_VERSION_T = wxT("8.3");

// The registry file
#ifndef __WXMSW__
#define REGISTRY_FILE wxT("/etc/postgres-reg.ini")
#endif

// Some redefines for modern Microsoft compilers
#if defined(_MSC_VER)
#define creat _creat
#define close _close
#define mkdir _mkdir
#define sprintf _sprintf
#define snprintf _snprintf
#define strcat _strcat
#define strdup _strdup
#define stricmp _stricmp
#define strincmp _strincmp
#endif

extern wxPathList path;                 // The search path
extern wxString loadPath;               // Where the program is loaded from
extern wxString docPath;                // Where docs are stored
extern wxString uiPath;                 // Where ui data is stored
extern wxString i18nPath;               // Where i18n data is stored
extern wxString pluginsDir;             // The plugins ini file directory
extern wxString settingsIni;            // The default settings file

extern sysSettings *settings;           // The settings manager

extern frmMain *winMain;                // The main app window

extern wxLocale *locale;                // Application locale
extern wxArrayInt existingLangs;        // Language IDs
extern wxArrayString existingLangNames; // Language Names

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
class CSSHTunnelThread;
extern CSSHTunnelThread *pgadminTunnelThread; // SSH Tunneling Thread Object
#endif

// Helper app paths - PG
extern wxString pgBackupExecutable;
extern wxString pgBackupAllExecutable;
extern wxString pgRestoreExecutable;

// Helper app paths - EDB
extern wxString edbBackupExecutable;
extern wxString edbBackupAllExecutable;
extern wxString edbRestoreExecutable;

// Helper app paths - Greenplum
extern wxString gpBackupExecutable;
extern wxString gpBackupAllExecutable;
extern wxString gpRestoreExecutable;

//
// Support for additional functions included in the EnterpriseDB
// version of libpq. These are enable via runtime loading of the
// functions on Windows, and a configure time macro on other
// platforms (EDB_LIBPQ).
//
// Currently, these are only use to support EDB callable statements
// so the debugger can grab OUT/INOUT parameters from EDB stored
// procedures.
//
#ifdef __WXMSW__
// Dynamically loaded PQgetOutResult
typedef PGresult *(*PQGETOUTRESULT)(PGconn *);
extern PQGETOUTRESULT PQiGetOutResult;
#define PQiGetOutResult (PQiGetOutResult)

// Dynamically loaded PQprepareOut
typedef PGresult *(*PQPREPAREOUT)(PGconn *, const char *, const char *, int, const Oid *, const int *);
extern PQPREPAREOUT PQiPrepareOut;
#define PQiPrepareOut (PQiPrepareOut)

// Dynamically loaded PQsendQueryPreparedOut
typedef int (*PQSENDQUERYPREPAREDOUT)(PGconn *, const char *, int, const char *const *, const int *, const int *, int);
extern PQSENDQUERYPREPAREDOUT PQiSendQueryPreparedOut;
#define PQiSendQueryPreparedOut (PQiSendQueryPreparedOut)

#else
#ifdef EDB_LIBPQ
#define PQiGetOutResult PQgetOutResult
#define PQiPrepareOut PQprepareOut
#define PQiSendQueryPreparedOut PQsendQueryPreparedOut
#endif
#endif

// Simple hash map used as an ad-hoc data cache
WX_DECLARE_STRING_HASH_MAP(wxString, cacheMap);

// Class declarations
class pgAdmin3 : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();

#ifdef __WXMAC__
	void MacOpenFile(const wxString &fileName);
#endif

private:
	wxString LocatePath(const wxString &pathToFind, const bool isFile);
	wxString GenerateHelpPath(const wxString &file, const wxString &current, wxPathList stdPaths, wxPathList dbmsPaths);
	bool LoadAllXrc(const wxString dir);

#ifdef __WXMAC__
	wxString macFileToOpen;
#endif

protected:
	void InitAppPaths();
	void InitXtraPaths();
	void InitHelp();
	void InitLogger();
	void InitNetwork();
	void InitXml();

#ifdef __WXMSW__
	void InitLibpq();
#endif

};

class pgAppearanceFactory
{
public:
	pgAppearanceFactory();

	void SetIcons(wxDialog *dlg);
	void SetIcons(wxTopLevelWindow *dlg);
	wxIcon GetSmallIconImage();
	wxIcon GetBigIconImage();
	wxBitmap GetSplashImage()
	{
		return wxBitmap(splash_image);
	};
	wxFont GetSplashTextFont();
	wxColour GetSplashTextColour()
	{
		return splash_text_colour;
	};
	long GetSplashTextOffset()
	{
		return splash_pos_offset;
	};
	wxPoint GetSplashTextPos()
	{
		return wxPoint(splash_pos_x, splash_pos_y);
	};
	wxString GetShortAppName()
	{
		return short_appname;
	};
	wxString GetLongAppName()
	{
		return long_appname;
	};
	wxString GetWebsiteUrl()
	{
		return website_url;
	};
	wxColour GetReportKeyColour()
	{
		return report_key_colour;
	};
	bool GetHideEnterprisedbHelp()
	{
		return hide_enterprisedb_help;
	};
	bool GetHideGreenplumHelp()
	{
		return hide_greenplum_help;
	};
	bool IsBranded()
	{
		return is_branded;
	};

private:
	wxString long_appname, short_appname, website_url, icon;
	wxImage large_icon, small_icon, splash_image;
	long splash_font_size, splash_pos_x, splash_pos_y, splash_pos_offset;
	wxColor splash_text_colour, report_key_colour;
	bool hide_enterprisedb_help, hide_greenplum_help, is_branded;
};

extern pgAppearanceFactory *appearanceFactory;


#endif // PGADMIN3_H
