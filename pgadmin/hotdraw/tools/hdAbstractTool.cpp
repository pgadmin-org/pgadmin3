//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractTool.cpp - An abstract tool to allow creation of all tools
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdAbstractTool.h"

hdAbstractTool::hdAbstractTool(hdDrawingView *view)
{
	anchorX = 0;
	anchorY = 0;
	ownerView = view;
}

hdAbstractTool::~hdAbstractTool()
{
}

void hdAbstractTool::mouseDown(hdMouseEvent &event)
{
	setAnchorCoords(event.GetPosition().x, event.GetPosition().y);
}

void hdAbstractTool::mouseUp(hdMouseEvent &event)
{
}

void hdAbstractTool::mouseMove(hdMouseEvent &event)
{
}

void hdAbstractTool::mouseDrag(hdMouseEvent &event)
{
}

void hdAbstractTool::keyDown(hdKeyEvent &event)
{
//	setAnchorCoords(event.GetPosition().x,event.GetPosition().y);
}

void hdAbstractTool::keyUp(hdKeyEvent &event)
{
}

void hdAbstractTool::setAnchorCoords(int x, int y)
{
	anchorX = x;
	anchorY = y;
}

hdDrawingView *hdAbstractTool::getDrawingView()
{
	return ownerView;
}

