//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdChangeConnectionHandle.cpp - Base Handle to allow change connected figures at connection figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/handles/hdChangeConnectionHandle.h"
#include "hotdraw/figures/hdLineConnection.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/main/hdDrawingView.h"


hdChangeConnectionHandle::hdChangeConnectionHandle(hdLineConnection *owner):
	hdIHandle(owner)
{
	connection = owner;
	targetFigure = NULL;
	originalTarget = NULL;
}

hdChangeConnectionHandle::~hdChangeConnectionHandle()
{

}

void hdChangeConnectionHandle::draw(wxBufferedDC &context, hdDrawingView *view)
{

	hdRect copy = getDisplayBox(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

	copy.Deflate(2, 2);
	context.SetPen(*wxGREEN_PEN);
	context.SetBrush(*wxGREEN_BRUSH);
	context.DrawRectangle(copy);
}

wxCursor hdChangeConnectionHandle::createCursor()
{
	return wxCursor(wxCURSOR_CROSS);
}

void hdChangeConnectionHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
	originalTarget = target();
	disconnect(view);
}

void hdChangeConnectionHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	hdPoint p = hdPoint(x, y);
	hdIFigure *figure = findConnectableFigure(view->getIdx(), x, y, view->getDrawing());
	targetFigure = figure;
	hdIConnector *target = findConnectionTarget(view->getIdx(), x, y, view->getDrawing());
	if(target)
	{
		p = target->getDisplayBox().center(view->getIdx());
	}
	setPoint(view->getIdx(), p);
	connection->updateConnection(view->getIdx());
	if(view)
		view->Refresh();
}

void hdChangeConnectionHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	hdIConnector *target = findConnectionTarget(view->getIdx(), x, y, view->getDrawing());
	if(!target)
	{
		target = originalTarget;
	}
	connect(target, view);
	connection->updateConnection(view->getIdx());
	if(view)
		view->Refresh();
}


hdIFigure *hdChangeConnectionHandle::findConnectableFigure (int posIdx, int x, int y, hdDrawing *drawing)
{
	hdIFigure *out = NULL;
	hdIteratorBase *iterator = drawing->figuresInverseEnumerator();
	while(iterator->HasNext())
	{
		hdIFigure *figure = (hdIFigure *) iterator->Next();
		if(figure->containsPoint(posIdx, x, y) && isConnectionPossible(figure))
		{
			out = figure;
			break;
		}
	}
	delete iterator;
	return out;
}
hdIConnector *hdChangeConnectionHandle::findConnectionTarget(int posIdx, int x, int y, hdDrawing *drawing)
{
	hdIFigure *target = findConnectableFigure(posIdx, x, y, drawing);
	if(target)
		return target->connectorAt(posIdx, x, y);
	else
		return NULL;
}
