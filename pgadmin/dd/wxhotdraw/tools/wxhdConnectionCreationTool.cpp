//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdConnectionCreationTool.cpp - A Tool that allow to create a connection figure between two figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdConnectionCreationTool.h"
#include "dd/wxhotdraw/tools/wxhdSelectionTool.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

wxhdConnectionCreationTool::wxhdConnectionCreationTool(wxhdDrawingView *view, wxhdLineConnection *figure):
	wxhdAbstractTool(view)
{
	toolConnection = figure;
	toolConnection->disconnectStart();
	toolConnection->disconnectEnd();
	handle = NULL;
	numClicks = 0;
	dragged = false;
}

wxhdConnectionCreationTool::~wxhdConnectionCreationTool()
{
}

void wxhdConnectionCreationTool::mouseDrag(wxhdMouseEvent &event)
{
	if(handle && event.LeftIsDown())
	{
		dragged = true;
		handle->invokeStep(event, event.getView());
	}
}

void wxhdConnectionCreationTool::mouseDown(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseDown(event);
	if(event.LeftDown())
	{
		numClicks++;
		int x = event.getScrolledPosX(), y = event.getScrolledPosY();
		wxhdDrawingView *view = event.getView();
		wxhdIFigure *figure = view->getDrawing()->findFigure(view->getIdx(), x, y);

		if(figure)
		{
			if(numClicks == 1) //first mouse click to select start/end figure
			{
				toolConnection->setEndPoint(view->getIdx(), wxhdPoint(x, y));
				toolConnection->setStartPoint(view->getIdx(), wxhdPoint(x, y));
				toolConnection->connectStart(figure->connectorAt(view->getIdx(), x, y));
				toolConnection->updateConnection(view->getIdx());
				view->editor()->addDiagramFigure(view->getIdx(), toolConnection);
				view->getDrawing()->clearSelection();
				view->getDrawing()->addToSelection(toolConnection);
				handle = toolConnection->getEndHandle();
			}
			else if(numClicks > 1) //second mouse click to select end figure only
			{
				toolConnection->setEndPoint(view->getIdx(), wxhdPoint(x, y));
				toolConnection->updateConnection(event.getView()->getIdx());
			}
		}
		else
		{
			event.getView()->setTool(new wxhdSelectionTool(event.getView()));
		}
	}
}

void wxhdConnectionCreationTool::mouseUp(wxhdMouseEvent &event)
{
	if(event.LeftUp())
	{
		//Hack to allow one click and drag creation of connections
		if(handle)
		{
			if(!dragged && numClicks == 1) //mouse haven't be dragged and is first click of mouse at this tool
			{
				toolConnection->setEndPoint(event.getView()->getIdx(), event.GetPosition());
				toolConnection->updateConnection(event.getView()->getIdx());
			}
			else
			{
				handle->invokeEnd(event, event.getView());
			}
		}

		if((toolConnection->getEndConnector() == NULL && numClicks > 1) || (toolConnection->getEndConnector() == NULL && dragged)) //Delete connection only if a second click a connection figures isn't found
		{
			//check if exists at drawing because automatically integrity check
			if(event.getView()->getDrawing()->includes(toolConnection))
				event.getView()->getDrawing()->remove(toolConnection);
			event.getView()->getDrawing()->clearSelection();
			toolConnection->disconnectStart();
			toolConnection->disconnectEnd();
			event.getView()->editor()->deleteModelFigure(toolConnection);
		}
	}
	if(dragged || numClicks > 1) //if drag to select a figure or is second or higher click (to select end figure) then this tool ends.
	{
		event.getView()->setTool(new wxhdSelectionTool(event.getView()));
	}
	else if(!dragged && numClicks == 1) //if not dragged before and is first click then allow to select end, disconnecting it
	{
		toolConnection->disconnectEnd();
	}

}

void wxhdConnectionCreationTool::mouseMove(wxhdMouseEvent &event)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	wxhdDrawingView *view = event.getView();
	wxhdIFigure *figure = view->getDrawing()->findFigure(view->getIdx(), x, y);
	if(figure)
	{
		view->SetCursor(wxCursor(wxCURSOR_PENCIL));
	}
	else
	{
		view->SetCursor(wxCursor(wxCURSOR_CROSS));
	}

	if(toolConnection && handle && numClicks > 0)
	{
		handle->invokeStep(event, view);
	}
}
