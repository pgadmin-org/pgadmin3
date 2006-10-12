//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLBox.cpp - SQL syntax highlighting textbox
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/stc/stc.h>

// App headers
#include "pgAdmin3.h"
#include "pgSet.h"
#include "ctl/ctlSQLBox.h"
#include "dlgFindReplace.h"
#include "menu.h"

// Must be last for reasons I haven't fully grokked...
#include <wx/regex.h>


wxString ctlSQLBox::sqlKeywords;


BEGIN_EVENT_TABLE(ctlSQLBox, wxStyledTextCtrl)
    EVT_KEY_DOWN(ctlSQLBox::OnKeyDown)
    EVT_MENU(MNU_FIND,ctlSQLBox::OnSearchReplace)
	EVT_MENU(MNU_AUTOCOMPLETE,ctlSQLBox::OnAutoComplete)
	EVT_KILL_FOCUS(ctlSQLBox::OnKillFocus)
    EVT_STC_UPDATEUI(-1,  ctlSQLBox::OnPositionStc)
END_EVENT_TABLE()



IMPLEMENT_DYNAMIC_CLASS(ctlSQLBox, wxStyledTextCtrl)


ctlSQLBox::ctlSQLBox()
{
    m_dlgFindReplace=0;
}


ctlSQLBox::ctlSQLBox(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    m_dlgFindReplace=0;

	m_database=NULL;

    Create(parent, id, pos, size, style);
}


void ctlSQLBox::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    wxLogInfo(wxT("Creating a ctlSQLBox"));

    wxStyledTextCtrl::Create(parent,id , pos, size, style);

    // Clear all styles
    StyleClearAll();
    
    // Font
    extern sysSettings *settings;
    wxFont fntSQLBox = settings->GetSQLFont();

    StyleSetFont(wxSTC_STYLE_DEFAULT, fntSQLBox);
    StyleSetFont(0, fntSQLBox);
    StyleSetFont(1, fntSQLBox);
    StyleSetFont(2, fntSQLBox);
    StyleSetFont(3, fntSQLBox);
    StyleSetFont(4, fntSQLBox);
    StyleSetFont(5, fntSQLBox);
    StyleSetFont(6, fntSQLBox);
    StyleSetFont(7, fntSQLBox);
    StyleSetFont(8, fntSQLBox);
    StyleSetFont(9, fntSQLBox);
    StyleSetFont(10, fntSQLBox);
    StyleSetFont(11, fntSQLBox);

    SetMarginWidth(1, 0);
    SetIndent(settings->GetIndentSpaces());
    
    // Setup the different highlight colurs
    StyleSetForeground(0,  wxColour(0x80, 0x80, 0x80));
    StyleSetForeground(1,  wxColour(0x00, 0x7f, 0x00));
    StyleSetForeground(2,  wxColour(0x00, 0x7f, 0x00));
    StyleSetForeground(3,  wxColour(0x7f, 0x7f, 0x7f));
    StyleSetForeground(4,  wxColour(0x00, 0x7f, 0x7f));
    StyleSetForeground(5,  wxColour(0x00, 0x00, 0x7f));
    StyleSetForeground(6,  wxColour(0x7f, 0x00, 0x7f));
    StyleSetForeground(7,  wxColour(0x7f, 0x00, 0x7f));
    StyleSetForeground(8,  wxColour(0x00, 0x7f, 0x7f));
    StyleSetForeground(9,  wxColour(0x7f, 0x7f, 0x7f));
    StyleSetForeground(10, wxColour(0x00, 0x00, 0x00));
    StyleSetForeground(11, wxColour(0x00, 0x00, 0x00));

	// Brace maching styles
    StyleSetBackground(34, wxColour(0x99, 0xF9, 0xFF));
    StyleSetBackground(35, wxColour(0xFF, 0xCF, 0x27));

    // SQL Lexer and keywords.
    if (sqlKeywords.IsEmpty())
        FillKeywords(sqlKeywords);
    SetLexer(wxSTC_LEX_SQL);
    SetKeyWords(0, sqlKeywords);

    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_CTRL, (int)'F', MNU_FIND);
	entries[1].Set(wxACCEL_CTRL, (int)' ', MNU_AUTOCOMPLETE);
    wxAcceleratorTable accel(2, entries);
    SetAcceleratorTable(accel);

	// Autocompletion configuration
	AutoCompSetSeparator('\t');
	AutoCompSetChooseSingle(true);
	AutoCompSetIgnoreCase(true);
	AutoCompSetFillUps(wxT(" \t"));
	AutoCompSetDropRestOfWord(true);
}

void ctlSQLBox::SetDatabase(pgConn *db)
{
	m_database = db;
}

void ctlSQLBox::OnSearchReplace(wxCommandEvent& ev)
{
    if (!m_dlgFindReplace)
    {
        m_dlgFindReplace = new dlgFindReplace(this);
        m_dlgFindReplace->Show(true);
    }
    else
    {
        m_dlgFindReplace->Show(true);
        m_dlgFindReplace->SetFocus();
    }
}

void ctlSQLBox::Find(const wxString &find, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse)
{
    if (!DoFind(find, wxString(wxEmptyString), false, wholeWord, matchCase, useRegexps, startAtTop, reverse))
         wxMessageBox(_("Reached the end of the document"), _("Find text"), wxICON_EXCLAMATION | wxOK, this);
}

void ctlSQLBox::Replace(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse)
{
    if (!DoFind(find, replace, true, wholeWord, matchCase, useRegexps, startAtTop, reverse))
         wxMessageBox(_("Reached the end of the document"), _("Replace text"), wxICON_EXCLAMATION | wxOK, this);
}

void ctlSQLBox::ReplaceAll(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps)
{
    // Use DoFind to repeatedly replace text
    int count = 0;
    int initialPos = GetCurrentPos();

    while(DoFind(find, replace, true, wholeWord, matchCase, useRegexps, true, false))
        count++;

    GotoPos(initialPos);

    wxString msg;
    msg.Printf(_("%d replacements made."), count);
    wxMessageBox(msg, _("Replace all"));
}

bool ctlSQLBox::DoFind(const wxString &find, const wxString &replace, bool doReplace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse)
{
    int flags = 0;
    int startPos = GetSelectionStart();
    int endPos = GetTextLength();

    if (wholeWord)
        flags |= wxSTC_FIND_WHOLEWORD;

    if (matchCase)
        flags |= wxSTC_FIND_MATCHCASE;

    if (startAtTop)
        startPos = 0;
    
    if (!reverse)
    {
        if (!doReplace)
            startPos += 1;
    }
    else
    {
        endPos = 0;
        if (!doReplace)
            startPos -= 1;
    }

    size_t selStart = 0, selEnd = 0;
    
    if (useRegexps)
    {
        wxRegEx *re = new wxRegEx(find);
        wxString section = GetText().Mid(startPos, startPos + endPos);
        if (!re->IsValid() || !re->Matches(section))
        {
            selStart = (unsigned int)(-1);
            selEnd = 0;
        }
        else
        {
            re->GetMatch(&selStart, &selEnd);
            selStart += startPos;
            selEnd += selStart;
        }
    }
    else
    {
        selStart = FindText(startPos, endPos, find, flags);
        selEnd = selStart + find.Length();
    }

    if (selStart >= 0 && selStart != (unsigned int)(-1))
    {
        SetSelectionStart(selStart);
        SetSelectionEnd(selEnd);
        if (doReplace)
        {
            ReplaceSelection(replace);
            SetSelectionStart(selStart);
            SetSelectionEnd(selStart + replace.Length());
        }
        EnsureCaretVisible();
        return true;
    }
    else
    {
        return false;
    }
}

void ctlSQLBox::OnKeyDown(wxKeyEvent& event)
{
#ifdef __WXGTK__
	event.m_metaDown=false;
#endif

	if (!AutoCompActive() &&
		 ( settings->GetTabForCompletion() && /* autocomplete on tab only if specifically configured */
		  !event.AltDown() && !event.CmdDown() && !event.ControlDown() && event.GetKeyCode() == '\t'
		 ))
	{
		wxCommandEvent e;
		OnAutoComplete(e);
	}
	else 
		event.Skip();
}

void ctlSQLBox::OnKillFocus(wxFocusEvent& event)
{
	AutoCompCancel();
    event.Skip();
}

void ctlSQLBox::OnPositionStc(wxStyledTextEvent& event)
{
	int pos = GetCurrentPos();
	wxChar ch = GetCharAt(pos-1);
    int st = GetStyleAt(pos-1);
	int match;

	// Clear all highlighting
	BraceBadLight(wxSTC_INVALID_POSITION);

    // Check for braces that aren't in comment styles
	if ((ch == '{' || ch == '}' ||
		 ch == '[' || ch == ']' ||
		 ch == '(' || ch == ')') &&
         st != 2) 
	{
		match = BraceMatch(pos-1);
		if (match != wxSTC_INVALID_POSITION)
			BraceHighlight(pos-1, match);
	}

	// Roll back through the doc and highlight any unmatched braces
	while ((pos--) >= 0)
	{
		ch = GetCharAt(pos);
        st = GetStyleAt(pos);

		if ((ch == '{' || ch == '}' ||
			 ch == '[' || ch == ']' ||
			 ch == '(' || ch == ')') &&
             st != 2)
		{
			match = BraceMatch(pos);
			if (match == wxSTC_INVALID_POSITION)
			{
				BraceBadLight(pos);
				break;
			}
		}
	}
    
	event.Skip();
}

extern "C" char *tab_complete(const char *allstr, const int startptr, const int endptr, void *dbptr);
void ctlSQLBox::OnAutoComplete(wxCommandEvent& rev)
{
	if (GetReadOnly())
		return;
	if (m_database == NULL)
		return;

	wxString what = GetCurLine().Left(GetCurrentPos()-PositionFromLine(GetCurrentLine()));;
	int spaceidx = what.Find(' ',true);
	
	char *tab_ret;
	if (spaceidx == -1)
		tab_ret = tab_complete(what.mb_str(wxConvUTF8), 0, what.Len()+1, m_database);
	else
		tab_ret = tab_complete(what.mb_str(wxConvUTF8), spaceidx+1, what.Len()+1, m_database);

	if (tab_ret == NULL || tab_ret[0] == '\0')
		return; /* No autocomplete available for this string */

	wxString wxRet = wxString(tab_ret, wxConvUTF8);
	free(tab_ret);

	if (spaceidx == -1)
		AutoCompShow(what.Len(), wxRet);
	else
		AutoCompShow(what.Len()-spaceidx-1, wxRet);
}


ctlSQLBox::~ctlSQLBox()
{
    wxLogInfo(wxT("Destroying a ctlSQLBox"));
    if (m_dlgFindReplace)
    {
        m_dlgFindReplace->Destroy();
        m_dlgFindReplace=0;
    }
}


/*
 * Callback function from tab-complete.c, bridging the gap between C++ and C.
 * Execute a query using the C++ APIs, returning it as a tab separated
 * "char*-string"
 * The query is expected to return only one column, and will have an ORDER BY
 * clause for this column added automatically.
 */
extern "C"
char *pg_query_to_single_ordered_string(char *query, void *dbptr)
{
	pgConn *db = (pgConn *)dbptr;
	pgSet *res = db->ExecuteSet(wxString(query, wxConvUTF8) + wxT(" ORDER BY 1"));
	if (!res)
		return NULL;

	wxString ret = wxString();
    wxString tmp;

	while (!res->Eof())
	{
        tmp =  res->GetVal(0);
        if (tmp.Mid(tmp.Length() - 1) == wxT("."))
		    ret += tmp + wxT("\t");
        else
		    ret += tmp + wxT(" \t");

		res->MoveNext();
	}

	ret.Trim();
	// Trims both space and tab, but we want to keep the space!
	if (ret.Length() > 0)
	    ret += wxT(" ");

	return strdup(ret.mb_str(wxConvUTF8));
}
