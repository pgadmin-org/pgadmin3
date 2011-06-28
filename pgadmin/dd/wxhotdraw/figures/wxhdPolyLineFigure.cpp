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
	points = new wxhdArrayCollection();
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
	while(points->count() > 0)
	{
		tmp = (wxhdPoint *) points->getItemAt(0);
		points->removeItemAt(0);
		delete tmp;
	}
	if(points)
		delete points;
	if(startTerminal)
		delete startTerminal;
	if(endTerminal)
		delete endTerminal;
}

wxhdRect &wxhdPolyLineFigure::getBasicDisplayBox()
{
	basicDisplayBox.height = 0;
	basicDisplayBox.width = 0;
	if(points->count() < 2)
	{

		return basicDisplayBox;
	}
	if(points->count() >= 1)
	{
		basicDisplayBox.SetPosition(pointAt(0));
	}
	else
	{
		basicDisplayBox.SetPosition(wxPoint(0, 0));
	}

	wxhdIteratorBase *iterator = points->createIterator();
	while(iterator->HasNext())
	{
		wxhdPoint *p = (wxhdPoint *) iterator->Next();
		wxhdRect r = wxhdRect(p->x, p->y, 0, 0);
		basicDisplayBox.add(r);
	}

	delete iterator;

	return basicDisplayBox;
}

int wxhdPolyLineFigure::pointCount()
{
	return points->count();
}

wxhdPoint &wxhdPolyLineFigure::getStartPoint()
{
	startPoint.x = ((wxhdPoint *)points->getItemAt(0))->x;
	startPoint.y = ((wxhdPoint *)points->getItemAt(0))->y;
	return startPoint;
}

void wxhdPolyLineFigure::setStartPoint(wxhdPoint point)
{
	willChange();
	if(points->count() == 0)
		addPoint(point.x, point.y);
	else
	{
		wxhdPoint *p = (wxhdPoint *) points->getItemAt(0);
		p->x = point.x;
		p->y = point.y;
	}

	changed();
}

wxhdPoint &wxhdPolyLineFigure::getEndPoint()
{
	endPoint.x = ((wxhdPoint *)points->getItemAt(points->count() - 1))->x;
	endPoint.y = ((wxhdPoint *)points->getItemAt(points->count() - 1))->y;
	return endPoint;
}

void wxhdPolyLineFigure::setEndPoint(wxhdPoint point)
{
	willChange();
	if(points->count() < 2)
		addPoint(point.x, point.y);
	else
	{
		wxhdPoint *p = (wxhdPoint *) points->getItemAt(points->count() - 1);
		p->x = point.x;
		p->y = point.y;
	}
	changed();
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

void wxhdPolyLineFigure::addPoint (int x, int y)
{
	willChange();
	points->addItem((wxhdObject *) new wxhdPoint(x, y) );
	//Update handles
	handles->addItem(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(0), 0));
	updateHandlesIndexes();
	changed();
}

void wxhdPolyLineFigure::changed()
{
	handlesChanged = true;
}

void wxhdPolyLineFigure::removePointAt (int index)
{
	willChange();
	wxhdPoint *p = (wxhdPoint *) points->getItemAt(index);
	points->removeItemAt(index);
	delete p;
	//Update handles
	handles->removeItemAt(index);
	updateHandlesIndexes();
	changed();
}

void wxhdPolyLineFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	basicDraw(context, view);
}

void wxhdPolyLineFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	if(points->count() < 2)
	{
		return;
	}
	wxhdPoint start, end;

	if(startTerminal)
	{
		startTerminal->setLinePen(linePen);
		start = startTerminal->draw(context, getStartPoint(), pointAt(1), view);
	}
	else
	{
		start = getStartPoint();
	}

	if(endTerminal)
	{
		endTerminal->setLinePen(linePen);
		end = endTerminal->draw(context, getEndPoint(), pointAt(pointCount() - 2), view);
	}
	else
	{
		end = getEndPoint();
	}

	context.SetPen(linePen);
	for(int i = 0; i < points->count() - 1; i++)
	{
		wxhdPoint *p1 = (wxhdPoint *) points->getItemAt(i);
		wxhdPoint *p2 = (wxhdPoint *) points->getItemAt(i + 1);

		wxhdPoint copyP1 = wxhdPoint (*p1);
		view->CalcScrolledPosition(copyP1.x, copyP1.y, &copyP1.x, &copyP1.y);
		wxhdPoint copyP2 = wxhdPoint (*p2);
		view->CalcScrolledPosition(copyP2.x, copyP2.y, &copyP2.x, &copyP2.y);

		context.DrawLine(copyP1, copyP2);
	}
}

void wxhdPolyLineFigure::basicMoveBy(int x, int y)
{
	wxhdPoint *movPoint;
	for(int i = 0 ; i < points->count() ; i++)
	{
		movPoint = (wxhdPoint *) points->getItemAt(i);
		movPoint->x += x;
		movPoint->y += y;
	}
}

wxhdITool *wxhdPolyLineFigure::CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool)
{
	return new wxhdPolyLineFigureTool(editor, this, new wxhdMenuTool(editor, this, defaultTool));
}


int wxhdPolyLineFigure::findSegment (int x, int y)
{
	for(int i = 0 ; i < points->count() - 1 ; i++)
	{
		wxhdPoint p1 = pointAt(i);
		wxhdPoint p2 = pointAt(i + 1);
		wxhdGeometry g;
		if(g.lineContainsPoint(p1.x, p1.y, p2.x, p2.y, x, y))
		{
			return i + 1;
		}
	}
	return -1;
}

wxhdPoint &wxhdPolyLineFigure::pointAt(int index)
{
	//hack to avoid error with bad indexes calls
	if(index < 0)
	{
		pointAtPos.x = 0;
		pointAtPos.y = 0;
	}
	else
	{
		pointAtPos.x = ((wxhdPoint *)points->getItemAt(index))->x;
		pointAtPos.y = ((wxhdPoint *)points->getItemAt(index))->y;
	}
	return pointAtPos;
}

bool wxhdPolyLineFigure::containsPoint (int x, int y)
{
	wxhdRect rect = wxhdRect(this->displayBox());
	rect.Inflate(4, 4);
	if(!rect.Contains(x, y))
	{
		return false;
	}

	for(int i = 0 ; i < points->count() - 1 ; i++)
	{
		wxhdPoint p1 = pointAt(i);
		wxhdPoint p2 = pointAt(i + 1);
		wxhdGeometry g;
		if(g.lineContainsPoint(p1.x, p1.y, p2.x, p2.y, x, y))
		{
			return true;
		}
	}
	return false;
}

void wxhdPolyLineFigure::clearPoints()
{
	points->deleteAll();
}

void wxhdPolyLineFigure::insertPointAt (int index, int x, int y)
{
	willChange();
	points->insertAtIndex((wxhdObject *) new wxhdPoint(x, y), index);
	//Update handles
	handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), index);
	updateHandlesIndexes();
	changed();
}

void wxhdPolyLineFigure::setPointAt (int index, int x, int y)
{
	willChange();
	wxhdPoint *p = (wxhdPoint *) points->getItemAt(index);
	p->x = x;
	p->y = y;
	changed();
}

void wxhdPolyLineFigure::splitSegment(int x, int y)
{
	int index = findSegment(x, y);

	if(index != -1)
	{
		insertPointAt(index, x, y);
	}
}

void wxhdPolyLineFigure::updateHandlesIndexes()
{
	wxhdPolyLineHandle *h = NULL;
	for(int i = 0; i < points->count(); i++)
	{
		h = (wxhdPolyLineHandle *) handles->getItemAt(i);
		h->setIndex(i);
	}
}

void wxhdPolyLineFigure::setLinePen(wxPen pen)
{
	linePen = pen;
}
