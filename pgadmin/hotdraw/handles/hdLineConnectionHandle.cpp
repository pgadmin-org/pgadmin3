//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLineConnectionHandle.cpp - Base class for Handles that are located at locator position
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/handles/hdLineConnectionHandle.h"
#include "hotdraw/figures/hdPolyLineFigure.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/utilities/hdGeometry.h"
#include "hotdraw/main/hdDrawingView.h"

hdLineConnectionHandle::hdLineConnectionHandle(hdPolyLineFigure *figure, hdILocator *loc, int index):
	hdPolyLineHandle(figure, loc, index)
{
}

void hdLineConnectionHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	hdPolyLineFigure *figure = (hdPolyLineFigure *) getOwner();
	//eliminate all handles in the middle of a straight line

	if( figure->pointCount(view->getIdx()) > 2 && getIndex() != 0 && getIndex() != (figure->pointCount(view->getIdx()) - 1) )
	{
		hdPoint p1 = figure->pointAt(view->getIdx(), getIndex() - 1);
		hdPoint p2 = figure->pointAt(view->getIdx(), getIndex() + 1);
		hdGeometry g;
		if(g.lineContainsPoint(p1.x, p1.y, p2.x, p2.y, x, y))
		{
			figure->removePointAt(view->getIdx(), getIndex());
		}
	}
}
