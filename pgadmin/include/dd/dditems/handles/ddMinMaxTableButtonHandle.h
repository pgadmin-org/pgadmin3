//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddMinMaxTableButtonHandle.h - A handle for a table figure that allow to graphically minimize or maximize table window size
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDMINMAXTABLEBUTTONHANDLE_H
#define DDMINMAXTABLEBUTTONHANDLE_H

#include "hotdraw/handles/hdButtonHandle.h"

class ddMinMaxTableButtonHandle : public hdButtonHandle
{
public:
	ddMinMaxTableButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxBitmap &buttonSecondImage, wxSize &size);
	~ddMinMaxTableButtonHandle();

	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
protected:

private:
	wxImage handleCursorImage;
	wxCursor handleCursor;
	wxBitmap buttonMaximizeImage, tmpImage;
	bool showFirst;
};
#endif
