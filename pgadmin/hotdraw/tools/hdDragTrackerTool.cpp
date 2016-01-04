//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDragTrackerTool.cpp - A Tool that allow to drag and drop figures at the view
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdDragTrackerTool.h"
#include "hotdraw/tools/hdAbstractTool.h"

hdDragTrackerTool::hdDragTrackerTool(hdDrawingView *view, hdIFigure *anchor)
	: hdAbstractTool(view)
{
	hasMovedValue = false;
	anchorFigure = anchor;
}

hdDragTrackerTool::~hdDragTrackerTool()
{
}

void hdDragTrackerTool::setLastCoords(int x, int y)
{
	lastX = x;
	lastY = y;
}

void hdDragTrackerTool::mouseDown(hdMouseEvent &event)
{
	hdAbstractTool::mouseDown(event);

	if(event.LeftDown())
	{
		int x = event.GetPosition().x, y = event.GetPosition().y;

		setLastCoords(x, y);

		if(event.m_shiftDown)
		{
			event.getView()->getDrawing()->toggleSelection(anchorFigure);
		}
		else if(!event.getView()->getDrawing()->isFigureSelected(anchorFigure))
		{
			event.getView()->getDrawing()->clearSelection();
			event.getView()->getDrawing()->addToSelection(anchorFigure);
		}
	}
}

void hdDragTrackerTool::mouseUp(hdMouseEvent &event)
{
	hdAbstractTool::mouseUp(event);
}

void hdDragTrackerTool::mouseDrag(hdMouseEvent &event)
{
	hdAbstractTool::mouseDrag(event);

	if(event.LeftIsDown())
	{
		int x = event.GetPosition().x, y = event.GetPosition().y;

		//Hack to avoid a weird bug that ocurrs when use double click very fast over figure and drag a same time, lastX
		//and lastY values becomes big negatives numbers, if this happens, then reset it to click point
		if(lastX < 0)
			lastX = x;
		if(lastY < 0)
			lastY = y;

		hasMovedValue = (abs (x - anchorX) > 4 || abs (y - anchorX) > 4);

		if (hasMoved())
		{
			hdIFigure *tmp = NULL;
			hdIteratorBase *iterator = event.getView()->getDrawing()->selectionFigures();
			while(iterator->HasNext())
			{
				tmp = (hdIFigure *)iterator->Next();
				tmp->moveBy(event.getView()->getIdx(), x - lastX, y - lastY);
				event.getView()->notifyChanged();
			}
			delete iterator;
		}
		setLastCoords (x, y);
	}
}

bool hdDragTrackerTool::hasMoved()
{
	return hasMovedValue;
}
