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
	wxhdCompositeFigureTool(wxhdDrawingEditor *editor, wxhdIFigure *fig, wxhdITool *dt);
	~wxhdCompositeFigureTool();
	virtual void setDefaultTool(wxhdITool *dt);
	virtual wxhdITool *getDefaultTool();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void activate();
	virtual void deactivate();
	virtual void setDelegateTool(wxhdITool *tool);
	virtual wxhdITool *getDelegateTool();
protected:
	wxhdITool *delegateTool;
private:
};
#endif
