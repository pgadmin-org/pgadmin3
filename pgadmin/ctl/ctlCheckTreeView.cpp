//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlCheckTreeView.cpp - TreeView with Checkboxes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/imaglist.h>
#include <wx/wizard.h>
#include <wx/treectrl.h>

// App headers
#include "ctl/ctlCheckTreeView.h"
#include "images/checked.xpm"
#include "images/disabled.xpm"
#include "images/unchecked.xpm"

BEGIN_EVENT_TABLE(ctlCheckTreeView, wxTreeCtrl)
	EVT_LEFT_DOWN(                            ctlCheckTreeView::OnLeftClick)
END_EVENT_TABLE()


ctlCheckTreeView::ctlCheckTreeView(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
	: wxTreeCtrl(parent, id, pos, size, style)
{
	wxImageList *treeimages = new wxImageList(16, 16, true, 3);
	treeimages->Add(wxIcon(unchecked_xpm));
	treeimages->Add(wxIcon(checked_xpm));
	treeimages->Add(wxIcon(disabled_xpm));
	SetImageList(treeimages);
}


void ctlCheckTreeView::OnLeftClick(wxMouseEvent &evt)
{
	int flags;
	wxTreeItemId node = HitTest(evt.GetPosition(), flags);
	int newimage = 0;

	if ((flags & wxTREE_HITTEST_ONITEMLABEL) || (flags & wxTREE_HITTEST_ONITEMICON))
	{
		if (GetItemImage(node) == 0)
			newimage = 1;
		else if (GetItemImage(node) == 1)
			newimage = 0;

		if (GetItemImage(node) == 0 || GetItemImage(node) == 1)
		{
			SetParentAndChildImage(node, newimage);
		}
	}

	evt.Skip();
}

void ctlCheckTreeView::SetParentAndChildImage(wxTreeItemId node, int newimage)
{
	SetItemImage(node, newimage);
	wxTreeItemIdValue childData;
	wxTreeItemId child = GetFirstChild(node, childData);
	while (child.IsOk())
	{
		SetParentAndChildImage(child, newimage);
		child = GetNextChild(node, childData);
	}
}

bool ctlCheckTreeView::IsChecked(const wxTreeItemId &node)
{
	return (GetItemImage(node) == 1);
}

