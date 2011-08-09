//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCompositeFigureTool.cpp - A Tool that allow to change between all tools in a composite figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdCompositeFigureTool.h"
#include "dd/wxhotdraw/figures/wxhdCompositeFigure.h"


wxhdCompositeFigureTool::wxhdCompositeFigureTool(wxhdDrawingView *view, wxhdIFigure *fig, wxhdITool *dt):
	wxhdFigureTool(view, fig, dt)
{
	delegateTool = NULL;
}

wxhdCompositeFigureTool::~wxhdCompositeFigureTool()
{
	wxhdITool *tmpDefault = wxhdFigureTool::getDefaultTool();
	wxhdFigureTool *tmpDelegateDefault;

	if(delegateTool->ms_classInfo.IsKindOf(&wxhdFigureTool::ms_classInfo))
		tmpDelegateDefault = (wxhdFigureTool *)delegateTool;
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

void wxhdCompositeFigureTool::setDefaultTool(wxhdITool *dt)
{
	wxhdFigureTool::setDefaultTool(dt);
}

wxhdITool *wxhdCompositeFigureTool::getDefaultTool()
{
	if(delegateTool)
	{
		return delegateTool;
	}
	else
	{
		return wxhdFigureTool::getDefaultTool();
	}
}

void wxhdCompositeFigureTool::mouseDown(wxhdMouseEvent &event)
{
	int x = event.GetPosition().x, y = event.GetPosition().y;
	wxhdCompositeFigure *cfigure = (wxhdCompositeFigure *) getFigure();
	wxhdIFigure *figure = cfigure->findFigure(event.getView()->getIdx(), x, y);

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

void wxhdCompositeFigureTool::activate(wxhdDrawingView *view)
{
	if(delegateTool)
	{
		delegateTool->activate(view);
	}
}

void wxhdCompositeFigureTool::deactivate(wxhdDrawingView *view)
{
	if(delegateTool)
	{
		delegateTool->deactivate(view);
	}
}

void wxhdCompositeFigureTool::setDelegateTool(wxhdDrawingView *view, wxhdITool *tool)
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

wxhdITool *wxhdCompositeFigureTool::getDelegateTool()
{
	return delegateTool;
}
