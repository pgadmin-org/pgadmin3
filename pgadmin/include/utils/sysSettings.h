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
    int GetShowTipOfTheDay() const { return showTipOfTheDay; }
    void SetShowTipOfTheDay(const bool newval);
    int GetNextTipOfTheDay() const { return nextTipOfTheDay; }
    void SetNextTipOfTheDay(const int newval);

    // Log
    wxString GetLogFile() const { return logFile; }
    void SetLogFile(const wxString& newval);
    int GetLogLevel() const { return logLevel; }
    void SetLogLevel(const int newval);

    // Last connection
    wxString GetLastDescription() const { return lastDescription; }
    void SetLastDescription(const wxString& newval);
    wxString GetLastServer() const { return lastServer; }
    void SetLastServer(const wxString& newval);
    wxString GetLastDatabase() const { return lastDatabase; }
    void SetLastDatabase(const wxString& newval);
    wxString GetLastUsername() const { return lastUsername; }
    void SetLastUsername(const wxString& newval);
    int GetLastPort() const { return lastPort; }
    void SetLastPort(const int newval);
    int GetLastSSL() const { return lastSSL; }
    void SetLastSSL(const int newval);
    long GetMaxRows() const { return maxRows; }
    void SetMaxRows(const long l) { maxRows=l; }
    long GetMaxColSize() const { return maxColSize; }
    void SetMaxColSize(const long l) { maxColSize=l; }
    bool GetAskSaveConfirmation() const { return askSaveConfirmation; }
    void SetAskSaveConfirmation(const bool b) { askSaveConfirmation=b; }
    bool GetConfirmDelete() const { return confirmDelete; }
    void SetConfirmDelete(const bool b) { confirmDelete=b; }

    wxString GetSlonyPath() const { return slonyPath; }
    void SetSlonyPath(const wxString &s) { slonyPath=s; }
    wxString GetPostgresqlPath() const { return postgresqlPath; }
    void SetPostgresqlPath(const wxString &s) { postgresqlPath=s; }
    wxString GetEnterprisedbPath() const { return enterprisedbPath; }
    void SetEnterprisedbPath(const wxString &s) { enterprisedbPath=s; }

    wxString GetPgHelpPath() const { return pgHelpPath; }
    void SetPgHelpPath(const wxString &s) { pgHelpPath = s; }
    wxString GetEdbHelpPath() const { return edbHelpPath; }
    void SetEdbHelpPath(const wxString &s) { edbHelpPath = s; }
    wxString GetSlonyHelpPath() const { return slonyHelpPath; }
    void SetSlonyHelpPath(const wxString &s) { slonyHelpPath = s; }
    
    void SetSystemSchemas(const wxString &s) { systemSchemas = s; }
    wxString GetSystemSchemas() const { return systemSchemas; }

    bool GetExplainVerbose() const { return explainVerbose; }
    void SetExplainVerbose(const bool b) { explainVerbose=b; }
    bool GetExplainAnalyze() const { return explainAnalyze; }
    void SetExplainAnalyze(const bool b) { explainAnalyze=b; }

    bool GetShowUsersForPrivileges() const { return showUsersForPrivileges; }
    void SetShowUsersForPrivileges(const bool b) { showUsersForPrivileges=b; }

    // Show System Objects
    bool GetShowSystemObjects() const { return showSystemObjects; }
    void SetShowSystemObjects(const bool newval);

    // Auto Row Count
    long GetAutoRowCountThreshold() const { return autoRowCountThreshold; }
    void SetAutoRowCountThreshold(const long l) { autoRowCountThreshold=l; }

    long GetIndentSpaces() const { return indentSpaces; }
    void SetIndentSpaces(long l) { indentSpaces=l; }

    bool GetSpacesForTabs() const { return spacesForTabs; }
    void SetSpacesForTabs(const bool newval) { spacesForTabs = newval; }

	// Tab for completion
	bool GetTabForCompletion() const { return tabForCompletion; }
	void SetTabForCompletion(const bool newval) { tabForCompletion = newval; }

    // Sticky SQL
    bool GetStickySql() const { return stickySql; }
    void SetStickySql(const bool newval);

    // Indicate Null
    bool GetIndicateNull() const { return indicateNull; }
    void SetIndicateNull(const bool newval);

    // DoubleClick for Properties
    bool GetDoubleClickProperties() const { return doubleClickProperties; }
    void SetDoubleClickProperties(const bool newval);

    // maximum size of server log to read
    long GetMaxServerLogSize() const { return maxServerLogSize; }
    void SetMaxServerLogSize(long l) { maxServerLogSize = l; }

    bool GetUnicodeFile() const { return unicodeFile; }
    void SetUnicodeFile(const bool b) {unicodeFile = b; }

    bool GetSuppressGuruHints() const { return suppressGuruHints; }
    void SetSuppressGuruHints(const bool b) { suppressGuruHints=b; }

    wxFont GetSQLFont() const { return sqlFont; }
    wxFont GetSystemFont() const { return systemFont; }
    void SetFont(const wxFont &font) { systemFont = font; }
    void SetSQLFont(const wxFont &font) { sqlFont = font; }
    wxString GetCanonicalLanguage() const { return canonicalLanguage; }

    bool Write(const wxString &key, const wxChar *value) { return wxConfig::Write(key, value); }
    bool Write(const wxString &key, long value) { return wxConfig::Write(key, value); }
    bool Write(const wxString &key, int value) { return wxConfig::Write(key, value); }
    bool Write(const wxString &key, bool value);
    bool Write(const wxString &key, const wxPoint &value);
    bool Write(const wxString &key, const wxSize &value);
    bool Write(const wxString &key, const wxSize &size, const wxPoint &point)
        { Write(key, point); Write(key, size); return true;}

    bool Read(const wxString& key, wxString* str, const wxString& defaultVal) const
        { return wxConfig::Read(key, str, defaultVal); }
    bool Read(const wxString& key, bool* str, bool defaultVal) const;
    bool Read(const wxString& key, int* i, int defaultVal) const
        { return wxConfig::Read(key, i, defaultVal); }
    bool Read(const wxString& key, long* l, long defaultVal) const
        { return wxConfig::Read(key, l, defaultVal); }
    wxString Read(const wxString& key, const wxString &defaultVal) const
        { return wxConfig::Read(key, defaultVal); }
    long Read(const wxString& key, long defaultVal) const
        { return wxConfig::Read(key, defaultVal); }
    wxPoint Read(const wxString& key, const wxPoint &defaultVal) const;
    wxSize Read(const wxString& key, const wxSize &defaultVal) const;

    wxString GetExportQuoteChar() const { return exportQuoteChar; }
    wxString GetExportRowSeparator() const { return exportRowSeparator; }
    wxString GetExportColSeparator() const { return exportColSeparator; }
    int GetExportQuoting() const { return exportQuoting; }  // 0=none 1=string 2=all
    bool GetExportUnicode() const { return exportUnicode; }

	wxString GetCopyQuoteChar() const { return copyQuoteChar; }
	wxString GetCopyColSeparator() const { return copyColSeparator; }
	int GetCopyQuoting() const { return copyQuoting; } // 0=none 1=string 2=all

    void SetExportQuoteChar(const wxString &s) { exportQuoteChar=s; }
    void SetExportRowSeparator(const wxString &s) { exportRowSeparator=s; }
    void SetExportColSeparator(const wxString &s) { exportColSeparator=s; }
    void SetExportQuoting(const int i) { exportQuoting = i; }
    void SetExportUnicode(const bool b) { exportUnicode=b; }

	void SetCopyQuoteChar(const wxString &s) { copyQuoteChar=s; }
	void SetCopyColSeparator(const wxString &s) { copyColSeparator=s; }
	void SetCopyQuoting(const int i) { copyQuoting = i; }

	void Save();


	enum configFileName
	{
		PGPASS,
		PGAFAVOURITES,
		PGAMACROS
	};
	static wxString GetConfigFile(configFileName cfgname);

private:

    bool moveStringValue(wxChar *oldKey, wxChar *newKey, int index=-1);
    bool moveLongValue(wxChar *oldKey, wxChar *newKey, int index=-1);

    wxFont systemFont, sqlFont; 

    // Tip Of The Day
    bool showTipOfTheDay;
    int nextTipOfTheDay;

    // Log
    wxString logFile;
    int logLevel;

    // Last connection
    wxString lastDescription;
    wxString lastServer;
    wxString lastDatabase;
    wxString lastUsername;
    int lastPort, lastSSL;

    // Show System Objects
    bool showSystemObjects;

    bool explainVerbose, explainAnalyze;

    wxString slonyPath, postgresqlPath, enterprisedbPath;
    wxString pgHelpPath, edbHelpPath, slonyHelpPath;
    wxString canonicalLanguage;
    bool showUsersForPrivileges;
    bool askSaveConfirmation;
    bool confirmDelete, suppressGuruHints;
    long maxRows, maxColSize, autoRowCountThreshold, indentSpaces;
    bool spacesForTabs, stickySql, indicateNull, unicodeFile, tabForCompletion;
    bool doubleClickProperties;
    long maxServerLogSize;

    wxString searchPath, systemSchemas;

    // export options
    wxString exportRowSeparator;
    wxString exportColSeparator;
    wxString exportQuoteChar;
    int exportQuoting;
    bool exportUnicode;

	// copy options
	wxString copyColSeparator;
	wxString copyQuoteChar;
	int copyQuoting;
};

#endif
