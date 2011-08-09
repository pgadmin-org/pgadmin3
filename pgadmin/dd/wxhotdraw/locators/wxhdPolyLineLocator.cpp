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

//This index refers to point index inside collection not the diagram index
wxhdPolyLineLocator::wxhdPolyLineLocator(int index)
{
	indx = index;
}

wxhdPoint &wxhdPolyLineLocator::locate(int posIdx, wxhdIFigure *owner)
{
	wxhdPolyLineFigure *figure = (wxhdPolyLineFigure *) owner;

	//A Handle at polyline figure without a respetive flexibility point at line
	//Hack to allow handles of polylines reuse between different versions of same line.
	if(figure && indx >= (figure->countPointsAt(posIdx) - 1) ) //indx 0 is first, count first is 1
	{
		locatePoint.x = -100; //Any negative number that don't allow to the mouse to reach this locator
		locatePoint.y = -100;
		return locatePoint;
	}
	else if(figure)
	{
		locatePoint = figure->pointAt(posIdx, indx);
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
