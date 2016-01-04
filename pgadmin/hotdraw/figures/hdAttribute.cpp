//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAttributeFigure.cpp - Base class for all figure attributes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "hotdraw/figures/hdAttribute.h"

hdAttribute::hdAttribute():
	hdObject()
{
}

void hdAttribute::apply(wxBufferedDC &context)
{
}

void hdAttribute::callDefaultChangeDialog(wxWindow *owner)
{
}

