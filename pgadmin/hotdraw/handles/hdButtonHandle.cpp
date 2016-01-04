//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdButtonHandle.cpp - Handle to allow creation of buttons at figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/handles/hdButtonHandle.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/main/hdDrawingView.h"



hdButtonHandle::hdButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size):
	hdIHandle(owner)
{
	buttonIcon = buttonImage;
	clicked = false;
	bLocator = buttonLocator;
	displayBox.SetSize(size);
}

hdButtonHandle::~hdButtonHandle()
{
	if(bLocator)
		delete bLocator;
}

wxCursor hdButtonHandle::createCursor()
{
	return wxCursor(wxCURSOR_ARROW);
}

hdRect &hdButtonHandle::getDisplayBox(int posIdx)
{
	hdPoint p = locate(posIdx);
	displayBox.SetPosition(p);
	return displayBox;
}

void hdButtonHandle::draw(wxBufferedDC &context, hdDrawingView *view)
{
	wxPoint copy = getDisplayBox(view->getIdx()).GetPosition();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	if(buttonIcon.IsOk())
		context.DrawBitmap(buttonIcon, copy.x, copy.y, true);
}


hdPoint &hdButtonHandle::locate(int posIdx)
{
	if(bLocator)
	{
		pointLocate = bLocator->locate(posIdx, getOwner());
		return pointLocate;
	}
	else
	{
		pointLocate = hdPoint(0, 0);
		return pointLocate;
	}
}
