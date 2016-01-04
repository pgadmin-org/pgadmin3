//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdStickyRectangleConnector.cpp - A StickyRectangleConnector locates connection points by choping
// the connection between the centers of the two figures at the display box. The location
// of the connection point is computed once, when the user connects the figure.
// Moving the figure around will not change the location.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/connectors/hdStickyRectangleConnector.h"
#include "hotdraw/figures/hdLineConnection.h"
#include "hotdraw/connectors/hdChopBoxConnector.h"
#include "hotdraw/utilities/hdGeometry.h"

class hdLineConnection;
class hdIFigure;

hdStickyRectangleConnector::hdStickyRectangleConnector(hdIFigure *owner, hdPoint p):
	hdChopBoxConnector(owner)
{
	hdGeometry g;
	updateAnchor(0, p);
}

hdStickyRectangleConnector::~hdStickyRectangleConnector()
{
}

void hdStickyRectangleConnector::setAngle(float newAngle)
{
	angle = newAngle;
}

void hdStickyRectangleConnector::updateAnchor(int posIdx, hdPoint p)
{
	hdGeometry g;

	hdRect rect = getDisplayBox().gethdRect(posIdx);  //hack to avoid linux bug
	angle = g.angleFromPoint(rect, p);
}

hdPoint hdStickyRectangleConnector::getAnchor(int posIdx)
{
	hdGeometry g;

	hdRect rect = getDisplayBox().gethdRect(posIdx);  //hack to avoid linux bug
	return g.edgePointFromAngle(rect, angle);
}

hdPoint hdStickyRectangleConnector::chop(int posIdx, hdIFigure *target, hdPoint point)
{

	hdGeometry g;

	hdRect rect = target->displayBox().gethdRect(posIdx);  //hack to avoid linux bug
	point = g.edgePointFromAngle(rect, angle);
	return point;
}

void hdStickyRectangleConnector::draw(wxBufferedDC &context)
{
}
