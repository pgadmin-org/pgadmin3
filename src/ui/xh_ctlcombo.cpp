//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// xh_ctlcombo.cpp - ctlComboBox handler
//
//////////////////////////////////////////////////////////////////////////
 
#include "wx/wx.h"
#include "xh_ctlcombo.h"
#include "ctlComboBox.h"

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
