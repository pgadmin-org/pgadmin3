//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// xh_ctlchecktreeview.cpp - ctlCheckTreeView handler
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#include "wx/wx.h"
#include "ctl/ctlCheckTreeView.h"
#include "ctl/xh_ctlchecktreeview.h"

IMPLEMENT_DYNAMIC_CLASS(ctlCheckTreeViewXmlHandler, wxTreeCtrlXmlHandler)


wxObject *ctlCheckTreeViewXmlHandler::DoCreateResource()
{
	ctlCheckTreeView *ctl = new ctlCheckTreeView(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());

	SetupWindow(ctl);

	return ctl;
}

bool ctlCheckTreeViewXmlHandler::CanHandle(wxXmlNode *node)
{
	return IsOfClass(node, wxT("ctlCheckTreeView"));
}
