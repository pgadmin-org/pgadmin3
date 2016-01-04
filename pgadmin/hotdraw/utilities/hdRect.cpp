//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdRect.cpp - wxRect improved class with new needed functionalities
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/utilities/hdGeometry.h"

hdRect::hdRect()
{
}

hdRect::hdRect(int xx, int yy, int ww, int hh):
	wxRect(xx, yy, ww, hh)
{
}

hdRect::hdRect(hdPoint *topLeft, hdPoint *bottomRight):
	wxRect(*topLeft, *bottomRight)
{
}

hdRect::hdRect(hdPoint *point):
	wxRect(point->x, point->y, 0, 0)
{
}

hdRect::hdRect(hdPoint &point):
	wxRect(point.x, point.y, 0, 0)
{
}

void hdRect::add (int newX, int newY)
{
	int x1 = hdGeometry::min(x , newX);
	int x2 = hdGeometry::max(x + width , newX);
	int y1 = hdGeometry::min(y , newY);
	int y2 = hdGeometry::max(y + height , newY);

	SetX(x1);
	SetWidth(x2 - x1);
	SetY(y1);
	SetHeight(y2 - y1);
}


void hdRect::add (hdRect *newRect)
{
	add(newRect->GetTopLeft().x , newRect->GetTopLeft().y);
	add(newRect->GetBottomRight().x , newRect->GetBottomRight().y);
}

void hdRect::add (hdRect newRect)
{
	add(newRect.GetTopLeft().x , newRect.GetTopLeft().y);
	add(newRect.GetBottomRight().x , newRect.GetBottomRight().y);
}

void hdRect::add(hdPoint *p)
{
	add(p->x, p->y);
}



hdPoint hdRect::center()
{
	point = hdPoint(x + (width / 2) , y + (height / 2));
	return point;
}

