//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdFigureTool.cpp - Base class for all figure tools
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdFigureTool.h"

hdFigureTool::hdFigureTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt):
	hdAbstractTool(view)
{
	defaultTool = dt;
	figure = fig;
}

hdFigureTool::~hdFigureTool()
{
	if(defaultTool)
	{
		delete defaultTool;
	}
}

void hdFigureTool::setDefaultTool(hdITool *dt)
{
	defaultTool = dt;
}

hdITool *hdFigureTool::getDefaultTool()
{
	return defaultTool;
}

void hdFigureTool::setFigure(hdIFigure *fig)
{
	figure = fig;
}

hdIFigure *hdFigureTool::getFigure()
{
	return figure;
}

void hdFigureTool::mouseDown(hdMouseEvent &event)
{
	if(defaultTool)
	{
		defaultTool->mouseDown(event);
	}
}

void hdFigureTool::mouseUp(hdMouseEvent &event)
{
	if(defaultTool)
	{
		defaultTool->mouseUp(event);
	}
}

void hdFigureTool::mouseMove(hdMouseEvent &event)
{
	if(defaultTool)
	{
		defaultTool->mouseMove(event);
	}
}

void hdFigureTool::mouseDrag(hdMouseEvent &event)
{
	if(defaultTool)
	{
		defaultTool->mouseDrag(event);
	}
}

void hdFigureTool::keyDown(hdKeyEvent &event)
{
	if(defaultTool)
	{
		defaultTool->keyDown(event);
	}
}

void hdFigureTool::keyUp(hdKeyEvent &event)
{
	if(defaultTool)
	{
		defaultTool->keyUp(event);
	}
}
