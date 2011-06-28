//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdFontAttribute.cpp - Default attribute for color of fonts
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/colordlg.h>

// App headers
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFontColorAttribute.h"
#include "dd/wxhotdraw/figures/wxhdAttribute.h"

wxhdFontColorAttribute::wxhdFontColorAttribute():
	wxhdAttribute()
{
	fontColor = wxColour(*wxBLACK);
}

void wxhdFontColorAttribute::apply(wxBufferedDC &context)
{
	context.SetTextForeground(fontColor);
}

void wxhdFontColorAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	fontColor = wxGetColourFromUser(owner, fontColor, wxT("Select a color for font..."));
}