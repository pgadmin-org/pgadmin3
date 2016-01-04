//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddSouthTableSizeHandle.h - Allow to change table size by using drag and drop from south side of table rectangle
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDSOUTHTABLESIZEHANDLE_H
#define DDSOUTHTABLESIZEHANDLE_H

#include "hotdraw/handles/hdLocatorHandle.h"

class ddTableFigure;

class ddSouthTableSizeHandle : public hdLocatorHandle
{
public:
	ddSouthTableSizeHandle(ddTableFigure *owner, hdILocator *locator);
	~ddSouthTableSizeHandle();
	virtual wxCursor createCursor();
	virtual void draw(wxBufferedDC &context, hdDrawingView *view);
	virtual hdRect &getDisplayBox(int posIdx);
	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
protected:

private:
	int anchorY;

};
#endif
