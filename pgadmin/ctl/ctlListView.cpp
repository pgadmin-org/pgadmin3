//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlListView.cpp - enhanced listview control
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/imaglist.h>

// App headers
#include "ctl/ctlListView.h"
#include "utils/misc.h"


ctlListView::ctlListView(wxWindow *p, int id, wxPoint pos, wxSize siz, long attr)
	: wxListView(p, id, pos, siz, attr | wxLC_REPORT)
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
	if (size == wxLIST_AUTOSIZE || size == wxLIST_AUTOSIZE_USEHEADER)
	{
		InsertColumn(GetColumnCount(), text, format, size);
	}
	else
	{
		InsertColumn(GetColumnCount(), text, format, ConvertDialogToPixels(wxPoint(size, 0)).x);
	}
}


long ctlListView::AppendItem(int icon, const wxChar *val, const wxChar *val2, const wxChar *val3, const wxChar *val4)
{
	long pos = InsertItem(GetItemCount(), val, icon);
	if (val2 && *val2)
		SetItem(pos, 1, val2);
	if (val3 && *val3)
		SetItem(pos, 2, val3);
	if (val4 && *val4)
		SetItem(pos, 3, val4);

	return pos;
}


void ctlListView::CreateColumns(wxImageList *images, const wxString &left, const wxString &right, int leftSize)
{
	int rightSize;
	if (leftSize < 0)
	{
#ifdef __WXMAC__
		leftSize = rightSize = (GetParent()->GetSize().GetWidth() - 20) / 2;
#else
		leftSize = rightSize = GetSize().GetWidth() / 2;
#endif
	}
	else
	{
		if (leftSize)
			leftSize = ConvertDialogToPixels(wxPoint(leftSize, 0)).x;

#ifdef __WXMAC__
		rightSize = (GetParent()->GetSize().GetWidth() - 20) - leftSize;
#else
		rightSize = GetClientSize().GetWidth() - leftSize;
#endif
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


void ctlListView::CreateColumns(wxImageList *images, const wxString &str1, const wxString &str2, const wxString &str3, int leftSize)
{
	int rightSize;
	if (leftSize < 0)
	{
#ifdef __WXMAC__
		leftSize = rightSize = (GetParent()->GetSize().GetWidth() - 20) / 2;
#else
		leftSize = rightSize = GetSize().GetWidth() / 2;
#endif
	}
	else
	{
		if (leftSize)
			leftSize = ConvertDialogToPixels(wxPoint(leftSize, 0)).x;

#ifdef __WXMAC__
		rightSize = (GetParent()->GetSize().GetWidth() - 20) - leftSize;
#else
		rightSize = GetClientSize().GetWidth() - leftSize;
#endif
	}
	if (!leftSize)
	{
		InsertColumn(0, str1, wxLIST_FORMAT_LEFT, GetClientSize().GetWidth());
	}
	else
	{
		InsertColumn(0, str1, wxLIST_FORMAT_LEFT, leftSize);
		InsertColumn(1, str2, wxLIST_FORMAT_LEFT, rightSize / 2);
		InsertColumn(2, str3, wxLIST_FORMAT_LEFT, rightSize / 2);
	}

	if (images)
		SetImageList(images, wxIMAGE_LIST_SMALL);
}
