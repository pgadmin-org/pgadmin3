//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddScrollBarHandle.h - A handle for a table figure that allow to scroll it when table is not in full size
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDSCROLLBARHANDLE_H
#define DDSCROLLBARHANDLE_H

#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/handles/wxhdLocatorHandle.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/dditems/figures/ddTableFigure.h"


class ddScrollBarHandle : public wxhdLocatorHandle
{
public:
	ddScrollBarHandle(ddTableFigure *owner, wxhdILocator *scrollBarLocator , wxSize &size);
	~ddScrollBarHandle();


	virtual void draw(wxBufferedDC &context, wxhdDrawingView *view);
	virtual wxhdPoint &locate();
	virtual wxCursor createCursor();
	virtual void invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual wxhdRect &getDisplayBox();
protected:
private:
	wxhdPoint pointLocate;
	wxhdILocator *scrollLocator;
	ddTableFigure *table;
	wxBitmap upBitmap, downBitmap;
	wxSize barSize;
	int anchorY;

};
#endif
