//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPolyLineHandle.cpp - Handle for manipulation of multiple flexibility points lines.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/handles/hdPolyLineHandle.h"
#include "hotdraw/figures/hdPolyLineFigure.h"
#include "hotdraw/locators/hdPolyLineLocator.h"
#include "hotdraw/main/hdDrawingView.h"


hdPolyLineHandle::hdPolyLineHandle(hdPolyLineFigure *figure, hdILocator *loc, int index):
	hdLocatorHandle((hdIFigure *)figure, loc)
{
	indx = index;
}

hdPolyLineHandle::~hdPolyLineHandle()
{
}

void hdPolyLineHandle::draw(wxBufferedDC &context, hdDrawingView *view)
{
	//A Handle at polyline figure without a respetive flexibility point at line
	//Hack to allow handles of polylines reuse between different versions of same line.
	if(getOwner() && indx < (((hdPolyLineFigure *) getOwner())->countPointsAt(view->getIdx()) - 1) ) //indx 0 is first, count first is 1
	{
		hdRect copy = getDisplayBox(view->getIdx());
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

void hdPolyLineHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	((hdPolyLineFigure *) getOwner())->setPointAt(view->getIdx(), indx, x, y);
	view->notifyChanged();

}

void hdPolyLineHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
	if(event.RightDown())
	{
		((hdPolyLineFigure *) getOwner())->removePointAt(view->getIdx(), indx);
		view->notifyChanged();
	}
}
wxCursor hdPolyLineHandle::createCursor()
{
	return wxCursor(wxCURSOR_CROSS);

}

int hdPolyLineHandle::getIndex()
{
	return indx;
}

void hdPolyLineHandle::setIndex(int index)
{
	indx = index;
	hdPolyLineLocator *l = (hdPolyLineLocator *) locator();
	l->setIndex(index);
}
