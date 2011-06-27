//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLocatorConnector.cpp - class that puts connects at locator position
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/connectors/wxhdLocatorConnector.h"
wxhdLocatorConnector::wxhdLocatorConnector(wxhdIFigure *owner, wxhdILocator *locator):
wxhdIConnector(owner)
{
	figureOwner = owner;
	figureLocator = locator;
	size = 8;
}

wxhdLocatorConnector::~wxhdLocatorConnector()
{
}

wxhdPoint wxhdLocatorConnector::locate()
{
	return figureLocator->locate(getOwner());
}

void wxhdLocatorConnector::draw(wxBufferedDC& context)
{
}

wxhdRect& wxhdLocatorConnector::getDisplayBox()
{
	wxhdPoint p = figureLocator->locate(getOwner());
	displayBox = wxhdRect(p.x-(size/2), p.y-(size/2), size,size);
	return displayBox;
}

bool wxhdLocatorConnector::containsPoint(int x, int y)
{
	return getDisplayBox().Contains(x,y);
}

wxhdPoint wxhdLocatorConnector::findStart(wxhdLineConnection *connection)
{
	return getDisplayBox().center();
}

wxhdPoint wxhdLocatorConnector::findEnd(wxhdLineConnection *connection)
{
	return getDisplayBox().center();
}
