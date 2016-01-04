//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRemoveTableButtonHandle.h - A handle for a table figure that allow to delete it
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDREMOVETABLEBUTTONHANDLE_H
#define DDREMOVETABLEBUTTONHANDLE_H

#include "hotdraw/handles/hdButtonHandle.h"

class ddRemoveTableButtonHandle : public hdButtonHandle
{
public:
	ddRemoveTableButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size);
	~ddRemoveTableButtonHandle();

	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
protected:

private:
	wxImage handleCursorImage;
	wxCursor handleCursor;
};
#endif
