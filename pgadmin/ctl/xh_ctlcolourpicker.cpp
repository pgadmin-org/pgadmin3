//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// xh_ctlcolourpicker.cpp - ctlColourPicker handler
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#include "wx/wx.h"
#include "ctl/ctlColourPicker.h"
#include "ctl/xh_ctlcolourpicker.h"

IMPLEMENT_DYNAMIC_CLASS(ctlColourPickerXmlHandler, wxBitmapButtonXmlHandler)


wxObject *ctlColourPickerXmlHandler::DoCreateResource()
{
	ctlColourPicker *ctl = new ctlColourPicker(m_parentAsWindow, GetID(), GetPosition(), GetSize());

	SetupWindow(ctl);

	return ctl;
}

bool ctlColourPickerXmlHandler::CanHandle(wxXmlNode *node)
{
	return IsOfClass(node, wxT("ctlColourPicker"));
}
