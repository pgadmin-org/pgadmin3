//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineHandle.cpp - Handle for manipulation of multiple flexibility points lines.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/handles/wxhdPolyLineHandle.h"
#include "dd/wxhotdraw/figures/wxhdPolyLineFigure.h"
#include "dd/wxhotdraw/locators/wxhdPolyLineLocator.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"


wxhdPolyLineHandle::wxhdPolyLineHandle(wxhdPolyLineFigure *figure, wxhdILocator *loc, int index):
	wxhdLocatorHandle((wxhdIFigure *)figure, loc)
{
	indx = index;
}

wxhdPolyLineHandle::~wxhdPolyLineHandle()
{
}

void wxhdPolyLineHandle::draw(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = getDisplayBox();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

	double middle = copy.width / 2;
	context.DrawCircle(
	    wxPoint(copy.x + middle, copy.y + middle),
	    wxCoord(middle)
	);
}

void wxhdPolyLineHandle::invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	((wxhdPolyLineFigure *) getOwner())->setPointAt(indx, x, y);

}

void wxhdPolyLineHandle::invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	if(event.RightDown())
	{
		((wxhdPolyLineFigure *) getOwner())->removePointAt(indx);
	}
}
wxCursor wxhdPolyLineHandle::createCursor()
{
	return wxCursor(wxCURSOR_CROSS);

}

int wxhdPolyLineHandle::getIndex()
{
	return indx;
}

void wxhdPolyLineHandle::setIndex(int index)
{
	indx = index;
	wxhdPolyLineLocator *l = (wxhdPolyLineLocator *) locator();
	l->setIndex(index);
}
