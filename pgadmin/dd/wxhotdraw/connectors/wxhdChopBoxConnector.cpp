//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdChopBoxConnector.cpp - Connector for center of figure to line crossing one limit line of rect
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/connectors/wxhdChopBoxConnector.h"
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"

wxhdChopBoxConnector::wxhdChopBoxConnector(wxhdIFigure *owner):
	wxhdIConnector(owner)
{
}

wxhdChopBoxConnector::~wxhdChopBoxConnector()
{
}


wxhdPoint wxhdChopBoxConnector::chop(int posIdx, wxhdIFigure *target, wxhdPoint point)
{
	if(target && target->containsPoint(posIdx, point.x, point.y))
	{
		point = target->displayBox().center(posIdx);
		return point;
	}
	else if(!target)
	{
		point = wxhdPoint(0, 0);
		return point;
	}

	wxhdGeometry g;

	rect = getDisplayBox().getwxhdRect(posIdx);  //hack to avoid linux bug
	double angle = g.angleFromPoint(rect, point);
	point = g.edgePointFromAngle(rect, angle);
	return point;
}

wxhdPoint wxhdChopBoxConnector::findStart(int posIdx, wxhdLineConnection *connFigure)
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

	wxhdIFigure *start = connFigure->getStartConnector()->getOwner();
	point = connFigure->pointAt(posIdx, 1);
	point = chop(posIdx, start, point);
	return point;
}

wxhdPoint wxhdChopBoxConnector::findEnd(int posIdx, wxhdLineConnection *connFigure)
{
	if(!connFigure)
	{
		return getDisplayBox().center(posIdx);
	}
	wxhdIFigure *end = connFigure->getEndConnector()->getOwner();
	point = connFigure->pointAt(posIdx, connFigure->pointCount(posIdx) - 2);
	point = chop(posIdx, end, point);
	return point;
}
