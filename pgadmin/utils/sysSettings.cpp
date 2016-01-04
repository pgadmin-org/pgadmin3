//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// sysSettings.cpp - Settings handling class
//
// Note: This class stores and manages all the applications settings.
//       Settings are all read in the ctor, but may be written either in
//       the relevant SetXXX() member function for rarely written settings
//       or in the dtor for regularly changed settings such as form sizes.
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"


// wxWindows headers
#include <wx/wx.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/url.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <wx/dir.h>
// App headers
#include "utils/sysSettings.h"
#include "utils/sysLogger.h"
#include "utils/misc.h"
sysSettings::sysSettings(const wxString &name) : wxConfig(name)
{
	// Open the default settings file
	defaultSettings = NULL;
	if (!settingsIni.IsEmpty())
	{
		wxFileInputStream fst(settingsIni);
		defaultSettings = new wxFileConfig(fst);
	}

	// Convert settings from pre-1.5
	long i, serverCount;
	Read(wxT("Servers/Count"), &serverCount, 0L);
	for (i = 1 ; i <= serverCount ; i++)
	{
		if (moveStringValue(wxT("Servers/Database%d"), wxT("Servers/%d/Database"), i))
		{
			moveStringValue(wxT("Servers/Description%d"), wxT("Servers/%d/Description"), i);
			moveStringValue(wxT("Servers/LastDatabase%d"), wxT("Servers/%d/LastDatabase"), i);
			moveStringValue(wxT("Servers/LastSchema%d"), wxT("Servers/%d/LastSchema"), i);
			moveStringValue(wxT("Servers/Server%d"), wxT("Servers/%d/Server"), i);
			moveStringValue(wxT("Servers/ServiceId%d"), wxT("Servers/%d/ServiceId"), i);
			moveStringValue(wxT("Servers/StorePWD%d"), wxT("Servers/%d/StorePWD"), i);
			moveStringValue(wxT("Servers/Rolename%d"), wxT("Servers/%d/Rolename"), i);
			moveStringValue(wxT("Servers/Username%d"), wxT("Servers/%d/Username"), i);
			moveLongValue(wxT("Servers/Port%d"), wxT("Servers/%d/Port"), i);
			moveLongValue(wxT("Servers/SSL%d"), wxT("Servers/%d/SSL"), i);
		}
	}
}

sysSettings::~sysSettings()
{

	if(defaultSettings)
	{
		delete defaultSettings;
		defaultSettings = NULL;
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
	else if (objtype == _("pgAgent Jobs"))
		engtype = wxT("pgAgent Jobs");
	else if (objtype == _("Groups/group Roles"))
		engtype = wxT("Groups-login Roles");
	else if (objtype == _("Users/login Roles"))
		engtype = wxT("Users-login Roles");
	else if (objtype == _("Resource Queues"))
		engtype = wxT("Resource Queues");
	else if (objtype == _("Resource Groups"))
		engtype = wxT("Resource Groups");
	else if (objtype == _("Catalogs"))
		engtype = wxT("Catalogs");
	else if (objtype == _("Casts"))
	{
		engtype = wxT("Casts");
		def = false;
	}
	else if (objtype == _("Foreign Data Wrappers"))
	{
		engtype = wxT("Foreign Data Wrappers");
		def = false;
	}
	else if (objtype == _("Foreign Servers"))
	{
		engtype = wxT("Foreign Servers");
		def = false;
	}
	else if (objtype == _("User Mappings"))
	{
		engtype = wxT("User Mappings");
		def = false;
	}
	else if (objtype == _("Foreign Tables"))
	{
		engtype = wxT("Foreign Tables");
		def = false;
	}
	else if (objtype == _("Languages"))
	{
		engtype = wxT("Languages");
		def = false;
	}
	else if (objtype == _("Extensions"))
		engtype = wxT("Extensions");
	else if (objtype == _("Synonyms"))
		engtype = wxT("Synonyms");
	else if (objtype == _("Schemas"))
		engtype = wxT("Schemas");
	else if (objtype == _("Slony-I Clusters"))
		engtype = wxT("Slony-I Clusters");
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
	else if (objtype == _("Trigger Functions"))
		engtype = wxT("Trigger Functions");
	else if (objtype == _("Packages"))
		engtype = wxT("Packages");
	else if (objtype == _("Procedures"))
		engtype = wxT("Procedures");
	else if (objtype == _("Operators"))
	{
		engtype = wxT("Operators");
		def = false;
	}
	else if (objtype == _("Operator Classes"))
	{
		engtype = wxT("Operator Classes");
		def = false;
	}
	else if (objtype == _("Operator Families"))
	{
		engtype = wxT("Operator Families");
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
	else if (objtype == _("External Tables"))
		engtype = wxT("External Tables");
	else if (objtype == _("Event Triggers"))
		engtype = wxT("Event Triggers");

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
	else if (objtype == _("pgAgent Jobs")) engtype = wxT("pgAgent Jobs");
	else if (objtype == _("Groups/group Roles")) engtype = wxT("Groups-login Roles");
	else if (objtype == _("Users/login Roles")) engtype = wxT("Users-login Roles");
	else if (objtype == _("Resource Queues")) engtype = wxT("Resource Queues");
	else if (objtype == _("Resource Groups")) engtype = wxT("Resource Groups");
	else if (objtype == _("Catalogs")) engtype = wxT("Catalogs");
	else if (objtype == _("Casts")) engtype = wxT("Casts");
	else if (objtype == _("Foreign Data Wrappers")) engtype = wxT("Foreign Data Wrappers");
	else if (objtype == _("Foreign Servers")) engtype = wxT("Foreign Servers");
	else if (objtype == _("User Mappings")) engtype = wxT("User mappings");
	else if (objtype == _("Foreign Tables")) engtype = wxT("Foreign Tables");
	else if (objtype == _("Languages")) engtype = wxT("Languages");
	else if (objtype == _("Event Triggers")) engtype = wxT("Event Triggers");
	else if (objtype == _("Extensions")) engtype = wxT("Extensions");
	else if (objtype == _("Synonyms")) engtype = wxT("Synonyms");
	else if (objtype == _("Schemas")) engtype = wxT("Schemas");
	else if (objtype == _("Slony-I Clusters")) engtype = wxT("Slony-I Clusters");
	else if (objtype == _("Aggregates")) engtype = wxT("Aggregates");
	else if (objtype == _("Conversions")) engtype = wxT("Conversions");
	else if (objtype == _("Domains")) engtype = wxT("Domains");
	else if (objtype == _("Functions")) engtype = wxT("Functions");
	else if (objtype == _("Trigger Functions")) engtype = wxT("Trigger Functions");
	else if (objtype == _("Packages")) engtype = wxT("Packages");
	else if (objtype == _("Procedures")) engtype = wxT("Procedures");
	else if (objtype == _("Operators")) engtype = wxT("Operators");
	else if (objtype == _("Operator Classes")) engtype = wxT("Operator Classes");
	else if (objtype == _("Operator Families")) engtype = wxT("Operator Families");
	else if (objtype == _("Sequences")) engtype = wxT("Sequences");
	else if (objtype == _("Tables")) engtype = wxT("Tables");
	else if (objtype == _("FTS Configurations")) engtype = wxT("FTS Configurations");
	else if (objtype == _("FTS Dictionaries")) engtype = wxT("FTS Dictionaries");
	else if (objtype == _("FTS Parsers")) engtype = wxT("FTS Parsers");
	else if (objtype == _("FTS Templates")) engtype = wxT("FTS Templates");
	else if (objtype == _("Types")) engtype = wxT("Types");
	else if (objtype == _("Views")) engtype = wxT("Views");
	else if (objtype == _("External Tables")) engtype = wxT("External Tables");

	WriteBool(wxT("Display/") + engtype, display);
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
		k1 = oldKey;
		k2 = newKey;
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
		k1 = oldKey;
		k2 = newKey;
	}

	if (!Exists(k2) && Exists(k1))
	{
		long value;
		Read(k1, &value, 0L);
		WriteLong(k2, value);

		return true;
	}

	return false;
}

// Read a string value
bool sysSettings::Read(const wxString &key, wxString *str, const wxString &defaultVal) const
{
	wxString actualDefault = defaultVal;

	// Get the default from the defaults file, in preference
	// to the hardcoded value
	if (defaultSettings)
		defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, str, actualDefault);
}

// Return a string value
wxString sysSettings::Read(const wxString &key, const wxString &defaultVal) const
{
	wxString actualDefault = defaultVal;

	// Get the default from the defaults file, in preference
	// to the hardcoded value
	if (defaultSettings)
		defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, actualDefault);
}

// Read an int value
bool sysSettings::Read(const wxString &key, int *i, int defaultVal) const
{
	int actualDefault = defaultVal;

	// Get the default from the defaults file, in preference
	// to the hardcoded value
	if (defaultSettings)
		defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, i, actualDefault);
}

// Read a long value
bool sysSettings::Read(const wxString &key, long *l, long defaultVal) const
{
	long actualDefault = defaultVal;

	// Get the default from the defaults file, in preference
	// to the hardcoded value
	if (defaultSettings)
		defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, l, actualDefault);
}


// Return a long value
long sysSettings::Read(const wxString &key, long defaultVal) const
{
	long actualDefault = defaultVal;

	// Get the default from the defaults file, in preference
	// to the hardcoded value
	if (defaultSettings)
		defaultSettings->Read(key, &actualDefault, defaultVal);

	return wxConfig::Read(key, actualDefault);
}

// Read a boolean value
bool sysSettings::Read(const wxString &key, bool *val, bool defaultVal) const
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
wxPoint sysSettings::Read(const wxString &key, const wxPoint &defaultVal) const
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
wxSize sysSettings::Read(const wxString &key, const wxSize &defaultVal) const
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
bool sysSettings::WriteBool(const wxString &key, bool value)
{
	return Write(key, BoolToStr(value));
}

// Write a point value
bool sysSettings::WritePoint(const wxString &key, const wxPoint &value)
{
	bool rc = wxConfig::Write(key + wxT("/Left"), value.x);
	if (rc)
		rc = wxConfig::Write(key + wxT("/Top"), value.y);
	return rc;
}

// Write a size value
bool sysSettings::WriteSize(const wxString &key, const wxSize &value)
{
	bool rc = wxConfig::Write(key + wxT("/Width"), value.x);
	if (rc)
		rc = wxConfig::Write(key + wxT("/Height"), value.y);
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
#if wxCHECK_VERSION(2, 9, 5)
		wxStandardPaths &paths = wxStandardPaths::Get();
#else
		wxStandardPaths paths;
#endif
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

wxString sysSettings::GetGpHelpPath()
{
	wxString path;

	Read(wxT("GreenplumDBHelpPath"), &path, wxT(""));
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
		Write(wxT("Font"), wxString(wxEmptyString));
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
#ifdef __WXGTK__
		return wxFont(10, wxTELETYPE, wxNORMAL, wxNORMAL);
#else
		return wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
#endif
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
// Database Designer Font
//////////////////////////////////////////////////////////////////////////

wxFont sysSettings::GetDDFont()
{
	wxString fontName;

	Read(wxT("DDFont"), &fontName, wxEmptyString);

	if (fontName.IsEmpty())
		return wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT);
	else
		return wxFont(fontName);
}

void sysSettings::SetDDFont(const wxFont &font)
{
	wxString fontName = font.GetNativeFontInfoDesc();

	if (fontName == wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT).GetNativeFontInfoDesc())
		Write(wxT("DDFont"), wxString(wxEmptyString));
	else
		Write(wxT("DDFont"), fontName);
}

//////////////////////////////////////////////////////////////////////////
// Language
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetCanonicalLanguageName()
{
	const wxLanguageInfo *langInfo;

	langInfo = wxLocale::GetLanguageInfo(Read(wxT("LanguageId"), wxLANGUAGE_DEFAULT));

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
			settings->WriteLong(wxT("LanguageId"), (long)lang);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Get system wide configuration file names
//////////////////////////////////////////////////////////////////////////
wxString sysSettings::GetConfigFile(configFileName cfgname)
{
	wxASSERT_MSG(cfgname == sysSettings::PGPASS,
	             wxT("Handles only pgpass configuration"));
	if (cfgname == PGPASS)
	{
#if wxCHECK_VERSION(2, 9, 5)
		wxStandardPaths &stdp = wxStandardPaths::Get();
#else
		wxStandardPaths stdp;
#endif
		wxString fname;
		bool bpsfile = wxGetEnv(wxT("PGPASSFILE"), &fname);
		if (!bpsfile)
		{
			fname = stdp.GetUserConfigDir();
#ifdef WIN32
			fname += wxT("\\postgresql");
			if (!wxDirExists(fname))
				wxMkdir(fname);
			fname += wxT("\\pgpass.conf");
		}
#else
			fname += wxT("/.pgpass");
		}
#endif
		else
		{
			if(!wxFileName::FileExists(fname))
			{
				wxFileName dirTemp = wxFileName::DirName(fname);
				/*
				 * wxFileName::DirName() does not return the directory path of
				 * the file. It assumes that the given path is of a directory,
				 * when the specified file/directory does not exist.
				 *
				 * Hence - removeing it to get the actual parent directory.
				 */
				dirTemp.RemoveLastDir();

				if (!dirTemp.DirExists())
				{
					wxFileName dir = dirTemp;

					// Decide which are folders we need to create
					wxString sRemoveOnError = dirTemp.GetPath();

					while(!dirTemp.DirExists())
					{
						sRemoveOnError = dirTemp.GetPath();
						dirTemp.RemoveLastDir();
					}

					if (!dir.Mkdir(0755, wxPATH_MKDIR_FULL))
					{
						// In case of failure decide - we may need to delete
						// the created directory structure from exists parent
						// directory.
						if (wxDir::Exists(sRemoveOnError))
							wxFileName::Rmdir(sRemoveOnError);
					}
				}
			}
		}

		wxFile f;
		if (!f.Exists(fname))
			f.Create(fname, false, wxS_IRUSR | wxS_IWUSR);

		return fname;
	}

	return wxT("");
}


wxString sysSettings::GetFavouritesFile()
{
	wxString s, tmp;

#if wxCHECK_VERSION(2, 9, 5)
	wxStandardPaths &stdp = wxStandardPaths::Get();
#else
	wxStandardPaths stdp;
#endif
	tmp = stdp.GetUserConfigDir();
#ifdef WIN32
	tmp += wxT("\\postgresql");
	if (!wxDirExists(tmp))
		wxMkdir(tmp);
	tmp += wxT("\\pgadmin_favourites.xml");
#else
	tmp += wxT("/.pgadminfavourites");
#endif

	Read(wxT("FavouritesFile"), &s, tmp);

	return s;
}


wxString sysSettings::GetMacrosFile()
{
	wxString s, tmp;

#if wxCHECK_VERSION(2, 9, 5)
	wxStandardPaths &stdp = wxStandardPaths::Get();
#else
	wxStandardPaths stdp;
#endif
	tmp = stdp.GetUserConfigDir();
#ifdef WIN32
	tmp += wxT("\\postgresql");
	if (!wxDirExists(tmp))
		wxMkdir(tmp);
	tmp += wxT("\\pgadmin_macros.xml");
#else
	tmp += wxT("/.pgadminmacros");
#endif

	Read(wxT("MacrosFile"), &s, tmp);

	return s;
}


wxString sysSettings::GetHistoryFile()
{
	wxString s, tmp;

#if wxCHECK_VERSION(2, 9, 5)
	wxStandardPaths &stdp = wxStandardPaths::Get();
#else
	wxStandardPaths stdp;
#endif
	tmp = stdp.GetUserConfigDir();
#ifdef WIN32
	tmp += wxT("\\postgresql");
	if (!wxDirExists(tmp))
		wxMkdir(tmp);
	tmp += wxT("\\pgadmin_histoqueries.xml");
#else
	tmp += wxT("/.pgadmin_histoqueries");
#endif

	Read(wxT("History/File"), &s, tmp);

	return s;
}

