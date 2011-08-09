//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCreationTool.cpp - A Tool that create a figure by just click on view
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdCreationTool.h"
#include "dd/wxhotdraw/tools/wxhdSelectionTool.h"

wxhdCreationTool::wxhdCreationTool(wxhdDrawingView *view, wxhdIFigure *prototype):
	wxhdAbstractTool(view)
{
	figurePrototype = prototype;
}

wxhdCreationTool::~wxhdCreationTool()
{
}

void wxhdCreationTool::mouseDown(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseDown(event);
	if(event.LeftDown())
	{
		event.getView()->getDrawing()->add(figurePrototype);
		int x = event.GetPosition().x, y = event.GetPosition().y;
		figurePrototype->moveTo(event.getView()->getIdx(), x, y);
		event.getView()->getDrawing()->clearSelection();
		event.getView()->getDrawing()->addToSelection(figurePrototype);
	}
}

void wxhdCreationTool::mouseUp(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseUp(event);
	event.getView()->setTool(new wxhdSelectionTool(event.getView()));
}

void wxhdCreationTool::activate(wxhdDrawingView *view)
{
	wxhdAbstractTool::activate(view);
	view->SetCursor(wxCursor(wxCURSOR_CROSS));
}

void wxhdCreationTool::deactivate(wxhdDrawingView *view)
{
	wxhdAbstractTool::deactivate(view);
	view->SetCursor(wxCursor(wxCURSOR_ARROW));
}

void wxhdCreationTool::setPrototype(wxhdIFigure *prototype)
{
	figurePrototype = prototype;
}

wxhdIFigure *wxhdCreationTool::getPrototype()
{
	return figurePrototype;
}
