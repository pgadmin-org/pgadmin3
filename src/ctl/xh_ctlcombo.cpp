//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: xh_ctlcombo.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// xh_ctlcombo.cpp - ctlComboBox handler
//
//////////////////////////////////////////////////////////////////////////
 
#include "wx/wx.h"
#include "ctl/ctlComboBox.h"
#include "ctl/xh_ctlcombo.h"

IMPLEMENT_DYNAMIC_CLASS(ctlComboBoxXmlHandler, wxComboBoxXmlHandler)


wxObject *ctlComboBoxXmlHandler::DoCreateResource()
{ 
    ctlComboBox *ctl=new ctlComboBox(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());
    
    SetupWindow(ctl);
   
    return ctl;
}

bool ctlComboBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("ctlComboBox"));
}
