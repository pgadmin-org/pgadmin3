//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// xh_ctlchecktreeview.h - ctlCheckTreeView handler
//
//////////////////////////////////////////////////////////////////////////


#ifndef _WX_XH_CTLCHECKTREEVIEW_H_
#define _WX_XH_CTLCHECKTREEVIEW_H_

#include "wx/xrc/xmlres.h"
#include "wx/xrc/xh_tree.h"

//class WXDLLIMPEXP_XRC
class ctlCheckTreeViewXmlHandler : public wxTreeCtrlXmlHandler
{
	DECLARE_DYNAMIC_CLASS(ctlCheckTreeViewXmlHandler)
public:
	ctlCheckTreeViewXmlHandler() : wxTreeCtrlXmlHandler() {}
	virtual wxObject *DoCreateResource();
	virtual bool CanHandle(wxXmlNode *node);
};


#endif
