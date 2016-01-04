//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLineConnection.cpp - Base class for line connection figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/figures/hdLineConnection.h"
#include "hotdraw/handles/hdChangeConnectionStartHandle.h"
#include "hotdraw/handles/hdChangeConnectionEndHandle.h"
#include "hotdraw/handles/hdLineConnectionHandle.h"
#include "hotdraw/locators/hdPolyLineLocator.h"
#include "hotdraw/utilities/hdArrayCollection.h"

hdLineConnection::hdLineConnection():
	hdPolyLineFigure()
{
	startConnector = NULL;
	endConnector = NULL;
	changeConnStartHandle = NULL;
	changeConnEndHandle = NULL;
}

hdLineConnection::hdLineConnection(int posIdx, hdIFigure *figure1, hdIFigure *figure2):
	hdPolyLineFigure()
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

hdLineConnection::~hdLineConnection()
{
}

hdIConnector *hdLineConnection::getStartConnector()
{
	return startConnector;
}

hdIConnector *hdLineConnection::getEndConnector()
{
	return endConnector;
}

void hdLineConnection::setStartConnector(hdIConnector *connector)
{
	startConnector = connector;
}

void hdLineConnection::setEndConnector(hdIConnector *connector)
{
	endConnector = connector;
}

void hdLineConnection::connectStart(hdIConnector *start, hdDrawingView *view)
{
	if(startConnector == start)
	{
		return;
	}

	disconnectStart();
	startConnector = start;
	connectFigure(startConnector);
}

void hdLineConnection::connectEnd(hdIConnector *end, hdDrawingView *view)
{
	if(endConnector == end)
	{
		return;
	}

	disconnectEnd();
	endConnector = end;
	connectFigure(endConnector);
}

void hdLineConnection::disconnectStart(hdDrawingView *view)
{
	disconnectFigure (startConnector);
	startConnector = NULL;
}

void hdLineConnection::disconnectEnd(hdDrawingView *view)
{
	disconnectFigure (endConnector);
	endConnector = NULL;
}

bool hdLineConnection::canConnectStart(hdIFigure *figure)
{
	return true;
}

bool hdLineConnection::canConnectEnd(hdIFigure *figure)
{
	return true;
}

hdIFigure *hdLineConnection::getStartFigure()
{
	if(startConnector)
	{
		return startConnector->getOwner();
	}

	return NULL;
}

hdIFigure *hdLineConnection::getEndFigure()
{
	if(endConnector)
	{
		return endConnector->getOwner();
	}

	return NULL;
}

void hdLineConnection::updateConnection(int posIdx)
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

hdIHandle *hdLineConnection::getStartHandle()
{
	if(!changeConnStartHandle)
	{
		changeConnStartHandle =  new hdChangeConnectionStartHandle(this);
	}
	return changeConnStartHandle;
}

hdIHandle *hdLineConnection::getEndHandle()
{
	if(!changeConnEndHandle)
	{
		changeConnEndHandle =  new hdChangeConnectionEndHandle(this);
	}
	return changeConnEndHandle;
}

void hdLineConnection::basicMoveBy(int posIdx, int x, int y)
{
	hdPolyLineFigure::basicMoveBy(posIdx, x, y);
	updateConnection(posIdx);
}

bool hdLineConnection::canConnect()
{
	return false;
}

void hdLineConnection::setPointAt (int posIdx, int index, int x, int y)
{
	hdPolyLineFigure::setPointAt(posIdx, index, x, y);
	updateConnection(posIdx);
}

hdCollection *hdLineConnection::handlesEnumerator()
{
	return handles;
}


void hdLineConnection::connectFigure (hdIConnector *connector)
{
	if(connector)
	{
		connector->getOwner()->addObserver(this);
	}
}

void hdLineConnection::disconnectFigure (hdIConnector *connector)
{
	if(connector)
	{
		connector->getOwner()->removeObserver(this);
	}
}

void hdLineConnection::onFigureChanged(int posIdx, hdIFigure *figure)
{
	updateConnection(posIdx);
}

void hdLineConnection::addPoint (int posIdx, int x, int y)
{
	willChange();
	points[posIdx]->addItem((hdObject *) new hdPoint(x, y) );
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
			handles->insertAtIndex(new hdPolyLineHandle(this, new hdPolyLineLocator(0), 0), handles->count() - 1);
		}
	}
	updateHandlesIndexes();
	changed(posIdx);
}

void hdLineConnection::insertPointAt (int posIdx, int index, int x, int y)
{
	willChange();
	points[posIdx]->insertAtIndex((hdObject *) new hdPoint(x, y), index);
	//Update handles
	//Is there need of a new handle if is first point
	if(index == 0 && handles->count() == 0 )
	{
		//add a new handle "normal" for a point in next position 0,1 in 1... in 0 startHandle is not moved
		handles->insertAtIndex(new hdPolyLineHandle(this, new hdPolyLineLocator(index), index), 1);
	}
	else if(index == (points[posIdx]->count() - 1) &&  handles->count() < getMaximunIndex() ) //last point
	{
		//add a new handle "normal" for a point in before last item position
		handles->insertAtIndex(new hdPolyLineHandle(this, new hdPolyLineLocator(index), index), (points[posIdx]->count() - 1));
	}
	else if(handles->count() < getMaximunIndex())
	{
		//add handle at index
		handles->insertAtIndex(new hdPolyLineHandle(this, new hdPolyLineLocator(index), index), index);
	}
	updateHandlesIndexes();
	changed(posIdx);
}

//Update points between start and end, because start and end don't have index (is other kind of handle)
void hdLineConnection::updateHandlesIndexes()
{
	hdPolyLineHandle *h = NULL;

	//Get maximun point position in a collection of points
	int maxPosition = getMaximunIndex();

	//Update Handles indexes
	for(int i = 1; i < maxPosition - 1; i++)
	{
		h = (hdPolyLineHandle *) handles->getItemAt(i);
		h->setIndex(i);
	}
}
