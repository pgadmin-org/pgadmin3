//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnFigureTool.cpp - Improvement to hdFigureTool to work with composite table figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/tools/ddColumnFigureTool.h"
#include "dd/dditems/figures/ddColumnFigure.h"
#include "hotdraw/tools/hdDragTrackerTool.h"


ddColumnFigureTool::ddColumnFigureTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt):
	hdFigureTool(view, fig, dt)
{
	delegateTool = NULL;
}

ddColumnFigureTool::~ddColumnFigureTool()
{
	//This tool destructor is at compositeTool, because this is only a selection tool and neither tool belongs to it.
	hdITool *tmpDefault = hdFigureTool::getDefaultTool();
	hdFigureTool *tmpDelegateDefault;

	if(delegateTool->ms_classInfo.IsKindOf(&hdFigureTool::ms_classInfo))
		tmpDelegateDefault = (hdFigureTool *)delegateTool;
	else
		tmpDelegateDefault = NULL;

	if(delegateTool && delegateTool != tmpDefault)
	{
		//Hack to avoid delete defaultTool (Delegate->defaultTool) of delegate tool
		// if this is the same as defaultTool (this->defaultTool) of this Object.
		if(tmpDelegateDefault && tmpDelegateDefault->getDefaultTool() == tmpDefault)
			tmpDelegateDefault->setDefaultTool(NULL);

		//Hack to avoid delete hdDragTrackerTool Default twice because this figure is only used inside
		//a table, and then create a compositeTool and default in both tools is hdDragTrackerTool
		//but I can't hard code this is Composite because that class should remain generic
		if(tmpDelegateDefault->getDefaultTool()->ms_classInfo.IsKindOf(&hdDragTrackerTool::ms_classInfo))
			tmpDelegateDefault->setDefaultTool(NULL);
		delete delegateTool;
	}

}

void ddColumnFigureTool::setDefaultTool(hdITool *dt)
{
	hdFigureTool::setDefaultTool(dt);
}

hdITool *ddColumnFigureTool::getDefaultTool()
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

void ddColumnFigureTool::mouseDown(hdMouseEvent &event)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	ddColumnFigure *cfigure = (ddColumnFigure *) getFigure();
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

void ddColumnFigureTool::activate(hdDrawingView *view)
{
	if(delegateTool)
	{
		delegateTool->activate(view);
	}
}

void ddColumnFigureTool::deactivate(hdDrawingView *view)
{
	if(delegateTool)
	{
		delegateTool->deactivate(view);
	}
}

void ddColumnFigureTool::setDelegateTool(hdDrawingView *view, hdITool *tool)
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

hdITool *ddColumnFigureTool::getDelegateTool()
{
	return delegateTool;
}
