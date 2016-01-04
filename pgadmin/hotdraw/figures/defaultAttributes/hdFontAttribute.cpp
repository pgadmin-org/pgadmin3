//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdFontAttribute.cpp - Default attribute for attributes of fonts
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/fontdlg.h>

// App headers
#include "hotdraw/figures/defaultAttributes/hdFontAttribute.h"
#include "hotdraw/figures/hdAttribute.h"

wxFont *hdFontAttribute::defaultFont = NULL;

hdFontAttribute::hdFontAttribute():
	hdAttribute()
{
	fontAttributes = *defaultFont;
}

void hdFontAttribute::apply(wxBufferedDC &context)
{
	context.SetFont(fontAttributes);
}

void hdFontAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	fontAttributes = wxGetFontFromUser(owner, fontAttributes, wxT("Select a font..."));
}

wxFont &hdFontAttribute::font()
{
	return fontAttributes;
}

void hdFontAttribute::InitFont()
{
	defaultFont = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
}
