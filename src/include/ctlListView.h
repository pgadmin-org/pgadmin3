//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlListView.h - enhanced listview control
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLLISTVIEW_H
#define CTLLISTVIEW_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

class frmMain;

class ctlListView : public wxListView
{
public:
    ctlListView(wxWindow *p, int id, wxPoint pos, wxSize siz, long attr=0);
    long GetSelection();
    wxString GetText(long row, long col=0);

    void CreateColumns(frmMain *form, const wxString &left, const wxString &right, int leftSize=60);
    void CreateColumns(wxImageList *images, const wxString &left, const wxString &right, int leftSize=60);

    void AddColumn(const wxChar *text, int size=-1, int format=wxLIST_FORMAT_LEFT);

    long AppendItem(int icon, const wxChar *val, const wxChar *val2=0, const wxChar *val3=0);
    long AppendItem(const wxChar *val, const wxChar *val2=0, const wxChar *val3=0)
        {  return AppendItem(PGICON_PROPERTY, val, val2, val3); }
    void AppendItem(const wxChar *str, const long l)
        { AppendItem(str, NumToStr(l)); }
    void AppendItem(const wxChar *str, const bool b)
        { AppendItem(str, BoolToYesNo(b)); }
    void AppendItem(const wxChar *str, const double d)
        { AppendItem(str, NumToStr(d)); }
    void AppendItem(const wxChar *str, const OID o)
        { AppendItem(str, NumToStr(o)); }
    void AppendItem(const wxChar *str, const wxDateTime &d)
        { AppendItem(str, DateToStr(d)); }
    void AppendItem(const wxChar *str, const wxLongLong &l)
        { AppendItem(str, l.ToString()); }
    void AppendItem(const wxChar *str, const wxULongLong &l)
        { AppendItem(str, l.ToString()); }

    void DeleteCurrentItem() { DeleteItem(GetSelection()); }
};


#endif
