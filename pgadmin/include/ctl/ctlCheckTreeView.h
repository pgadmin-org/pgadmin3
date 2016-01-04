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

#ifndef _CTLCHECKTREEVIEW_H
#define _CTLCHECKTREEVIEW_H


// wxWindows headers
#include <wx/wx.h>
#include <wx/wizard.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>

class ctlCheckTreeView : public wxTreeCtrl
{
public:
	ctlCheckTreeView(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
	bool IsChecked(const wxTreeItemId &node);

private:
	void OnLeftClick(wxMouseEvent &evt);
	void SetParentAndChildImage(wxTreeItemId node, int newimage);
	void SetParentImage(wxTreeItemId node, int newimage);

	DECLARE_EVENT_TABLE()
};

#endif
