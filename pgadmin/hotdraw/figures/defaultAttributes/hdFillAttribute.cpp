//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdFillAttribute.cpp - Default attribute for fill color of figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/colordlg.h>

// App headers
#include "hotdraw/figures/defaultAttributes/hdFillAttribute.h"
#include "hotdraw/figures/hdAttribute.h"

hdFillAttribute::hdFillAttribute():
	hdAttribute()
{
	fillAttributes = wxBrush(*wxWHITE);
}

void hdFillAttribute::apply(wxBufferedDC &context)
{
	context.SetBrush(fillAttributes);
}

void hdFillAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	//create brush dialog
	wxColour color = wxGetColourFromUser(owner, fillAttributes.GetColour(), wxT("Select a color for fill color..."));
	fillAttributes = wxBrush(color);
}

wxBrush &hdFillAttribute::brush()
{
	return fillAttributes;
}
