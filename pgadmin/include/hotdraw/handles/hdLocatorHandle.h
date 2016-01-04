//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLocatorHandle.h - Base class for Handles that are located at locator position
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDLOCATORHANDLE_H
#define HDLOCATORHANDLE_H

#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/locators/hdILocator.h"

class hdDrawingView;

class hdLocatorHandle : public hdIHandle
{
public:
	hdLocatorHandle(hdIFigure *owner, hdILocator *locator);
	~hdLocatorHandle();

	virtual hdPoint &locate(int posIdx);
	virtual hdILocator *locator();
	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
protected:

private:
	hdILocator *loc;
	hdPoint p;

};
#endif
