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
	//A Handle at polyline figure without a respetive flexibility point at line
	//Hack to allow handles of polylines reuse between different versions of same line.
	if(getOwner() && indx < (((wxhdPolyLineFigure *) getOwner())->countPointsAt(view->getIdx()) - 1) ) //indx 0 is first, count first is 1
	{
		wxhdRect copy = getDisplayBox(view->getIdx());
		view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

		/* Uncomment this for testing purposes of handles in a polyline figure
		wxString pos = wxString::Format(_("%d"),indx);
		double middle2 = copy.width / 2;
		context.DrawText(pos,copy.x + middle2+3, copy.y + middle2);
		*/

		double middle = copy.width / 2;
		context.DrawCircle(
		    wxPoint(copy.x + middle, copy.y + middle),
		    wxCoord(middle)
		);
	}
}

void wxhdPolyLineHandle::invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	((wxhdPolyLineFigure *) getOwner())->setPointAt(view->getIdx(), indx, x, y);
	view->notifyChanged();

}

void wxhdPolyLineHandle::invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	if(event.RightDown())
	{
		((wxhdPolyLineFigure *) getOwner())->removePointAt(view->getIdx(), indx);
		view->notifyChanged();
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
