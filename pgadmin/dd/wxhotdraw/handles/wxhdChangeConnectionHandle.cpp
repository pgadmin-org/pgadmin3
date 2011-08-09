//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdChangeConnectionHandle.cpp - Base Handle to allow change connected figures at connection figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/handles/wxhdChangeConnectionHandle.h"
#include "dd/wxhotdraw/figures/wxhdLineConnection.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"


wxhdChangeConnectionHandle::wxhdChangeConnectionHandle(wxhdLineConnection *owner):
	wxhdIHandle(owner)
{
	connection = owner;
	targetFigure = NULL;
	originalTarget = NULL;
}

wxhdChangeConnectionHandle::~wxhdChangeConnectionHandle()
{

}

void wxhdChangeConnectionHandle::draw(wxBufferedDC &context, wxhdDrawingView *view)
{

	wxhdRect copy = getDisplayBox(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

	copy.Deflate(2, 2);
	context.SetPen(*wxGREEN_PEN);
	context.SetBrush(*wxGREEN_BRUSH);
	context.DrawRectangle(copy);
}

wxCursor wxhdChangeConnectionHandle::createCursor()
{
	return wxCursor(wxCURSOR_CROSS);
}

void wxhdChangeConnectionHandle::invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	originalTarget = target();
	disconnect(view);
}

void wxhdChangeConnectionHandle::invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	wxhdPoint p = wxhdPoint(x, y);
	wxhdIFigure *figure = findConnectableFigure(view->getIdx(), x, y, view->getDrawing());
	targetFigure = figure;
	wxhdIConnector *target = findConnectionTarget(view->getIdx(), x, y, view->getDrawing());
	if(target)
	{
		p = target->getDisplayBox().center(view->getIdx());
	}
	setPoint(view->getIdx(), p);
	connection->updateConnection(view->getIdx());
	if(view)
		view->Refresh();
}

void wxhdChangeConnectionHandle::invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	wxhdIConnector *target = findConnectionTarget(view->getIdx(), x, y, view->getDrawing());
	if(!target)
	{
		target = originalTarget;
	}
	connect(target, view);
	connection->updateConnection(view->getIdx());
	if(view)
		view->Refresh();
}


wxhdIFigure *wxhdChangeConnectionHandle::findConnectableFigure (int posIdx, int x, int y, wxhdDrawing *drawing)
{
	wxhdIFigure *out = NULL;
	wxhdIteratorBase *iterator = drawing->figuresInverseEnumerator();
	while(iterator->HasNext())
	{
		wxhdIFigure *figure = (wxhdIFigure *) iterator->Next();
		if(figure->containsPoint(posIdx, x, y) && isConnectionPossible(figure))
		{
			out = figure;
			break;
		}
	}
	delete iterator;
	return out;
}
wxhdIConnector *wxhdChangeConnectionHandle::findConnectionTarget(int posIdx, int x, int y, wxhdDrawing *drawing)
{
	wxhdIFigure *target = findConnectableFigure(posIdx, x, y, drawing);
	if(target)
		return target->connectorAt(posIdx, x, y);
	else
		return NULL;
}
