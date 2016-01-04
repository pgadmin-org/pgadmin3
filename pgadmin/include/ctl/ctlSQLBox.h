//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlSQLBox.h - SQL syntax highlighting textbox
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLBOX_H
#define CTLSQLBOX_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>

#include "db/pgConn.h"
#include "dlg/dlgFindReplace.h"

// These structs are from Scintilla.h which isn't easily #included :-(
struct CharacterRange
{
	long cpMin;
	long cpMax;
};

struct TextToFind
{
	struct CharacterRange chrg;
	char *lpstrText;
	struct CharacterRange chrgText;
};

class sysProcess;

// Class declarations
class ctlSQLBox : public wxStyledTextCtrl
{
	static wxString sqlKeywords;

public:
	ctlSQLBox(wxWindow *parent, wxWindowID id = -1, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);
	ctlSQLBox();
	~ctlSQLBox();

	void Create(wxWindow *parent, wxWindowID id = -1, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);

	void SetDatabase(pgConn *db);

	void OnKeyDown(wxKeyEvent &event);
	void OnAutoComplete(wxCommandEvent &event);
	void OnSearchReplace(wxCommandEvent &event);
	void OnKillFocus(wxFocusEvent &event);

	bool Find(const wxString &find, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse);
	bool Replace(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse);
	bool ReplaceAll(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps);
	bool DoFind(const wxString &find, const wxString &replace, bool doReplace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse);
	void SetAutoIndent(bool on)
	{
		m_autoIndent = on;
	}
	void EnableAutoComp(bool on)
	{
		m_autocompDisabled = on;
	}
	bool BlockComment(bool uncomment = false);
	void UpdateLineNumber();
	wxString ExternalFormat();
	void AbortProcess();

	CharacterRange RegexFindText(int minPos, int maxPos, const wxString &text);

	DECLARE_DYNAMIC_CLASS(ctlSQLBox)
	DECLARE_EVENT_TABLE()

protected:
	void OnEndProcess(wxProcessEvent &ev);

	sysProcess *process;
	long processID;
	wxString processOutput, processErrorOutput;
	int processExitCode;

private:
	void OnPositionStc(wxStyledTextEvent &event);
	void OnMarginClick(wxStyledTextEvent &event);

	dlgFindReplace *m_dlgFindReplace;
	pgConn *m_database;
	bool m_autoIndent, m_autocompDisabled;

	friend class QueryPrintout;
};

#endif

