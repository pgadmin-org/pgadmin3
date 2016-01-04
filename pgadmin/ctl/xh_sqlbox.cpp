//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// xh_sqlbox.cpp - ctlSQLBox handler
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

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
	ctlSQLBox *sqlbox = new ctlSQLBox(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());

	SetupWindow(sqlbox);

	return sqlbox;
}

bool ctlSQLBoxXmlHandler::CanHandle(wxXmlNode *node)
{
	return IsOfClass(node, wxT("ctlSQLBox"));
}
