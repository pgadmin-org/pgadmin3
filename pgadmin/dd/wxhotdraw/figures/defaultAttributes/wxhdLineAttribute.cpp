//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLineAttribute.cpp - Default Attribute for lines style, color an others at figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>
#include <wx/colordlg.h>

// App headers
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdLineAttribute.h"
#include "dd/wxhotdraw/figures/wxhdAttribute.h"

wxhdLineAttribute::wxhdLineAttribute():
wxhdAttribute()
{
	penAttributes = *wxBLACK_PEN;
}

void wxhdLineAttribute::apply(wxBufferedDC& context)
{
	context.SetPen(penAttributes);
}

void wxhdLineAttribute::callDefaultChangeDialog(wxWindow *owner)
{
	//create line dialog
}

wxPen& wxhdLineAttribute::pen()
{
	return penAttributes;
}