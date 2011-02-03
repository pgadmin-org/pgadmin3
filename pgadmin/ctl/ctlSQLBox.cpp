//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlSQLBox.cpp - SQL syntax highlighting textbox
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/sysopt.h>

// App headers
#include "db/pgSet.h"
#include "ctl/ctlSQLBox.h"
#include "dlg/dlgFindReplace.h"
#include "frm/menu.h"

// Must be last for reasons I haven't fully grokked...
#include <wx/regex.h>

wxString ctlSQLBox::sqlKeywords;

// Additional pl/pgsql keywords we should highlight
wxString plpgsqlKeywords = wxT(" elsif exception exit loop raise record return text while");
//
// Additional Text Search keywords we should highlight
wxString ftsKeywords = wxT(" gettoken lextypes headline init lexize");

// Additional pgScript keywords we should highlight
wxString pgscriptKeywords = wxT(" assert break columns continue date datetime file go lines ")
                            wxT(" log print record reference regexrmline string waitfor while");

BEGIN_EVENT_TABLE(ctlSQLBox, wxStyledTextCtrl)
	EVT_KEY_DOWN(ctlSQLBox::OnKeyDown)
	EVT_MENU(MNU_FIND, ctlSQLBox::OnSearchReplace)
	EVT_MENU(MNU_AUTOCOMPLETE, ctlSQLBox::OnAutoComplete)
	EVT_KILL_FOCUS(ctlSQLBox::OnKillFocus)
	EVT_STC_UPDATEUI(-1,  ctlSQLBox::OnPositionStc)
END_EVENT_TABLE()



IMPLEMENT_DYNAMIC_CLASS(ctlSQLBox, wxStyledTextCtrl)


ctlSQLBox::ctlSQLBox()
{
	m_dlgFindReplace = 0;
	m_autoIndent = false;
	m_autocompDisabled = false;
}


ctlSQLBox::ctlSQLBox(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
{
	m_dlgFindReplace = 0;

	m_database = NULL;

	m_autocompDisabled = false;

	Create(parent, id, pos, size, style);
}


void ctlSQLBox::Create(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
{
	wxStyledTextCtrl::Create(parent, id , pos, size, style);

	// Clear all styles
	StyleClearAll();

	// Font
	extern sysSettings *settings;
	wxFont fntSQLBox = settings->GetSQLFont();

	wxColour bgColor = settings->GetSQLBoxColourBackground();
	if (settings->GetSQLBoxUseSystemBackground())
	{
		bgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	}

	wxColour frColor = settings->GetSQLBoxColourForeground();
	if (settings->GetSQLBoxUseSystemForeground())
	{
		frColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	}
	StyleSetBackground(wxSTC_STYLE_DEFAULT, bgColor);
	StyleSetForeground(wxSTC_STYLE_DEFAULT, frColor);
	StyleSetFont(wxSTC_STYLE_DEFAULT, fntSQLBox);

	SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

	SetMarginWidth(1, 0);
	SetTabWidth(settings->GetIndentSpaces());
	SetUseTabs(!settings->GetSpacesForTabs());

	// Setup the different highlight colurs
	for (int i = 0; i < 34; ++ i )
	{
		if (i > 0 && i < 12)
			StyleSetForeground(i, settings->GetSQLBoxColour(i));
		else
			StyleSetForeground(i, frColor);
		StyleSetBackground(i, bgColor);
		StyleSetFont(i, fntSQLBox);
	}

	// Margin style
	StyleSetBackground(wxSTC_STYLE_LINENUMBER, settings->GetSQLMarginBackgroundColour());

	// Brace maching styles
	StyleSetBackground(34, wxColour(0x99, 0xF9, 0xFF));
	StyleSetBackground(35, wxColour(0xFF, 0xCF, 0x27));
	StyleSetFont(34, fntSQLBox);
	StyleSetFont(35, fntSQLBox);

	// SQL Lexer and keywords.
	if (sqlKeywords.IsEmpty())
		FillKeywords(sqlKeywords);
	SetLexer(wxSTC_LEX_SQL);
	SetKeyWords(0, sqlKeywords + plpgsqlKeywords + ftsKeywords + pgscriptKeywords);

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

	SetEOLMode(settings->GetLineEndingType());
}

void ctlSQLBox::SetDatabase(pgConn *db)
{
	m_database = db;
}

void ctlSQLBox::OnSearchReplace(wxCommandEvent &ev)
{
	if (!m_dlgFindReplace)
	{
		m_dlgFindReplace = new dlgFindReplace(this);
		m_dlgFindReplace->Show(true);
		m_dlgFindReplace->FocusSearch();
	}
	else
	{
		m_dlgFindReplace->Show(true);
		m_dlgFindReplace->SetFocus();
		m_dlgFindReplace->FocusSearch();
	}
}

bool ctlSQLBox::Find(const wxString &find, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse)
{
	if (!DoFind(find, wxString(wxEmptyString), false, wholeWord, matchCase, useRegexps, startAtTop, reverse))
	{
		wxWindow *w = wxWindow::FindFocus();
		wxMessageBox(_("Reached the end of the document"), _("Find text"), wxICON_EXCLAMATION | wxOK, w);
		return false;
	}
	return true;
}

bool ctlSQLBox::Replace(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse)
{
	if (!DoFind(find, replace, true, wholeWord, matchCase, useRegexps, startAtTop, reverse))
	{
		wxWindow *w = wxWindow::FindFocus();
		wxMessageBox(_("Reached the end of the document"), _("Replace text"), wxICON_EXCLAMATION | wxOK, w);
		return false;
	}
	return true;
}

bool ctlSQLBox::ReplaceAll(const wxString &find, const wxString &replace, bool wholeWord, bool matchCase, bool useRegexps)
{
	// Use DoFind to repeatedly replace text
	int count = 0;
	int initialPos = GetCurrentPos();
	GotoPos(0);

	while(DoFind(find, replace, true, wholeWord, matchCase, useRegexps, false, false))
		count++;

	GotoPos(initialPos);

	wxString msg;
	msg.Printf(wxPLURAL("%d replacement made.", "%d replacements made.", count), count);
	wxMessageBox(msg, _("Replace all"));

	if (count)
		return true;
	else
		return false;
}

bool ctlSQLBox::DoFind(const wxString &find, const wxString &replace, bool doReplace, bool wholeWord, bool matchCase, bool useRegexps, bool startAtTop, bool reverse)
{
	int flags = 0;
	int startPos = GetSelectionStart();
	int endPos = GetTextLength();

	// Setup flags
	if (wholeWord)
		flags |= wxSTC_FIND_WHOLEWORD;

	if (matchCase)
		flags |= wxSTC_FIND_MATCHCASE;

	// Replace the current selection, if there is one and it matches the find param.
	wxString current = GetSelectedText();
	if (doReplace)
	{
		if (useRegexps)
		{
			CharacterRange cr = RegexFindText(GetSelectionStart(), GetSelectionEnd(), find);
			if (GetSelectionStart() == cr.cpMin && GetSelectionEnd() == cr.cpMax)
			{
				if (cr.cpMin == cr.cpMax) // Must be finding a special char, such as $ (line end)
				{
					InsertText(cr.cpMax, replace);
					SetSelection(cr.cpMax, cr.cpMax + replace.Length());
					SetCurrentPos(cr.cpMax + replace.Length());

					// Stop if we've got to the end. This is important for the $
					// case where it'll just keep finding the end of the line!!
					if ((int)(cr.cpMin + replace.Length()) == GetLength())
						return false;
				}
				else
				{
					ReplaceSelection(replace);
					SetSelection(startPos, startPos + replace.Length());
					SetCurrentPos(startPos + replace.Length());
				}
			}
		}
		else if ((matchCase && current == find) || (!matchCase && current.Upper() == find.Upper()))
		{
			ReplaceSelection(replace);
			if (!reverse)
			{
				SetSelection(startPos, startPos + replace.Length());
				SetCurrentPos(startPos + replace.Length());
			}
			else
			{
				SetSelection(startPos + replace.Length(), startPos);
				SetCurrentPos(startPos);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Figure out the starting position for the next search
	////////////////////////////////////////////////////////////////////////

	if (startAtTop)
	{
		startPos = 0;
		endPos = GetTextLength();
	}
	else
	{
		if (reverse)
		{
			endPos = 0;
			startPos = GetCurrentPos();
		}
		else
		{
			endPos = GetTextLength();
			startPos = GetCurrentPos();
		}
	}

	size_t selStart = 0, selEnd = 0;

	if (useRegexps)
	{
		CharacterRange cr = RegexFindText(startPos, endPos, find);
		selStart = cr.cpMin;
		selEnd = cr.cpMax;
	}
	else
	{
		selStart = FindText(startPos, endPos, find, flags);
		selEnd = selStart + find.Length();
	}

	if (selStart >= 0 && selStart != (size_t)(-1))
	{
		if (reverse)
		{
			SetCurrentPos(selStart);
			SetSelection(selEnd, selStart);
		}
		else
		{
			SetCurrentPos(selEnd);
			SetSelection(selStart, selEnd);
		}
		EnsureCaretVisible();
		return true;
	}
	else
		return false;
}

void ctlSQLBox::OnKeyDown(wxKeyEvent &event)
{
#ifdef __WXGTK__
	event.m_metaDown = false;
#endif

	// Get the line ending type
	wxString lineEnd;
	switch (GetEOLMode())
	{
		case wxSTC_EOL_LF:
			lineEnd = wxT("\n");
			break;
		case wxSTC_EOL_CRLF:
			lineEnd = wxT("\r\n");
			break;
		case wxSTC_EOL_CR:
			lineEnd = wxT("\r");
			break;
	}

	// Block comment/uncomment
	if (event.GetKeyCode() == 'K')
	{
		// Comment (Ctrl+k)
		if (event.GetModifiers() == wxMOD_CONTROL)
		{
			if (BlockComment(false))
				return;
		}
		// Uncomment (Ctrl+Shift+K)
		else if (event.GetModifiers() == (wxMOD_CONTROL | wxMOD_SHIFT))
		{
			if (BlockComment(true))
				return;
		}
	}

	// Autoindent
	if (m_autoIndent && event.GetKeyCode() == WXK_RETURN)
	{
		wxString indent, line;
		line = GetLine(GetCurrentLine());

		// Get the offset for the current line - basically, whether
		// or not it ends with a \r\n, \n or \r, and if so, the length
		int offset =  0;
		if (line.EndsWith(wxT("\r\n")))
			offset = 2;
		else if (line.EndsWith(wxT("\n")))
			offset = 1;
		else if (line.EndsWith(wxT("\r")))
			offset = 1;

		// Get the indent. This is every leading space or tab on the
		// line, up until the current cursor position.
		int x = 0;
		int max = line.Length() - (GetLineEndPosition(GetCurrentLine()) - GetCurrentPos()) - offset;
		while ((line[x] == '\t' || line[x] == ' ') && x < max)
			indent += line[x++];

		// Select any indent in front of the cursor to be removed. If
		// the cursor is positioned after any non-indent characters,
		// we don't remove anything. If there is already some selected,
		// don't select anything new at all.
		if (indent.Length() != 0 &&
		        (unsigned int)GetCurrentPos() <= ((GetLineEndPosition(GetCurrentLine()) - line.Length()) + indent.Length() + offset) &&
		        GetSelectedText() == wxEmptyString)
			SetSelection(GetLineEndPosition(GetCurrentLine()) - line.Length() + offset, GetLineEndPosition(GetCurrentLine()) - line.Length() + indent.Length() + offset);

		// Lose any selected text.
		ReplaceSelection(wxEmptyString);

		// Insert a replacement \n (or whatever), and the indent at the insertion point.
		InsertText(GetCurrentPos(), lineEnd + indent);

		// Now, reset the position, and clear the selection
		SetCurrentPos(GetCurrentPos() + indent.Length() + lineEnd.Length());
		SetSelection(GetCurrentPos(), GetCurrentPos());
	}
	else if (m_dlgFindReplace && event.GetKeyCode() == WXK_F3)
	{
		m_dlgFindReplace->FindNext();
	}
	else
		event.Skip();
}

bool ctlSQLBox::BlockComment(bool uncomment)
{
	wxString lineEnd;
	switch (GetEOLMode())
	{
		case wxSTC_EOL_LF:
			lineEnd = wxT("\n");
			break;
		case wxSTC_EOL_CRLF:
			lineEnd = wxT("\r\n");
			break;
		case wxSTC_EOL_CR:
			lineEnd = wxT("\r");
			break;
	}

	// Save the start position
	const wxString comment = wxT("-- ");
	int start = GetSelectionStart();

	if (!GetSelectedText().IsEmpty())
	{
		wxString selection = GetSelectedText();
		if (!uncomment)
		{
			selection.Replace(lineEnd, lineEnd + comment);
			selection.Prepend(comment);
			if (selection.EndsWith(comment))
				selection = selection.Left(selection.Length() - comment.Length());
		}
		else
		{
			selection.Replace(lineEnd + comment, lineEnd);
			if (selection.StartsWith(comment))
				selection = selection.Right(selection.Length() - comment.Length());
		}
		ReplaceSelection(selection);
		SetSelection(start, start + selection.Length());
	}
	else
	{
		// No text selection - (un)comment the current line
		int column = GetColumn(start);
		int curLineNum = GetCurrentLine();
		int pos = PositionFromLine(curLineNum);

		if (!uncomment)
		{
			InsertText(pos, comment);
		}
		else
		{
			wxString t = GetTextRange(pos, pos + comment.Length());
			if (t == comment)
			{
				// The line starts with a comment, so we remove it
				SetTargetStart(pos);
				SetTargetEnd(pos + comment.Length());
				ReplaceTarget(wxT(""));
			}
			else
			{
				// The line doesn't start with a comment, do nothing
				return false;
			}
		}

		if (GetLineCount() > curLineNum)
		{
			wxString nextLine = GetLine(curLineNum + 1);
			if (nextLine.EndsWith(lineEnd))
				nextLine = nextLine.Left(nextLine.Length() - lineEnd.Length());

			int nextColumn = (nextLine.Length() < (unsigned int)column ? nextLine.Length() : column);
			GotoPos(PositionFromLine(curLineNum + 1) + nextColumn);
		}
	}

	return true;
}

void ctlSQLBox::OnKillFocus(wxFocusEvent &event)
{
	AutoCompCancel();
	event.Skip();
}

void ctlSQLBox::UpdateLineNumber()
{
	bool showlinenumber;

	settings->Read(wxT("frmQuery/ShowLineNumber"), &showlinenumber, false);
	if (showlinenumber)
	{
		long int width = TextWidth(wxSTC_STYLE_LINENUMBER,
		                           wxT(" ") + NumToStr((long int)GetLineCount()) + wxT(" "));
		if (width != GetMarginWidth(0))
			SetMarginWidth(0, width);
	}
	else
	{
		SetMarginWidth(0, 0);
	}
}

void ctlSQLBox::OnPositionStc(wxStyledTextEvent &event)
{
	int pos = GetCurrentPos();
	wxChar ch = GetCharAt(pos - 1);
	int st = GetStyleAt(pos - 1);
	int match;


	// Line numbers
	UpdateLineNumber();

	// Clear all highlighting
	BraceBadLight(wxSTC_INVALID_POSITION);

	// Check for braces that aren't in comment styles,
	// double quoted styles or single quoted styles
	if ((ch == '{' || ch == '}' ||
	        ch == '[' || ch == ']' ||
	        ch == '(' || ch == ')') &&
	        st != 2 && st != 6 && st != 7)
	{
		match = BraceMatch(pos - 1);
		if (match != wxSTC_INVALID_POSITION)
			BraceHighlight(pos - 1, match);
	}

	// Roll back through the doc and highlight any unmatched braces
	while ((pos--) >= 0)
	{
		ch = GetCharAt(pos);
		st = GetStyleAt(pos);

		if ((ch == '{' || ch == '}' ||
		        ch == '[' || ch == ']' ||
		        ch == '(' || ch == ')') &&
		        st != 2 && st != 6 && st != 7)
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
void ctlSQLBox::OnAutoComplete(wxCommandEvent &rev)
{
	if (GetReadOnly())
		return;
	if (m_database == NULL)
		return;
	if (m_autocompDisabled)
		return;

	wxString what = GetCurLine().Left(GetCurrentPos() - PositionFromLine(GetCurrentLine()));;
	int spaceidx = what.Find(' ', true);

	char *tab_ret;
	if (spaceidx == -1)
		tab_ret = tab_complete(what.mb_str(wxConvUTF8), 0, what.Len() + 1, m_database);
	else
		tab_ret = tab_complete(what.mb_str(wxConvUTF8), spaceidx + 1, what.Len() + 1, m_database);

	if (tab_ret == NULL || tab_ret[0] == '\0')
		return; /* No autocomplete available for this string */

	wxString wxRet = wxString(tab_ret, wxConvUTF8);
	free(tab_ret);

	// Switch to the generic list control. Native doesn't play well with
	// autocomplete on Mac.
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
#endif

	if (spaceidx == -1)
		AutoCompShow(what.Len(), wxRet);
	else
		AutoCompShow(what.Len() - spaceidx - 1, wxRet);

	// Now switch back
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), false);
#endif
}


ctlSQLBox::~ctlSQLBox()
{
	if (m_dlgFindReplace)
	{
		m_dlgFindReplace->Destroy();
		m_dlgFindReplace = 0;
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



// Find some text in the document.
CharacterRange ctlSQLBox::RegexFindText(int minPos, int maxPos, const wxString &text)
{
	TextToFind  ft;
	ft.chrg.cpMin = minPos;
	ft.chrg.cpMax = maxPos;
	wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
	ft.lpstrText = (char *)(const char *)buf;

	if (SendMsg(2150, wxSTC_FIND_REGEXP, (long)&ft) == -1)
	{
		ft.chrgText.cpMin = -1;
		ft.chrgText.cpMax = -1;
	}

	return ft.chrgText;
}
