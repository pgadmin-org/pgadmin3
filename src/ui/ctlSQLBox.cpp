//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAdmin3.cpp - The application
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/stc/stc.h>

// App headers
#include "pgAdmin3.h"
#include "ctlSQLBox.h"


#include "menu.h"


// Maximum string length exceeded for VC6...
const wxString keywords1=
        wxT("ABORT ADD AGGREGATE ALL ALTER ANALYZE AND AS ASC ASCENDING AUTHORIZATION BEGIN BY ")
        wxT("CASCADE CASE CAST CHECK CHECKPOINT CLASS CLOSE CLUSTER COLUMN COMMENT COMMIT CONVERSION CONSTRAINT COPY CREATE CURSOR ")
        wxT("DATABASE DEALLOCATE DECLARE DEFAULT DELETE DESC DESCENDING DO DOMAIN DROP EACH ELSE END EXPLAIN ")
        wxT("FETCH FOR FOREIGN FREEZE FROM FULL FUNCTION GRANT GROUP ");

const wxString keywords2=
        wxT("HAVING ILIKE IMMUTABLE IMPLICIT IN INDEX INHERITS INNER INSERT INSTEAD INTO IS JOIN KEY LANGUAGE LIKE LISTEN LOAD LOCK MOVE NO NOT NOTHING NOTIFY NULL ")
        wxT("OFF OIDS ON OPERATOR OPTION OR ORDER OUTER PLAIN PREPARE PRIMARY PROCEDURE REINDEX REFERENCES RENAME REPLACE RESET RESTART RESTRICT RETURN RETURNS REVOKE ROLLBACK ROW RULE ")
        wxT("SCHEMA SELECT SET SEQUENCE SESSION SHOW STABLE STATEMENT STORAGE STRICT TABLE THEN TO TRIGGER TRUNCATE TYPE ")
        wxT("UNION UNIQUE UNLISTEN UPDATE USER USING VACUUM VALUE VALUES VIEW VOLATILE WHEN WHERE WITH WITHOUT");

wxString SQL_KEYWORDS= keywords1+keywords2;


#if 0

#include "../src/stc/scintilla/include/Platform.h"
#include "../src/stc/scintilla/include/PropSet.h"
#include "../src/stc/scintilla/include/Accessor.h"
#include "../src/stc/scintilla/include/KeyWords.h"
#include "../src/stc/scintilla/include/Scintilla.h"
#include "../src/stc/scintilla/include/SciLexer.h"


static void classifyWordSQL(unsigned int start, unsigned int end, 
                            WordList &keywords, Accessor &styler, wxString& lastWord)
{
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
    unsigned int i;

	for (i = 0; i < end - start + 1 && i < 30; i++)
    {
		s[i] = static_cast<char>(toupper(styler[start + i]));
		s[i + 1] = '\0';
	}
    lastWord = s;

    char chAttr = SCE_C_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_C_NUMBER;
	else
    {
        if (keywords.InList(s))
			chAttr = SCE_C_WORD;
	}
	styler.ColourTo(end, chAttr);
}


static void ColouriseSQLDoc(unsigned int startPos, int length,
                            int initStyle, WordList *keywordlists[], Accessor &styler)
{
	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);

	bool fold = styler.GetPropertyInt("fold") != 0;
	int lineCurrent = styler.GetLine(startPos);
	int spaceFlags = 0;

	wxString lastWord;

	int state = initStyle;

    bool bInFunctionDefinition;
	if (lineCurrent > 0)
    {
		styler.SetLineState(lineCurrent, styler.GetLineState(lineCurrent-1));
		bInFunctionDefinition = (styler.GetLineState(lineCurrent) == 1);
	}
    else
    {
		styler.SetLineState(lineCurrent, 0);
		bInFunctionDefinition = false;
	}

	char chPrev = ' ';
	char chNext = styler[startPos];
	styler.StartSegment(startPos);
	unsigned int lengthDoc = startPos + length;
    unsigned int i;

    for (i = startPos; i < lengthDoc; i++)
    {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n'))
        {
			int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags);
			int lev = indentCurrent;
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG))
            {
				// Only non whitespace lines can be headers
				int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags);

				if (indentCurrent < (indentNext & ~SC_FOLDLEVELWHITEFLAG))
                {
					lev |= SC_FOLDLEVELHEADERFLAG;
				}
			}
			if (fold)
            {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			styler.SetLineState(lineCurrent, (bInFunctionDefinition? 1 : 0));
		}

		if (styler.IsLeadByte(ch))
        {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_C_DEFAULT)
        {
			if (iswordstart(ch))
            {
				styler.ColourTo(i - 1, state);
				state = SCE_C_WORD;
			}
            else if (ch == '/' && chNext == '*')
            {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENT;
			}
            else if (ch == '-' && chNext == '-')
            {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
			}
            else if (ch == '\'')
            {
				styler.ColourTo(i - 1, state);
				if (bInFunctionDefinition && chPrev != '\\')
                {
					bInFunctionDefinition = false;
					styler.SetLineState(lineCurrent,0);

				}
                else if (!bInFunctionDefinition && lastWord.IsSameAs(wxT("AS"), false))
                {
					bInFunctionDefinition = true;
					styler.SetLineState(lineCurrent, 1);

				}
                else
                {
					state = SCE_C_STRING;
					if (chPrev == '\\')
                    {
						styler.ColourTo(i - 1, state);
					}
				}
			}
            else if (isoperator(ch))
            {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_C_OPERATOR);
			}
		} 
        else if (state == SCE_C_WORD)
        {
			if (!iswordchar(ch))
            {
			   classifyWordSQL(styler.GetStartSegment(), i - 1, keywords, styler, lastWord);

				state = SCE_C_DEFAULT;
				if (ch == '/' && chNext == '*')
					state = SCE_C_COMMENT;
				else if (ch == '-' && chNext == '-')
					state = SCE_C_COMMENTLINE;
				else if (ch == '\'')
					state = SCE_C_STRING;
				else if (isoperator(ch))
					styler.ColourTo(i, SCE_C_OPERATOR);
			}
		}
        else
        {
			if (state == SCE_C_COMMENT)
            {
				if (ch == '/' && chPrev == '*')
                {
					if (((i > (styler.GetStartSegment() + 2)) 
                        || ((initStyle == SCE_C_COMMENT) && (styler.GetStartSegment() == startPos))))
                    {
						styler.ColourTo(i, state);
						state = SCE_C_DEFAULT;
					}
				}
			}
            else if (state == SCE_C_COMMENTLINE)
            {
				if (ch == '\r' || ch == '\n')
                {
					styler.ColourTo(i - 1, state);
					state = SCE_C_DEFAULT;
				}
			} 
            else if (state == SCE_C_STRING)
            {
				if (ch == '\'')
                {
					if ( chNext == '\'' )
                    {
						i++;
					}
                    else
                    {
						styler.ColourTo(i,state);

						state = SCE_C_DEFAULT;
						i++;
					}
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);

				}
			}
			if (state == SCE_C_DEFAULT) 
            {    // One of the above succeeded

				if (ch == '/' && chNext == '*')
					state = SCE_C_COMMENT;
				else if (ch == '-' && chNext == '-')
					state = SCE_C_COMMENTLINE;
				else if (ch == '\'')
                {
				    if (bInFunctionDefinition && chPrev != '\\')
                    {
					    bInFunctionDefinition = false; 
                        styler.SetLineState(lineCurrent, 0);  
				    }
                    else if (!bInFunctionDefinition && lastWord.IsSameAs(wxT("AS"), false))
                    {
					    bInFunctionDefinition = true;
					    styler.SetLineState(lineCurrent, 1);
					} 
                    else 
                    {
						state = SCE_C_STRING;
					}
				} 
                else if (iswordstart(ch))
					state = SCE_C_WORD;
				else if (isoperator(ch))
					styler.ColourTo(i, SCE_C_OPERATOR);
			}
		}
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);
}


LexerModule lmPostgreSQL(SCLEX_AUTOMATIC, ColouriseSQLDoc, "sql");

#endif


BEGIN_EVENT_TABLE(ctlSQLBox, wxStyledTextCtrl)
#ifdef __WXGTK__
    EVT_KEY_DOWN(ctlSQLBox::OnKeyDown)
#endif
    EVT_MENU(MNU_FIND,ctlSQLBox::OnFind)
    EVT_FIND(-1, ctlSQLBox::OnFindDialog)
    EVT_FIND_NEXT(-1, ctlSQLBox::OnFindDialog)
    EVT_FIND_REPLACE(-1, ctlSQLBox::OnFindDialog)
    EVT_FIND_REPLACE_ALL(-1, ctlSQLBox::OnFindDialog)
    EVT_FIND_CLOSE(-1, ctlSQLBox::OnFindDialog)
END_EVENT_TABLE()


ctlSQLBox::ctlSQLBox(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxStyledTextCtrl(parent,id , pos, size, style)
{
    m_dlgFind=0;    
    wxLogInfo(wxT("Creating a ctlSQLBox"));

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
    
    // SQL Lexer and keywords.
//    SetLexer(lmPostgreSQL.GetLanguage());
    SetLexer(wxSTC_LEX_SQL);
    SetKeyWords(0, SQL_KEYWORDS);

    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, (int)'F', MNU_FIND);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    m_findData.SetFlags(wxFR_DOWN);

}


void ctlSQLBox::OnFind(wxCommandEvent& ev)
{
    if (!m_dlgFind)
    {
        m_dlgFind = new wxFindReplaceDialog(this, &m_findData, _("Find text"), 0);

        m_dlgFind->Show(TRUE);
    }
    else
        m_dlgFind->SetFocus();
}


void ctlSQLBox::OnFindDialog(wxFindDialogEvent& event)
{
    wxEventType type = event.GetEventType();

    if (type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT)
    {
        int flags = 0;
        if (event.GetFlags() & wxFR_MATCHCASE)
            flags |= wxSTC_FIND_MATCHCASE;

        if (event.GetFlags() & wxFR_WHOLEWORD)
            flags |= wxSTC_FIND_WHOLEWORD;

        int startPos = GetSelectionStart();
        int endPos = GetTextLength();
        
        if (event.GetFlags() & wxFR_DOWN)
        {
            startPos += 1;
        }
        else
        {
            endPos = 0;
            startPos -= 1;
        }

        int pos = FindText(startPos, endPos, event.GetFindString().c_str(), flags);

        if (pos >= 0)
        {
            SetSelectionStart(pos);
            SetSelectionEnd(pos + event.GetFindString().Length());
            EnsureCaretVisible();
        }
        else
        {
            wxMessageBox(_("Reached end of the document"), _("Find text"),
                        wxICON_EXCLAMATION | wxOK, this);
        }
    }
    else if (type == wxEVT_COMMAND_FIND_REPLACE || type == wxEVT_COMMAND_FIND_REPLACE_ALL)
    {
        wxMessageBox(_("Not implemented"), _("Find text"),
                     wxICON_EXCLAMATION | wxOK, this);
    }
    else if (type == wxEVT_COMMAND_FIND_CLOSE)
    {
        wxFindReplaceDialog *dlg = event.GetDialog();
        m_dlgFind = NULL;
        dlg->Destroy();
    }
    else
    {
        wxLogError(__("Unknown find dialog event!"));
    }
}


void ctlSQLBox::OnKeyDown(wxKeyEvent& event)
{
    event.m_metaDown=false;
    event.Skip();
}


ctlSQLBox::~ctlSQLBox()
{
    wxLogInfo(wxT("Destroying a ctlSQLBox"));
    if (m_dlgFind)
        m_dlgFind->Destroy();
}
