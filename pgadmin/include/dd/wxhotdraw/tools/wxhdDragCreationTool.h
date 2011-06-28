//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDragCreationTool.h - A Tool that allow to move figure around view
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDDRAGCREATIONTOOL_H
#define WXHDDRAGCREATIONTOOL_H

#include "dd/wxhotdraw/tools/wxhdCreationTool.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"


class wxhdDragCreationTool : public wxhdCreationTool
{
public:
	wxhdDragCreationTool(wxhdDrawingEditor *editor, wxhdIFigure *prototype);
	~wxhdDragCreationTool();
	virtual void mouseDrag(wxhdMouseEvent &event);
};
#endif
