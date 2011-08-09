//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnFigureTool.h - Improvement to wxhdFigureTool to work with composite table figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDCOLUMNFIGURETOOL_H
#define DDCOLUMNFIGURETOOL_H

#include "dd/wxhotdraw/tools/wxhdFigureTool.h"


class ddColumnFigureTool : public wxhdFigureTool
{
public:
	ddColumnFigureTool(wxhdDrawingView *view, wxhdIFigure *fig, wxhdITool *dt);
	~ddColumnFigureTool();
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
