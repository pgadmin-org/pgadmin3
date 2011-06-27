//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLocatorHandle.h - Base class for Handles that are located at locator position
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDLOCATORHANDLE_H
#define WXHDLOCATORHANDLE_H

#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/locators/wxhdILocator.h"

class wxhdDrawingView;

class wxhdLocatorHandle : public wxhdIHandle
{
public:
	wxhdLocatorHandle(wxhdIFigure *owner, wxhdILocator *locator);
    ~wxhdLocatorHandle();

	virtual wxhdPoint& locate();
	virtual wxhdILocator* locator();
	virtual void invokeStart(wxhdMouseEvent& event, wxhdDrawingView *view);
	virtual void invokeStep(wxhdMouseEvent& event, wxhdDrawingView *view);
	virtual void invokeEnd(wxhdMouseEvent& event, wxhdDrawingView *view);
protected:

private:
	wxhdILocator *loc;
	wxhdPoint p;

};
#endif
