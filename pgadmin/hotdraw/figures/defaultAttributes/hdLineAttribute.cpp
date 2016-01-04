//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLineAttribute.cpp - Default Attribute for lines style, color an others at figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/colordlg.h>

// App headers
#include "hotdraw/figures/defaultAttributes/hdLineAttribute.h"
#include "hotdraw/figures/hdAttribute.h"

hdLineAttribute::hdLineAttribute():
	hdAttribute()
{
	penAttributes = *wxBLACK_PEN;
}

void hdLineAttribute::apply(wxBufferedDC &context)
{
	context.SetPen(penAttributes);
}

void hdLineAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	//create line dialog
}

wxPen &hdLineAttribute::pen()
{
	return penAttributes;
}
