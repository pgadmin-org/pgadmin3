//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDragTrackerTool.h - A Tool that allow to drag and drop figures at the view
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDDRAGTRACKERTOOL_H
#define WXHDDRAGTRACKERTOOL_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"


class wxhdDragTrackerTool : public wxhdAbstractTool
{
public:
	wxhdDragTrackerTool(wxhdDrawingView *view, wxhdIFigure *anchor);
	~wxhdDragTrackerTool();

	void setLastCoords(int x, int y);
	bool hasMoved();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseDrag(wxhdMouseEvent &event);

protected:
	wxhdIFigure *anchorFigure;
	bool hasMovedValue;

private:
	int lastX, lastY;

};
#endif
