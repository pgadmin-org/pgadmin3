//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineFigureTool.h - Tool to allow creation of flexibility points at polylines figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDPOLYLINEFIGURETOOL_H
#define WXHDPOLYLINEFIGURETOOL_H

#include "dd/wxhotdraw/tools/wxhdFigureTool.h"


class wxhdPolyLineFigureTool : public wxhdFigureTool
{
public:
	wxhdPolyLineFigureTool(wxhdDrawingEditor *editor, wxhdIFigure *fig, wxhdITool *dt);
	~wxhdPolyLineFigureTool();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
protected:
private:
};
#endif
