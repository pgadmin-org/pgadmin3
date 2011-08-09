//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLineConnection.cpp - Base class for line connection figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdLineConnection.h"
#include "dd/wxhotdraw/handles/wxhdChangeConnectionStartHandle.h"
#include "dd/wxhotdraw/handles/wxhdChangeConnectionEndHandle.h"
#include "dd/wxhotdraw/handles/wxhdLineConnectionHandle.h"
#include "dd/wxhotdraw/locators/wxhdPolyLineLocator.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"

wxhdLineConnection::wxhdLineConnection():
	wxhdPolyLineFigure()
{
	startConnector = NULL;
	endConnector = NULL;
	changeConnStartHandle = NULL;
	changeConnEndHandle = NULL;
}

wxhdLineConnection::wxhdLineConnection(int posIdx, wxhdIFigure *figure1, wxhdIFigure *figure2):
	wxhdPolyLineFigure()
{
	//Check figure available positions for diagrams, add at least needed to allow initialization of the class
	int i, start;
	start = basicDisplayBox.CountPositions();
	for(i = start; i < (posIdx + 1); i++)
	{
		AddPosForNewDiagram();
	}

	startConnector = NULL;
	endConnector = NULL;

	if(figure1)
	{
		connectStart(figure1->connectorAt(posIdx, 0, 0));
	}

	if(figure2)
	{
		connectEnd(figure2->connectorAt(posIdx, 0, 0));
	}
}

wxhdLineConnection::~wxhdLineConnection()
{
}

wxhdIConnector *wxhdLineConnection::getStartConnector()
{
	return startConnector;
}

wxhdIConnector *wxhdLineConnection::getEndConnector()
{
	return endConnector;
}

void wxhdLineConnection::setStartConnector(wxhdIConnector *connector)
{
	startConnector = connector;
}

void wxhdLineConnection::setEndConnector(wxhdIConnector *connector)
{
	endConnector = connector;
}

void wxhdLineConnection::connectStart(wxhdIConnector *start, wxhdDrawingView *view)
{
	if(startConnector == start)
	{
		return;
	}

	disconnectStart();
	startConnector = start;
	connectFigure(startConnector);
}

void wxhdLineConnection::connectEnd(wxhdIConnector *end, wxhdDrawingView *view)
{
	if(endConnector == end)
	{
		return;
	}

	disconnectEnd();
	endConnector = end;
	connectFigure(endConnector);
}

void wxhdLineConnection::disconnectStart(wxhdDrawingView *view)
{
	disconnectFigure (startConnector);
	startConnector = NULL;
}

void wxhdLineConnection::disconnectEnd(wxhdDrawingView *view)
{
	disconnectFigure (endConnector);
	endConnector = NULL;
}

bool wxhdLineConnection::canConnectStart(wxhdIFigure *figure)
{
	return true;
}

bool wxhdLineConnection::canConnectEnd(wxhdIFigure *figure)
{
	return true;
}

wxhdIFigure *wxhdLineConnection::getStartFigure()
{
	if(startConnector)
	{
		return startConnector->getOwner();
	}

	return NULL;
}

wxhdIFigure *wxhdLineConnection::getEndFigure()
{
	if(endConnector)
	{
		return endConnector->getOwner();
	}

	return NULL;
}

void wxhdLineConnection::updateConnection(int posIdx)
{
	if(startConnector)
	{
		setStartPoint(posIdx, startConnector->findStart(posIdx, this));
	}
	if(endConnector)
	{
		setEndPoint(posIdx, endConnector->findEnd(posIdx, this));
	}
}

wxhdIHandle *wxhdLineConnection::getStartHandle()
{
	if(!changeConnStartHandle)
	{
		changeConnStartHandle =  new wxhdChangeConnectionStartHandle(this);
	}
	return changeConnStartHandle;
}

wxhdIHandle *wxhdLineConnection::getEndHandle()
{
	if(!changeConnEndHandle)
	{
		changeConnEndHandle =  new wxhdChangeConnectionEndHandle(this);
	}
	return changeConnEndHandle;
}

void wxhdLineConnection::basicMoveBy(int posIdx, int x, int y)
{
	wxhdPolyLineFigure::basicMoveBy(posIdx, x, y);
	updateConnection(posIdx);
}

bool wxhdLineConnection::canConnect()
{
	return false;
}

void wxhdLineConnection::setPointAt (int posIdx, int index, int x, int y)
{
	wxhdPolyLineFigure::setPointAt(posIdx, index, x, y);
	updateConnection(posIdx);
}

wxhdCollection *wxhdLineConnection::handlesEnumerator()
{
	return handles;
}


void wxhdLineConnection::connectFigure (wxhdIConnector *connector)
{
	if(connector)
	{
		connector->getOwner()->addObserver(this);
	}
}

void wxhdLineConnection::disconnectFigure (wxhdIConnector *connector)
{
	if(connector)
	{
		connector->getOwner()->removeObserver(this);
	}
}

void wxhdLineConnection::onFigureChanged(int posIdx, wxhdIFigure *figure)
{
	updateConnection(posIdx);
}

void wxhdLineConnection::addPoint (int posIdx, int x, int y)
{
	willChange();
	points[posIdx]->addItem((wxhdObject *) new wxhdPoint(x, y) );
	//Update handles
	if(points[posIdx]->count() == 1)
	{
		//first point add start handle
		if(handles->count() == 0)
			handles->addItem(getStartHandle());
	}
	else if(points[posIdx]->count() == 2)
	{
		//second point add end handle
		if(handles->count() == 1)
			handles->addItem(getEndHandle());
	}
	else if(points[posIdx]->count() > 2)
	{
		//Locate maximum index if there is need for one new handle then added it
		if( getMaximunIndex() > handles->count() )
		{
			//third and above point, add a polylinehandle before end handle
			handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(0), 0), handles->count() - 1);
		}
	}
	updateHandlesIndexes();
	changed(posIdx);
}

void wxhdLineConnection::insertPointAt (int posIdx, int index, int x, int y)
{
	willChange();
	points[posIdx]->insertAtIndex((wxhdObject *) new wxhdPoint(x, y), index);
	//Update handles
	//Is there need of a new handle if is first point
	if(index == 0 && handles->count() == 0 )
	{
		//add a new handle "normal" for a point in next position 0,1 in 1... in 0 startHandle is not moved
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), 1);
	}
	else if(index == (points[posIdx]->count() - 1) &&  handles->count() < getMaximunIndex() ) //last point
	{
		//add a new handle "normal" for a point in before last item position
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), (points[posIdx]->count() - 1));
	}
	else if(handles->count() < getMaximunIndex())
	{
		//add handle at index
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), index);
	}
	updateHandlesIndexes();
	changed(posIdx);
}

//Update points between start and end, because start and end don't have index (is other kind of handle)
void wxhdLineConnection::updateHandlesIndexes()
{
	wxhdPolyLineHandle *h = NULL;

	//Get maximun point position in a collection of points
	int maxPosition = getMaximunIndex();

	//Update Handles indexes
	for(int i = 1; i < maxPosition - 1; i++)
	{
		h = (wxhdPolyLineHandle *) handles->getItemAt(i);
		h->setIndex(i);
	}
}
