//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdFigureTool.cpp - Base class for all figure tools
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdFigureTool.h"

wxhdFigureTool::wxhdFigureTool(wxhdDrawingEditor *editor, wxhdIFigure *fig, wxhdITool *dt):
wxhdAbstractTool(editor)
{
	defaultTool=dt;
	figure=fig;
}

wxhdFigureTool::~wxhdFigureTool()
{
	if(defaultTool)
	{
		delete defaultTool;
	}
}

void wxhdFigureTool::setDefaultTool(wxhdITool *dt)
{
	defaultTool=dt;
}

wxhdITool* wxhdFigureTool::getDefaultTool()
{
	return defaultTool;
}

void wxhdFigureTool::setFigure(wxhdIFigure *fig)
{
	figure=fig;
}

wxhdIFigure* wxhdFigureTool::getFigure()
{
	return figure;
}

void wxhdFigureTool::mouseDown(wxhdMouseEvent& event)
{
	if(defaultTool)
	{
		defaultTool->mouseDown(event);
	}
}

void wxhdFigureTool::mouseUp(wxhdMouseEvent& event)
{
	if(defaultTool)
	{
		defaultTool->mouseUp(event);
	}
}

void wxhdFigureTool::mouseMove(wxhdMouseEvent& event)
{
	if(defaultTool)
	{
		defaultTool->mouseMove(event);
	}
}

void wxhdFigureTool::mouseDrag(wxhdMouseEvent& event)
{
	if(defaultTool)
	{
		defaultTool->mouseDrag(event);
	}
}

void wxhdFigureTool::keyDown(wxhdKeyEvent& event)
{
	if(defaultTool)
	{
		defaultTool->keyDown(event);
	}
}

void wxhdFigureTool::keyUp(wxhdKeyEvent& event)
{
		if(defaultTool)
	{
		defaultTool->keyUp(event);
	}
}
