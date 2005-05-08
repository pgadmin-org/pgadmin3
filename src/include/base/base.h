//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:  $
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// base.h - Miscellaneous Basic Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef BASE_H
#define BASE_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/tokenzr.h>

class wxImageList;
extern wxImageList *imageList;


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

#if wxCHECK_VERSION(2, 5, 1)
#define wxCookieType wxTreeItemIdValue
#else
#define wxCookieType long
#endif

class sysSettings;
extern sysSettings *settings;

#undef wxStaticCast
#define wxStaticCast(obj, className) ((className *)(obj))

// making life easier
#define CTRL(id, typ)           (XRCCTRL(*this, id, typ))

#define CTRL_STATIC(id)         (XRCCTRL(*this, id, wxStaticText))
#define CTRL_STATICBOX(id)      (XRCCTRL(*this, id, wxStaticBox))
#define CTRL_TEXT(id)           (XRCCTRL(*this, id, wxTextCtrl))
#define CTRL_LISTBOX(id)        (XRCCTRL(*this, id, wxListBox))
#define CTRL_LISTCTRL(id)       (XRCCTRL(*this, id, wxListCtrl))
#define CTRL_COMBOBOX(id)       (XRCCTRL(*this, id, wxComboBoxFix))
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
#define CTRL_DATEPICK(id)       (XRCCTRL(*this, id, wxDatePickerCtrl))

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

// check if size/pos have reasonable values
void CheckOnScreen(wxPoint &pos, wxSize &size, const int w0=100, const int h0=70);

#endif
