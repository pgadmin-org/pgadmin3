//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPolyLineFigureTool.cpp - Tool to allow creation of flexibility points at polylines figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdPolyLineFigureTool.h"
#include "hotdraw/tools/hdFigureTool.h"
#include "hotdraw/tools/hdHandleTrackerTool.h"
#include "hotdraw/figures/hdPolyLineFigure.h"
#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/tools/hdMenuTool.h"

hdPolyLineFigureTool::hdPolyLineFigureTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt):
	hdFigureTool(view, fig, dt)
{
}

hdPolyLineFigureTool::~hdPolyLineFigureTool()
{
}

void hdPolyLineFigureTool::mouseDown(hdMouseEvent &event)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	setAnchorCoords(x, y);
	//Other events like other mouse button click (no left double click) should be done at handle
	//because this tool only add flexibility points to polylines.
	if(event.LeftDClick())
	{
		hdPolyLineFigure *connection = (hdPolyLineFigure *) figure;
		connection->splitSegment(event.getView()->getIdx(), x, y);
		event.getView()->getDrawing()->clearSelection();
		event.getView()->getDrawing()->addToSelection(figure);
		hdIHandle *handle = event.getView()->findHandle(event.getView()->getIdx(), x, y);
		event.getView()->SetCursor(handle->createCursor());
		if(defaultTool)
			delete defaultTool;
		defaultTool = new hdHandleTrackerTool(event.getView(), handle);
		event.getView()->notifyChanged();
	}
	defaultTool->mouseDown(event);
}
