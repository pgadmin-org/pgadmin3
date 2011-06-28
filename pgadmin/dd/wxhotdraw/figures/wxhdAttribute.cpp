//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAttributeFigure.cpp - Base class for all figure attributes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdAttribute.h"

wxhdAttribute::wxhdAttribute():
	wxhdObject()
{
}

void wxhdAttribute::apply(wxBufferedDC &context)
{
}

void wxhdAttribute::callDefaultChangeDialog(wxWindow *owner)
{
}

