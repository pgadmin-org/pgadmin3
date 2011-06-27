//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdRect.h - wxRect improved class with new needed functionalities
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDRECT_H
#define WXHDRECT_H
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

class wxhdRect : public wxRect
{
public:
	wxhdRect();
	wxhdRect(int xx, int yy, int ww, int hh);
	wxhdRect(wxhdPoint *topLeft, wxhdPoint *bottomRight);
	wxhdRect(wxhdPoint *point);
	wxhdRect(wxhdPoint &point);
	void add (int newX, int netY);
	void add (wxhdRect *newRect);
	void add (wxhdRect& newRect);
	void add (wxhdPoint *p);
	wxhdPoint center();
protected:

private:
	wxhdPoint point;
};
#endif
