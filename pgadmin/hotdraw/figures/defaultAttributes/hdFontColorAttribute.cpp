//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdFontAttribute.cpp - Default attribute for color of fonts
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/colordlg.h>

// App headers
#include "hotdraw/figures/defaultAttributes/hdFontColorAttribute.h"
#include "hotdraw/figures/hdAttribute.h"

hdFontColorAttribute::hdFontColorAttribute():
	hdAttribute()
{
	fontColor = wxColour(*wxBLACK);
}

void hdFontColorAttribute::apply(wxBufferedDC &context)
{
	context.SetTextForeground(fontColor);
}

void hdFontColorAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	fontColor = wxGetColourFromUser(owner, fontColor, wxT("Select a color for font..."));
}
