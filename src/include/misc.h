//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef MISC_H
#define MISC_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/tokenzr.h>

typedef unsigned long OID;

// we dont have an appropriate wxLongLong method
#ifdef __WIN32__
#define atolonglong _atoi64
#else
#ifdef __WXMAC__
#define atolonglong(str) strtoll(str, (char **)NULL, 10) 
#else
#ifdef __FreeBSD__
#define atolonglong(str) strtoll(str, (char **)NULL, 10) 
#else
#define atolonglong atoll
#endif
#endif
#endif

#if wxCHECK_VERSION(2, 5, 1)
#define wxCookieType wxTreeItemIdValue
#else
#define wxCookieType long
#endif

// Global Stuff
void StartMsg(const wxString& msg);
void EndMsg();

class sysSettings;
extern sysSettings *settings;

// making life easier
#define CTRL(id, typ) (XRCCTRL(*this, id, typ))

// Conversions
wxString BoolToStr(bool value);         // english; used for config values
bool StrToBool(const wxString& value);  // english
long StrToLong(const wxString& value);
double StrToDouble(const wxString& value);
wxULongLong StrToLongLong(const wxString& value);

OID StrToOid(const wxString& value);

// nls aware
wxString BoolToYesNo(bool value);
wxString NumToStr(long value);
wxString NumToStr(double value);
wxString NumToStr(OID value);
wxString NumToStr(wxLongLong value);

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

// easier getting of text
class wxListCtrl;
wxString GetListText(wxListCtrl *lst, long row, long col);
long GetListSelected(wxListCtrl *lst);

// splitting of strings, obeying quotes
class queryTokenizer : public wxStringTokenizer
{
public:
    queryTokenizer(const wxString& str, const wxChar delim=(wxChar)' ');
    wxString GetNextToken();
private:
    char delimiter;
};


// File handling including encoding according to sysSettings if format<0,
// 0-> local charset, 1->utf8
wxString FileRead(const wxString &filename, wxWindow *errParent=0, int format=-1);
bool FileWrite(const wxString &filename, const wxString &data, int format=-1);

void DisplayHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);
void DisplaySqlHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);


class frmMain;
class DialogWithHelp : public wxDialog
{
public:
    DialogWithHelp(frmMain *frame);

protected:
    frmMain *mainForm;
    void OnHelp(wxCommandEvent& ev);

private:
    virtual wxString GetHelpPage() const = 0;

    DECLARE_EVENT_TABLE();
};


enum        // depends on frmMain browserImages->Add order!
{
    PGICON_PROPERTY,
    PGICON_STATISTICS,
    PGICON_SERVERS,
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
    PGICON_FOREIGNKEY,
    PGICON_CAST,
    PGICON_CONVERSION,
    PGICON_OPERATORCLASS,
    PGICON_TRIGGERFUNCTION,
    PGICON_CONSTRAINT,
    PGICON_PRIMARYKEY,
    PGICON_UNIQUE,
    PGICON_PUBLIC
};



#endif
