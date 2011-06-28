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


wxhdPoint wxhdChopBoxConnector::chop(wxhdIFigure *target, wxhdPoint point)
{
	if(target && target->containsPoint(point.x, point.y))
	{
		point = target->displayBox().center();
		return point;
	}
	else if(!target)
	{
		point = wxhdPoint(0, 0);
		return point;
	}

	wxhdGeometry g;

	rect = getDisplayBox();  //hack to avoid linux bug
	double angle = g.angleFromPoint(rect, point);
	point = g.edgePointFromAngle(rect, angle);
	return point;
}

wxhdPoint wxhdChopBoxConnector::findStart(wxhdLineConnection *connFigure)
{
	if(!connFigure)
	{
		point = getDisplayBox().center();
		return point;
	}

	if(connFigure->pointCount() < 2)
	{
		point = getDisplayBox().center();
		return point;
	}

	wxhdIFigure *start = connFigure->getStartConnector()->getOwner();
	point = connFigure->pointAt(1);
	point = chop(start, point);
	return point;
}

wxhdPoint wxhdChopBoxConnector::findEnd(wxhdLineConnection *connFigure)
{
	if(!connFigure)
	{
		return getDisplayBox().center();
	}
	wxhdIFigure *end = connFigure->getEndConnector()->getOwner();
	point = connFigure->pointAt(connFigure->pointCount() - 2);
	point = chop(end, point);
	return point;
}
