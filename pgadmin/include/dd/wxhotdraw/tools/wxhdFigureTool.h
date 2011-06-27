//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdFigureTool.h - Base class for all figure tools
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDFIGURETOOL_H
#define WXHDFIGURETOOL_H

#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"


class wxhdFigureTool : public wxhdAbstractTool
{
public:
	wxhdFigureTool(wxhdDrawingEditor *editor, wxhdIFigure *fig, wxhdITool *dt);
    ~wxhdFigureTool();
	void setDefaultTool(wxhdITool *dt);
	wxhdITool* getDefaultTool();
	void setFigure(wxhdIFigure *fig);
	wxhdIFigure* getFigure();
	virtual void mouseDown(wxhdMouseEvent& event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent& event);
	virtual void mouseMove(wxhdMouseEvent& event);
	virtual void mouseDrag(wxhdMouseEvent& event);
	virtual void keyDown(wxhdKeyEvent& event);
	virtual void keyUp(wxhdKeyEvent& event);
protected:
	wxhdITool *defaultTool;
	wxhdIFigure *figure;
private:
};
#endif
