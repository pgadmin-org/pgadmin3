//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLocatorHandle.cpp - Base class for Handles that are located at locator position
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/handles/hdLocatorHandle.h"

hdLocatorHandle::hdLocatorHandle(hdIFigure *owner, hdILocator *locator):
	hdIHandle(owner)
{
	loc = locator;
}

hdLocatorHandle::~hdLocatorHandle()
{
	if(loc)
		delete loc;
}

hdPoint &hdLocatorHandle::locate(int posIdx)
{
	p = hdPoint(0, 0);
	return loc == NULL ? p : loc->locate(posIdx, getOwner());
}

hdILocator *hdLocatorHandle::locator()
{
	return loc;
}

void hdLocatorHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
}

void hdLocatorHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
}

void hdLocatorHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
}
