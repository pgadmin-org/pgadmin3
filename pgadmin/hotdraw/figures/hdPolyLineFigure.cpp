//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPolyLineFigure.cpp - A simple line figure that can be split on several lines joined by flexibility points
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "hotdraw/figures/hdPolyLineFigure.h"
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/locators/hdILocator.h"
#include "hotdraw/handles/hdPolyLineHandle.h"
#include "hotdraw/figures/hdLineTerminal.h"
#include "hotdraw/locators/hdPolyLineLocator.h"
#include "hotdraw/utilities/hdGeometry.h"
#include "hotdraw/tools/hdPolyLineFigureTool.h"
#include "hotdraw/tools/hdMenuTool.h"

hdPolyLineFigure::hdPolyLineFigure()
{
	unsigned int i;

	for(i = 0; i < MAXPOS ; i++)
	{
		points.Add(new hdArrayCollection());
	}

	startTerminal = NULL;
	endTerminal = NULL;
	handlesChanged = false;
	startPoint = hdPoint(0, 0);
	endPoint = hdPoint(0, 0);
	pointAtPos = hdPoint(0, 0);
	linePen = wxPen(wxString(wxT("BLACK")), 1, wxSOLID);
}

hdPolyLineFigure::~hdPolyLineFigure()
{
	hdPoint *tmp; //Hack: If just delete points collection an error is raised.
	hdArrayCollection *tmpCollection;

	unsigned int i;
	for(i = 0; i < points.Count(); i++)
	{
		while(points[i]->count() > 0)
		{
			tmp = (hdPoint *) points[i]->getItemAt(0);
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

void hdPolyLineFigure::AddPosForNewDiagram()
{
	//Add position for new displaybox at new diagram
	hdIFigure::AddPosForNewDiagram();
	//Add new array of point for polylinefigure
	points.Add(new hdArrayCollection());
}

void hdPolyLineFigure::RemovePosOfDiagram(int posIdx)
{
	hdIFigure::RemovePosOfDiagram(posIdx);

	//Hack: If just delete points collection an error is raised.
	hdPoint *tmp;
	hdArrayCollection *tmpCollection;
	while(points[posIdx]->count() > 0)
	{
		tmp = (hdPoint *) points[posIdx]->getItemAt(0);
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

int hdPolyLineFigure::getMaximunIndex()
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


hdMultiPosRect &hdPolyLineFigure::getBasicDisplayBox()
{
	basicDisplayBox.height = 0;
	basicDisplayBox.width = 0;

	int posIdx;
	//optimize this if needed in a future, because right now calculate displaybox for all posIdx
	hdIteratorBase *iterator;
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
			hdPoint *p = (hdPoint *) iterator->Next();
			hdRect r = hdRect(p->x, p->y, 0, 0);
			basicDisplayBox.add(posIdx, r);
		}

		delete iterator;
	}
	return basicDisplayBox;
}

int hdPolyLineFigure::pointLinesCount()
{
	return points.Count();
}
int hdPolyLineFigure::pointCount(int posIdx)
{
	return points[posIdx]->count();
}

hdPoint &hdPolyLineFigure::getStartPoint(int posIdx)
{
	startPoint.x = ((hdPoint *)points[posIdx]->getItemAt(0))->x;
	startPoint.y = ((hdPoint *)points[posIdx]->getItemAt(0))->y;
	return startPoint;
}

void hdPolyLineFigure::setStartPoint(int posIdx, hdPoint point)
{
	willChange();
	if(points[posIdx]->count() == 0)
		addPoint(posIdx, point.x, point.y);
	else
	{
		hdPoint *p = (hdPoint *) points[posIdx]->getItemAt(0);
		p->x = point.x;
		p->y = point.y;
	}
	changed(posIdx);
}

hdPoint &hdPolyLineFigure::getEndPoint(int posIdx)
{
	endPoint.x = ((hdPoint *)points[posIdx]->getItemAt(points[posIdx]->count() - 1))->x;
	endPoint.y = ((hdPoint *)points[posIdx]->getItemAt(points[posIdx]->count() - 1))->y;
	return endPoint;
}

void hdPolyLineFigure::setEndPoint(int posIdx, hdPoint point)
{
	willChange();
	if(points[posIdx]->count() < 2)
		addPoint(posIdx, point.x, point.y);
	else
	{
		hdPoint *p = (hdPoint *) points[posIdx]->getItemAt(points[posIdx]->count() - 1);
		p->x = point.x;
		p->y = point.y;
	}
	changed(posIdx);
}

void hdPolyLineFigure::setStartTerminal(hdLineTerminal *terminal)
{
	startTerminal = terminal;
}

hdLineTerminal *hdPolyLineFigure::getStartTerminal()
{
	return startTerminal;
}

void hdPolyLineFigure::setEndTerminal(hdLineTerminal *terminal)
{
	endTerminal = terminal;
}

hdLineTerminal *hdPolyLineFigure::getEndTerminal()
{
	return endTerminal;
}

hdCollection *hdPolyLineFigure::handlesEnumerator()
{
	return handles;
}

void hdPolyLineFigure::addPoint (int posIdx, int x, int y)
{
	willChange();
	points[posIdx]->addItem((hdObject *) new hdPoint(x, y) );

	if( handles->count() < getMaximunIndex() )
	{
		//Update handles
		handles->addItem(new hdPolyLineHandle(this, new hdPolyLineLocator(0), 0));
		updateHandlesIndexes();
	}
	changed(posIdx);
}

void hdPolyLineFigure::changed(int posIdx)
{
	handlesChanged = true;
}

void hdPolyLineFigure::removePointAt (int posIdx, int index)
{
	willChange();
	hdPoint *p = (hdPoint *) points[posIdx]->getItemAt(index);
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

void hdPolyLineFigure::basicDrawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	basicDraw(context, view);
}

void hdPolyLineFigure::basicDraw(wxBufferedDC &context, hdDrawingView *view)
{
	int posIdx = view->getIdx();
	if(points[posIdx]->count() < 2)
	{
		return;
	}
	hdPoint start, end;

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
		hdPoint *p1 = (hdPoint *) points[posIdx]->getItemAt(i);
		hdPoint *p2 = (hdPoint *) points[posIdx]->getItemAt(i + 1);

		hdPoint copyP1 = hdPoint (*p1);
		view->CalcScrolledPosition(copyP1.x, copyP1.y, &copyP1.x, &copyP1.y);
		hdPoint copyP2 = hdPoint (*p2);
		view->CalcScrolledPosition(copyP2.x, copyP2.y, &copyP2.x, &copyP2.y);

		context.DrawLine(copyP1, copyP2);
	}
}

void hdPolyLineFigure::basicMoveBy(int posIdx, int x, int y)
{
	hdPoint *movPoint;
	for(int i = 0 ; i < points[posIdx]->count() ; i++)
	{
		movPoint = (hdPoint *) points[posIdx]->getItemAt(i);
		movPoint->x += x;
		movPoint->y += y;
	}
}

hdITool *hdPolyLineFigure::CreateFigureTool(hdDrawingView *view, hdITool *defaultTool)
{
	return new hdPolyLineFigureTool(view, this, new hdMenuTool(view, this, defaultTool));
}


int hdPolyLineFigure::findSegment (int posIdx, int x, int y)
{
	for(int i = 0 ; i < points[posIdx]->count() - 1 ; i++)
	{
		hdPoint p1 = pointAt(posIdx, i);
		hdPoint p2 = pointAt(posIdx, i + 1);
		hdGeometry g;
		if(g.lineContainsPoint(p1.x, p1.y, p2.x, p2.y, x, y))
		{
			return i + 1;
		}
	}
	return -1;
}

hdPoint &hdPolyLineFigure::pointAt(int posIdx, int index)
{
	//hack to avoid error with bad indexes calls
	if(index < 0)
	{
		pointAtPos.x = 0;
		pointAtPos.y = 0;
	}
	else
	{
		pointAtPos.x = ((hdPoint *)points[posIdx]->getItemAt(index))->x;
		pointAtPos.y = ((hdPoint *)points[posIdx]->getItemAt(index))->y;
	}
	return pointAtPos;
}

bool hdPolyLineFigure::containsPoint (int posIdx, int x, int y)
{
	hdRect rect = this->displayBox().gethdRect(posIdx);
	rect.Inflate(4, 4);
	if(!rect.Contains(x, y))
	{
		return false;
	}

	for(int i = 0 ; i < points[posIdx]->count() - 1 ; i++)
	{
		hdPoint p1 = pointAt(posIdx, i);
		hdPoint p2 = pointAt(posIdx, i + 1);
		hdGeometry g;
		if(g.lineContainsPoint(p1.x, p1.y, p2.x, p2.y, x, y))
		{
			return true;
		}
	}
	return false;
}

void hdPolyLineFigure::clearPoints(int posIdx)
{
	points[posIdx]->deleteAll();
}

void hdPolyLineFigure::insertPointAt (int posIdx, int index, int x, int y)
{
	willChange();
	points[posIdx]->insertAtIndex((hdObject *) new hdPoint(x, y), index);

	if( handles->count() < getMaximunIndex() )
	{
		//Update handles
		handles->insertAtIndex(new hdPolyLineHandle(this, new hdPolyLineLocator(index), index), index);
		updateHandlesIndexes();
	}

	changed(posIdx);
}

void hdPolyLineFigure::setPointAt (int posIdx, int index, int x, int y)
{
	willChange();
	hdPoint *p = (hdPoint *) points[posIdx]->getItemAt(index);
	p->x = x;
	p->y = y;
	changed(posIdx);
}

void hdPolyLineFigure::splitSegment(int posIdx, int x, int y)
{
	int index = findSegment(posIdx, x, y);

	if(index != -1)
	{
		insertPointAt(posIdx, index, x, y);
	}
}

void hdPolyLineFigure::updateHandlesIndexes()
{
	hdPolyLineHandle *h = NULL;

	//Get maximun point position in a collection of points
	int i, maxPosition = getMaximunIndex();

	//Update Handles indexes
	for(i = 0; i < maxPosition; i++)
	{
		h = (hdPolyLineHandle *) handles->getItemAt(i);
		h->setIndex(i);
	}

}

void hdPolyLineFigure::setLinePen(wxPen pen)
{
	linePen = pen;
}

int hdPolyLineFigure::countPointsAt(int posIdx)
{
	return points[posIdx]->count();
}
