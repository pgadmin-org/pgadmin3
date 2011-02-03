//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
	sysSettings(const wxString &name);

	// Display options
	bool GetDisplayOption(const wxString &objtype, bool GetDefault = false);
	void SetDisplayOption(const wxString &objtype, bool display);

	// Log
	wxString GetLogFile();
	void SetLogFile(const wxString &newval)
	{
		Write(wxT("LogFile"), newval);
		sysLogger::logFile = newval;
	}
	int GetLogLevel() const
	{
		int i;
		Read(wxT("LogLevel"), &i, LOG_ERRORS);
		return i;
	}
	void SetLogLevel(const int newval)
	{
		Write(wxT("LogLevel"), newval);
		sysLogger::logLevel = newval;
	}

	// Last connection
	wxString GetLastDescription() const
	{
		wxString s;
		Read(wxT("LastDescription"), &s, wxT("PostgreSQL Server"));
		return s;
	}
	void SetLastDescription(const wxString &newval)
	{
		Write(wxT("LastDescription"), newval);
	}
	wxString GetLastServer() const
	{
		wxString s;
		Read(wxT("LastServer"), &s, wxT("localhost"));
		return s;
	}
	void SetLastServer(const wxString &newval)
	{
		Write(wxT("LastServer"), newval);
	}
	wxString GetLastDatabase() const
	{
		wxString s;
		Read(wxT("LastDatabase"), &s, wxT("postgres"));
		return s;
	}
	void SetLastDatabase(const wxString &newval)
	{
		Write(wxT("LastDatabase"), newval);
	}
	wxString GetLastUsername() const
	{
		wxString s;
		Read(wxT("LastUsername"), &s, wxT("postgres"));
		return s;
	}
	void SetLastUsername(const wxString &newval)
	{
		Write(wxT("LastUsername"), newval);
	}
	int GetLastPort() const
	{
		int i;
		Read(wxT("LastPort"), &i, 5432);
		return i;
	}
	void SetLastPort(const int newval)
	{
		Write(wxT("LastPort"), newval);
	}
	int GetLastSSL() const
	{
		int i;
		Read(wxT("LastSSL"), &i, 0);
		return i;
	}
	void SetLastSSL(const int newval)
	{
		Write(wxT("LastSSL"), newval);
	}

	// Helper paths
	wxString GetSlonyPath() const
	{
		wxString s;
		Read(wxT("SlonyPath"), &s, wxEmptyString);
		return s;
	}
	void SetSlonyPath(const wxString &newval)
	{
		Write(wxT("SlonyPath"), newval);
	}
	wxString GetPostgresqlPath() const
	{
		wxString s;
		Read(wxT("PostgreSQLPath"), &s, wxEmptyString);
		return s;
	}
	void SetPostgresqlPath(const wxString &newval)
	{
		Write(wxT("PostgreSQLPath"), newval);
	}
	wxString GetEnterprisedbPath() const
	{
		wxString s;
		Read(wxT("EnterpriseDBPath"), &s, wxEmptyString);
		return s;
	}
	void SetEnterprisedbPath(const wxString &newval)
	{
		Write(wxT("EnterpriseDBPath"), newval);
	}
	wxString GetGPDBPath() const
	{
		wxString s;
		Read(wxT("GreenplumDBPath"), &s, wxEmptyString);
		return s;
	}
	void SetGPDBPath(const wxString &newval)
	{
		Write(wxT("GreenplumDBPath"), newval);
	}

	// Help paths
	wxString GetSlonyHelpPath();
	void SetSlonyHelpPath(const wxString &newval)
	{
		Write(wxT("SlonyHelpPath"), newval);
	}
	wxString GetPgHelpPath();
	void SetPgHelpPath(const wxString &newval)
	{
		Write(wxT("PostgreSQLHelpPath"), newval);
	}
	wxString GetEdbHelpPath();
	void SetEdbHelpPath(const wxString &newval)
	{
		Write(wxT("EnterpriseDBHelpPath"), newval);
	}
	wxString GetGpHelpPath();
	void SetGpHelpPath(const wxString &newval)
	{
		Write(wxT("GreenplumDBHelpPath"), newval);
	}

	// Copy options
	wxString GetCopyQuoteChar() const
	{
		wxString s;
		Read(wxT("Copy/QuoteChar"), &s, wxT("\""));
		return s;
	}
	void SetCopyQuoteChar(const wxString &newval)
	{
		Write(wxT("Copy/QuoteChar"), newval);
	}
	wxString GetCopyColSeparator() const
	{
		wxString s;
		Read(wxT("Copy/ColSeparator"), &s, wxT(";"));
		return s;
	}
	void SetCopyColSeparator(const wxString &newval)
	{
		Write(wxT("Copy/ColSeparator"), newval);
	}
	int GetCopyQuoting(); // 0=none 1=string 2=all
	void SetCopyQuoting(const int i);

	// Export options
	wxString GetExportQuoteChar() const
	{
		wxString s;
		Read(wxT("Export/QuoteChar"), &s, wxT("\""));
		return s;
	}
	void SetExportQuoteChar(const wxString &newval)
	{
		Write(wxT("Export/QuoteChar"), newval);
	}
	wxString GetExportColSeparator() const
	{
		wxString s;
		Read(wxT("Export/ColSeparator"), &s, wxT(";"));
		return s;
	}
	void SetExportColSeparator(const wxString &newval)
	{
		Write(wxT("Export/ColSeparator"), newval);
	}
	wxString GetExportRowSeparator();
	void SetExportRowSeparator(const wxString &s);
	int GetExportQuoting();  // 0=none 1=string 2=all
	void SetExportQuoting(const int i);
	bool GetExportUnicode() const
	{
		bool b;
		Read(wxT("Export/Unicode"), &b, true);
		return b;
	}
	void SetExportUnicode(const bool newval)
	{
		Write(wxT("Export/Unicode"), newval);
	}

	// Explain options
	bool GetExplainVerbose() const
	{
		bool b;
		Read(wxT("frmQuery/ExplainVerbose"), &b, false);
		return b;
	}
	void SetExplainVerbose(const bool newval)
	{
		Write(wxT("frmQuery/ExplainVerbose"), newval);
	}
	bool GetExplainCosts() const
	{
		bool b;
		Read(wxT("frmQuery/ExplainCosts"), &b, true);
		return b;
	}
	void SetExplainCosts(const bool newval)
	{
		Write(wxT("frmQuery/ExplainCosts"), newval);
	}
	bool GetExplainBuffers() const
	{
		bool b;
		Read(wxT("frmQuery/ExplainBuffers"), &b, false);
		return b;
	}
	void SetExplainBuffers(const bool newval)
	{
		Write(wxT("frmQuery/ExplainBuffers"), newval);
	}

	// Display options
	wxString GetSystemSchemas() const
	{
		wxString s;
		Read(wxT("SystemSchemas"), &s, wxEmptyString);
		return s;
	}
	void SetSystemSchemas(const wxString &newval)
	{
		Write(wxT("SystemSchemas"), newval);
	}
	bool GetShowUsersForPrivileges() const
	{
		bool b;
		Read(wxT("ShowUsersForPrivileges"), &b, false);
		return b;
	}
	void SetShowUsersForPrivileges(const bool newval)
	{
		Write(wxT("ShowUsersForPrivileges"), newval);
	}
	bool GetShowSystemObjects() const
	{
		bool b;
		Read(wxT("ShowSystemObjects"), &b, false);
		return b;
	}
	void SetShowSystemObjects(const bool newval)
	{
		Write(wxT("ShowSystemObjects"), newval);
	}

	// Editor options
	bool GetSpacesForTabs() const
	{
		bool b;
		Read(wxT("SpacesForTabs"), &b, false);
		return b;
	}
	void SetSpacesForTabs(const bool newval)
	{
		Write(wxT("SpacesForTabs"), newval);
	}
	long GetIndentSpaces() const
	{
		long l;
		Read(wxT("IndentSpaces"), &l, 0L);
		return l;
	}
	void SetIndentSpaces(const long newval)
	{
		Write(wxT("IndentSpaces"), newval);
	}
	bool GetIndicateNull() const
	{
		bool b;
		Read(wxT("frmQuery/IndicateNull"), &b, false);
		return b;
	}
	void SetIndicateNull(const bool newval)
	{
		Write(wxT("frmQuery/IndicateNull"), newval);
	}
	bool GetAutoRollback() const
	{
		bool b;
		Read(wxT("frmQuery/AutoRollback"), &b, true);
		return b;
	}
	void SetAutoRollback(const bool newval)
	{
		Write(wxT("frmQuery/AutoRollback"), newval);
	}
	bool GetLineNumber() const
	{
		bool b;
		Read(wxT("ShowLineNumber"), &b, false);
		return b;
	}
	void SetLineNumber(const bool newval)
	{
		Write(wxT("ShowLineNumber"), newval);
	}
	bool GetUnicodeFile() const
	{
		bool b;
		Read(wxT("WriteUnicodeFile"), &b, true);
		return b;
	}
	void SetUnicodeFile(const bool newval)
	{
		Write(wxT("WriteUnicodeFile"), newval);
	}
	wxFont GetSystemFont();
	void SetSystemFont(const wxFont &font);
	wxFont GetSQLFont();
	void SetSQLFont(const wxFont &font);
	int GetLineEndingType() const
	{
		int i;
		Read(wxT("LineEndingType"), &i, 2);
		return i;
	}
	void SetLineEndingType(const int newval)
	{
		Write(wxT("LineEndingType"), newval);
	}
	wxString GetFavouritesFile();
	void SetFavouritesFile(const wxString &newval)
	{
		Write(wxT("FavouritesFile"), newval);
	}
	wxString GetMacrosFile();
	void SetMacrosFile(const wxString &newval)
	{
		Write(wxT("MacrosFile"), newval);
	}
	wxString GetHistoryFile();
	void SetHistoryFile(const wxString &newval)
	{
		Write(wxT("History/File"), newval);
	}
	long  GetHistoryMaxQueries() const
	{
		long l;
		Read(wxT("History/MaxQueries"), &l, 10L);
		return l;
	}
	void SetHistoryMaxQueries(const long newval)
	{
		Write(wxT("History/MaxQueries"), newval);
	}
	long  GetHistoryMaxQuerySize() const
	{
		long l;
		Read(wxT("History/MaxQuerySize"), &l, 1024L);
		return l;
	}
	void SetHistoryMaxQuerySize(const long newval)
	{
		Write(wxT("History/MaxQuerySize"), newval);
	}

	// Status Colours options
	wxString GetIdleProcessColour() const
	{
		wxString s;
		Read(wxT("IdleProcessColour"), &s, wxT("#5fa4d9"));
		return s;
	}
	void SetIdleProcessColour(const wxString &newval)
	{
		Write(wxT("IdleProcessColour"), newval);
	}
	wxString GetActiveProcessColour() const
	{
		wxString s;
		Read(wxT("ActiveProcessColour"), &s, wxT("#5fd95f"));
		return s;
	}
	void SetActiveProcessColour(const wxString &newval)
	{
		Write(wxT("ActiveProcessColour"), newval);
	}
	wxString GetSlowProcessColour() const
	{
		wxString s;
		Read(wxT("SlowProcessColour"), &s, wxT("#d9a75f"));
		return s;
	}
	void SetSlowProcessColour(const wxString &newval)
	{
		Write(wxT("SlowProcessColour"), newval);
	}
	wxString GetBlockedProcessColour() const
	{
		wxString s;
		Read(wxT("BlockedProcessColour"), &s, wxT("#d96e5f"));
		return s;
	}
	void SetBlockedProcessColour(const wxString &newval)
	{
		Write(wxT("BlockedProcessColour"), newval);
	}

	// SQL Editor Colours options
	bool GetSQLBoxUseSystemBackground() const
	{
		bool b;
		Read(wxT("ctlSQLBox/UseSystemBackground"), &b, true);
		return b;
	}
	void SetSQLBoxUseSystemBackground(const bool newval)
	{
		Write(wxT("ctlSQLBox/UseSystemBackground"), newval);
	}
	bool GetSQLBoxUseSystemForeground() const
	{
		bool b;
		Read(wxT("ctlSQLBox/UseSystemForeground"), &b, true);
		return b;
	}
	void SetSQLBoxUseSystemForeground(const bool newval)
	{
		Write(wxT("ctlSQLBox/UseSystemForeground"), newval);
	}

	wxString GetSQLBoxColourBackground() const
	{
		wxString s;
		Read(wxT("ctlSQLBox/ColourBackground"), &s, wxT("#ffffff"));
		return s;
	}
	void SetSQLBoxColourBackground(const wxString &newval)
	{
		Write(wxT("ctlSQLBox/ColourBackground"), newval);
	}
	wxString GetSQLBoxColourForeground() const
	{
		wxString s;
		Read(wxT("ctlSQLBox/ColourForeground"), &s, wxT("#000000"));
		return s;
	}
	void SetSQLBoxColourForeground(const wxString &newval)
	{
		Write(wxT("ctlSQLBox/ColourForeground"), newval);
	}

	wxString GetSQLBoxColour(int index) const
	{
		wxString s;
		Read(wxString::Format(wxT("ctlSQLBox/Colour%i"), index), &s, getDefaultElementColor(index));
		return s;
	}
	void SetSQLBoxColour(int index, const wxString &newval)
	{
		Write(wxString::Format(wxT("ctlSQLBox/Colour%i"), index), newval);
	}

	wxString GetSQLMarginBackgroundColour() const
	{
		wxString s;
		Read(wxT("ctlSQLBox/MarginBackgroundColour"), &s, wxT("#dddddd"));
		return s;
	}
	void SetSQLMarginBackgroundColour(const wxString &newval)
	{
		Write(wxT("ctlSQLBox/MarginBackgroundColour"), newval);
	}

	// Misc options
	long GetAutoRowCountThreshold() const
	{
		long l;
		Read(wxT("AutoRowCount"), &l, 2000L);
		return l;
	}
	void SetAutoRowCountThreshold(const long newval)
	{
		Write(wxT("AutoRowCount"), newval);
	}
	bool GetStickySql() const
	{
		bool b;
		Read(wxT("StickySql"), &b, false);
		return b;
	}
	void SetStickySql(const bool newval)
	{
		Write(wxT("StickySql"), newval);
	}
	bool GetDoubleClickProperties() const
	{
		bool b;
		Read(wxT("DoubleClickProperties"), &b, false);
		return b;
	}
	void SetDoubleClickProperties(const bool newval)
	{
		Write(wxT("DoubleClickProperties"), newval);
	}
	long GetMaxServerLogSize() const
	{
		long l;
		Read(wxT("MaxServerLogSize"), &l, 100000L);
		return l;
	}
	void SetMaxServerLogSize(const long newval)
	{
		Write(wxT("MaxServerLogSize"), newval);
	}
	bool GetSuppressGuruHints() const
	{
		bool b;
		Read(wxT("SuppressGuruHints"), &b, false);
		return b;
	}
	void SetSuppressGuruHints(const bool newval)
	{
		Write(wxT("SuppressGuruHints"), newval);
	}
	long GetMaxRows() const
	{
		long l;
		Read(wxT("frmQuery/MaxRows"), &l, 100L);
		return l;
	}
	void SetMaxRows(const long newval)
	{
		Write(wxT("frmQuery/MaxRows"), newval);
	}
	long GetMaxColSize() const
	{
		long l;
		Read(wxT("frmQuery/MaxColSize"), &l, 256L);
		return l;
	}
	void SetMaxColSize(const long newval)
	{
		Write(wxT("frmQuery/MaxColSize"), newval);
	}
	bool GetAskSaveConfirmation() const
	{
		bool b;
		Read(wxT("AskSaveConfirmation"), &b, true);
		return b;
	}
	void SetAskSaveConfirmation(const bool newval)
	{
		Write(wxT("AskSaveConfirmation"), newval);
	}
	bool GetConfirmDelete() const
	{
		bool b;
		Read(wxT("ConfirmDelete"), &b, true);
		return b;
	}
	void SetConfirmDelete(const bool newval)
	{
		Write(wxT("ConfirmDelete"), newval);
	}
	wxString GetCanonicalLanguageName();
	wxLanguage GetCanonicalLanguage() const
	{
		int i;
		Read(wxT("LanguageId"), &i, wxLANGUAGE_UNKNOWN);
		return (wxLanguage)i;
	}
	void SetCanonicalLanguage(const wxLanguage &lang);
	bool GetIgnoreVersion() const
	{
		bool b;
		Read(wxT("IgnoreVersion"), &b, false);
		return b;
	}
	void SetIgnoreVersion(const bool newval)
	{
		Write(wxT("IgnoreVersion"), newval);
	}

	// Functions for storing settings
	bool Write(const wxString &key, const wxChar *value)
	{
		return wxConfig::Write(key, value);
	}
	bool Write(const wxString &key, long value)
	{
		return wxConfig::Write(key, value);
	}
	bool Write(const wxString &key, int value)
	{
		return wxConfig::Write(key, value);
	}
	bool Write(const wxString &key, bool value);
	bool Write(const wxString &key, const wxPoint &value);
	bool Write(const wxString &key, const wxSize &value);
	bool Write(const wxString &key, const wxSize &size, const wxPoint &point)
	{
		Write(key, point);
		Write(key, size);
		return true;
	}

	// Functions for reading settings
	bool Read(const wxString &key, wxString *str, const wxString &defaultVal) const;
	bool Read(const wxString &key, bool *str, bool defaultVal) const;
	bool Read(const wxString &key, int *i, int defaultVal) const;
	bool Read(const wxString &key, long *l, long defaultVal) const;
	wxString Read(const wxString &key, const wxString &defaultVal) const;
	long Read(const wxString &key, long defaultVal) const;
	wxPoint Read(const wxString &key, const wxPoint &defaultVal) const;
	wxSize Read(const wxString &key, const wxSize &defaultVal) const;

	enum configFileName
	{
		PGPASS
	};
	static wxString GetConfigFile(configFileName cfgname);

private:

	static const wxString &getDefaultElementColor(int index)
	{
		static const wxString colors[] =
		{
			wxT("#808080"), wxT("#007f00"), wxT("#007f00"), wxT("#7f7f7f"),
			wxT("#007f7f"), wxT("#00007f"), wxT("#7f007f"), wxT("#7f007f"),
			wxT("#007f7f"), wxT("#7f7f7f"), wxT("#000000"), wxT("#000000")
		};
		return colors[index];
	}

	bool moveStringValue(const wxChar *oldKey, const wxChar *newKey, int index = -1);
	bool moveLongValue(const wxChar *oldKey, const wxChar *newKey, int index = -1);

	wxFileConfig *defaultSettings;
};

#endif
