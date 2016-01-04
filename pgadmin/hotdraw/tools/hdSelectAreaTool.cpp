//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdSelectAreaTool.cpp - Tool to allow selection of figures inside a rectangle
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/pen.h>

// App headers
#include "hotdraw/tools/hdSelectAreaTool.h"
#include "hotdraw/tools/hdAbstractTool.h"
#include "hotdraw/utilities/hdGeometry.h"


hdSelectAreaTool::hdSelectAreaTool(hdDrawingView *view)
	: hdAbstractTool(view)
{
}

hdSelectAreaTool::~hdSelectAreaTool()
{
}

void hdSelectAreaTool::mouseDown(hdMouseEvent &event)
{
	hdAbstractTool::mouseDown(event);
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

void hdSelectAreaTool::mouseUp(hdMouseEvent &event)
{
	hdAbstractTool::mouseUp(event);
	hdGeometry g;
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

void hdSelectAreaTool::mouseDrag(hdMouseEvent &event)
{
	hdAbstractTool::mouseDrag(event);
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

void hdSelectAreaTool::selectFiguresOnRect(bool shiftPressed, hdDrawingView *view)
{
	hdIFigure *figure;
	hdIteratorBase *iterator = view->getDrawing()->figuresInverseEnumerator();
	while(iterator->HasNext())
	{
		figure = (hdIFigure *)iterator->Next();
		if(selectionRect.Contains(figure->displayBox().gethdRect(view->getIdx())))
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

void hdSelectAreaTool::drawSelectionRect(hdDrawingView *view)
{
	view->setSelRect(selectionRect);
}

