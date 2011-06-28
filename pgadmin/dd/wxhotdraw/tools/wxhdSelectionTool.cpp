//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdSelectionTool.cpp - Tool to allow selection of figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdSelectionTool.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/tools/wxhdHandleTrackerTool.h"
#include "dd/wxhotdraw/tools/wxhdDragTrackerTool.h"
#include "dd/wxhotdraw/tools/wxhdSelectAreaTool.h"
#include "dd/wxhotdraw/tools/wxhdCanvasMenuTool.h"


class wxhdDrawingView;
class wxhdDrawingEditor;

wxhdSelectionTool::wxhdSelectionTool(wxhdDrawingEditor *owner):
	wxhdAbstractTool(owner)
{
	_delegateTool = NULL;
}

wxhdSelectionTool::~wxhdSelectionTool()
{
	if(_delegateTool)
		delete _delegateTool;
}

void wxhdSelectionTool::mouseDown(wxhdMouseEvent &event)
{
	wxhdITool::mouseDown(event);

	wxhdDrawingView *view = getDrawingEditor()->view();
	int x = event.GetPosition().x, y = event.GetPosition().y;

	wxhdIHandle *handle = view->findHandle(x, y);
	if(handle)
	{
		setDelegateTool(new wxhdHandleTrackerTool(getDrawingEditor(), handle));
	}
	else
	{
		wxhdIFigure *figure = view->getDrawing()->findFigure(x, y);
		if(figure)
		{
			view->getDrawing()->bringToFront(figure);
			setDelegateTool(figure->CreateFigureTool(getDrawingEditor(), new wxhdDragTrackerTool(getDrawingEditor(), figure)));
		}
		else
		{
			setDelegateTool( new wxhdCanvasMenuTool(getDrawingEditor(), new wxhdSelectAreaTool(getDrawingEditor())) );
		}
	}

	wxhdITool *delegateTool = getDelegateTool();
	if (delegateTool)
		delegateTool->mouseDown(event);
}

void wxhdSelectionTool::mouseUp(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseUp(event);
	wxhdITool *delegateTool = getDelegateTool();
	if (delegateTool)
		delegateTool->mouseUp(event);
}

void wxhdSelectionTool::mouseMove(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseMove(event);
	wxhdDrawingView *view = getDrawingEditor()->view();
	int x = event.GetPosition().x, y = event.GetPosition().y;
	wxhdIHandle *handle = view->findHandle(x, y);

	if(handle)
	{
		view->SetCursor(handle->createCursor());
	}
	else
	{
		wxhdIFigure *figure = view->getDrawing()->findFigure(x, y);
		if(figure)
		{
			view->SetCursor(wxCursor(wxCURSOR_HAND));
		}
		else
		{
			view->SetCursor(wxCursor(wxCURSOR_ARROW));
		}
	}
}

void wxhdSelectionTool::mouseDrag(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseDrag(event);
	wxhdITool *delegateTool = getDelegateTool();
	if (delegateTool)
		delegateTool->mouseDrag(event);
}

void wxhdSelectionTool::keyDown(wxhdKeyEvent &event)
{
	if(getDelegateTool())
	{
		getDelegateTool()->keyDown(event);
	}
	if(event.GetKeyCode() == WXK_DELETE)
	{
		event.getView()->deleteSelectedFigures();
	}
}

void wxhdSelectionTool::keyUp(wxhdKeyEvent &event)
{
	if(getDelegateTool())
	{
		getDelegateTool()->keyUp(event);
	}
}

void wxhdSelectionTool::setDelegateTool(wxhdITool *tool)
{
	if(_delegateTool)
	{
		_delegateTool->deactivate();
		delete _delegateTool;
	}

	_delegateTool = tool;

	if(_delegateTool)
	{
		_delegateTool->activate();
	}
}

wxhdITool *wxhdSelectionTool::getDelegateTool()
{
	return _delegateTool;
}

void wxhdSelectionTool::deleteAllFigures(wxhdDrawingView *view)
{
	view->clearSelection();
	view->getDrawing()->deleteAllFigures();
}
