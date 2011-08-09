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

wxhdIFigure *wxhdIConnector::getOwner()
{
	return figureOwner;

}

void wxhdIConnector::setOwner(wxhdIFigure *owner)
{
	figureOwner = owner;
}

void wxhdIConnector::draw(wxBufferedDC &context)
{
}

wxhdMultiPosRect &wxhdIConnector::getDisplayBox()
{
	return figureOwner->displayBox();
}

bool wxhdIConnector::containsPoint(int posIdx, int x, int y)
{
	return figureOwner->containsPoint(posIdx, x, y);
}

wxhdPoint wxhdIConnector::findStart(int posIdx, wxhdLineConnection *connection)
{
	return getDisplayBox().center(posIdx);
}

wxhdPoint wxhdIConnector::findEnd(int posIdx, wxhdLineConnection *connection)
{
	return getDisplayBox().center(posIdx);
}
