//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlListView.h - enhanced listview control
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLLISTVIEW_H
#define CTLLISTVIEW_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "utils/misc.h"

class frmMain;

class ctlListView : public wxListView
{
public:
	ctlListView(wxWindow *p, int id, wxPoint pos, wxSize siz, long attr = 0);
	long GetSelection();
	wxString GetText(long row, long col = 0);

	void CreateColumns(wxImageList *images, const wxString &left, const wxString &right, int leftSize = 60);
	void CreateColumns(wxImageList *images, const wxString &str1, const wxString &str2, const wxString &str3, int leftSize = 60);

	void AddColumn(const wxString &text, int size = wxLIST_AUTOSIZE_USEHEADER, int format = wxLIST_FORMAT_LEFT);

	long AppendItem(int icon, const wxString &val, const wxString &val2 = wxString(), const wxString &val3 = wxString(), const wxString &val4 = wxString());
	long AppendItem(const wxString &val, const wxString &val2 = wxString(), const wxString &val3 = wxString())
	{
		return AppendItem(PGICON_PROPERTY, val, val2, val3);
	}
	void AppendItem(const wxString &str, long l)
	{
		AppendItem(str, NumToStr(l));
	}
	void AppendItem(const wxString &str, double d)
	{
		AppendItem(str, NumToStr(d));
	}
	void AppendItem(const wxString &str, OID o)
	{
		AppendItem(str, NumToStr(o));
	}
	void AppendItem(const wxString &str, const wxDateTime &d)
	{
		AppendItem(str, DateToStr(d));
	}
	void AppendItem(const wxString &str, const wxLongLong &l)
	{
		AppendItem(str, l.ToString());
	}
	void AppendItem(const wxString &str, const wxULongLong &l)
	{
		AppendItem(str, l.ToString());
	}
	void AppendYesNoItem(const wxString &str, bool b)
	{
		AppendItem(str, BoolToYesNo(b));
	}

	void DeleteCurrentItem()
	{
		DeleteItem(GetSelection());
	}
};


#endif
