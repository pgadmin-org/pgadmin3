//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

#else
    calendar->Create(m_parentAsWindow,
                     GetID(),
                     wxDefaultDateTime,
                     GetPosition(), GetSize(),
                     GetStyle(),
                     GetName());
#endif
    
    SetupWindow(calendar);
    
    return calendar;
}

bool wxCalendarBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxCalendarBox"));
}
