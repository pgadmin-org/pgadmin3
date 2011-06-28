//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineLocator.cpp - Return multiple location at same time for a PolyLine
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/locators/wxhdPolyLineLocator.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/figures/wxhdPolyLineFigure.h"

wxhdPolyLineLocator::~wxhdPolyLineLocator()
{
}

wxhdPolyLineLocator::wxhdPolyLineLocator(int index)
{
	indx = index;
}

wxhdPoint &wxhdPolyLineLocator::locate(wxhdIFigure *owner)
{
	if(owner)
	{
		wxhdPolyLineFigure *figure = (wxhdPolyLineFigure *) owner;
		locatePoint = figure->pointAt(indx);
		return locatePoint;
	}
	else
	{
		return locatePoint;
	}
}

void wxhdPolyLineLocator::setIndex(int index)
{
	indx = index;
}
