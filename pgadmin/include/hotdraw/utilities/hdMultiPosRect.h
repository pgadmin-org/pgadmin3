//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdMultiRect.h - hdRect improved class with new needed functionalities for allowing multiple displaybox for same figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDMULTIPOSRECT_H
#define HDMULTIPOSRECT_H
#include "hotdraw/main/hdObject.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/utilities/hdRect.h"

#define MAXPOS 1

class hdMultiPosRect : public hdObject
{
public:
	hdMultiPosRect()
		: width(0), height(0)
	{
		init();
	}

	hdMultiPosRect(int xx, int yy, int ww, int hh)
		: width(ww), height(hh)
	{
		init(xx, yy);
	}

	hdMultiPosRect(int posIdx, const wxPoint &topLeft, const wxPoint &bottomRight);

	hdMultiPosRect(hdPoint *point)
		: width(0), height(0)
	{
		init(point->x, point->y);
	}

	hdMultiPosRect(hdPoint &point)
		: width(0), height(0)
	{
		init(point.x, point.y);
	}

	hdMultiPosRect(const wxPoint &pt, const wxSize &size)
		: width(size.x), height(size.y)
	{
		init(pt.x, pt.y);
	}

	hdMultiPosRect(const wxSize &size)
		: width(size.x), height(size.y)
	{
		init(0, 0);
	}
	// default copy ctor and assignment operators ok

	int GetX(int posIdx) const
	{
		return x[posIdx];
	}
	void SetX(int posIdx, int xx)
	{
		x[posIdx] = xx;
	}

	int GetY(int posIdx) const
	{
		return y[posIdx];
	}
	void SetY(int posIdx, int yy)
	{
		y[posIdx] = yy;
	}

	int GetWidth() const
	{
		return width;
	}
	void SetWidth(int w)
	{
		width = w;
	}

	int GetHeight() const
	{
		return height;
	}
	void SetHeight(int h)
	{
		height = h;
	}

	wxPoint GetPosition(int posIdx) const
	{
		return wxPoint(x[posIdx], y[posIdx]);
	}
	void SetPosition( int posIdx, const wxPoint &p )
	{
		x[posIdx] = p.x;
		y[posIdx] = p.y;
	}

	wxSize GetSize() const
	{
		return wxSize(width, height);
	}
	void SetSize( const wxSize &s )
	{
		width = s.GetWidth();
		height = s.GetHeight();
	}

	bool IsEmpty() const
	{
		return (width <= 0) || (height <= 0);
	}

	int GetLeft(int posIdx)   const
	{
		return x[posIdx];
	}
	int GetTop(int posIdx)    const
	{
		return y[posIdx];
	}
	int GetBottom(int posIdx) const
	{
		return y[posIdx] + height - 1;
	}
	int GetRight(int posIdx)  const
	{
		return x[posIdx] + width - 1;
	}

	void SetLeft(int posIdx, int left)
	{
		x[posIdx] = left;
	}
	void SetRight(int posIdx, int right)
	{
		width = right - x[posIdx] + 1;
	}
	void SetTop(int posIdx, int top)
	{
		y[posIdx] = top;
	}
	void SetBottom(int posIdx, int bottom)
	{
		height = bottom - y[posIdx] + 1;
	}

	wxPoint GetTopLeft(int posIdx) const
	{
		return GetPosition(posIdx);
	}
	wxPoint GetLeftTop(int posIdx) const
	{
		return GetTopLeft(posIdx);
	}
	void SetTopLeft(int posIdx, const wxPoint &p)
	{
		SetPosition(posIdx, p);
	}
	void SetLeftTop(int posIdx, const wxPoint &p)
	{
		SetTopLeft(posIdx, p);
	}

	wxPoint GetBottomRight(int posIdx) const
	{
		return wxPoint(GetRight(posIdx), GetBottom(posIdx));
	}
	wxPoint GetRightBottom(int posIdx) const
	{
		return GetBottomRight(posIdx);
	}
	void SetBottomRight(int posIdx, const wxPoint &p)
	{
		SetRight(posIdx, p.x);
		SetBottom(posIdx, p.y);
	}
	void SetRightBottom(int posIdx, const wxPoint &p)
	{
		SetBottomRight(posIdx, p);
	}

	wxPoint GetTopRight(int posIdx) const
	{
		return wxPoint(GetRight(posIdx), GetTop(posIdx));
	}
	wxPoint GetRightTop(int posIdx) const
	{
		return GetTopRight(posIdx);
	}
	void SetTopRight(int posIdx, const wxPoint &p)
	{
		SetRight(posIdx, p.x);
		SetTop(posIdx, p.y);
	}
	void SetRightTop(int posIdx, const wxPoint &p)
	{
		SetTopLeft(posIdx, p);
	}

	wxPoint GetBottomLeft(int posIdx) const
	{
		return wxPoint(GetLeft(posIdx), GetBottom(posIdx));
	}
	wxPoint GetLeftBottom(int posIdx) const
	{
		return GetBottomLeft(posIdx);
	}
	void SetBottomLeft(int posIdx, const wxPoint &p)
	{
		SetLeft(posIdx, p.x);
		SetBottom(posIdx, p.y);
	}
	void SetLeftBottom(int posIdx, const wxPoint &p)
	{
		SetBottomLeft(posIdx, p);
	}

	// operations with rect
	hdMultiPosRect &Inflate(int posIdx, wxCoord dx, wxCoord dy);
	hdMultiPosRect &Inflate(int posIdx, const wxSize &d)
	{
		return Inflate(posIdx, d.x, d.y);
	}
	hdMultiPosRect &Inflate(int posIdx, wxCoord d)
	{
		return Inflate(posIdx, d, d);
	}

	hdMultiPosRect Inflate(int posIdx, wxCoord dx, wxCoord dy) const
	{
		hdMultiPosRect r = *this;
		r.Inflate(posIdx, dx, dy);
		return r;
	}

	hdMultiPosRect &Deflate(int posIdx, wxCoord dx, wxCoord dy)
	{
		return Inflate(posIdx, -dx, -dy);
	}
	hdMultiPosRect &Deflate(int posIdx, const wxSize &d)
	{
		return Inflate(posIdx, -d.x, -d.y);
	}
	hdMultiPosRect &Deflate(int posIdx, wxCoord d)
	{
		return Inflate(posIdx, -d);
	}

	hdMultiPosRect Deflate(int posIdx, wxCoord dx, wxCoord dy) const
	{
		hdMultiPosRect r = *this;
		r.Deflate(posIdx, dx, dy);
		return r;
	}


	void Offset(int posIdx, wxCoord dx, wxCoord dy)
	{
		x[posIdx] += dx;
		y[posIdx] += dy;
	}
	void Offset(int posIdx, const wxPoint &pt)
	{
		Offset(posIdx, pt.x, pt.y);
	}

	hdMultiPosRect &Intersect(int posIdx, const hdMultiPosRect &rect);

	hdMultiPosRect Intersect(int posIdx, const hdMultiPosRect &rect) const
	{
		hdMultiPosRect r = *this;
		r.Intersect(posIdx, rect);
		return r;
	}

	hdMultiPosRect &Union(int posIdx, const hdMultiPosRect &rect);


	hdMultiPosRect Union(int posIdx, const hdMultiPosRect &rect) const
	{
		hdMultiPosRect r = *this;
		r.Union(posIdx, rect);
		return r;
	}

	// return true if the point is (not strcitly) inside the rect
	bool Contains(int posIdx, int x, int y) const;
	bool Contains(int posIdx, const wxPoint &pt) const
	{
		return Contains(posIdx, pt.x, pt.y);
	}
	// return true if the rectangle is (not strcitly) inside the rect
	bool Contains(int posIdx, const hdRect &rect) const;

	// return true if the rectangles have a non empty intersection
	bool Intersects(int posIdx, const hdMultiPosRect &rect) const;

	// centre this rectangle in the given (usually, but not necessarily,
	// larger) one
	hdMultiPosRect CentreIn(int posIdxThis, const hdMultiPosRect &r, int posIdxr, int dir = wxBOTH) const
	{
		return hdMultiPosRect(dir & wxHORIZONTAL ? r.x[posIdxr] + (r.width - width) / 2 : x[posIdxThis],
		                      dir & wxVERTICAL ? r.y[posIdxr] + (r.height - height) / 2 : y[posIdxThis],
		                      width, height);
	}

	hdMultiPosRect CenterIn(int posIdxThis, const hdMultiPosRect &r, int posIdxr, int dir = wxBOTH) const
	{
		return CentreIn(posIdxThis, r, posIdxr, dir);
	}

	void add (int posIdx, int newX, int netY);
	void add (int posIdx, hdRect *newRect);
	void add (int posIdx, hdRect newRect);
	void add (int posIdx, hdPoint *p);
	hdPoint center(int posIdx);
	hdRect gethdRect(int posIdx)
	{
		return hdRect(x[posIdx], y[posIdx], width, height);
	}
	int CountPositions();
	void addNewXYPosition();
	void removeXYPosition(int posIdx);
	wxArrayInt x, y;
	int width, height;

private:
	void init(int valX = 0, int valY = 0);
	hdPoint point;
};
#endif

