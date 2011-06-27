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

wxhdCreationTool::wxhdCreationTool(wxhdDrawingEditor *editor, wxhdIFigure *prototype):
wxhdAbstractTool(editor)
{
	figurePrototype=prototype;
}

wxhdCreationTool::~wxhdCreationTool()
{
}

void wxhdCreationTool::mouseDown(wxhdMouseEvent& event)
{
	wxhdAbstractTool::mouseDown(event);
	if(event.LeftDown())
	{
		getDrawingEditor()->view()->getDrawing()->add(figurePrototype);
		int x=event.GetPosition().x, y=event.GetPosition().y;
		figurePrototype->moveTo(x,y);
		getDrawingEditor()->view()->clearSelection();
		getDrawingEditor()->view()->addToSelection(figurePrototype);
	}
}

void wxhdCreationTool::mouseUp(wxhdMouseEvent& event)
{
	wxhdAbstractTool::mouseUp(event);
	getDrawingEditor()->setTool(new wxhdSelectionTool(getDrawingEditor()));
}

void wxhdCreationTool::activate()
{
	wxhdAbstractTool::activate();
	getDrawingEditor()->view()->SetCursor(wxCursor(wxCURSOR_CROSS));
}

void wxhdCreationTool::deactivate()
{
	wxhdAbstractTool::deactivate();
	getDrawingEditor()->view()->SetCursor(wxCursor(wxCURSOR_ARROW));
}

void wxhdCreationTool::setPrototype(wxhdIFigure *prototype)
{
	figurePrototype=prototype;
}

wxhdIFigure* wxhdCreationTool::getPrototype()
{
	return figurePrototype;
}
