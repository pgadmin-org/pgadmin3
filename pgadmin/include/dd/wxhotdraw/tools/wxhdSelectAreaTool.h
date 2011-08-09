//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdSelectAreaTool.h - Tool to allow selection of figures inside a rectangle
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDSELECTAREATOOL_H
#define WXHDSELECTAREATOOL_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"


class wxhdSelectAreaTool : public wxhdAbstractTool
{
public:
	wxhdSelectAreaTool(wxhdDrawingView *view);
	~wxhdSelectAreaTool();

	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseDrag(wxhdMouseEvent &event);
	void selectFiguresOnRect(bool shiftPressed, wxhdDrawingView *view);
protected:
	void drawSelectionRect(wxhdDrawingView *view);
private:
	wxhdRect selectionRect;
};
#endif
