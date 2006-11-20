//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

#include "pgConn.h"
#include "dlgFindReplace.h"

// These structs are from Scintilla.h which isn't easily #included :-(
struct CharacterRange {
	long cpMin;
	long cpMax;
};

struct TextToFind {
	struct CharacterRange chrg;
	char *lpstrText;
	struct CharacterRange chrgText;
};

// Class declarations
class ctlSQLBox : public wxStyledTextCtrl
{
    static wxString sqlKeywords;

public:
    ctlSQLBox(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    ctlSQLBox();
    ~ctlSQLBox();

    void Create(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	void SetDatabase(pgConn *db);

    void OnKeyDown(wxKeyEvent& event);
	void OnAutoComplete(wxCommandEvent& event);
    void OnSearchReplace(wxCommandEvent& event);
	void OnKillFocus(wxFocusEvent& event);

    bool Find(const wxString &find, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse);
    bool Replace(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse);
    bool ReplaceAll(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps);
    bool DoFind(const wxString &find, const wxString &replace, bool doReplace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse);
    
    CharacterRange RegexFindText(int minPos, int maxPos, const wxString& text);

    DECLARE_DYNAMIC_CLASS(ctlSQLBox)
    DECLARE_EVENT_TABLE()
		
private:

    void OnPositionStc(wxStyledTextEvent& event);

    dlgFindReplace* m_dlgFindReplace;
	pgConn *m_database;
};


#endif
