//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef MISC_H
#define MISC_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/datetime.h>
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

#ifdef __WXMSW__
#define END_OF_LINE wxT("\r\n")
#else
#define END_OF_LINE wxT("\n")
#endif

#if wxCHECK_VERSION(2, 5, 1)
#define wxCookieType wxTreeItemIdValue
#else
#define wxCookieType long
#endif

class sysSettings;
extern sysSettings *settings;

// making life easier
#define CTRL(id, typ)           (XRCCTRL(*this, id, typ))

#define CTRL_STATIC(id)         (XRCCTRL(*this, id, wxStaticText))
#define CTRL_STATICBOX(id)      (XRCCTRL(*this, id, wxStaticBox))
#define CTRL_TEXT(id)           (XRCCTRL(*this, id, wxTextCtrl))
#define CTRL_LISTBOX(id)        (XRCCTRL(*this, id, wxListBox))
#define CTRL_LISTCTRL(id)       (XRCCTRL(*this, id, wxListCtrl))
#define CTRL_COMBOBOX(id)       (XRCCTRL(*this, id, wxComboBox))
#define CTRL_LISTVIEW(id)       ((ctlListView*)(XRCCTRL(*this, id, wxListCtrl)))
#define CTRL_COMBOBOX2(id)      (XRCCTRL(*this, id, ctlComboBox))
#define CTRL_CHECKBOX(id)       (XRCCTRL(*this, id, wxCheckBox))
#define CTRL_RADIOBOX(id)       (XRCCTRL(*this, id, wxRadioBox))
#define CTRL_BUTTON(id)         (XRCCTRL(*this, id, wxButton))
#define CTRL_CALENDAR(id)       (XRCCTRL(*this, id, wxCalendarBox))
#define CTRL_TIME(id)           (XRCCTRL(*this, id, wxTimeSpinCtrl))
#define CTRL_PANEL(id)          (XRCCTRL(*this, id, wxPanel))
#define CTRL_SQLBOX(id)         (XRCCTRL(*this, id, ctlSQLBox))
#define CTRL_RADIOBUTTON(id)    (XRCCTRL(*this, id, wxRadioButton))
#define CTRL_NOTEBOOK(id)       (XRCCTRL(*this, id, wxNotebook))
#define CTRL_SPIN(id)           (XRCCTRL(*this, id, wxSpinCtrl))
#define CTRL_CHECKLISTBOX(id)   (XRCCTRL(*this, id, wxCheckListBox))

// Conversions
wxString BoolToStr(bool value);         // english; used for config values
wxString DateToAnsiStr(const wxDateTime &datetime);

bool StrToBool(const wxString& value);  // english
long StrToLong(const wxString& value);
double StrToDouble(const wxString& value);
wxULongLong StrToLongLong(const wxString& value);
wxDateTime StrToDateTime(const wxString &value);
OID StrToOid(const wxString& value);

// nls aware
wxString BoolToYesNo(bool value);
wxString NumToStr(long value);
wxString NumToStr(double value);
wxString NumToStr(OID value);
wxString NumToStr(wxLongLong value);
wxString DateToStr(const wxDateTime &datetime);

// Quoting
wxString qtString(const wxString& value);   // add ' and escape if necessary
wxString qtIdent(const wxString& value);    // add " if necessary
wxString qtTypeIdent(const wxString& value);    // add " if necessary
wxString qtStringDollar(const wxString &value);
wxString qtStrip(const wxString& value);    // remove \"

// check if size/pos have reasonable values
void CheckOnScreen(wxPoint &pos, wxSize &size, const int w0=100, const int h0=70);

// string build helper
void AppendIfFilled(wxString &str, const wxString &delimiter, const wxString &what);

// Create keyword list from PostgreSQL list
void FillKeywords(wxString &str);

// Fill array, splitting the string separated by commas (maybe quoted elements)
void FillArray(wxArrayString &array, const wxString &str);


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
wxString FileRead(const wxString &filename, int format=-1);
bool FileWrite(const wxString &filename, const wxString &data, int format=-1);

void DisplayHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);
void DisplaySqlHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);

#ifndef WIN32
wxString ExecProcess(const wxString &cmd);
#endif


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
    PGICON_TABLESPACE,
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
    PGICON_PUBLIC,
    PGAICON_AGENT,
    PGAICON_JOB,
    PGAICON_JOBDISABLED,
    PGAICON_STEP,
    PGAICON_SCHEDULE
};



#endif
