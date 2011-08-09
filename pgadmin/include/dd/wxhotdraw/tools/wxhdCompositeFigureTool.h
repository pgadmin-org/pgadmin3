//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCompositeFigureTool.h - A Tool that allow to change between all tools in a composite figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCOMPOSITEFIGURETOOL_H
#define WXHDCOMPOSITEFIGURETOOL_H

#include "dd/wxhotdraw/tools/wxhdFigureTool.h"


class wxhdCompositeFigureTool : public wxhdFigureTool
{
public:
	wxhdCompositeFigureTool(wxhdDrawingView *view, wxhdIFigure *fig, wxhdITool *dt);
	~wxhdCompositeFigureTool();
	virtual void setDefaultTool(wxhdITool *dt);
	virtual wxhdITool *getDefaultTool();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void activate(wxhdDrawingView *view);
	virtual void deactivate(wxhdDrawingView *view);
	virtual void setDelegateTool(wxhdDrawingView *view, wxhdITool *tool);
	virtual wxhdITool *getDelegateTool();
protected:
	wxhdITool *delegateTool;
private:
};
#endif
