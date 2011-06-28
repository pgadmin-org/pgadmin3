//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdButtonHandle.cpp - Handle to allow creation of buttons at figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/handles/wxhdButtonHandle.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"



wxhdButtonHandle::wxhdButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size):
	wxhdIHandle(owner)
{
	buttonIcon = buttonImage;
	clicked = false;
	bLocator = buttonLocator;
	displayBox.SetSize(size);
}

wxhdButtonHandle::~wxhdButtonHandle()
{
	if(bLocator)
		delete bLocator;
}

wxCursor wxhdButtonHandle::createCursor()
{
	return wxCursor(wxCURSOR_PENCIL);
}

wxhdRect &wxhdButtonHandle::getDisplayBox()
{
	wxhdPoint p = locate();
	displayBox.SetPosition(p);
	return displayBox;
}

void wxhdButtonHandle::draw(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxPoint copy = getDisplayBox().GetPosition();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	if(buttonIcon.IsOk())
		context.DrawBitmap(buttonIcon, copy.x, copy.y, true);
}


wxhdPoint &wxhdButtonHandle::locate()
{
	if(bLocator)
	{
		pointLocate = bLocator->locate(getOwner());
		return pointLocate;
	}
	else
	{
		pointLocate = wxhdPoint(0, 0);
		return pointLocate;
	}
}
