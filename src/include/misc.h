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

// string build helper
void AppendIfFilled(wxString &str, const wxString &delimiter, const wxString &what);

// splitting of strings, obeying quotes
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
    PGICON_KEY,
    PGICON_CAST,
    PGICON_CONVERSION,
    PGICON_OPERATORCLASS,
    PGICON_TRIGGERFUNCTION,
};



// Menu options
enum
{
    MNU_ADDSERVER = 101,
    MNU_PASSWORD,
    MNU_SAVEDEFINITION,
    MNU_EXIT,
    MNU_UPGRADEWIZARD,
    MNU_OPTIONS,
    MNU_SYSTEMOBJECTS,
    MNU_CONTENTS,
    MNU_TIPOFTHEDAY,
    MNU_ABOUT,
    MNU_REFRESH,
    MNU_CONNECT,
    MNU_DISCONNECT,
    MNU_DROP,
    MNU_PROPERTIES,
    MNU_QUERYBUILDER,
    MNU_SQL,
    MNU_VIEWDATA,
    MNU_VACUUM,
    MNU_CREATE,
    MNU_RECORD,
    MNU_STOP,
    MNU_STATUS,
    MNU_APPEND,
    MNU_DELETE,
    MNU_OPEN,
    MNU_SAVE,
    MNU_SAVEAS,
    MNU_CUT,
    MNU_COPY,
    MNU_PASTE,
    MNU_CLEAR,
    MNU_FIND,
    MNU_UNDO,
    MNU_REDO,
    MNU_CANCEL,
    MNU_EXECUTE,
    MNU_EXPLAIN,
    MNU_RECENT   // must be last, because recent file numbers are added automatically
};



#endif
