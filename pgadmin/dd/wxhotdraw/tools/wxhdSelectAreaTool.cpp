//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdSelectAreaTool.cpp - Tool to allow selection of figures inside a rectangle
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/pen.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdSelectAreaTool.h"
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"


wxhdSelectAreaTool::wxhdSelectAreaTool(wxhdDrawingView *view)
	: wxhdAbstractTool(view)
{
}

wxhdSelectAreaTool::~wxhdSelectAreaTool()
{
}

void wxhdSelectAreaTool::mouseDown(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseDown(event);
	if(!event.ShiftDown())
	{
		event.getView()->getDrawing()->clearSelection();
	}
	if(event.LeftDown())
	{
		int x = event.GetPosition().x, y = event.GetPosition().y;
		selectionRect.x = x;
		selectionRect.y = y;
		selectionRect.width = 0;
		selectionRect.height = 0;
		drawSelectionRect(event.getView());
	}
}

void wxhdSelectAreaTool::mouseUp(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseUp(event);
	wxhdGeometry g;
	//hack-fix for bug when selecting figures from right to left
	if(event.LeftUp())
	{
		if( selectionRect.width < 0 )
		{
			int tmp;
			tmp = selectionRect.width;
			selectionRect.x += tmp;
			selectionRect.width = g.ddabs(tmp);

		}
		if( selectionRect.height < 0 )
		{
			int tmp;
			tmp = selectionRect.height;
			selectionRect.y += tmp;
			selectionRect.height = g.ddabs(tmp);
		}
		//end hack-fix
		drawSelectionRect(event.getView());
		selectFiguresOnRect(event.ShiftDown(), event.getView());
		event.getView()->disableSelRectDraw();
	}
}

void wxhdSelectAreaTool::mouseDrag(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseDrag(event);
	if(event.LeftIsDown())
	{
		drawSelectionRect(event.getView());
		int x = event.GetPosition().x, y = event.GetPosition().y;
		selectionRect.x = anchorX;
		selectionRect.y = anchorY;
		selectionRect.SetBottomRight(wxPoint(x, y));
		drawSelectionRect(event.getView());
		event.getView()->ScrollToMakeVisible(event.GetPosition());
	}
}

void wxhdSelectAreaTool::selectFiguresOnRect(bool shiftPressed, wxhdDrawingView *view)
{
	wxhdIFigure *figure;
	wxhdIteratorBase *iterator = view->getDrawing()->figuresInverseEnumerator();
	while(iterator->HasNext())
	{
		figure = (wxhdIFigure *)iterator->Next();
		if(selectionRect.Contains(figure->displayBox().getwxhdRect(view->getIdx())))
		{
			if(shiftPressed)
			{
				view->getDrawing()->toggleSelection(figure);
			}
			else
			{
				view->getDrawing()->addToSelection(figure);
			}
		}
	}
	delete iterator;
}

void wxhdSelectAreaTool::drawSelectionRect(wxhdDrawingView *view)
{
	view->setSelRect(selectionRect);
}

