//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdIConnector.cpp - Base class for all connectors
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/connectors/hdIConnector.h"

class hdLineConnection;
class hdIFigure;

hdIConnector::hdIConnector(hdIFigure *owner):
	hdObject()
{
	figureOwner = owner;
}

hdIConnector::~hdIConnector()
{
}

hdIFigure *hdIConnector::getOwner()
{
	return figureOwner;

}

void hdIConnector::setOwner(hdIFigure *owner)
{
	figureOwner = owner;
}

void hdIConnector::draw(wxBufferedDC &context)
{
}

hdMultiPosRect &hdIConnector::getDisplayBox()
{
	return figureOwner->displayBox();
}

bool hdIConnector::containsPoint(int posIdx, int x, int y)
{
	return figureOwner->containsPoint(posIdx, x, y);
}

hdPoint hdIConnector::findStart(int posIdx, hdLineConnection *connection)
{
	return getDisplayBox().center(posIdx);
}

hdPoint hdIConnector::findEnd(int posIdx, hdLineConnection *connection)
{
	return getDisplayBox().center(posIdx);
}
