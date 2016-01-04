//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddScrollBarHandle.h - A handle for a table figure that allow to scroll it when table is not in full size
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDSCROLLBARHANDLE_H
#define DDSCROLLBARHANDLE_H

#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/handles/hdLocatorHandle.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/utilities/hdPoint.h"
#include "dd/dditems/figures/ddTableFigure.h"


class ddScrollBarHandle : public hdLocatorHandle
{
public:
	ddScrollBarHandle(ddTableFigure *owner, hdILocator *scrollBarLocator , wxSize &size);
	~ddScrollBarHandle();


	virtual void draw(wxBufferedDC &context, hdDrawingView *view);
	virtual hdPoint &locate(int posIdx);
	virtual wxCursor createCursor();
	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
	virtual hdRect &getDisplayBox(int posIdx);
protected:
private:
	hdPoint pointLocate;
	hdILocator *scrollLocator;
	ddTableFigure *table;
	wxBitmap upBitmap, downBitmap;
	wxSize barSize;
	int anchorY;

};
#endif
