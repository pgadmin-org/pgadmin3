//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdFontAttribute.cpp - Default attribute for attributes of fonts
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/fontdlg.h>

// App headers
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFontAttribute.h"
#include "dd/wxhotdraw/figures/wxhdAttribute.h"

wxFont wxhdFontAttribute::defaultFont = wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);

wxhdFontAttribute::wxhdFontAttribute():
	wxhdAttribute()
{
	fontAttributes = defaultFont;
}

void wxhdFontAttribute::apply(wxBufferedDC &context)
{
	context.SetFont(fontAttributes);
}

void wxhdFontAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	fontAttributes = wxGetFontFromUser(owner, fontAttributes, wxT("Select a font..."));
}

wxFont &wxhdFontAttribute::font()
{
	return fontAttributes;
}