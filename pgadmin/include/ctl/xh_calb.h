//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// xh_calb.h - wxCalendarBox handler
//
//////////////////////////////////////////////////////////////////////////


#ifndef _WX_XH_CALB_H_
#define _WX_XH_CALB_H_


#include "wx/xrc/xmlres.h"

//class WXDLLIMPEXP_XRC
class wxCalendarBoxXmlHandler : public wxXmlResourceHandler
{
	DECLARE_DYNAMIC_CLASS(wxCalendarBoxXmlHandler)
public:
	wxCalendarBoxXmlHandler();
	virtual wxObject *DoCreateResource();
	virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_CALB_H_
