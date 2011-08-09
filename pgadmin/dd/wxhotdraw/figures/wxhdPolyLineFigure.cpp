//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineFigure.cpp - A simple line figure that can be split on several lines joined by flexibility points
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdPolyLineFigure.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/locators/wxhdILocator.h"
#include "dd/wxhotdraw/handles/wxhdPolyLineHandle.h"
#include "dd/wxhotdraw/figures/wxhdLineTerminal.h"
#include "dd/wxhotdraw/locators/wxhdPolyLineLocator.h"
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"
#include "dd/wxhotdraw/tools/wxhdPolyLineFigureTool.h"
#include "dd/wxhotdraw/tools/wxhdMenuTool.h"

wxhdPolyLineFigure::wxhdPolyLineFigure()
{
	unsigned int i;

	for(i = 0; i < MAXPOS ; i++)
	{
		points.Add(new wxhdArrayCollection());
	}

	startTerminal = NULL;
	endTerminal = NULL;
	handlesChanged = false;
	startPoint = wxhdPoint(0, 0);
	endPoint = wxhdPoint(0, 0);
	pointAtPos = wxhdPoint(0, 0);
	linePen = wxPen(wxString(wxT("BLACK")), 1, wxSOLID);
}

wxhdPolyLineFigure::~wxhdPolyLineFigure()
{
	wxhdPoint *tmp; //Hack: If just delete points collection an error is raised.
	wxhdArrayCollection *tmpCollection;

	unsigned int i;
	for(i = 0; i < points.Count(); i++)
	{
		while(points[i]->count() > 0)
		{
			tmp = (wxhdPoint *) points[i]->getItemAt(0);
			points[i]->removeItemAt(0);
			delete tmp;
		}
		if(points[i])
		{
			tmpCollection = points[i];
			points.RemoveAt(i);
			delete tmpCollection;
		}
	}
	if(startTerminal)
		delete startTerminal;
	if(endTerminal)
		delete endTerminal;
}

void wxhdPolyLineFigure::AddPosForNewDiagram()
{
	//Add position for new displaybox at new diagram
	wxhdIFigure::AddPosForNewDiagram();
	//Add new array of point for polylinefigure
	points.Add(new wxhdArrayCollection());
}

void wxhdPolyLineFigure::RemovePosOfDiagram(int posIdx)
{
	wxhdIFigure::RemovePosOfDiagram(posIdx);

	//Hack: If just delete points collection an error is raised.
	wxhdPoint *tmp;
	wxhdArrayCollection *tmpCollection;
	while(points[posIdx]->count() > 0)
	{
		tmp = (wxhdPoint *) points[posIdx]->getItemAt(0);
		points[posIdx]->removeItemAt(0);
		delete tmp;
	}

	if(points[posIdx])
	{
		tmpCollection = points[posIdx];
		points.RemoveAt(posIdx);
		delete tmpCollection;
	}
}

int wxhdPolyLineFigure::getMaximunIndex()
{
	unsigned int i;
	int max = points[0]->count();

	for(i = 1; i < points.Count(); i++)
	{
		if(points[i]->count() > max)
		{
			max = points[i]->count();
		}

	}
	return max;
}


wxhdMultiPosRect &wxhdPolyLineFigure::getBasicDisplayBox()
{
	basicDisplayBox.height = 0;
	basicDisplayBox.width = 0;

	int posIdx;
	//optimize this if needed in a future, because right now calculate displaybox for all posIdx
	wxhdIteratorBase *iterator;
	for(posIdx = 0; posIdx < basicDisplayBox.CountPositions(); posIdx++)
	{
		if(points[posIdx]->count() >= 1)
		{
			basicDisplayBox.SetPosition(posIdx, pointAt(posIdx, 0));
		}
		else
		{
			basicDisplayBox.SetPosition(posIdx, wxPoint(0, 0));
		}

		iterator = points[posIdx]->createIterator();
		while(iterator->HasNext())
		{
			wxhdPoint *p = (wxhdPoint *) iterator->Next();
			wxhdRect r = wxhdRect(p->x, p->y, 0, 0);
			basicDisplayBox.add(posIdx, r);
		}

		delete iterator;
	}
	return basicDisplayBox;
}

int wxhdPolyLineFigure::pointLinesCount()
{
	return points.Count();
}
int wxhdPolyLineFigure::pointCount(int posIdx)
{
	return points[posIdx]->count();
}

wxhdPoint &wxhdPolyLineFigure::getStartPoint(int posIdx)
{
	startPoint.x = ((wxhdPoint *)points[posIdx]->getItemAt(0))->x;
	startPoint.y = ((wxhdPoint *)points[posIdx]->getItemAt(0))->y;
	return startPoint;
}

void wxhdPolyLineFigure::setStartPoint(int posIdx, wxhdPoint point)
{
	willChange();
	if(points[posIdx]->count() == 0)
		addPoint(posIdx, point.x, point.y);
	else
	{
		wxhdPoint *p = (wxhdPoint *) points[posIdx]->getItemAt(0);
		p->x = point.x;
		p->y = point.y;
	}
	changed(posIdx);
}

wxhdPoint &wxhdPolyLineFigure::getEndPoint(int posIdx)
{
	endPoint.x = ((wxhdPoint *)points[posIdx]->getItemAt(points[posIdx]->count() - 1))->x;
	endPoint.y = ((wxhdPoint *)points[posIdx]->getItemAt(points[posIdx]->count() - 1))->y;
	return endPoint;
}

void wxhdPolyLineFigure::setEndPoint(int posIdx, wxhdPoint point)
{
	willChange();
	if(points[posIdx]->count() < 2)
		addPoint(posIdx, point.x, point.y);
	else
	{
		wxhdPoint *p = (wxhdPoint *) points[posIdx]->getItemAt(points[posIdx]->count() - 1);
		p->x = point.x;
		p->y = point.y;
	}
	changed(posIdx);
}

void wxhdPolyLineFigure::setStartTerminal(wxhdLineTerminal *terminal)
{
	startTerminal = terminal;
}

wxhdLineTerminal *wxhdPolyLineFigure::getStartTerminal()
{
	return startTerminal;
}

void wxhdPolyLineFigure::setEndTerminal(wxhdLineTerminal *terminal)
{
	endTerminal = terminal;
}

wxhdLineTerminal *wxhdPolyLineFigure::getEndTerminal()
{
	return endTerminal;
}

wxhdCollection *wxhdPolyLineFigure::handlesEnumerator()
{
	return handles;
}

void wxhdPolyLineFigure::addPoint (int posIdx, int x, int y)
{
	willChange();
	points[posIdx]->addItem((wxhdObject *) new wxhdPoint(x, y) );

	if( handles->count() < getMaximunIndex() )
	{
		//Update handles
		handles->addItem(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(0), 0));
		updateHandlesIndexes();
	}
	changed(posIdx);
}

void wxhdPolyLineFigure::changed(int posIdx)
{
	handlesChanged = true;
}

void wxhdPolyLineFigure::removePointAt (int posIdx, int index)
{
	willChange();
	wxhdPoint *p = (wxhdPoint *) points[posIdx]->getItemAt(index);
	points[posIdx]->removeItemAt(index);
	delete p;
	//Update handles [If there are more handles than maximum points of a line in a view]
	if( handles->count() > getMaximunIndex() )
	{
		handles->removeItemAt(index);
		updateHandlesIndexes();
	}
	changed(posIdx);
}

void wxhdPolyLineFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	basicDraw(context, view);
}

void wxhdPolyLineFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	int posIdx = view->getIdx();
	if(points[posIdx]->count() < 2)
	{
		return;
	}
	wxhdPoint start, end;

	if(startTerminal)
	{
		startTerminal->setLinePen(linePen);
		start = startTerminal->draw(context, getStartPoint(posIdx), pointAt(posIdx, 1), view);
	}
	else
	{
		start = getStartPoint(posIdx);
	}

	if(endTerminal)
	{
		endTerminal->setLinePen(linePen);
		end = endTerminal->draw(context, getEndPoint(posIdx), pointAt(posIdx, pointCount(posIdx) - 2), view);
	}
	else
	{
		end = getEndPoint(posIdx);
	}

	context.SetPen(linePen);
	for(int i = 0; i < points[posIdx]->count() - 1; i++)
	{
		wxhdPoint *p1 = (wxhdPoint *) points[posIdx]->getItemAt(i);
		wxhdPoint *p2 = (wxhdPoint *) points[posIdx]->getItemAt(i + 1);

		wxhdPoint copyP1 = wxhdPoint (*p1);
		view->CalcScrolledPosition(copyP1.x, copyP1.y, &copyP1.x, &copyP1.y);
		wxhdPoint copyP2 = wxhdPoint (*p2);
		view->CalcScrolledPosition(copyP2.x, copyP2.y, &copyP2.x, &copyP2.y);

		context.DrawLine(copyP1, copyP2);
	}
}

void wxhdPolyLineFigure::basicMoveBy(int posIdx, int x, int y)
{
	wxhdPoint *movPoint;
	for(int i = 0 ; i < points[posIdx]->count() ; i++)
	{
		movPoint = (wxhdPoint *) points[posIdx]->getItemAt(i);
		movPoint->x += x;
		movPoint->y += y;
	}
}

wxhdITool *wxhdPolyLineFigure::CreateFigureTool(wxhdDrawingView *view, wxhdITool *defaultTool)
{
	return new wxhdPolyLineFigureTool(view, this, new wxhdMenuTool(view, this, defaultTool));
}


int wxhdPolyLineFigure::findSegment (int posIdx, int x, int y)
{
	for(int i = 0 ; i < points[posIdx]->count() - 1 ; i++)
	{
		wxhdPoint p1 = pointAt(posIdx, i);
		wxhdPoint p2 = pointAt(posIdx, i + 1);
		wxhdGeometry g;
		if(g.lineContainsPoint(p1.x, p1.y, p2.x, p2.y, x, y))
		{
			return i + 1;
		}
	}
	return -1;
}

wxhdPoint &wxhdPolyLineFigure::pointAt(int posIdx, int index)
{
	//hack to avoid error with bad indexes calls
	if(index < 0)
	{
		pointAtPos.x = 0;
		pointAtPos.y = 0;
	}
	else
	{
		pointAtPos.x = ((wxhdPoint *)points[posIdx]->getItemAt(index))->x;
		pointAtPos.y = ((wxhdPoint *)points[posIdx]->getItemAt(index))->y;
	}
	return pointAtPos;
}

bool wxhdPolyLineFigure::containsPoint (int posIdx, int x, int y)
{
	wxhdRect rect = this->displayBox().getwxhdRect(posIdx);
	rect.Inflate(4, 4);
	if(!rect.Contains(x, y))
	{
		return false;
	}

	for(int i = 0 ; i < points[posIdx]->count() - 1 ; i++)
	{
		wxhdPoint p1 = pointAt(posIdx, i);
		wxhdPoint p2 = pointAt(posIdx, i + 1);
		wxhdGeometry g;
		if(g.lineContainsPoint(p1.x, p1.y, p2.x, p2.y, x, y))
		{
			return true;
		}
	}
	return false;
}

void wxhdPolyLineFigure::clearPoints(int posIdx)
{
	points[posIdx]->deleteAll();
}

void wxhdPolyLineFigure::insertPointAt (int posIdx, int index, int x, int y)
{
	willChange();
	points[posIdx]->insertAtIndex((wxhdObject *) new wxhdPoint(x, y), index);

	if( handles->count() < getMaximunIndex() )
	{
		//Update handles
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), index);
		updateHandlesIndexes();
	}

	changed(posIdx);
}

void wxhdPolyLineFigure::setPointAt (int posIdx, int index, int x, int y)
{
	willChange();
	wxhdPoint *p = (wxhdPoint *) points[posIdx]->getItemAt(index);
	p->x = x;
	p->y = y;
	changed(posIdx);
}

void wxhdPolyLineFigure::splitSegment(int posIdx, int x, int y)
{
	int index = findSegment(posIdx, x, y);

	if(index != -1)
	{
		insertPointAt(posIdx, index, x, y);
	}
}

void wxhdPolyLineFigure::updateHandlesIndexes()
{
	wxhdPolyLineHandle *h = NULL;

	//Get maximun point position in a collection of points
	int i, maxPosition = getMaximunIndex();

	//Update Handles indexes
	for(i = 0; i < maxPosition; i++)
	{
		h = (wxhdPolyLineHandle *) handles->getItemAt(i);
		h->setIndex(i);
	}

}

void wxhdPolyLineFigure::setLinePen(wxPen pen)
{
	linePen = pen;
}

int wxhdPolyLineFigure::countPointsAt(int posIdx)
{
	return points[posIdx]->count();
}