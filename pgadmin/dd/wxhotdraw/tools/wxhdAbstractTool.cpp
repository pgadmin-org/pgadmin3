//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractTool.cpp - An abstract tool to allow creation of all tools
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"

wxhdAbstractTool::wxhdAbstractTool(wxhdDrawingEditor *editor)
{
	anchorX = 0;
	anchorY = 0;
	ownerEditor = editor;
}

wxhdAbstractTool::~wxhdAbstractTool()
{
}

void wxhdAbstractTool::mouseDown(wxhdMouseEvent &event)
{
	setAnchorCoords(event.GetPosition().x, event.GetPosition().y);
}

void wxhdAbstractTool::mouseUp(wxhdMouseEvent &event)
{
}

void wxhdAbstractTool::mouseMove(wxhdMouseEvent &event)
{
}

void wxhdAbstractTool::mouseDrag(wxhdMouseEvent &event)
{
}

void wxhdAbstractTool::keyDown(wxhdKeyEvent &event)
{
//	setAnchorCoords(event.GetPosition().x,event.GetPosition().y);
}

void wxhdAbstractTool::keyUp(wxhdKeyEvent &event)
{
}

void wxhdAbstractTool::setAnchorCoords(int x, int y)
{
	anchorX = x;
	anchorY = y;
}

wxhdDrawingEditor *wxhdAbstractTool::getDrawingEditor()
{
	return ownerEditor;
}

