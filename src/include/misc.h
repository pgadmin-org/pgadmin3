//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef MISC_H
#define MISC_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/tokenzr.h>

// Global Stuff
void StartMsg(const wxString& msg);
void EndMsg();

class sysSettings;
extern sysSettings *settings;

// Conversions
wxString StrToYesNo(const wxString& value);
wxString BoolToYesNo(bool value);
bool StrToBool(const wxString& value);
wxString NumToStr(long value);
long StrToLong(const wxString& value);
wxString NumToStr(double value);
double StrToDouble(const wxString& value);

// Quoting
wxString qtString(const wxString& value);
wxString qtIdent(const wxString& value);

// check if size/pos have reasonable values
void CheckOnScreen(wxPoint &pos, wxSize &size, const int w0=100, const int h0=70);

// Keith 2003.03.11
// We need an identifier validation function

// Validation
bool IsValidIdentifier(wxString ident);


class queryTokenizer : public wxStringTokenizer
{
public:
    queryTokenizer(const wxString& str, const char delim=' ');
    wxString GetNextToken();
private:
    char delimiter;
};

enum        // depends on frmMain browserImages->Add order!
{
    PGICON_SERVER,
    PGICON_SERVERBAD,
    PGICON_DATABASE,
    PGICON_LANGUAGE,
    PGICON_SCHEMA,
    PGICON_AGGREGATE,
    PGICON_FUNCTION,
    PGICON_OPERATOR,
    PGICON_SEQUENCE,
    PGICON_TABLE,
    PGICON_TYPE,
    PGICON_VIEW,
    PGICON_USER,
    PGICON_GROUP,
    PGICON_BADDATABASE,
    PGICON_CLOSEDDATABASE,
    PGICON_DOMAIN,
    PGICON_CHECK,
    PGICON_COLUMN,
    PGICON_RELATIONSHIP,
    PGICON_INDEX,
    PGICON_RULE,
    PGICON_TRIGGER,
    PGICON_KEY
};

#endif
