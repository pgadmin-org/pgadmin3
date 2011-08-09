//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdRect.cpp - wxRect improved class with new needed functionalities
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"

wxhdRect::wxhdRect()
{
}

wxhdRect::wxhdRect(int xx, int yy, int ww, int hh):
	wxRect(xx, yy, ww, hh)
{
}

wxhdRect::wxhdRect(wxhdPoint *topLeft, wxhdPoint *bottomRight):
	wxRect(*topLeft, *bottomRight)
{
}

wxhdRect::wxhdRect(wxhdPoint *point):
	wxRect(point->x, point->y, 0, 0)
{
}

wxhdRect::wxhdRect(wxhdPoint &point):
	wxRect(point.x, point.y, 0, 0)
{
}

void wxhdRect::add (int newX, int newY)
{
	int x1 = wxhdGeometry::min(x , newX);
	int x2 = wxhdGeometry::max(x + width , newX);
	int y1 = wxhdGeometry::min(y , newY);
	int y2 = wxhdGeometry::max(y + height , newY);

	SetX(x1);
	SetWidth(x2 - x1);
	SetY(y1);
	SetHeight(y2 - y1);
}


void wxhdRect::add (wxhdRect *newRect)
{
	add(newRect->GetTopLeft().x , newRect->GetTopLeft().y);
	add(newRect->GetBottomRight().x , newRect->GetBottomRight().y);
}

void wxhdRect::add (wxhdRect newRect)
{
	add(newRect.GetTopLeft().x , newRect.GetTopLeft().y);
	add(newRect.GetBottomRight().x , newRect.GetBottomRight().y);
}

void wxhdRect::add(wxhdPoint *p)
{
	add(p->x, p->y);
}



wxhdPoint wxhdRect::center()
{
	point = wxhdPoint(x + (width / 2) , y + (height / 2));
	return point;
}

