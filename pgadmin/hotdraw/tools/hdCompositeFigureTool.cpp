//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCompositeFigureTool.cpp - A Tool that allow to change between all tools in a composite figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdCompositeFigureTool.h"
#include "hotdraw/figures/hdCompositeFigure.h"


hdCompositeFigureTool::hdCompositeFigureTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt):
	hdFigureTool(view, fig, dt)
{
	delegateTool = NULL;
}

hdCompositeFigureTool::~hdCompositeFigureTool()
{
	hdITool *tmpDefault = hdFigureTool::getDefaultTool();
	hdFigureTool *tmpDelegateDefault;

	if(delegateTool->ms_classInfo.IsKindOf(&hdFigureTool::ms_classInfo))
		tmpDelegateDefault = (hdFigureTool *)delegateTool;
	else
		tmpDelegateDefault = NULL;

	if(delegateTool && delegateTool != tmpDefault)
	{
		// Hack to avoid delete defaultTool (Delegate->defaultTool) of delegate tool
		// if this is the same as defaultTool (this->defaultTool) of this Object.
		if(tmpDelegateDefault && tmpDelegateDefault->getDefaultTool() == tmpDefault)
			tmpDelegateDefault->setDefaultTool(NULL);
		delete delegateTool;
	}
}

void hdCompositeFigureTool::setDefaultTool(hdITool *dt)
{
	hdFigureTool::setDefaultTool(dt);
}

hdITool *hdCompositeFigureTool::getDefaultTool()
{
	if(delegateTool)
	{
		return delegateTool;
	}
	else
	{
		return hdFigureTool::getDefaultTool();
	}
}

void hdCompositeFigureTool::mouseDown(hdMouseEvent &event)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	hdCompositeFigure *cfigure = (hdCompositeFigure *) getFigure();
	hdIFigure *figure = cfigure->findFigure(event.getView()->getIdx(), x, y);

	if(figure)
	{
		setDelegateTool(event.getView(), figure->CreateFigureTool(event.getView(), getDefaultTool()));
	}
	else
	{
		setDelegateTool(event.getView(), getDefaultTool());
	}

	if(delegateTool)
	{
		delegateTool->mouseDown(event);
	}
}

void hdCompositeFigureTool::activate(hdDrawingView *view)
{
	if(delegateTool)
	{
		delegateTool->activate(view);
	}
}

void hdCompositeFigureTool::deactivate(hdDrawingView *view)
{
	if(delegateTool)
	{
		delegateTool->deactivate(view);
	}
}

void hdCompositeFigureTool::setDelegateTool(hdDrawingView *view, hdITool *tool)
{
	if(delegateTool)
	{
		delegateTool->deactivate(view);
		delete delegateTool;
		delegateTool = NULL;
	}

	delegateTool = tool;
	if(delegateTool)
	{
		delegateTool->activate(view);
	}
}

hdITool *hdCompositeFigureTool::getDelegateTool()
{
	return delegateTool;
}
