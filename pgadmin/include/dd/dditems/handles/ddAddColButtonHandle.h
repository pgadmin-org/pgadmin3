//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddColButtonHandle.h - A handle for a table figure that allow to graphically add columns 
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDADDCOLBUTTONHANDLE_H
#define DDADDCOLBUTTONHANDLE_H

#include "dd/wxhotdraw/handles/wxhdButtonHandle.h"

class ddAddColButtonHandle : public wxhdButtonHandle
{
public:
	ddAddColButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator ,wxBitmap &buttonImage, wxSize &size);
    ~ddAddColButtonHandle();

	virtual void invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view);
	wxCursor createCursor();
protected:

private:
    wxImage handleCursorImage;
    wxCursor handleCursor;
};
#endif
