//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddColButtonHandle.h - A handle for a table figure that allow to graphically add columns
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDADDCOLBUTTONHANDLE_H
#define DDADDCOLBUTTONHANDLE_H

#include "hotdraw/handles/hdButtonHandle.h"

class ddAddColButtonHandle : public hdButtonHandle
{
public:
	ddAddColButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size);
	~ddAddColButtonHandle();

	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
protected:

private:
	wxImage handleCursorImage;
	wxCursor handleCursor;
};
#endif
