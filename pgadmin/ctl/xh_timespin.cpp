//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// xh_timespin.cpp - wxTimeSpinCtrl handler
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#include "wx/wx.h"
#include "ctl/timespin.h"
#include "ctl/xh_timespin.h"


IMPLEMENT_DYNAMIC_CLASS(wxTimeSpinXmlHandler, wxXmlResourceHandler)

wxTimeSpinXmlHandler::wxTimeSpinXmlHandler()
	: wxXmlResourceHandler()
{
	XRC_ADD_STYLE(wxSP_WRAP);
	XRC_ADD_STYLE(wxSP_ARROW_KEYS);

	AddWindowStyles();
}


wxObject *wxTimeSpinXmlHandler::DoCreateResource()
{
	XRC_MAKE_INSTANCE(timespin, wxTimeSpinCtrl);

	timespin->Create(m_parentAsWindow,
	                 GetID(),
	                 GetPosition(), GetSize(),
	                 GetStyle(),
	                 GetName());

	SetupWindow(timespin);

	return timespin;
}

bool wxTimeSpinXmlHandler::CanHandle(wxXmlNode *node)
{
	return IsOfClass(node, wxT("wxTimeSpinCtrl"));
}
