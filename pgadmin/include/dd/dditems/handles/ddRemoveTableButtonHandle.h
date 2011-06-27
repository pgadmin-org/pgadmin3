//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRemoveTableButtonHandle.h - A handle for a table figure that allow to delete it
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDREMOVETABLEBUTTONHANDLE_H
#define DDREMOVETABLEBUTTONHANDLE_H

#include "dd/wxhotdraw/handles/wxhdButtonHandle.h"

class ddRemoveTableButtonHandle : public wxhdButtonHandle
{
public:
	ddRemoveTableButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator ,wxBitmap &buttonImage, wxSize &size);
    ~ddRemoveTableButtonHandle();

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
