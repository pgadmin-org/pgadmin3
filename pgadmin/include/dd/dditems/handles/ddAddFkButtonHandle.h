//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddFkButtonHandle.h - A handle for a table figure that allow to graphically add relationships (fk)
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDADDFKBUTTONHANDLE_H
#define DDADDFKBUTTONHANDLE_H

#include "hotdraw/handles/hdButtonHandle.h"

class ddAddFkButtonHandle : public hdButtonHandle
{
public:
	ddAddFkButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size);
	~ddAddFkButtonHandle();

	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
protected:

private:
	wxImage handleCursorImage;
	wxCursor handleCursor;
};
#endif
