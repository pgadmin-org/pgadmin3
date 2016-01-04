//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCreationTool.cpp - A Tool that create a figure by just click on view
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdCreationTool.h"
#include "hotdraw/tools/hdSelectionTool.h"

hdCreationTool::hdCreationTool(hdDrawingView *view, hdIFigure *prototype):
	hdAbstractTool(view)
{
	figurePrototype = prototype;
}

hdCreationTool::~hdCreationTool()
{
}

void hdCreationTool::mouseDown(hdMouseEvent &event)
{
	hdAbstractTool::mouseDown(event);
	if(event.LeftDown())
	{
		event.getView()->getDrawing()->add(figurePrototype);
		int x = event.GetPosition().x, y = event.GetPosition().y;
		figurePrototype->moveTo(event.getView()->getIdx(), x, y);
		event.getView()->getDrawing()->clearSelection();
		event.getView()->getDrawing()->addToSelection(figurePrototype);
	}
}

void hdCreationTool::mouseUp(hdMouseEvent &event)
{
	hdAbstractTool::mouseUp(event);
	event.getView()->setTool(new hdSelectionTool(event.getView()));
}

void hdCreationTool::activate(hdDrawingView *view)
{
	hdAbstractTool::activate(view);
	view->SetCursor(wxCursor(wxCURSOR_CROSS));
}

void hdCreationTool::deactivate(hdDrawingView *view)
{
	hdAbstractTool::deactivate(view);
	view->SetCursor(wxCursor(wxCURSOR_ARROW));
}

void hdCreationTool::setPrototype(hdIFigure *prototype)
{
	figurePrototype = prototype;
}

hdIFigure *hdCreationTool::getPrototype()
{
	return figurePrototype;
}
