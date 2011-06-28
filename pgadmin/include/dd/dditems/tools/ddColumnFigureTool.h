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
	ddColumnFigureTool(wxhdDrawingEditor *editor, wxhdIFigure *fig, wxhdITool *dt);
	~ddColumnFigureTool();
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
