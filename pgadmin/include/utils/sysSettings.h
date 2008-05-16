//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// sysSettings.h - Settings handling class
//
//////////////////////////////////////////////////////////////////////////

#ifndef SYSSETTINGS_H
#define SYSSETTINGS_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/config.h>
#include <wx/fileconf.h>

// Class declarations
class sysSettings : public wxConfig
{
public:
    sysSettings(const wxString& name);
    ~sysSettings();

	// Display options
	bool GetDisplayOption(const wxString &objtype, bool GetDefault = false);
	void SetDisplayOption(const wxString &objtype, bool display);

    // Tip Of The Day
    bool GetShowTipOfTheDay() const { bool b; Read(wxT("ShowTipOfTheDay"), &b, true); return b; }
	void SetShowTipOfTheDay(const bool newval) { Write(wxT("ShowTipOfTheDay"), newval); }
    int GetNextTipOfTheDay() const { int i; Read(wxT("NextTipOfTheDay"), &i, true); return i; }
    void SetNextTipOfTheDay(const int newval) { Write(wxT("NextTipOfTheDay"), newval); }

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

    // Helper paths
    wxString GetSlonyPath() const { wxString s; Read(wxT("SlonyPath"), &s, wxEmptyString); return s; }
	void SetSlonyPath(const wxString &newval) { Write(wxT("SlonyPath"), newval); }
    wxString GetPostgresqlPath() const { wxString s; Read(wxT("PostgreSQLPath"), &s, wxEmptyString); return s; }
	void SetPostgresqlPath(const wxString &newval) { Write(wxT("PostgreSQLPath"), newval); }
    wxString GetEnterprisedbPath() const { wxString s; Read(wxT("EnterpriseDBPath"), &s, wxEmptyString); return s; }
	void SetEnterprisedbPath(const wxString &newval) { Write(wxT("EnterpriseDBPath"), newval); }

	// Help paths
    wxString GetSlonyHelpPath();
    void SetSlonyHelpPath(const wxString &newval) { Write(wxT("SlonyHelpPath"), newval); }
    wxString GetPgHelpPath();
    void SetPgHelpPath(const wxString &newval) { Write(wxT("PostgreSQLHelpPath"), newval); }
    wxString GetEdbHelpPath();
    void SetEdbHelpPath(const wxString &newval) { Write(wxT("EnterpriseDBHelpPath"), newval); }

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


    // TODO: rewrite the rest of these like those above!!

    // Display options
    void SetSystemSchemas(const wxString &s) { systemSchemas = s; }
    wxString GetSystemSchemas() const { return systemSchemas; }
    bool GetShowUsersForPrivileges() const { return showUsersForPrivileges; }
    void SetShowUsersForPrivileges(const bool b) { showUsersForPrivileges=b; }
    bool GetShowSystemObjects() const { return showSystemObjects; }
    void SetShowSystemObjects(const bool newval);

	// Editor options
    bool GetSpacesForTabs() const { return spacesForTabs; }
    void SetSpacesForTabs(const bool newval) { spacesForTabs = newval; }
    long GetIndentSpaces() const { return indentSpaces; }
    void SetIndentSpaces(long l) { indentSpaces=l; }
	bool GetTabForCompletion() const { return tabForCompletion; }
	void SetTabForCompletion(const bool newval) { tabForCompletion = newval; }
    bool GetIndicateNull() const { return indicateNull; }
    void SetIndicateNull(const bool newval);
    bool GetUnicodeFile() const { return unicodeFile; }
    void SetUnicodeFile(const bool b) {unicodeFile = b; }
    wxFont GetSQLFont() const { return sqlFont; }
    wxFont GetSystemFont() const { return systemFont; }
    void SetFont(const wxFont &font) { systemFont = font; }
    void SetSQLFont(const wxFont &font) { sqlFont = font; }


    // Misc options
    long GetAutoRowCountThreshold() const { return autoRowCountThreshold; }
    void SetAutoRowCountThreshold(const long l) { autoRowCountThreshold=l; }
    bool GetStickySql() const { return stickySql; }
    void SetStickySql(const bool newval);
    bool GetDoubleClickProperties() const { return doubleClickProperties; }
    void SetDoubleClickProperties(const bool newval);
    long GetMaxServerLogSize() const { return maxServerLogSize; }
    void SetMaxServerLogSize(long l) { maxServerLogSize = l; }
    bool GetSuppressGuruHints() const { return suppressGuruHints; }
    void SetSuppressGuruHints(const bool b) { suppressGuruHints=b; }
    long GetMaxRows() const { return maxRows; }
    void SetMaxRows(const long l) { maxRows=l; }
    long GetMaxColSize() const { return maxColSize; }
    void SetMaxColSize(const long l) { maxColSize=l; }
    bool GetAskSaveConfirmation() const { return askSaveConfirmation; }
    void SetAskSaveConfirmation(const bool b) { askSaveConfirmation=b; }
    bool GetConfirmDelete() const { return confirmDelete; }
    void SetConfirmDelete(const bool b) { confirmDelete=b; }

    wxString GetCanonicalLanguage() const { return canonicalLanguage; }

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

	void Save();


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

    wxFont systemFont, sqlFont; 

    // Show System Objects
    bool showSystemObjects;

    wxString canonicalLanguage;
    bool showUsersForPrivileges;
    bool askSaveConfirmation;
    bool confirmDelete, suppressGuruHints;
    long maxRows, maxColSize, autoRowCountThreshold, indentSpaces;
    bool spacesForTabs, stickySql, indicateNull, unicodeFile, tabForCompletion;
    bool doubleClickProperties;
    long maxServerLogSize;

    wxString searchPath, systemSchemas;
};

#endif
