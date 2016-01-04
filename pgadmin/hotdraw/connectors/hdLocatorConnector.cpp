//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLocatorConnector.cpp - class that puts connects at locator position
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/connectors/hdLocatorConnector.h"
hdLocatorConnector::hdLocatorConnector(hdIFigure *owner, hdILocator *locator):
	hdIConnector(owner)
{
	figureOwner = owner;
	figureLocator = locator;
	size = 8;
}

hdLocatorConnector::~hdLocatorConnector()
{
}

hdPoint hdLocatorConnector::locate(int posIdx)
{
	return figureLocator->locate(posIdx, getOwner());
}

void hdLocatorConnector::draw(wxBufferedDC &context)
{
}

hdRect &hdLocatorConnector::getDisplayBox(int posIdx)
{
	hdPoint p = figureLocator->locate(posIdx, getOwner());
	displayBox = hdRect(p.x - (size / 2), p.y - (size / 2), size, size);
	return displayBox;
}

bool hdLocatorConnector::containsPoint(int posIdx, int x, int y)
{
	return getDisplayBox(posIdx).Contains(x, y);
}

hdPoint hdLocatorConnector::findStart(int posIdx, hdLineConnection *connection)
{
	return getDisplayBox(posIdx).center();
}

hdPoint hdLocatorConnector::findEnd(int posIdx, hdLineConnection *connection)
{
	return getDisplayBox(posIdx).center();
}
