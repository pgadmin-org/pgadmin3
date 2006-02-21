//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: xh_sqlbox.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// xh_sqlbox.cpp - ctlSQLBox handler
//
//////////////////////////////////////////////////////////////////////////
 
#include "wx/wx.h"
#include "ctl/xh_sqlbox.h"
#include "ctl/ctlSQLBox.h"


IMPLEMENT_DYNAMIC_CLASS(ctlSQLBoxXmlHandler, wxXmlResourceHandler)

ctlSQLBoxXmlHandler::ctlSQLBoxXmlHandler() 
: wxXmlResourceHandler() 
{
    XRC_ADD_STYLE(wxTE_MULTILINE);
    XRC_ADD_STYLE(wxSIMPLE_BORDER);
    XRC_ADD_STYLE(wxSUNKEN_BORDER);
    XRC_ADD_STYLE(wxTE_RICH2);

    AddWindowStyles();
}


wxObject *ctlSQLBoxXmlHandler::DoCreateResource()
{ 
    ctlSQLBox *sqlbox=new ctlSQLBox(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());
    
    SetupWindow(sqlbox);
   
    return sqlbox;
}

bool ctlSQLBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("ctlSQLBox"));
}
