//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdIFigure.cpp - Base class for all figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/connectors/hdIConnector.h"
#include "hotdraw/connectors/hdChopBoxConnector.h"

hdIFigure::hdIFigure()
{
	handles = new hdCollection(new hdArrayCollection());
	observers = new hdCollection(new hdArrayCollection());
	unsigned int i;
	for(i = 0; i < MAXPOS; i++)
	{
		selected.Add(false);
	}
	connector = NULL;
	basicDisplayBox.SetSize(wxSize(0, 0));
}

hdIFigure::~hdIFigure()
{
	if(connector)
		delete connector;
	if(handles)
		delete handles;
	if(observers)
	{
		observers->removeAll();
		delete observers;
	}
}

void hdIFigure::AddPosForNewDiagram()
{
	basicDisplayBox.addNewXYPosition();
	selected.Add(false);
}

void hdIFigure::RemovePosOfDiagram(int posIdx)
{
	basicDisplayBox.removeXYPosition(posIdx);
}

hdMultiPosRect &hdIFigure::displayBox()
{
	return getBasicDisplayBox();
}

hdMultiPosRect &hdIFigure::getBasicDisplayBox()
{
	return basicDisplayBox;
}

void hdIFigure::draw (wxBufferedDC &context, hdDrawingView *view)
{

}

void hdIFigure::drawSelected (wxBufferedDC &context, hdDrawingView *view)
{

}

hdCollection *hdIFigure::handlesEnumerator()
{
	return handles;
}

void hdIFigure::addHandle (hdIHandle *handle)
{
	if(!handles)
	{
		handles  = new hdCollection(new hdArrayCollection());
	}
	handles->addItem(handle);
}

void hdIFigure::removeHandle (hdIHandle *handle)
{
	if(handles)
	{
		handles->removeItem(handle);
	}
}

hdITool *hdIFigure::CreateFigureTool(hdDrawingView *view, hdITool *defaultTool)
{
	return defaultTool;
}

bool hdIFigure::isSelected(int posIdx)
{
	return selected[posIdx];
}

void hdIFigure::setSelected(int posIdx, bool value)
{
	selected[posIdx] = value;
}

hdIConnector *hdIFigure::connectorAt (int posIdx, int x, int y)
{
	if(!connector)
		connector = new hdChopBoxConnector(this);
	return connector;
}

bool hdIFigure::includes(hdIFigure *figure)
{
	return (this == figure);
}

void hdIFigure::onFigureChanged(int posIdx, hdIFigure *figure)
{

	hdIteratorBase *iterator = observers->createIterator();
	while(iterator->HasNext())
	{
		hdIFigure *o = (hdIFigure *) iterator->Next();
		o->onFigureChanged(posIdx, this);
	}
	delete iterator;
}

void hdIFigure::addObserver(hdIFigure *observer)
{
	if(!observers)
	{
		observers  = new hdCollection(new hdArrayCollection());
	}
	observers->addItem(observer);
}

void hdIFigure::removeObserver(hdIFigure *observer)
{
	if(observers)
	{
		observers->removeItem(observer);
	}
}

hdIteratorBase *hdIFigure::observersEnumerator()
{
	return observers->createIterator();
}

void hdIFigure::setKindId(int hiddenId)
{
	kindHiddenId = hiddenId;
}

//Hack because is kindof in not powerful as it should be
int hdIFigure::getKindId()
{
	return kindHiddenId;
}
