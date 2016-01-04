//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdRect.h - wxRect improved class with new needed functionalities
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDRECT_H
#define HDRECT_H
#include "hotdraw/utilities/hdPoint.h"

class hdRect : public wxRect
{
public:
	hdRect();
	hdRect(int xx, int yy, int ww, int hh);
	hdRect(hdPoint *topLeft, hdPoint *bottomRight);
	hdRect(hdPoint *point);
	hdRect(hdPoint &point);
	void add (int newX, int netY);
	void add (hdRect *newRect);
	void add (hdRect newRect);
	void add (hdPoint *p);
	hdPoint center();
protected:

private:
	hdPoint point;
};
#endif
