//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdFillAttribute.cpp - Default attribute for fill color of figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/colordlg.h>

// App headers
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFillAttribute.h"
#include "dd/wxhotdraw/figures/wxhdAttribute.h"

wxhdFillAttribute::wxhdFillAttribute():
wxhdAttribute()
{
	fillAttributes = wxBrush(*wxWHITE);
}

void wxhdFillAttribute::apply(wxBufferedDC& context)
{
	context.SetBrush(fillAttributes);
}

void wxhdFillAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	//create brush dialog
	wxColour color = wxGetColourFromUser(owner, fillAttributes.GetColour(),wxT("Select a color for fill color..."));
	fillAttributes = wxBrush(color);
}

wxBrush& wxhdFillAttribute::brush()
{
	return fillAttributes;
}