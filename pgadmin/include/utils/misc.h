//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef MISC_H
#define MISC_H

#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/tokenzr.h>

#include "utils/misc.h"

class wxImageList;
extern wxImageList *imageList;

class wxHelpControllerBase;

#define __(str) wxT(str)

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

#if !defined(PGSCLI)

#define wxCookieType wxTreeItemIdValue

class sysSettings;
extern sysSettings *settings;

#undef wxStaticCast
#define wxStaticCast(obj, className) ((className *)(obj))

// making life easier
#define CTRL(id, typ)           (XRCCTRL(*this, id, typ))

#define CTRL_STATIC(id)         (XRCCTRL(*this, id, wxStaticText))
#define CTRL_STATICBOX(id)      (XRCCTRL(*this, id, wxStaticBox))
#define CTRL_TEXT(id)           (XRCCTRL(*this, id, wxTextCtrl))
#define CTRL_TEXTNUMERIC(id)    ((wxTextNumericCtrl*)(XRCCTRL(*this, id, wxTextCtrl)))
#define CTRL_LISTBOX(id)        (XRCCTRL(*this, id, wxListBox))
#define CTRL_LISTCTRL(id)       (XRCCTRL(*this, id, wxListCtrl))
#define CTRL_COMBOBOX(id)       ((ctlComboBoxFix*)(XRCCTRL(*this, id, wxComboBox)))
#define CTRL_COMBOBOX1(id)      (XRCCTRL(*this, id, wxComboBox))
#define CTRL_LISTVIEW(id)       ((ctlListView*)(XRCCTRL(*this, id, wxListCtrl)))
#define CTRL_COMBOBOX2(id)      (XRCCTRL(*this, id, ctlComboBox))
#define CTRL_CHOICE(id)         (XRCCTRL(*this, id, wxChoice))
#define CTRL_CHECKBOX(id)       (XRCCTRL(*this, id, wxCheckBox))
#define CTRL_RADIOBOX(id)       (XRCCTRL(*this, id, wxRadioBox))
#define CTRL_BUTTON(id)         (XRCCTRL(*this, id, wxButton))
#define CTRL_CALENDAR(id)       (XRCCTRL(*this, id, wxCalendarBox))
#define CTRL_TIME(id)           (XRCCTRL(*this, id, wxTimeSpinCtrl))
#define CTRL_FLEXGRIDSIZER(id)  (XRCCTRL(*this, id, wxFlexGridSizer))
#define CTRL_PANEL(id)          (XRCCTRL(*this, id, wxPanel))
#define CTRL_SLIDER(id)         (XRCCTRL(*this, id, wxSlider))
#define CTRL_SQLBOX(id)         (XRCCTRL(*this, id, ctlSQLBox))
#define CTRL_RADIOBUTTON(id)    (XRCCTRL(*this, id, wxRadioButton))
#define CTRL_NOTEBOOK(id)       (XRCCTRL(*this, id, wxNotebook))
#define CTRL_SPIN(id)           (XRCCTRL(*this, id, wxSpinCtrl))
#define CTRL_CHECKLISTBOX(id)   (XRCCTRL(*this, id, wxCheckListBox))
#define CTRL_DATEPICK(id)       (XRCCTRL(*this, id, wxDatePickerCtrl))
#define CTRL_TREE(id)           (XRCCTRL(*this, id, ctlTree))
#define CTRL_COLOURPICKER(id)   (XRCCTRL(*this, id, ctlColourPicker))
#define CTRL_DIRPICKER(id)      (XRCCTRL(*this, id, wxDirPickerCtrl))
#define CTRL_FILEPICKER(id)     (XRCCTRL(*this, id, wxFilePickerCtrl))
#define CTRL_FONTPICKER(id)     (XRCCTRL(*this, id, wxFontPickerCtrl))
#define CTRL_CHECKTREEVIEW(id)  (XRCCTRL(*this, id, ctlCheckTreeView))

#endif // PGSCLI

// Conversions
wxString BoolToStr(bool value);         // english; used for config values
wxString DateToAnsiStr(const wxDateTime &datetime);

bool StrToBool(const wxString &value);  // english
long StrToLong(const wxString &value);
double StrToDouble(const wxString &value);
wxLongLong StrToLongLong(const wxString &value);
wxDateTime StrToDateTime(const wxString &value);
OID StrToOid(const wxString &value);

wxString generate_spaces(int length);

// nls aware
wxString BoolToYesNo(bool value);
wxString NumToStr(long value);
wxString NumToStr(double value);
wxString NumToStr(OID value);
wxString NumToStr(wxLongLong value);
wxString DateToStr(const wxDateTime &datetime);


// Quoting
wxString qtConnString(const wxString &value); // connection strings always have single quotes escaped with backslash

#if !defined(PGSCLI)

// check if size/pos have reasonable values
void CheckOnScreen(wxWindow *win, wxPoint &pos, wxSize &size, const int w0 = 100, const int h0 = 70);

// compile ID and Name into one string
wxString IdAndName(long id, const wxString &name);

// Quoting
wxString qtIdent(const wxString &value);    // add " if necessary
wxString qtTypeIdent(const wxString &value);    // add " if necessary
wxString qtDbStringDollar(const wxString &value);
wxString qtStrip(const wxString &value);    // remove \"


// string build helper
void AppendIfFilled(wxString &str, const wxString &delimiter, const wxString &what);

// Create keyword list from PostgreSQL list
void FillKeywords(wxString &str);

// Recreate a 9.0 datconfig
wxString TransformToNewDatconfig(const wxString &list);

// Fill array, splitting the string separated by commas (maybe quoted elements)
void FillArray(wxArrayString &array, const wxString &str);


// splitting of strings, obeying quotes
class queryTokenizer : public wxStringTokenizer
{
public:
	queryTokenizer(const wxString &str, const wxChar delim = (wxChar)' ');
	wxString GetNextToken();
private:
	char delimiter;
};

// Get an array from a comma(,) separated list
bool getArrayFromCommaSeparatedList(const wxString &str, wxArrayString &res);

// File handling including encoding according to sysSettings if format<0,
// 0-> local charset, 1->utf8
wxString FileRead(const wxString &filename, int format = -1);
bool FileWrite(const wxString &filename, const wxString &data, int format = -1);

typedef enum
{
	HELP_PGADMIN,
	HELP_POSTGRESQL,
	HELP_ENTERPRISEDB,
	HELP_GREENPLUM,
	HELP_SLONY
} HelpType;

wxString CleanHelpPath(const wxString &path);
bool HelpPathValid(const wxString &path);
void DisplayHelp(const wxString &helpTopic, const HelpType helpType);
void DisplayPgAdminHelp(const wxString &helpTopic);
void DisplayExternalHelp(const wxString &helpTopic, const wxString &docPath, wxHelpControllerBase *helpCtl, const bool init);

#ifndef WIN32
wxString ExecProcess(const wxString &cmd);
int ExecProcess(const wxString &command, wxArrayString &result);
#endif

wxString GetHtmlEntity(const wxChar ch);
wxString HtmlEntities(const wxString &str);

wxString firstLineOnly(const wxString &str);

bool pgAppMinimumVersion(const wxString &cmd, const int majorVer, const int minorVer);
bool isPgApp(const wxString &app);
bool isEdbApp(const wxString &app);
bool isGpApp(const wxString &app);

enum
{
	EDB_PACKAGE,
	EDB_PACKAGEFUNCTION,
	EDB_PACKAGEVARIABLE,

	PGM_CATALOG,
	PGM_CATALOGOBJECT,
	PGM_CHECK,
	PGM_COLUMN,
	PGM_CONSTRAINT,
	PGM_DATABASE,
	PGM_FOREIGNKEY,
	PGM_FUNCTION,
	PGM_INDEX,
	PGM_OPCLASS,
	PGM_OPFAMILY,
	PGM_PRIMARYKEY,
	PGM_ROLE,
	PGM_RULE,
	PGM_SCHEMA,
	PGM_SERVER,
	PGM_SEQUENCE,
	PGM_TABLE,
	PGM_TABLESPACE,
	PGM_TRIGGER,
	PGM_UNKNOWN,
	PGM_UNIQUE,
	PGM_VIEW,

	GP_EXTTABLE,
	GP_RESOURCE_QUEUE,
	GP_PARTITION,

	PGM_JOB,
	PGM_SCHEDULE,
	PGM_STEP,

	SLM_LISTEN,
	SLM_NODE,
	SLM_PATH,
	SLM_SEQUENCE,
	SLM_SET,
	SLM_SUBSCRIPTION,
	SLM_TABLE
};


enum        // depends on pgaFactory::addImage order!
{
	PGICON_PROPERTY,
	PGICON_STATISTICS,
	PGICON_PUBLIC
};

// File/directory name cleanup
wxString sanitizePath(const wxString &path);
wxString commandLineCleanOption(const wxString &option, bool schemaObject = false);
#endif // PGSCLI

#endif

