//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

ctlSQLBox::ctlSQLBox(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxStyledTextCtrl(parent,id , pos, size, style)
{
    
    wxLogInfo(wxT("Creating a ctlSQLBox"));

    // Clear all styles
    StyleClearAll();
    
    // Font
#ifdef __WIN32__
    wxFont fntSQLBox(9, wxMODERN, wxNORMAL, wxNORMAL);
#else
    wxFont fntSQLBox(12, wxMODERN, wxNORMAL, wxNORMAL);
#endif
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
    SetLexer(wxSTC_LEX_SQL);
    SetKeyWords(0, SQL_KEYWORDS);

    
}

ctlSQLBox::~ctlSQLBox()
{
    wxLogInfo(wxT("Destroying a ctlSQLBox"));
}
