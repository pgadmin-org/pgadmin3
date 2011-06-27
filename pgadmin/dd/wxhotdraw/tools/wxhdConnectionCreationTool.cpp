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

wxhdConnectionCreationTool::wxhdConnectionCreationTool(wxhdDrawingEditor *editor, wxhdLineConnection *figure):
wxhdAbstractTool(editor)
{
	toolConnection=figure;
	toolConnection->disconnectStart();
	toolConnection->disconnectEnd();
	handle=NULL;
	numClicks=0;
	dragged=false;
}

wxhdConnectionCreationTool::~wxhdConnectionCreationTool()
{
}

void wxhdConnectionCreationTool::mouseDrag(wxhdMouseEvent& event)
{
	if(handle && event.LeftIsDown())
	{
		dragged=true;
		handle->invokeStep(event,getDrawingEditor()->view());
	}
}

void wxhdConnectionCreationTool::mouseDown(wxhdMouseEvent& event)
{
	wxhdAbstractTool::mouseDown(event);
	if(event.LeftDown())
	{
		numClicks++;
		int x=event.getScrolledPosX(), y=event.getScrolledPosY();
		wxhdDrawingView *view = getDrawingEditor()->view();
		wxhdIFigure *figure = view->getDrawing()->findFigure(x,y);

		if(figure)
		{
			if(numClicks==1)  //first mouse click to select start/end figure
			{
				toolConnection->setEndPoint(wxhdPoint(x,y));
				toolConnection->setStartPoint(wxhdPoint(x,y));
				toolConnection->connectStart(figure->connectorAt(x,y));
				toolConnection->updateConnection();
				view->add(toolConnection);
				view->clearSelection();
				view->addToSelection(toolConnection);
				handle = toolConnection->getEndHandle();
			}
			else if(numClicks>1) //second mouse click to select end figure only
			{
					toolConnection->setEndPoint(wxhdPoint(x,y));
					toolConnection->updateConnection();
			}
		}
		else
		{
			getDrawingEditor()->setTool(new wxhdSelectionTool(getDrawingEditor()));
		}
	}
}

void wxhdConnectionCreationTool::mouseUp(wxhdMouseEvent& event)
{
	if(event.LeftUp())
	{
		 //Hack to allow one click and drag creation of connections
		if(handle)
		{
		if(!dragged && numClicks==1)   //mouse haven't be dragged and is first click of mouse at this tool
		{ 
				toolConnection->setEndPoint(event.GetPosition());
				toolConnection->updateConnection();
			}
			else
			{
				handle->invokeEnd(event,getDrawingEditor()->view());
			}
		}

		if((toolConnection->getEndConnector()==NULL && numClicks > 1) || (toolConnection->getEndConnector()==NULL && dragged))  //Delete connection only if a second click a connection figures isn't found
		{
			toolConnection->disconnectStart();
			toolConnection->disconnectEnd();
			getDrawingEditor()->view()->remove(toolConnection);
			getDrawingEditor()->view()->clearSelection(); 
		}
	}
	if(dragged || numClicks>1)   //if drag to select a figure or is second or higher click (to select end figure) then this tool ends.
	{
		getDrawingEditor()->setTool(new wxhdSelectionTool(getDrawingEditor()));
	}
	else if(!dragged && numClicks==1)  //if not dragged before and is first click then allow to select end, disconnecting it
	{
		toolConnection->disconnectEnd();
	}

}

void wxhdConnectionCreationTool::mouseMove(wxhdMouseEvent& event)
{
	int x=event.GetPosition().x, y=event.GetPosition().y;
	wxhdDrawingView *view = getDrawingEditor()->view();
	wxhdIFigure *figure = view->getDrawing()->findFigure(x,y);
	if(figure)
	{
		getDrawingEditor()->view()->SetCursor(wxCursor(wxCURSOR_PENCIL));
	}
	else
	{
		getDrawingEditor()->view()->SetCursor(wxCursor(wxCURSOR_CROSS));
	}

	if(handle && numClicks>0)
	{
		handle->invokeStep(event,getDrawingEditor()->view());
	}
}
