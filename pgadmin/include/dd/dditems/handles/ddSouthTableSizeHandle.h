//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddSouthTableSizeHandle.h - Allow to change table size by using drag and drop from south side of table rectangle
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDSOUTHTABLESIZEHANDLE_H
#define DDSOUTHTABLESIZEHANDLE_H

#include "dd/wxhotdraw/handles/wxhdLocatorHandle.h"

class ddTableFigure;

class ddSouthTableSizeHandle : public wxhdLocatorHandle
{
public:
	ddSouthTableSizeHandle(ddTableFigure *owner, wxhdILocator *locator);
	~ddSouthTableSizeHandle();
	virtual wxCursor createCursor();
	virtual void draw(wxBufferedDC &context, wxhdDrawingView *view);
	virtual wxhdRect &getDisplayBox(int posIdx);
	virtual void invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view);
protected:

private:
	int anchorY;

};
#endif
