//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// xh_ctltree.cpp - ctlTree handler
//
//////////////////////////////////////////////////////////////////////////
 
#include "wx/wx.h"

#include "base/base.h"
#include "ctl/ctlTree.h"
#include "ctl/xh_ctltree.h"

IMPLEMENT_DYNAMIC_CLASS(ctlTreeXmlHandler, wxTreeCtrlXmlHandler)


wxObject *ctlTreeXmlHandler::DoCreateResource()
{ 
    ctlTree *ctl=new ctlTree(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());
    
    SetupWindow(ctl);
   
    return ctl;
}

bool ctlTreeXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("ctlTree"));
}
