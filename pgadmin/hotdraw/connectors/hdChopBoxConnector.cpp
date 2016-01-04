//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdChopBoxConnector.cpp - Connector for center of figure to line crossing one limit line of rect
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/connectors/hdChopBoxConnector.h"
#include "hotdraw/utilities/hdGeometry.h"

hdChopBoxConnector::hdChopBoxConnector(hdIFigure *owner):
	hdIConnector(owner)
{
}

hdChopBoxConnector::~hdChopBoxConnector()
{
}


hdPoint hdChopBoxConnector::chop(int posIdx, hdIFigure *target, hdPoint point)
{
	if(target && target->containsPoint(posIdx, point.x, point.y))
	{
		point = target->displayBox().center(posIdx);
		return point;
	}
	else if(!target)
	{
		point = hdPoint(0, 0);
		return point;
	}

	hdGeometry g;

	rect = getDisplayBox().gethdRect(posIdx);  //hack to avoid linux bug
	double angle = g.angleFromPoint(rect, point);
	point = g.edgePointFromAngle(rect, angle);
	return point;
}

hdPoint hdChopBoxConnector::findStart(int posIdx, hdLineConnection *connFigure)
{
	if(!connFigure)
	{
		point = getDisplayBox().center(posIdx);
		return point;
	}

	if(connFigure->pointCount(posIdx) < 2)
	{
		point = getDisplayBox().center(posIdx);
		return point;
	}

	hdIFigure *start = connFigure->getStartConnector()->getOwner();
	point = connFigure->pointAt(posIdx, 1);
	point = chop(posIdx, start, point);
	return point;
}

hdPoint hdChopBoxConnector::findEnd(int posIdx, hdLineConnection *connFigure)
{
	if(!connFigure)
	{
		return getDisplayBox().center(posIdx);
	}
	hdIFigure *end = connFigure->getEndConnector()->getOwner();
	point = connFigure->pointAt(posIdx, connFigure->pointCount(posIdx) - 2);
	point = chop(posIdx, end, point);
	return point;
}
