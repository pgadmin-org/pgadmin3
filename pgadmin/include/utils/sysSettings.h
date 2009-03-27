//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// sysSettings.h - Settings handling class
//
//////////////////////////////////////////////////////////////////////////

#ifndef SYSSETTINGS_H
#define SYSSETTINGS_H

#include "utils/sysLogger.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/config.h>
#include <wx/fileconf.h>

// Class declarations
class sysSettings : private wxConfig
{
public:
    sysSettings(const wxString& name);

	// Display options
	bool GetDisplayOption(const wxString &objtype, bool GetDefault = false);
	void SetDisplayOption(const wxString &objtype, bool display);

    // Log
    wxString GetLogFile();
	void SetLogFile(const wxString& newval) { Write(wxT("LogFile"), newval); sysLogger::logFile = newval; }
    int GetLogLevel() const { int i; Read(wxT("LogLevel"), &i, LOG_ERRORS); return i; }
	void SetLogLevel(const int newval) { Write(wxT("LogLevel"), newval); sysLogger::logLevel = newval; }

    // Last connection
    wxString GetLastDescription() const { wxString s; Read(wxT("LastDescription"), &s, wxT("PostgreSQL Server")); return s; }
	void SetLastDescription(const wxString &newval) { Write(wxT("LastDescription"), newval); }
    wxString GetLastServer() const { wxString s; Read(wxT("LastServer"), &s, wxT("localhost")); return s; }
	void SetLastServer(const wxString &newval) { Write(wxT("LastServer"), newval); }
    wxString GetLastDatabase() const { wxString s; Read(wxT("LastDatabase"), &s, wxT("postgres")); return s; }
	void SetLastDatabase(const wxString &newval) { Write(wxT("LastDatabase"), newval); }
    wxString GetLastUsername() const { wxString s; Read(wxT("LastUsername"), &s, wxT("postgres")); return s; }
	void SetLastUsername(const wxString &newval) { Write(wxT("LastUsername"), newval); }
    int GetLastPort() const { int i; Read(wxT("LastPort"), &i, 5432); return i; }
	void SetLastPort(const int newval) { Write(wxT("LastPort"), newval); }
    int GetLastSSL() const { int i; Read(wxT("LastSSL"), &i, 0); return i; }
	void SetLastSSL(const int newval) { Write(wxT("LastSSL"), newval); }
    int GetLastSSLverify() const { int i; Read(wxT("LastSSLverify"), &i, 0); return i; }
	void SetLastSSLverify(const int newval) { Write(wxT("LastSSLverify"), newval); }

    // Helper paths
    wxString GetSlonyPath() const { wxString s; Read(wxT("SlonyPath"), &s, wxEmptyString); return s; }
	void SetSlonyPath(const wxString &newval) { Write(wxT("SlonyPath"), newval); }
    wxString GetPostgresqlPath() const { wxString s; Read(wxT("PostgreSQLPath"), &s, wxEmptyString); return s; }
	void SetPostgresqlPath(const wxString &newval) { Write(wxT("PostgreSQLPath"), newval); }
    wxString GetEnterprisedbPath() const { wxString s; Read(wxT("EnterpriseDBPath"), &s, wxEmptyString); return s; }
	void SetEnterprisedbPath(const wxString &newval) { Write(wxT("EnterpriseDBPath"), newval); }
    wxString GetGPDBPath() const { wxString s; Read(wxT("GreenplumDBPath"), &s, wxEmptyString); return s; }
	void SetGPDBPath(const wxString &newval) { Write(wxT("GreenplumDBPath"), newval); }

	// Help paths
    wxString GetSlonyHelpPath();
    void SetSlonyHelpPath(const wxString &newval) { Write(wxT("SlonyHelpPath"), newval); }
    wxString GetPgHelpPath();
    void SetPgHelpPath(const wxString &newval) { Write(wxT("PostgreSQLHelpPath"), newval); }
    wxString GetEdbHelpPath();
    void SetEdbHelpPath(const wxString &newval) { Write(wxT("EnterpriseDBHelpPath"), newval); }
    wxString GetGpHelpPath();
    void SetGpHelpPath(const wxString &newval) { Write(wxT("GreenplumDBHelpPath"), newval); }

    // Copy options
    wxString GetCopyQuoteChar() const { wxString s; Read(wxT("Copy/QuoteChar"), &s, wxT("\"")); return s; }
	void SetCopyQuoteChar(const wxString &newval) { Write(wxT("Copy/QuoteChar"), newval); }
    wxString GetCopyColSeparator() const { wxString s; Read(wxT("Copy/ColSeparator"), &s, wxT(";")); return s; }
	void SetCopyColSeparator(const wxString &newval) { Write(wxT("Copy/ColSeparator"), newval); }
	int GetCopyQuoting(); // 0=none 1=string 2=all
	void SetCopyQuoting(const int i);
    
    // Export options
    wxString GetExportQuoteChar() const { wxString s; Read(wxT("Export/QuoteChar"), &s, wxT("\"")); return s; }
	void SetExportQuoteChar(const wxString &newval) { Write(wxT("Export/QuoteChar"), newval); }
    wxString GetExportColSeparator() const { wxString s; Read(wxT("Export/ColSeparator"), &s, wxT(";")); return s; }
	void SetExportColSeparator(const wxString &newval) { Write(wxT("Export/ColSeparator"), newval); }
	wxString GetExportRowSeparator();
    void SetExportRowSeparator(const wxString &s);
	int GetExportQuoting();  // 0=none 1=string 2=all
    void SetExportQuoting(const int i);
    bool GetExportUnicode() const { bool b; Read(wxT("Export/Unicode"), &b, true); return b; }
	void SetExportUnicode(const bool newval) { Write(wxT("Export/Unicode"), newval); }
	
    // Explain options
    bool GetExplainVerbose() const { bool b; Read(wxT("frmQuery/ExplainVerbose"), &b, false); return b; }
	void SetExplainVerbose(const bool newval) { Write(wxT("frmQuery/ExplainVerbose"), newval); }
    bool GetExplainAnalyze() const { bool b; Read(wxT("frmQuery/ExplainAnalyze"), &b, false); return b; }
	void SetExplainAnalyze(const bool newval) { Write(wxT("frmQuery/ExplainAnalyze"), newval); }

    // Display options
    wxString GetSystemSchemas() const { wxString s; Read(wxT("SystemSchemas"), &s, wxEmptyString); return s; }
	void SetSystemSchemas(const wxString &newval) { Write(wxT("SystemSchemas"), newval); }
    bool GetShowUsersForPrivileges() const { bool b; Read(wxT("ShowUsersForPrivileges"), &b, false); return b; }
	void SetShowUsersForPrivileges(const bool newval) { Write(wxT("ShowUsersForPrivileges"), newval); }
    bool GetShowSystemObjects() const { bool b; Read(wxT("ShowSystemObjects"), &b, false); return b; }
	void SetShowSystemObjects(const bool newval) { Write(wxT("ShowSystemObjects"), newval); }

	// Editor options
    bool GetSpacesForTabs() const { bool b; Read(wxT("SpacesForTabs"), &b, false); return b; }
	void SetSpacesForTabs(const bool newval) { Write(wxT("SpacesForTabs"), newval); }
    long GetIndentSpaces() const { long l; Read(wxT("IndentSpaces"), &l, 0L); return l; }
	void SetIndentSpaces(const long newval) { Write(wxT("IndentSpaces"), newval); }
    bool GetIndicateNull() const { bool b; Read(wxT("frmQuery/IndicateNull"), &b, false); return b; }
	void SetIndicateNull(const bool newval) { Write(wxT("frmQuery/IndicateNull"), newval); }
    bool GetUnicodeFile() const { bool b; Read(wxT("WriteUnicodeFile"), &b, true); return b; }
	void SetUnicodeFile(const bool newval) { Write(wxT("WriteUnicodeFile"), newval); }
    wxFont GetSystemFont();
	void SetSystemFont(const wxFont &font);
	wxFont GetSQLFont();
    void SetSQLFont(const wxFont &font);
    int GetLineEndingType() const { int i; Read(wxT("LineEndingType"), &i, 2); return i; }
	void SetLineEndingType(const int newval) { Write(wxT("LineEndingType"), newval); }

    // Misc options
    long GetAutoRowCountThreshold() const { long l; Read(wxT("AutoRowCount"), &l, 2000L); return l; }
	void SetAutoRowCountThreshold(const long newval) { Write(wxT("AutoRowCount"), newval); }
    bool GetStickySql() const { bool b; Read(wxT("StickySql"), &b, true); return b; }
	void SetStickySql(const bool newval) { Write(wxT("StickySql"), newval); }
    bool GetDoubleClickProperties() const { bool b; Read(wxT("DoubleClickProperties"), &b, false); return b; }
	void SetDoubleClickProperties(const bool newval) { Write(wxT("DoubleClickProperties"), newval); }
    long GetMaxServerLogSize() const { long l; Read(wxT("MaxServerLogSize"), &l, 100000L); return l; }
	void SetMaxServerLogSize(const long newval) { Write(wxT("MaxServerLogSize"), newval); }
    bool GetSuppressGuruHints() const { bool b; Read(wxT("SuppressGuruHints"), &b, false); return b; }
	void SetSuppressGuruHints(const bool newval) { Write(wxT("SuppressGuruHints"), newval); }
    long GetMaxRows() const { long l; Read(wxT("frmQuery/MaxRows"), &l, 100L); return l; }
	void SetMaxRows(const long newval) { Write(wxT("frmQuery/MaxRows"), newval); }
    long GetMaxColSize() const { long l; Read(wxT("frmQuery/MaxColSize"), &l, 256L); return l; }
	void SetMaxColSize(const long newval) { Write(wxT("frmQuery/MaxColSize"), newval); }
    bool GetAskSaveConfirmation() const { bool b; Read(wxT("AskSaveConfirmation"), &b, true); return b; }
	void SetAskSaveConfirmation(const bool newval) { Write(wxT("AskSaveConfirmation"), newval); }
    bool GetConfirmDelete() const { bool b; Read(wxT("ConfirmDelete"), &b, true); return b; }
	void SetConfirmDelete(const bool newval) { Write(wxT("ConfirmDelete"), newval); }
    wxString GetCanonicalLanguageName();
	wxLanguage GetCanonicalLanguage() const { int i; Read(wxT("LanguageId"), &i, wxLANGUAGE_UNKNOWN); return (wxLanguage)i; }
	void SetCanonicalLanguage(const wxLanguage &lang);
    bool GetIgnoreVersion() const { bool b; Read(wxT("IgnoreVersion"), &b, false); return b; }
	void SetIgnoreVersion(const bool newval) { Write(wxT("IgnoreVersion"), newval); }

    // Functions for storing settings
    bool Write(const wxString &key, const wxChar *value) { return wxConfig::Write(key, value); }
    bool Write(const wxString &key, long value) { return wxConfig::Write(key, value); }
    bool Write(const wxString &key, int value) { return wxConfig::Write(key, value); }
    bool Write(const wxString &key, bool value);
    bool Write(const wxString &key, const wxPoint &value);
    bool Write(const wxString &key, const wxSize &value);
    bool Write(const wxString &key, const wxSize &size, const wxPoint &point) { Write(key, point); Write(key, size); return true;}

	// Functions for reading settings
    bool Read(const wxString& key, wxString* str, const wxString& defaultVal) const;
    bool Read(const wxString& key, bool* str, bool defaultVal) const;
    bool Read(const wxString& key, int* i, int defaultVal) const;
    bool Read(const wxString& key, long* l, long defaultVal) const;
    wxString Read(const wxString& key, const wxString &defaultVal) const;
    long Read(const wxString& key, long defaultVal) const;
    wxPoint Read(const wxString& key, const wxPoint &defaultVal) const;
    wxSize Read(const wxString& key, const wxSize &defaultVal) const;

	enum configFileName
	{
		PGPASS,
		PGAFAVOURITES,
		PGAMACROS
	};
	static wxString GetConfigFile(configFileName cfgname);

private:

    bool moveStringValue(const wxChar *oldKey, const wxChar *newKey, int index=-1);
    bool moveLongValue(const wxChar *oldKey, const wxChar *newKey, int index=-1);

	wxFileConfig *defaultSettings; 
};

#endif
