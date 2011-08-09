//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdHandleTrackerTool.cpp - A Tool that allow to use multiple handles
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdHandleTrackerTool.h"
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"

wxhdHandleTrackerTool::wxhdHandleTrackerTool(wxhdDrawingView *view, wxhdIHandle *anchor)
	: wxhdAbstractTool(view)
{
	anchorHandle = anchor;
}

wxhdHandleTrackerTool::~wxhdHandleTrackerTool()
{
}

void wxhdHandleTrackerTool::mouseDown(wxhdMouseEvent &event)
{
	wxhdAbstractTool::mouseDown(event);
	anchorHandle->invokeStart(event, event.getView());
}

void wxhdHandleTrackerTool::mouseUp(wxhdMouseEvent &event)
{
	anchorHandle->invokeEnd(event, event.getView());
}

void wxhdHandleTrackerTool::mouseDrag(wxhdMouseEvent &event)
{
	anchorHandle->invokeStep(event, event.getView());
}
