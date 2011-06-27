//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdIConnector.cpp - Base class for all connectors
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/connectors/wxhdIConnector.h"

class wxhdLineConnection;
class wxhdIFigure;

wxhdIConnector::wxhdIConnector(wxhdIFigure *owner):
wxhdObject()
{
	figureOwner = owner;
}

wxhdIConnector::~wxhdIConnector()
{
}

wxhdIFigure* wxhdIConnector::getOwner()
{
	return figureOwner;

}

void wxhdIConnector::setOwner(wxhdIFigure *owner)
{
	figureOwner=owner;
}

void wxhdIConnector::draw(wxBufferedDC& context)
{
}

wxhdRect& wxhdIConnector::getDisplayBox()
{
	return figureOwner->displayBox();
}

bool wxhdIConnector::containsPoint(int x, int y)
{
	return figureOwner->containsPoint(x,y);
}

wxhdPoint wxhdIConnector::findStart(wxhdLineConnection *connection)
{
	return getDisplayBox().center();
}

wxhdPoint wxhdIConnector::findEnd(wxhdLineConnection *connection)
{
	return getDisplayBox().center();
}
