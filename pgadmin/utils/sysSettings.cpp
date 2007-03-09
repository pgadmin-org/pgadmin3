//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// sysSettings.cpp - Settings handling class
//
// Note: This class stores and manages all the applications settings.
//       Settings are all read in the ctor, but may be written either in
//       the relevant SetXXX() member function for rarely written settings
//       or in the dtor for reguarly changed settings such as form sizes.
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"


// wxWindows headers
#include <wx/wx.h>
#include <wx/config.h>
#include <wx/url.h>
#include <wx/stdpaths.h>

// App headers
#include "utils/sysSettings.h"
#include "utils/sysLogger.h"
#include "utils/misc.h"

extern wxString docPath;



sysSettings::sysSettings(const wxString& name) : wxConfig(name)
{

    // Convert setting from pre-1.3
#ifdef __WXMSW__
    DWORD type=0;
    HKEY hkey=0;
    RegOpenKeyEx(HKEY_CURRENT_USER, wxT("Software\\") + GetAppName(), 0, KEY_READ, &hkey);
    if (hkey)
    {
        RegQueryValueEx(hkey, wxT("ShowTipOfTheDay"), 0, &type, 0, 0);
        if (type == REG_DWORD)
        {
            long value;
            Read(wxT("ShowTipOfTheDay"), &value, 0L);

            Write(wxT("ShowTipOfTheDay"), value != 0);
        }
        RegCloseKey(hkey);
    }
#endif


    // Convert settings from pre-1.5
    long i, serverCount;
    Read(wxT("Servers/Count"), &serverCount, 0L);
    for (i=1 ; i <= serverCount ; i++)
    {
        if (moveStringValue(wxT("Servers/Database%d"), wxT("Servers/%d/Database"), i))
        {
            moveStringValue(wxT("Servers/Description%d"), wxT("Servers/%d/Description"), i);
            moveStringValue(wxT("Servers/LastDatabase%d"), wxT("Servers/%d/LastDatabase"), i);
            moveStringValue(wxT("Servers/LastSchema%d"), wxT("Servers/%d/LastSchema"), i);
            moveStringValue(wxT("Servers/Server%d"), wxT("Servers/%d/Server"), i);
            moveStringValue(wxT("Servers/ServiceId%d"), wxT("Servers/%d/ServiceId"), i);
            moveStringValue(wxT("Servers/StorePWD%d"), wxT("Servers/%d/StorePWD"), i);
            moveStringValue(wxT("Servers/Username%d"), wxT("Servers/%d/Username"), i);
            moveLongValue(wxT("Servers/Port%d"), wxT("Servers/%d/Port"), i);
            moveLongValue(wxT("Servers/SSL%d"), wxT("Servers/%d/SSL"), i);
        }
    }

        
    // Tip Of The Day
    Read(wxT("ShowTipOfTheDay"), &showTipOfTheDay, true); 
    Read(wxT("NextTipOfTheDay"), &nextTipOfTheDay, 0); 

    // Log. Try to get a vaguely usable default path.
    char *homedir;
#ifdef __WXMSW__
    char *homedrive;
#endif

    wxString deflog;
    
#ifdef __WXMSW__
    homedrive = getenv("HOMEDRIVE");
    homedir = getenv("HOMEPATH");
#else
    homedir = getenv("HOME");
#endif

    if (!homedir)
        deflog = wxT("pgadmin.log");
    else 
    {
        
#ifdef __WXMSW__
        deflog = wxString::FromAscii(homedrive);
        deflog += wxString::FromAscii(homedir);
        deflog += wxT("\\pgadmin.log");
#else
        deflog = wxString::FromAscii(homedir);
        deflog += wxT("/pgadmin.log");
#endif
    }

    Read(wxT("LogFile"), &logFile, deflog); 
    Read(wxT("LogLevel"), &logLevel, LOG_ERRORS);
    sysLogger::logFile = logFile;
    sysLogger::logLevel = logLevel;

    // Last Connection
    Read(wxT("LastServer"), &lastServer, wxT("localhost")); 
    Read(wxT("LastDatabase"), &lastDatabase, wxEmptyString); 
	Read(wxT("LastDescription"), &lastDescription, wxT("PostgreSQL Server")); 
    Read(wxT("LastUsername"), &lastUsername, wxT("postgres")); 
    Read(wxT("LastPort"), &lastPort, 5432);
    Read(wxT("LastSSL"), &lastSSL, 0);

    // Show System Objects
    Read(wxT("ShowSystemObjects"), &showSystemObjects, false); 

//    Read(wxT("SqlHelpSite"), &sqlHelpSite, docPath + wxT("/en_US/pg/"));
    Read(wxT("SqlHelpSite"), &sqlHelpSite, wxT(""));
    if (sqlHelpSite.length() > 0) {
        if (sqlHelpSite.Last() != '/' && sqlHelpSite.Last() != '\\')
            sqlHelpSite += wxT("/");
    }
    Read(wxT("Proxy"), &proxy, wxGetenv(wxT("HTTP_PROXY")));
    SetProxy(proxy);

    maxRows=Read(wxT("frmQuery/MaxRows"), 100L);
    maxColSize=Read(wxT("frmQuery/MaxColSize"), 256L);
    Read(wxT("frmQuery/ExplainVerbose"), &explainVerbose, false);
    Read(wxT("frmQuery/ExplainAnalyze"), &explainAnalyze, false);
    askSaveConfirmation=StrToBool(Read(wxT("AskSaveConfirmation"), wxT("Yes")));
    confirmDelete=StrToBool(Read(wxT("ConfirmDelete"), wxT("Yes")));
    showUsersForPrivileges=StrToBool(Read(wxT("ShowUsersForPrivileges"), wxT("No")));
    Read(wxT("AutoRowCount"), &autoRowCountThreshold, 2000L);
    Read(wxT("IndentSpaces"), &indentSpaces, 0L);
	Read(wxT("TabForCompletion"), &tabForCompletion, false);
    Read(wxT("StickySql"), &stickySql, false);
    Read(wxT("frmQuery/IndicateNull"), &indicateNull, false);
    Read(wxT("DoubleClickProperties"), &doubleClickProperties, true);
    Read(wxT("SuppressGuruHints"), &suppressGuruHints, false);
    Read(wxT("WriteUnicodeFile"), &unicodeFile, false);
    Read(wxT("SystemSchemas"), &systemSchemas, wxEmptyString);
    Read(wxT("MaxServerLogSize"), &maxServerLogSize, 100000L);
    Read(wxT("Export/Unicode"), &exportUnicode, false);
    Read(wxT("SlonyPath"), &slonyPath, wxEmptyString);
    Read(wxT("PostgreSQLPath"), &postgresqlPath, wxEmptyString);
    Read(wxT("EnterpriseDBPath"), &enterprisedbPath, wxEmptyString);

    wxString val;
#ifdef __WXMSW__
    Read(wxT("Export/RowSeparator"), &val, wxT("CR/LF"));
#else
    Read(wxT("Export/RowSeparator"), &val, wxT("LF"));
#endif
    if (val == wxT("CRLF"))
        exportRowSeparator = wxT("\r\n");
    else
        exportRowSeparator = wxT("\n");
    Read(wxT("Export/ColSeparator"), &exportColSeparator, wxT(";"));
    Read(wxT("Export/QuoteChar"), &exportQuoteChar, wxT("\""));
    Read(wxT("Export/Quote"), &val, wxT("Strings"));
    if (val == wxT("All"))
        exportQuoting = 2;
    else if (val == wxT("Strings"))
        exportQuoting = 1;
    else
        exportQuoting = 0;
	Read(wxT("Copy/ColSeparator"), &copyColSeparator, wxT(";"));
	Read(wxT("Copy(QuoteChar"), &copyQuoteChar, wxT("\""));
	Read(wxT("Copy/Quote"), &val, wxT("Strings"));
	if (val == wxT("All"))
		copyQuoting = 2;
	else if (val == wxT("Strings"))
		copyQuoting = 1;
	else
		copyQuoting = 0;



    const wxLanguageInfo *langInfo;
    langInfo = wxLocale::GetLanguageInfo(Read(wxT("LanguageId"), wxLANGUAGE_UNKNOWN));
    if (langInfo)
        canonicalLanguage=langInfo->CanonicalName;

	wxString fontName;
    Read(wxT("Font"), &fontName, wxEmptyString);

    if (fontName.IsEmpty())
        systemFont = wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT);
    else
        systemFont = wxFont(fontName);

    Read(wxT("frmQuery/Font"), &fontName, wxEmptyString);

    if (fontName.IsEmpty())
    {
#ifdef __WXMSW__
        sqlFont = wxFont(9, wxTELETYPE, wxNORMAL, wxNORMAL);
#else
        sqlFont = wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
#endif
    }
    else
    	sqlFont = wxFont(fontName);
}


sysSettings::~sysSettings()
{
    wxLogInfo(wxT("Destroying sysSettings object and saving settings"));
    // frMain size/position
	Save();
}

bool sysSettings::GetDisplayOption(const wxString &objtype)
{
	bool retval, def = true;
	wxString engtype;

	if (objtype == _("Databases")) 
		engtype = wxT("Databases");
	else if (objtype == _("Tablespaces")) 
		engtype = wxT("Tablespaces");
	else if (objtype == _("pgAgent jobs")) 
		engtype = wxT("pgAgent jobs");
	else if (objtype == _("Groups/group roles")) 
		engtype = wxT("Groups/login roles");
	else if (objtype == _("Users/login roles")) 
		engtype = wxT("Users/login roles");
	else if (objtype == _("Catalogs")) 
		engtype = wxT("Catalogs");
	else if (objtype == _("Casts")) 
	{
		engtype = wxT("Casts");
		def = false;
	}
	else if (objtype == _("Languages")) 
	{
		engtype = wxT("Languages");
		def = false;
	}
	else if (objtype == _("Public synonyms")) 
		engtype = wxT("Public synonyms");
	else if (objtype == _("Schemas")) 
		engtype = wxT("Schemas");
	else if (objtype == _("Slony-I clusters")) 
		engtype = wxT("Slony-I clusters");
	else if (objtype == _("Aggregates")) 
	{
		engtype = wxT("Aggregates");
		def = false;
	}
	else if (objtype == _("Conversions"))
	{
		engtype = wxT("Conversions");
		def = false;
	}
	else if (objtype == _("Domains")) 
		engtype = wxT("Domains");
	else if (objtype == _("Functions")) 
		engtype = wxT("Functions");
	else if (objtype == _("Trigger functions")) 
		engtype = wxT("Trigger functions");
	else if (objtype == _("Packages")) 
		engtype = wxT("Packages");
	else if (objtype == _("Procedures")) 
		engtype = wxT("Procedures");
	else if (objtype == _("Operators")) 
	{
		engtype = wxT("Operators");
		def = false;
	}
	else if (objtype == _("Operator classes")) 
	{
		engtype = wxT("Operator classes");
		def = false;
	}
	else if (objtype == _("Operator families")) 
	{
		engtype = wxT("Operator families");
		def = false;
	}
	else if (objtype == _("Rules")) 
	{
		engtype = wxT("Rules");
		def = false;
	}
	else if (objtype == _("Sequences")) 
		engtype = wxT("Sequences");
	else if (objtype == _("Tables")) 
		engtype = wxT("Tables");
	else if (objtype == _("Types")) 
	{
		engtype = wxT("Types");
		def = false;
	}
	else if (objtype == _("Views")) 
		engtype = wxT("Views");

	Read(wxT("Display/") + objtype, &retval, def);

	return retval;
}

void sysSettings::SetDisplayOption(const wxString &objtype, bool display)
{
	wxString engtype;

	if (objtype == _("Databases")) engtype = wxT("Databases");
	else if (objtype == _("Tablespaces")) engtype = wxT("Tablespaces");
	else if (objtype == _("pgAgent jobs")) engtype = wxT("pgAgent jobs");
	else if (objtype == _("Groups/group roles")) engtype = wxT("Groups/login roles");
	else if (objtype == _("Users/login roles")) engtype = wxT("Users/login roles");
	else if (objtype == _("Catalogs")) engtype = wxT("Catalogs");
	else if (objtype == _("Casts")) engtype = wxT("Casts");
	else if (objtype == _("Languages")) engtype = wxT("Languages");
	else if (objtype == _("Schemas")) engtype = wxT("Schemas");
	else if (objtype == _("Slony-I clusters")) engtype = wxT("Slony-I clusters");
	else if (objtype == _("Aggregates")) engtype = wxT("Aggregates");
	else if (objtype == _("Conversions")) engtype = wxT("Conversions");
	else if (objtype == _("Domains")) engtype = wxT("Domains");
	else if (objtype == _("Functions")) engtype = wxT("Functions");
	else if (objtype == _("Trigger functions")) engtype = wxT("Trigger functions");
	else if (objtype == _("Procedures")) engtype = wxT("Procedures");
	else if (objtype == _("Operators")) engtype = wxT("Operators");
	else if (objtype == _("Operator classes")) engtype = wxT("Operator classes");
	else if (objtype == _("Operator families")) engtype = wxT("Operator families");
	else if (objtype == _("Rules")) engtype = wxT("Rules");
	else if (objtype == _("Sequences")) engtype = wxT("Sequences");
	else if (objtype == _("Tables")) engtype = wxT("Tables");
	else if (objtype == _("Types")) engtype = wxT("Types");
	else if (objtype == _("Views")) engtype = wxT("Views");

	Write(wxT("Display/") + engtype, display);
}

bool sysSettings::moveStringValue(wxChar *oldKey, wxChar *newKey, int index)
{
    wxString k1, k2;
    if (index >= 0)
    {
        k1.Printf(oldKey, index);
        k2.Printf(newKey, index);
    }
    else
    {
        k1=oldKey;
        k2=newKey;
    }

    if (!Exists(k2) && Exists(k1))
    {
        wxString value;
        Read(k1, &value, wxEmptyString);
        Write(k2, value);

        return true;
    }

    return false;
}



bool sysSettings::moveLongValue(wxChar *oldKey, wxChar *newKey, int index)
{
    wxString k1, k2;
    if (index >= 0)
    {
        k1.Printf(oldKey, index);
        k2.Printf(newKey, index);
    }
    else
    {
        k1=oldKey;
        k2=newKey;
    }

    if (!Exists(k2) && Exists(k1))
    {
        long value;
        Read(k1, &value, 0L);
        Write(k2, value);

        return true;
    }

    return false;
}

void sysSettings::Save()
{
    Write(wxT("LogFile"), logFile);
    Write(wxT("LogLevel"), logLevel);

    Write(wxT("frmQuery/MaxRows"), maxRows);
    Write(wxT("frmQuery/MaxColSize"), maxColSize);
    Write(wxT("frmQuery/ExplainVerbose"), explainVerbose);
    Write(wxT("frmQuery/ExplainAnalyze"), explainAnalyze);
	Write(wxT("frmQuery/Font"), sqlFont.GetNativeFontInfoDesc());
    Write(wxT("AskSaveConfirmation"), BoolToStr(askSaveConfirmation));
    Write(wxT("ConfirmDelete"), BoolToStr(confirmDelete));
    Write(wxT("ShowUsersForPrivileges"), BoolToStr(showUsersForPrivileges));
    Write(wxT("SqlHelpSite"), sqlHelpSite);
    Write(wxT("Proxy"), proxy);
    Write(wxT("AutoRowCount"), autoRowCountThreshold);
    Write(wxT("WriteUnicodeFile"), unicodeFile);
    Write(wxT("SystemSchemas"), systemSchemas);
    Write(wxT("MaxServerLogSize"), maxServerLogSize);
    Write(wxT("SuppressGuruHints"), suppressGuruHints);
    Write(wxT("SlonyPath"), slonyPath);
    Write(wxT("PostgreSQLPath"), postgresqlPath);
    Write(wxT("EnterpriseDBPath"), enterprisedbPath);
    Write(wxT("IndentSpaces"), indentSpaces);
	Write(wxT("TabForCompletion"), tabForCompletion);

    Write(wxT("Export/Unicode"), exportUnicode);
    Write(wxT("Export/QuoteChar"), exportQuoteChar);
    Write(wxT("Export/ColSeparator"), exportColSeparator);
    if (exportRowSeparator == wxT("\r\n"))
        Write(wxT("Export/RowSeparator"), wxT("CR/LF"));
    else
        Write(wxT("Export/RowSeparator"), wxT("LF"));


    switch(exportQuoting)
    {
        case 2:
            Write(wxT("Export/Quote"), wxT("All"));
            break;
        case 1:
            Write(wxT("Export/Quote"), wxT("Strings"));
            break;
        case 0:
            Write(wxT("Export/Quote"), wxT("None"));
            break;
        default:
            break;
    }

	Write(wxT("Copy/QuoteChar"), copyQuoteChar);
	Write(wxT("Copy/ColSeparator"), copyColSeparator);
	switch (copyQuoting)
	{
		case 2:
			Write(wxT("Copy/Quote"), wxT("All"));
			break;
		case 1:
			Write(wxT("Copy/Quote"), wxT("Strings"));
			break;
		case 0:
			Write(wxT("Copy/Quote"), wxT("None"));
			break;
		default:
			break;
	}

    wxString fontName = systemFont.GetNativeFontInfoDesc();

	if (fontName == wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT).GetNativeFontInfoDesc())
        Write(wxT("Font"), wxEmptyString);
    else
        Write(wxT("Font"), fontName);
}


void sysSettings::SetProxy(const wxString &s)
{
    proxy=s;
    if (!s.IsEmpty() && s.Find(':') < 0)
        proxy += wxT(":80");
    wxURL::SetDefaultProxy(proxy);
}


bool sysSettings::Read(const wxString& key, bool *val, bool defaultVal) const
{
    wxString str;
    Read(key, &str, BoolToStr(defaultVal));
	*val = StrToBool(str);
	return true;
}

bool sysSettings::Write(const wxString &key, bool value)
{
    return Write(key, BoolToStr(value));
}

bool sysSettings::Write(const wxString &key, const wxPoint &value)
{
    bool rc=wxConfig::Write(key + wxT("/Left"), value.x);
    if (rc)
        rc=wxConfig::Write(key + wxT("/Top"), value.y);
    return rc;
}


bool sysSettings::Write(const wxString &key, const wxSize &value)
{
    bool rc=wxConfig::Write(key + wxT("/Width"), value.x);
    if (rc)
        rc=wxConfig::Write(key + wxT("/Height"), value.y);
    return rc;
}


wxPoint sysSettings::Read(const wxString& key, const wxPoint &defaultVal) const
{
    return wxPoint(wxConfig::Read(key + wxT("/Left"), defaultVal.x), 
                   wxConfig::Read(key + wxT("/Top"), defaultVal.y));
}


wxSize sysSettings::Read(const wxString& key, const wxSize &defaultVal) const
{
    return wxSize(wxConfig::Read(key + wxT("/Width"), defaultVal.x), 
                  wxConfig::Read(key + wxT("/Height"), defaultVal.y));
}

//////////////////////////////////////////////////////////////////////////
// Tip of the Day
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetShowTipOfTheDay(const bool newval)
{
    showTipOfTheDay = newval;
    Write(wxT("ShowTipOfTheDay"), showTipOfTheDay);
}

void sysSettings::SetNextTipOfTheDay(const int newval)
{
    nextTipOfTheDay = newval;
    Write(wxT("NextTipOfTheDay"), nextTipOfTheDay);
}

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetLogFile(const wxString& newval)
{
    logFile = newval;
    sysLogger::logFile = newval;
}

void sysSettings::SetLogLevel(const int newval)
{
    logLevel = newval;
    sysLogger::logLevel = newval;
}

//////////////////////////////////////////////////////////////////////////
// Last Connection
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetLastServer(const wxString& newval)
{
    lastServer = newval;
    Write(wxT("LastServer"), lastServer);
}

void sysSettings::SetLastDescription(const wxString& newval)
{
    lastDescription = newval;
    Write(wxT("LastDescription"), lastDescription);
}

void sysSettings::SetLastDatabase(const wxString& newval)
{
    lastDatabase = newval;
    Write(wxT("LastDatabase"), lastDatabase);
}

void sysSettings::SetLastUsername(const wxString& newval)
{
    lastUsername = newval;
    Write(wxT("LastUsername"), lastUsername);
}

void sysSettings::SetLastPort(const int newval)
{
    lastPort = newval;
    Write(wxT("LastPort"), lastPort);
}

void sysSettings::SetLastSSL(const int newval)
{
    lastSSL = newval;
    Write(wxT("LastSSL"), lastSSL);
}

//////////////////////////////////////////////////////////////////////////
// Show System Objects
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetShowSystemObjects(const bool newval)
{
    showSystemObjects = newval;
    Write(wxT("ShowSystemObjects"), showSystemObjects);
}


//////////////////////////////////////////////////////////////////////////
// Sticky SQL
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetStickySql(const bool newval)
{
    stickySql = newval;
    Write(wxT("StickySql"), stickySql);
}

//////////////////////////////////////////////////////////////////////////
// IndicateNull
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetIndicateNull(const bool newval)
{
    indicateNull = newval;
    Write(wxT("frmQuery/IndicateNull"), indicateNull);
}

//////////////////////////////////////////////////////////////////////////
// Double click for properties
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetDoubleClickProperties(const bool newval)
{
    doubleClickProperties = newval;
    Write(wxT("DoubleClickProperties"), doubleClickProperties);
}

//////////////////////////////////////////////////////////////////////////
// Get system wide configuration file names
//////////////////////////////////////////////////////////////////////////
wxString sysSettings::GetConfigFile(configFileName cfgname)
{
	if (cfgname == PGPASS || cfgname == PGAFAVOURITES)
	{
	    wxStandardPaths stdp;
	    wxString fname=stdp.GetUserConfigDir()
#ifdef WIN32
	        + wxT("\\postgresql"); 
	    mkdir(fname.ToAscii());
		fname += (cfgname==PGPASS)?wxT("\\pgpass.conf"):wxT("\\pgadmin_favourites.xml");
#else
			+ ((cfgname==PGPASS)?wxT("/.pgpass"):wxT("/.pgadminfavourites"));
#endif
		return fname;
	}
	return wxT("");
}
