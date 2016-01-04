//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDragTrackerTool.h - A Tool that allow to drag and drop figures at the view
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDDRAGTRACKERTOOL_H
#define HDDRAGTRACKERTOOL_H

#include "hotdraw/main/hdObject.h"
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/main/hdDrawingEditor.h"
#include "hotdraw/tools/hdAbstractTool.h"


class hdDragTrackerTool : public hdAbstractTool
{
public:
	hdDragTrackerTool(hdDrawingView *view, hdIFigure *anchor);
	~hdDragTrackerTool();

	void setLastCoords(int x, int y);
	bool hasMoved();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(hdMouseEvent &event);
	virtual void mouseDrag(hdMouseEvent &event);

protected:
	hdIFigure *anchorFigure;
	bool hasMovedValue;

private:
	int lastX, lastY;

};
#endif
