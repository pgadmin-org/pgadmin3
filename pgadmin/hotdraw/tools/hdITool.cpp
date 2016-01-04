//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdITool.cpp - Base class for all tools
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/utilities/hdMouseEvent.h"

hdITool::hdITool()
{
	activatedValue = false;
	undoableValue = false;
}

hdITool::~hdITool()
{
}

void hdITool::mouseDown(hdMouseEvent &event)
{
	/*
	EVT_LEFT_DOWN(func)
	EVT_MIDDLE_DOWN(func)
	EVT_RIGHT_DOWN(func)
	EVT_LEFT_DCLICK(func)
	*/
}

void hdITool::mouseUp(hdMouseEvent &event)
{
	/*
	EVT_LEFT_UP(func)
	EVT_MIDDLE_UP(func)
	EVT_RIGHT_UP(func)
	EVT_RIGHT_DCLICK(func)
	*/
}

void hdITool::mouseMove(hdMouseEvent &event)
{
	/*
	EVT_MOTION(func)
	*/
}

void hdITool::mouseDrag(hdMouseEvent &event)
{
	/*
	When a mouse is dragged this returns true: event.Dragging()
	*/
}

void hdITool::keyDown(hdKeyEvent &event)
{
	/*
	setAnchorCoords(event.GetPosition().x,event.GetPosition().y);
	*/
}

void hdITool::keyUp(hdKeyEvent &event)
{
}

void hdITool::activate(hdDrawingView *view)
{
	activatedValue = true;
}

void hdITool::deactivate(hdDrawingView *view)
{
	activatedValue = false;
}

bool hdITool::activated()
{
	return activatedValue;
}

bool hdITool::undoable()
{
	return undoableValue;
}
