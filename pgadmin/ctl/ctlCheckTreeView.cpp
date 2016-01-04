//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "images/checked.pngc"
#include "images/disabled.pngc"
#include "images/unchecked.pngc"

BEGIN_EVENT_TABLE(ctlCheckTreeView, wxTreeCtrl)
	EVT_LEFT_DOWN(                            ctlCheckTreeView::OnLeftClick)
END_EVENT_TABLE()


ctlCheckTreeView::ctlCheckTreeView(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
	: wxTreeCtrl(parent, id, pos, size, style)
{
	wxImageList *treeimages = new wxImageList(16, 16, true, 3);
	treeimages->Add(*unchecked_png_img);
	treeimages->Add(*checked_png_img);
	treeimages->Add(*disabled_png_img);
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

		if (newimage == 0 || newimage == 1)
			SetParentAndChildImage(node, newimage);
		if (newimage == 1)
			SetParentImage(node, newimage);
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

void ctlCheckTreeView::SetParentImage(wxTreeItemId node, int newimage)
{
	if (node.IsOk())
	{
		SetItemImage(node, newimage);
		SetParentImage(GetItemParent(node), newimage);
	}
}

bool ctlCheckTreeView::IsChecked(const wxTreeItemId &node)
{
	return (GetItemImage(node) == 1);
}

