//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDragCreationTool.cpp - A Tool that allow to move figure around view
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdDragCreationTool.h"

wxhdDragCreationTool::wxhdDragCreationTool(wxhdDrawingEditor *editor, wxhdIFigure *prototype):
	wxhdCreationTool(editor, prototype)
{
}

wxhdDragCreationTool::~wxhdDragCreationTool()
{
}


void wxhdDragCreationTool::mouseDrag(wxhdMouseEvent &event)
{
	if(event.LeftIsDown())
	{
		figurePrototype->displayBox().SetPosition(event.GetPosition());
	}
}
