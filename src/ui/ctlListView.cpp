//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlListView.cpp - enhanced listview control
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "ctlListView.h"
#include <wx/imaglist.h>
#include "frmMain.h"


ctlListView::ctlListView(wxWindow *p, int id, wxPoint pos, wxSize siz, long attr)
: wxListCtrl(p, id, pos, siz, attr | wxLC_REPORT)
{
}


long ctlListView::GetSelection()
{
    return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}


wxString ctlListView::GetText(long row, long col)
{
    wxListItem item;
    item.SetId(row);
    item.SetColumn(col);
    item.SetMask(wxLIST_MASK_TEXT);
    GetItem(item);
    return item.GetText();
};


void ctlListView::AddColumn(const wxChar *text, int size, int format)
{
    InsertColumn(GetColumnCount(), text, format, ConvertDialogToPixels(wxPoint(size,0)).x);
}


long ctlListView::AppendItem(int icon, const wxChar *val, const wxChar *val2, const wxChar *val3)
{
    long pos=InsertItem(GetItemCount(), val, icon);
    if (val2 && *val2)
        SetItem(pos, 1, val2);
    if (val3 && *val3)
        SetItem(pos, 2, val3);

    return pos;
}



void ctlListView::CreateColumns(frmMain *form, const wxString &left, const wxString &right, int leftSize)
{
    CreateColumns(form ? form->GetImageList() : (wxImageList*)0, left, right, leftSize);
}


void ctlListView::CreateColumns(wxImageList *images, const wxString &left, const wxString &right, int leftSize)
{
    int rightSize;
    if (leftSize < 0)
    {
        leftSize = rightSize = GetClientSize().GetWidth()/2;
    }
    else
    {
        if (leftSize)
            leftSize = ConvertDialogToPixels(wxPoint(leftSize, 0)).x;
        rightSize = GetClientSize().GetWidth()-leftSize;
    }
    if (!leftSize)
    {
        InsertColumn(0, left, wxLIST_FORMAT_LEFT, GetClientSize().GetWidth());
    }
    else
    {
        InsertColumn(0, left, wxLIST_FORMAT_LEFT, leftSize);
        InsertColumn(1, right, wxLIST_FORMAT_LEFT, rightSize);
    }

    if (images)
        SetImageList(images, wxIMAGE_LIST_SMALL);
}