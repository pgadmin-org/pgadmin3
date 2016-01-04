//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdMultiRect.cpp - hdMultiPosRect improved class with new needed functionalities for allowing multiple displaybox for same figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dynarray.h>

// App headers
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/utilities/hdMultiPosRect.h"
#include "hotdraw/utilities/hdGeometry.h"

void hdMultiPosRect::init(int valX, int valY)
{
	int i;
	for(i = 0; i < MAXPOS; i++)
	{
		x.Add(valX);
		y.Add(valY);
	}
}

hdMultiPosRect::hdMultiPosRect(int posIdx, const wxPoint &topLeft, const wxPoint &bottomRight)
{
	x[posIdx] = topLeft.x;
	y[posIdx] = topLeft.y;
	width = bottomRight.x - topLeft.x;
	height = bottomRight.y - topLeft.y;

	if (width < 0)
	{
		width = -width;
		x[posIdx] = bottomRight.x;
	}
	width++;

	if (height < 0)
	{
		height = -height;
		y[posIdx] = bottomRight.y;
	}
	height++;
}

//simplify this function with SetCount() in a future
void hdMultiPosRect::addNewXYPosition()
{
	x.Add(-1);
	y.Add(-1);
}

void hdMultiPosRect::removeXYPosition(int posIdx)
{
	x.RemoveAt(posIdx);
	y.RemoveAt(posIdx);
}

hdMultiPosRect &hdMultiPosRect::Union(int posIdx, const hdMultiPosRect &rect)
{
	// ignore empty rectangles: union with an empty rectangle shouldn't extend
	// this one to (0, 0)
	if ( !width || !height )
	{
		*this = rect;
	}
	else if ( rect.width && rect.height )
	{
		int x1 = wxMin(x[posIdx], rect.x[posIdx]);
		int y1 = wxMin(y[posIdx], rect.y[posIdx]);
		int y2 = wxMax(y[posIdx] + height, rect.height + rect.y[posIdx]);
		int x2 = wxMax(x[posIdx] + width, rect.width + rect.x[posIdx]);

		x[posIdx] = x1;
		y[posIdx] = y1;
		width = x2 - x1;
		height = y2 - y1;
	}
	//else: we're not empty and rect is empty

	return *this;
}

hdMultiPosRect &hdMultiPosRect::Inflate(int posIdx, wxCoord dx, wxCoord dy)
{
	if (-2 * dx > width)
	{
		// Don't allow deflate to eat more width than we have,
		// a well-defined rectangle cannot have negative width.
		x[posIdx] += width / 2;
		width = 0;
	}
	else
	{
		// The inflate is valid.
		x[posIdx] -= dx;
		width += 2 * dx;
	}

	if (-2 * dy > height)
	{
		// Don't allow deflate to eat more height than we have,
		// a well-defined rectangle cannot have negative height.
		y[posIdx] += height / 2;
		height = 0;
	}
	else
	{
		// The inflate is valid.
		y[posIdx] -= dy;
		height += 2 * dy;
	}

	return *this;
}

bool hdMultiPosRect::Contains(int posIdx, int cx, int cy) const
{
	return ( (cx >= x[posIdx]) && (cy >= y[posIdx])
	         && ((cy - y[posIdx]) < height)
	         && ((cx - x[posIdx]) < width)
	       );
}

bool hdMultiPosRect::Contains(int posIdx, const hdRect &rect) const
{
	return Contains(posIdx, rect.GetTopLeft()) && Contains(posIdx, rect.GetBottomRight());
}

hdMultiPosRect &hdMultiPosRect::Intersect(int posIdx, const hdMultiPosRect &rect)
{
	int x2 = GetRight(posIdx),
	    y2 = GetBottom(posIdx);

	if ( x[posIdx] < rect.x[posIdx] )
		x[posIdx] = rect.x[posIdx];
	if ( y[posIdx] < rect.y[posIdx] )
		y[posIdx] = rect.y[posIdx];
	if ( x2 > rect.GetRight(posIdx) )
		x2 = rect.GetRight(posIdx);
	if ( y2 > rect.GetBottom(posIdx) )
		y2 = rect.GetBottom(posIdx);

	width = x2 - x[posIdx] + 1;
	height = y2 - y[posIdx] + 1;

	if ( width <= 0 || height <= 0 )
	{
		width =
		    height = 0;
	}

	return *this;
}

bool hdMultiPosRect::Intersects(int posIdx, const hdMultiPosRect &rect) const
{
	hdMultiPosRect r = Intersect(posIdx, rect);

	// if there is no intersection, both width and height are 0
	return r.width != 0;
}

void hdMultiPosRect::add (int posIdx, int newX, int newY)
{
	int x1 = hdGeometry::min(x[posIdx] , newX);
	int x2 = hdGeometry::max(x[posIdx] + width , newX);
	int y1 = hdGeometry::min(y[posIdx] , newY);
	int y2 = hdGeometry::max(y[posIdx] + height , newY);

	SetX(posIdx, x1);
	SetWidth(x2 - x1);
	SetY(posIdx, y1);
	SetHeight(y2 - y1);
}


void hdMultiPosRect::add (int posIdx, hdRect *newRect)
{
	add(posIdx, newRect->GetTopLeft().x , newRect->GetTopLeft().y);
	add(posIdx, newRect->GetBottomRight().x , newRect->GetBottomRight().y);
}

void hdMultiPosRect::add (int posIdx, hdRect newRect)
{
	add(posIdx, newRect.GetTopLeft().x , newRect.GetTopLeft().y);
	add(posIdx, newRect.GetBottomRight().x , newRect.GetBottomRight().y);
}

void hdMultiPosRect::add(int posIdx, hdPoint *p)
{
	add(posIdx, p->x, p->y);
}



hdPoint hdMultiPosRect::center(int posIdx)
{
	point = hdPoint(x[posIdx] + (width / 2) , y[posIdx] + (height / 2));
	return point;
}

int hdMultiPosRect::CountPositions()
{
	return x.Count();
}
