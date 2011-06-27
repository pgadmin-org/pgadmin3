//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddMinMaxTableButtonHandle.h - A handle for a table figure that allow to graphically minimize or maximize table window size
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDMINMAXTABLEBUTTONHANDLE_H
#define DDMINMAXTABLEBUTTONHANDLE_H

#include "dd/wxhotdraw/handles/wxhdButtonHandle.h"

class ddMinMaxTableButtonHandle : public wxhdButtonHandle
{
public:
	ddMinMaxTableButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator ,wxBitmap &buttonImage, wxBitmap &buttonSecondImage, wxSize &size);
    ~ddMinMaxTableButtonHandle();

	virtual void invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view);
	wxCursor createCursor();
protected:

private:
	wxImage handleCursorImage;
    wxCursor handleCursor;
	wxBitmap buttonMaximizeImage, tmpImage;
	bool showFirst;
};
#endif
