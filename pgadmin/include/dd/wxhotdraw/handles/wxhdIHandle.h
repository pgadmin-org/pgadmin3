//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdIHandle.cpp - Base class for all Handles
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDIHANDLE_H
#define WXHDIHANDLE_H

#include "wx/dcbuffer.h"
#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

class wxhdDrawingView;
class wxhdIFigure;
class wxhdMouseEvent;

class wxhdIHandle : public wxhdObject
{
public:
	wxhdIHandle(wxhdIFigure *owner);
	~wxhdIHandle();

	static const int size = 4;

	virtual bool containsPoint(int x, int y);
	virtual void draw(wxBufferedDC &context, wxhdDrawingView *view) = 0;
	virtual wxhdPoint &locate() = 0;
	virtual void invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view) = 0;
	virtual void invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view) = 0;
	virtual void invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view) = 0;
	virtual wxCursor createCursor() = 0;
	virtual wxhdRect &getDisplayBox();
protected:
	virtual wxhdIFigure *getOwner();
	wxhdRect displayBox;
private:
	wxhdIFigure *figureOwner;
	double lineWidth;

};
#endif
