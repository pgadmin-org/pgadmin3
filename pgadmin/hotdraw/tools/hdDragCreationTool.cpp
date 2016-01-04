//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDragCreationTool.cpp - A Tool that allow to move figure around view
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/tools/hdDragCreationTool.h"

hdDragCreationTool::hdDragCreationTool(hdDrawingView *view, hdIFigure *prototype):
	hdCreationTool(view, prototype)
{
}

hdDragCreationTool::~hdDragCreationTool()
{
}


void hdDragCreationTool::mouseDrag(hdMouseEvent &event)
{
	if(event.LeftIsDown())
	{
		figurePrototype->displayBox().SetPosition(event.getView()->getIdx(), event.GetPosition());
	}
}
