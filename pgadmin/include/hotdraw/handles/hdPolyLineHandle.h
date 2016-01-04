//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPolyLineHandle.h - Handle for manipulation of multiple flexibility points lines.
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDPOLYLINEHANDLE
#define HDPOLYLINEHANDLE

#include "hotdraw/handles/hdLocatorHandle.h"
#include "hotdraw/utilities/hdRect.h"

class hdDrawingView;
class hdPolyLineFigure;

class hdPolyLineHandle : public hdLocatorHandle
{
public:
	hdPolyLineHandle(hdPolyLineFigure *figure, hdILocator *loc, int index);
	~hdPolyLineHandle();

	virtual void draw(wxBufferedDC &context, hdDrawingView *view);
	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual wxCursor createCursor();
	virtual int getIndex();
	virtual void setIndex(int index);
protected:

private:
	int indx;


};
#endif
