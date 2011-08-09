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

wxhdPoint wxhdLocatorConnector::locate(int posIdx)
{
	return figureLocator->locate(posIdx, getOwner());
}

void wxhdLocatorConnector::draw(wxBufferedDC &context)
{
}

wxhdRect &wxhdLocatorConnector::getDisplayBox(int posIdx)
{
	wxhdPoint p = figureLocator->locate(posIdx, getOwner());
	displayBox = wxhdRect(p.x - (size / 2), p.y - (size / 2), size, size);
	return displayBox;
}

bool wxhdLocatorConnector::containsPoint(int posIdx, int x, int y)
{
	return getDisplayBox(posIdx).Contains(x, y);
}

wxhdPoint wxhdLocatorConnector::findStart(int posIdx, wxhdLineConnection *connection)
{
	return getDisplayBox(posIdx).center();
}

wxhdPoint wxhdLocatorConnector::findEnd(int posIdx, wxhdLineConnection *connection)
{
	return getDisplayBox(posIdx).center();
}
