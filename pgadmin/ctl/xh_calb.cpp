//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// xh_calb.cpp - wxCalendarBox handler
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#include "wx/wx.h"
#include "ctl/xh_calb.h"
#include "ctl/calbox.h"


IMPLEMENT_DYNAMIC_CLASS(wxCalendarBoxXmlHandler, wxXmlResourceHandler)

wxCalendarBoxXmlHandler::wxCalendarBoxXmlHandler()
	: wxXmlResourceHandler()
{
	/*
	 * Only available with the wxDatePickerCtrl
	 */
	XRC_ADD_STYLE(wxDP_DEFAULT);
	XRC_ADD_STYLE(wxDP_SPIN);
	XRC_ADD_STYLE(wxDP_DROPDOWN);
	XRC_ADD_STYLE(wxDP_ALLOWNONE);
	XRC_ADD_STYLE(wxDP_SHOWCENTURY);

	AddWindowStyles();
}


wxObject *wxCalendarBoxXmlHandler::DoCreateResource()
{
	XRC_MAKE_INSTANCE(calendar, wxCalendarBox);

#if pgUSE_WX_CAL
	calendar->Create(m_parentAsWindow,
	                 GetID(),
	                 wxDefaultDateTime,
	                 GetPosition(), GetSize(),
	                 wxDP_DEFAULT | wxDP_SHOWCENTURY | wxDP_ALLOWNONE,
	                 wxDefaultValidator,
	                 GetName());

#else // pgUSE_WX_CAL
#if !defined(wxUSE_DATEPICKCTRL) || !wxUSE_DATEPICKCTRL
	calendar->Create(m_parentAsWindow,
	                 GetID(),
	                 wxDefaultDateTime,
	                 GetPosition(), GetSize(),
	                 GetStyle(),
	                 GetName());
#else // !defined(wxUSE_DATEPICKCTRL) || !wxUSE_DATEPICKCTRL
	calendar->Create(m_parentAsWindow,
	                 (wxWindowID)GetID(),
	                 wxDefaultDateTime,
	                 GetPosition(), GetSize(),
	                 (long int)GetStyle(),
	                 wxDefaultValidator,
	                 GetName());
#endif // !defined(wxUSE_DATEPICKCTRL) || !wxUSE_DATEPICKCTRL
#endif // pgUSE_WX_CAL

	SetupWindow(calendar);

	return calendar;
}

bool wxCalendarBoxXmlHandler::CanHandle(wxXmlNode *node)
{
	return IsOfClass(node, wxT("wxCalendarBox"));
}
