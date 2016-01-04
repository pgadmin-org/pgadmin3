//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// xh_ctlcolourpicker.h - ctlColourPicker handler
//
//////////////////////////////////////////////////////////////////////////


#ifndef _WX_XH_CTLCOLOURPICKER_H_
#define _WX_XH_CTLCOLOURPICKER_H_


#include "wx/xrc/xmlres.h"
#include "wx/xrc/xh_bmpbt.h"

//class WXDLLIMPEXP_XRC
class ctlColourPickerXmlHandler : public wxBitmapButtonXmlHandler
{
	DECLARE_DYNAMIC_CLASS(ctlColourPickerXmlHandler)
public:
	ctlColourPickerXmlHandler() : wxBitmapButtonXmlHandler() {}
	virtual wxObject *DoCreateResource();
	virtual bool CanHandle(wxXmlNode *node);
};


#endif
