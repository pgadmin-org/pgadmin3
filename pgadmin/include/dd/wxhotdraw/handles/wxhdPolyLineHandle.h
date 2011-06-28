//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineHandle.h - Handle for manipulation of multiple flexibility points lines.
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDPOLYLINEHANDLE
#define WXHDPOLYLINEHANDLE

#include "dd/wxhotdraw/handles/wxhdLocatorHandle.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"

class wxhdDrawingView;
class wxhdPolyLineFigure;

class wxhdPolyLineHandle : public wxhdLocatorHandle
{
public:
	wxhdPolyLineHandle(wxhdPolyLineFigure *figure, wxhdILocator *loc, int index);
	~wxhdPolyLineHandle();

	virtual void draw(wxBufferedDC &context, wxhdDrawingView *view);
	virtual void invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual wxCursor createCursor();
	virtual int getIndex();
	virtual void setIndex(int index);
protected:

private:
	int indx;


};
#endif
