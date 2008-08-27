//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
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
#include <wx/fileconf.h>
#include <wx/url.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>

// App headers
#include "utils/sysSettings.h"
#include "utils/sysLogger.h"
#include "utils/misc.h"

sysSettings::sysSettings(const wxString& name) : wxConfig(name)
{
	// Open the default settings file
	defaultSettings = NULL;
	if (!settingsIni.IsEmpty())
	{
	    wxFileStream fst(settingsIni);
        defaultSettings = new wxFileConfig(fst);
	}	

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
			wxConfig::Read(wxT("ShowTipOfTheDay"), &value, 0L);

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
}

bool sysSettings::GetDisplayOption(const wxString &objtype, bool GetDefault)
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
        engtype = wxT("Groups-login roles");
    else if (objtype == _("Users/login roles")) 
        engtype = wxT("Users-login roles");
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
    else if (objtype == _("FTS Configurations")) 
        engtype = wxT("FTS Configurations");
    else if (objtype == _("FTS Dictionaries")) 
        engtype = wxT("FTS Dictionaries");
    else if (objtype == _("FTS Parsers")) 
        engtype = wxT("FTS Parsers");
    else if (objtype == _("FTS Templates")) 
        engtype = wxT("FTS Templates");
    else if (objtype == _("Types")) 
    {
        engtype = wxT("Types");
        def = false;
    }
    else if (objtype == _("Views")) 
        engtype = wxT("Views");

    // If we just want the default, return it.
    if (GetDefault)
        return def;
 
    // Otherwise get the reg value.
    Read(wxT("Display/") + engtype, &retval, def);
    return retval;
}

void sysSettings::SetDisplayOption(const wxString &objtype, bool display)
{
    wxString engtype;

    if (objtype == _("Databases")) engtype = wxT("Databases");
    else if (objtype == _("Tablespaces")) engtype = wxT("Tablespaces");
    else if (objtype == _("pgAgent jobs")) engtype = wxT("pgAgent jobs");
    else if (objtype == _("Groups/group roles")) engtype = wxT("Groups-login roles");
    else if (objtype == _("Users/login roles")) engtype = wxT("Users-login roles");
    else if (objtype == _("Catalogs")) engtype = wxT("Catalogs");
    else if (objtype == _("Casts")) engtype = wxT("Casts");
    else if (objtype == _("Languages")) engtype = wxT("Languages");
    else if (objtype == _("Public synonyms")) engtype = wxT("Public synonyms");
    else if (objtype == _("Schemas")) engtype = wxT("Schemas");
    else if (objtype == _("Slony-I clusters")) engtype = wxT("Slony-I clusters");
    else if (objtype == _("Aggregates")) engtype = wxT("Aggregates");
    else if (objtype == _("Conversions")) engtype = wxT("Conversions");
    else if (objtype == _("Domains")) engtype = wxT("Domains");
    else if (objtype == _("Functions")) engtype = wxT("Functions");
    else if (objtype == _("Trigger functions")) engtype = wxT("Trigger functions");
    else if (objtype == _("Packages")) engtype = wxT("Packages");
    else if (objtype == _("Procedures")) engtype = wxT("Procedures");
    else if (objtype == _("Operators")) engtype = wxT("Operators");
    else if (objtype == _("Operator classes")) engtype = wxT("Operator classes");
    else if (objtype == _("Operator families")) engtype = wxT("Operator families");
    else if (objtype == _("Rules")) engtype = wxT("Rules");
    else if (objtype == _("Sequences")) engtype = wxT("Sequences");
    else if (objtype == _("Tables")) engtype = wxT("Tables");
    else if (objtype == _("FTS Configurations")) engtype = wxT("FTS Configurations");
    else if (objtype == _("FTS Dictionaries")) engtype = wxT("FTS Dictionaries");
    else if (objtype == _("FTS Parsers")) engtype = wxT("FTS Parsers");
    else if (objtype == _("FTS Templates")) engtype = wxT("FTS Templates");
    else if (objtype == _("Types")) engtype = wxT("Types");
    else if (objtype == _("Views")) engtype = wxT("Views");

    Write(wxT("Display/") + engtype, display);
}

bool sysSettings::moveStringValue(const wxChar *oldKey, const wxChar *newKey, int index)
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



bool sysSettings::moveLongValue(const wxChar *oldKey, const wxChar *newKey, int index)
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

// Read a string value
bool sysSettings::Read(const wxString& key, wxString* str, const wxString& defaultVal) const
{ 
	wxString actualDefault = defaultVal;

	// Get the default from the defaults file, in preference 
	// to the hardcoded value
	if (defaultSettings)
        defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, str, actualDefault); 
}

// Return a string value
wxString sysSettings::Read(const wxString& key, const wxString &defaultVal) const
{ 
	wxString actualDefault = defaultVal;

	// Get the default from the defaults file, in preference 
	// to the hardcoded value
    if (defaultSettings)
        defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, actualDefault); 
}

// Read an int value
bool sysSettings::Read(const wxString& key, int* i, int defaultVal) const
{ 
	int actualDefault = defaultVal;
	
	// Get the default from the defaults file, in preference 
	// to the hardcoded value
	if (defaultSettings)
        defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, i, actualDefault); 
}

// Read a long value
bool sysSettings::Read(const wxString& key, long* l, long defaultVal) const
{ 
	long actualDefault = defaultVal;
	
	// Get the default from the defaults file, in preference 
	// to the hardcoded value
	if (defaultSettings)
        defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, l, actualDefault); 
}


// Return a long value
long sysSettings::Read(const wxString& key, long defaultVal) const
{ 
	long actualDefault = defaultVal;
	
	// Get the default from the defaults file, in preference 
	// to the hardcoded value
	if (defaultSettings)
        defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, actualDefault); 
}

// Read a boolean value
bool sysSettings::Read(const wxString& key, bool *val, bool defaultVal) const
{
	wxString actualDefault = BoolToStr(defaultVal);
    wxString str;

	// Get the default from the defaults file, in preference 
	// to the hardcoded value
    if (defaultSettings)
        defaultSettings->Read(key, &actualDefault, BoolToStr(defaultVal));

    Read(key, &str, actualDefault);
    *val = StrToBool(str);
    return true;
}

// Read a point value
wxPoint sysSettings::Read(const wxString& key, const wxPoint &defaultVal) const
{
	wxPoint actualDefault = defaultVal;

	// Get the default from the defaults file, in preference 
	// to the hardcoded value
	if (defaultSettings)
	{
	    actualDefault.x = defaultSettings->Read(key + wxT("/Left"), defaultVal.x);
        actualDefault.y = defaultSettings->Read(key + wxT("/Top"), defaultVal.y);
	}

    return wxPoint(wxConfig::Read(key + wxT("/Left"), actualDefault.x), 
                   wxConfig::Read(key + wxT("/Top"), actualDefault.y));
}

// Read a size value
wxSize sysSettings::Read(const wxString& key, const wxSize &defaultVal) const
{
	wxSize actualDefault = defaultVal;

	// Get the default from the defaults file, in preference 
	// to the hardcoded value
	if (defaultSettings)
	{
        actualDefault.x = defaultSettings->Read(key + wxT("/Width"), defaultVal.x);
        actualDefault.y = defaultSettings->Read(key + wxT("/Height"), defaultVal.y);
	}

    return wxSize(wxConfig::Read(key + wxT("/Width"), actualDefault.x), 
                  wxConfig::Read(key + wxT("/Height"), actualDefault.y));
}

// Write a boolean value
bool sysSettings::Write(const wxString &key, bool value)
{
    return Write(key, BoolToStr(value));
}

// Write a point value
bool sysSettings::Write(const wxString &key, const wxPoint &value)
{
    bool rc=wxConfig::Write(key + wxT("/Left"), value.x);
    if (rc)
        rc=wxConfig::Write(key + wxT("/Top"), value.y);
    return rc;
}

// Write a size value
bool sysSettings::Write(const wxString &key, const wxSize &value)
{
    bool rc=wxConfig::Write(key + wxT("/Width"), value.x);
    if (rc)
        rc=wxConfig::Write(key + wxT("/Height"), value.y);
    return rc;
}

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetLogFile()
{
	wxString logFile;

	// Try to get a vaguely usable default path.
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
        wxStandardPaths paths;
        deflog = paths.GetDocumentsDir();
        deflog += wxT("\\pgadmin.log");
#else
        deflog = wxString::FromAscii(homedir);
        deflog += wxT("/pgadmin.log");
#endif
    }

    Read(wxT("LogFile"), &logFile, deflog);

	return logFile;
}

//////////////////////////////////////////////////////////////////////////
// Help files
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetSlonyHelpPath()
{
	wxString path;

    Read(wxT("SlonyHelpPath"), &path, wxT(""));
	path = CleanHelpPath(path);

    if (!HelpPathValid(path))
        path = wxEmptyString;

	return path;
}

wxString sysSettings::GetPgHelpPath()
{
	wxString path;

    Read(wxT("PostgreSQLHelpPath"), &path, wxT(""));
	path = CleanHelpPath(path);

    if (!HelpPathValid(path))
        path = wxEmptyString;

	return path;
}

wxString sysSettings::GetEdbHelpPath()
{
	wxString path;

    Read(wxT("EnterpriseDBHelpPath"), &path, wxT(""));
	path = CleanHelpPath(path);

    if (!HelpPathValid(path))
        path = wxEmptyString;

	return path;
}

//////////////////////////////////////////////////////////////////////////
// Copy quoting
//////////////////////////////////////////////////////////////////////////

int sysSettings::GetCopyQuoting()
{
    wxString val;

    Read(wxT("Copy/Quote"), &val, wxT("Strings"));
    if (val == wxT("All"))
        return 2;
    else if (val == wxT("Strings"))
        return 1;
    else
        return 0;
}

void sysSettings::SetCopyQuoting(const int i)
{
    switch (i)
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
}

//////////////////////////////////////////////////////////////////////////
// Export quoting
//////////////////////////////////////////////////////////////////////////

int sysSettings::GetExportQuoting()
{
	wxString val;

    Read(wxT("Export/Quote"), &val, wxT("Strings"));
    if (val == wxT("All"))
        return 2;
    else if (val == wxT("Strings"))
        return 1;
    else
        return 0;
}

void sysSettings::SetExportQuoting(const int i)
{
    switch (i)
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
}

//////////////////////////////////////////////////////////////////////////
// Export quoting
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetExportRowSeparator()
{
    wxString val;
#ifdef __WXMSW__
    Read(wxT("Export/RowSeparator"), &val, wxT("CR/LF"));
#else
    Read(wxT("Export/RowSeparator"), &val, wxT("LF"));
#endif
    if (val == wxT("CR/LF"))
        return wxT("\r\n");
    else
        return wxT("\n");
}

void sysSettings::SetExportRowSeparator(const wxString &s)
{
    if (s == wxT("\r\n"))
        Write(wxT("Export/RowSeparator"), wxT("CR/LF"));
    else
        Write(wxT("Export/RowSeparator"), wxT("LF"));
}

//////////////////////////////////////////////////////////////////////////
// System Font
//////////////////////////////////////////////////////////////////////////

wxFont sysSettings::GetSystemFont()
{
    wxString fontName;

    Read(wxT("Font"), &fontName, wxEmptyString);

    if (fontName.IsEmpty())
        return wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT);
    else
        return wxFont(fontName);
}

void sysSettings::SetSystemFont(const wxFont &font)
{
    wxString fontName = font.GetNativeFontInfoDesc();

    if (fontName == wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT).GetNativeFontInfoDesc())
        Write(wxT("Font"), wxEmptyString);
    else
        Write(wxT("Font"), fontName);
}

//////////////////////////////////////////////////////////////////////////
// SQL Font
//////////////////////////////////////////////////////////////////////////

wxFont sysSettings::GetSQLFont()
{
    wxString fontName;

    Read(wxT("frmQuery/Font"), &fontName, wxEmptyString);

    if (fontName.IsEmpty())
    {
#ifdef __WXMSW__
        return wxFont(9, wxTELETYPE, wxNORMAL, wxNORMAL);
#else
        return wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
#endif
    }
    else
        return wxFont(fontName);
}

void sysSettings::SetSQLFont(const wxFont &font)
{
    wxString fontName = font.GetNativeFontInfoDesc();

    Write(wxT("frmQuery/Font"), fontName);
}

//////////////////////////////////////////////////////////////////////////
// Language
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetCanonicalLanguageName()
{
    const wxLanguageInfo *langInfo;

    langInfo = wxLocale::GetLanguageInfo(Read(wxT("LanguageId"), wxLANGUAGE_UNKNOWN));
    
	if (langInfo)
        return langInfo->CanonicalName;

	return wxEmptyString;
}

void sysSettings::SetCanonicalLanguage(const wxLanguage &lang)
{
	if (wxLocale::GetLanguageName(lang) != GetCanonicalLanguageName())
    {
        delete locale;
        locale = new wxLocale();
        if (locale->Init(lang))
        {
#ifdef __LINUX__
            {
                wxLogNull noLog;
                locale->AddCatalog(wxT("fileutils"));
            }
#endif
            locale->AddCatalog(wxT("pgadmin3"));
            settings->Write(wxT("LanguageId"), (long)lang);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Get system wide configuration file names
//////////////////////////////////////////////////////////////////////////
wxString sysSettings::GetConfigFile(configFileName cfgname)
{
    if (cfgname == PGPASS || cfgname == PGAFAVOURITES || cfgname == PGAMACROS)
    {
        wxStandardPaths stdp;
        wxString fname=stdp.GetUserConfigDir();
#ifdef WIN32
        fname += wxT("\\postgresql");
        if (!wxDirExists(fname))
            wxMkdir(fname);
        switch(cfgname)
        {
        case PGPASS:
            fname += wxT("\\pgpass.conf");
            break;
        case PGAFAVOURITES:
            fname += wxT("\\pgadmin_favourites.xml");
            break;
        case PGAMACROS:
            fname += wxT("\\pgadmin_macros.xml");
            break;
        }
#else
        switch(cfgname)
        {
        case PGPASS:
            fname += wxT("/.pgpass");
            break;
        case PGAFAVOURITES:
            fname += wxT("/.pgadminfavourites");
            break;
        case PGAMACROS:
            fname += wxT("/.pgadminmacros");
            break;
        }
#endif
        return fname;
    }
    return wxT("");
}
