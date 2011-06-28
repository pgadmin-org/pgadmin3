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

wxhdLineConnection::wxhdLineConnection(wxhdIFigure *figure1, wxhdIFigure *figure2):
	wxhdPolyLineFigure()
{
	startConnector = NULL;
	endConnector = NULL;

	if(figure1)
	{
		connectStart(figure1->connectorAt(0, 0));
	}

	if(figure2)
	{
		connectEnd(figure2->connectorAt(0, 0));
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

void wxhdLineConnection::updateConnection()
{
	if(startConnector)
	{
		setStartPoint(startConnector->findStart(this));
	}
	if(endConnector)
	{
		setEndPoint(endConnector->findEnd(this));
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

void wxhdLineConnection::basicMoveBy(int x, int y)
{
	wxhdPolyLineFigure::basicMoveBy(x, y);
	updateConnection();
}

bool wxhdLineConnection::canConnect()
{
	return false;
}

void wxhdLineConnection::setPointAt (int index, int x, int y)
{
	wxhdPolyLineFigure::setPointAt(index, x, y);
	updateConnection();
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

void wxhdLineConnection::onFigureChanged(wxhdIFigure *figure)
{
	updateConnection();
}

void wxhdLineConnection::addPoint (int x, int y)
{
	willChange();
	points->addItem((wxhdObject *) new wxhdPoint(x, y) );
	//Update handles
	if(points->count() == 1)
	{
		//first point add start handle
		handles->addItem(getStartHandle());
	}
	else if(points->count() == 2)
	{
		//second point add end handle
		handles->addItem(getEndHandle());
	}
	else if(points->count() > 2)
	{
		//third and above point, add a polylinehandle before end handle
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(0), 0), handles->count() - 1);
	}
	updateHandlesIndexes();
	changed();
}

void wxhdLineConnection::insertPointAt (int index, int x, int y)
{
	willChange();
	points->insertAtIndex((wxhdObject *) new wxhdPoint(x, y), index);
	//Update handles
	if(index == 0)
	{
		//add a new handle "normal" for a point in next position 0,1 in 1... in 0 startHandle is not moved
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), 1);
	}
	else if(index == (points->count() - 1)) //last point
	{
		//add a new handle "normal" for a point in before last item position
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), (points->count() - 1));
	}
	else
	{
		//add handle at index
		handles->insertAtIndex(new wxhdPolyLineHandle(this, new wxhdPolyLineLocator(index), index), index);
	}
	updateHandlesIndexes();
	changed();
}

void wxhdLineConnection::updateHandlesIndexes()
{
	wxhdPolyLineHandle *h = NULL;
	for(int i = 1; i < handles->count() - 1; i++)
	{
		h = (wxhdPolyLineHandle *) handles->getItemAt(i);
		h->setIndex(i);
	}
}
