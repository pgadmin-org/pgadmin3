//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineFigureTool.cpp - Tool to allow creation of flexibility points at polylines figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdPolyLineFigureTool.h"
#include "dd/wxhotdraw/tools/wxhdFigureTool.h"
#include "dd/wxhotdraw/tools/wxhdHandleTrackerTool.h"
#include "dd/wxhotdraw/figures/wxhdPolyLineFigure.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/tools/wxhdMenuTool.h"

wxhdPolyLineFigureTool::wxhdPolyLineFigureTool(wxhdDrawingView *view, wxhdIFigure *fig, wxhdITool *dt):
	wxhdFigureTool(view, fig, dt)
{
}

wxhdPolyLineFigureTool::~wxhdPolyLineFigureTool()
{
}

void wxhdPolyLineFigureTool::mouseDown(wxhdMouseEvent &event)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	setAnchorCoords(x, y);
	//Other events like other mouse button click (no left double click) should be done at handle
	//because this tool only add flexibility points to polylines.
	if(event.LeftDClick())
	{
		wxhdPolyLineFigure *connection = (wxhdPolyLineFigure *) figure;
		connection->splitSegment(event.getView()->getIdx(), x, y);
		event.getView()->getDrawing()->clearSelection();
		event.getView()->getDrawing()->addToSelection(figure);
		wxhdIHandle *handle = event.getView()->findHandle(event.getView()->getIdx(), x, y);
		event.getView()->SetCursor(handle->createCursor());
		if(defaultTool)
			delete defaultTool;
		defaultTool = new wxhdHandleTrackerTool(event.getView(), handle);
		event.getView()->notifyChanged();
	}
	defaultTool->mouseDown(event);
}