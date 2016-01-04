//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdHandleTrackerTool.cpp - A Tool that allow to use multiple handles
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdHandleTrackerTool.h"
#include "hotdraw/tools/hdAbstractTool.h"

hdHandleTrackerTool::hdHandleTrackerTool(hdDrawingView *view, hdIHandle *anchor)
	: hdAbstractTool(view)
{
	anchorHandle = anchor;
}

hdHandleTrackerTool::~hdHandleTrackerTool()
{
}

void hdHandleTrackerTool::mouseDown(hdMouseEvent &event)
{
	hdAbstractTool::mouseDown(event);
	anchorHandle->invokeStart(event, event.getView());
}

void hdHandleTrackerTool::mouseUp(hdMouseEvent &event)
{
	anchorHandle->invokeEnd(event, event.getView());
}

void hdHandleTrackerTool::mouseDrag(hdMouseEvent &event)
{
	anchorHandle->invokeStep(event, event.getView());
}
