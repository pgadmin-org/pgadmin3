//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLocatorHandle.cpp - Base class for Handles that are located at locator position
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/handles/wxhdLocatorHandle.h"

wxhdLocatorHandle::wxhdLocatorHandle(wxhdIFigure *owner, wxhdILocator *locator):
wxhdIHandle(owner)
{
	loc=locator;
}

wxhdLocatorHandle::~wxhdLocatorHandle()
{
	if(loc)
		delete loc;
}

wxhdPoint& wxhdLocatorHandle::locate()
{
	p=wxhdPoint(0,0);
	return loc==NULL ? p : loc->locate(getOwner());
}

wxhdILocator* wxhdLocatorHandle::locator()
{
	return loc;
}

void wxhdLocatorHandle::invokeStart(wxhdMouseEvent& event, wxhdDrawingView *view)
{
}

void wxhdLocatorHandle::invokeStep(wxhdMouseEvent& event, wxhdDrawingView *view)
{
}

void wxhdLocatorHandle::invokeEnd(wxhdMouseEvent& event, wxhdDrawingView *view)
{
}
