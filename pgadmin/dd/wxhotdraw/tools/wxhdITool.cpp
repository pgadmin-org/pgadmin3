//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdITool.cpp - Base class for all tools
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/utilities/wxhdMouseEvent.h"

wxhdITool::wxhdITool()
{
	activatedValue=false;
	undoableValue=false;
}

wxhdITool::~wxhdITool()
{
}

void wxhdITool::mouseDown(wxhdMouseEvent& event)
{
/*
EVT_LEFT_DOWN(func)
EVT_MIDDLE_DOWN(func)
EVT_RIGHT_DOWN(func)
EVT_LEFT_DCLICK(func)
*/
}

void wxhdITool::mouseUp(wxhdMouseEvent& event)
{
/*
EVT_LEFT_UP(func)
EVT_MIDDLE_UP(func)
EVT_RIGHT_UP(func)
EVT_RIGHT_DCLICK(func)
*/
}

void wxhdITool::mouseMove(wxhdMouseEvent& event)
{
/*
EVT_MOTION(func)
*/
}

void wxhdITool::mouseDrag(wxhdMouseEvent& event)
{
/*
When a mouse is dragged this returns true: event.Dragging()
*/
}

void wxhdITool::keyDown(wxhdKeyEvent& event)
{
/*
setAnchorCoords(event.GetPosition().x,event.GetPosition().y);
*/
}

void wxhdITool::keyUp(wxhdKeyEvent& event)
{
}

void wxhdITool::activate()
{
	activatedValue=true;
}

void wxhdITool::deactivate()
{
	activatedValue=false;
}

bool wxhdITool::activated()
{
	return activatedValue;
}

bool wxhdITool::undoable()
{
	return undoableValue;
}
