//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDragTrackerTool.cpp - A Tool that allow to drag and drop figures at the view
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdDragTrackerTool.h"
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"

wxhdDragTrackerTool::wxhdDragTrackerTool(wxhdDrawingEditor *editor, wxhdIFigure *anchor)
:wxhdAbstractTool(editor)
{
	hasMovedValue=false;
	view = editor->view();
	anchorFigure = anchor;
}

wxhdDragTrackerTool::~wxhdDragTrackerTool()
{
}

void wxhdDragTrackerTool::setLastCoords(int x, int y)
{
	lastX=x;
	lastY=y;
}

void wxhdDragTrackerTool::mouseDown(wxhdMouseEvent& event)
{
	wxhdAbstractTool::mouseDown(event);

	if(event.LeftDown())
	{
		int x=event.GetPosition().x, y=event.GetPosition().y;

		setLastCoords(x,y);

		if(event.m_shiftDown)
        {
				view->toggleSelection(anchorFigure);
		}
		else if(!view->isFigureSelected(anchorFigure))
		{
			view->clearSelection();
			view->addToSelection(anchorFigure);
		}
	}
}

void wxhdDragTrackerTool::mouseUp(wxhdMouseEvent& event)
{
	wxhdAbstractTool::mouseUp(event);
}

void wxhdDragTrackerTool::mouseDrag(wxhdMouseEvent& event)
{
	wxhdAbstractTool::mouseDrag(event);

	if(event.LeftIsDown())
	{
		int x=event.GetPosition().x, y=event.GetPosition().y;	

		//Hack to avoid a weird bug that ocurrs when use double click very fast over figure and drag a same time, lastX 
		//and lastY values becomes big negatives numbers, if this happens, then reset it to click point
		if(lastX<0)
			lastX=x;
		if(lastY<0) 
			lastY=y;

		hasMovedValue = (abs (x - anchorX) > 4 || abs (y - anchorX) > 4);

        if (hasMoved())
        {
            wxhdIFigure *tmp=NULL;
            wxhdIteratorBase *iterator=view->selectionFigures();
            while(iterator->HasNext())
            {
                tmp=(wxhdIFigure *)iterator->Next();
                tmp->moveBy(x - lastX, y - lastY);
            }
            delete iterator;
        }
        setLastCoords (x, y);
	}
}

bool wxhdDragTrackerTool::hasMoved()
{
	return hasMovedValue;
}
