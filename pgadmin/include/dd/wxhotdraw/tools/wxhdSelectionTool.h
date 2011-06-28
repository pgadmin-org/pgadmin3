//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdSelectionTool.h - Tool to allow selection of figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDSELECTIONTOOL_H
#define WXHDSELECTIONTOOL_H

#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"

class wxhdSelectionTool : public wxhdAbstractTool
{
public:
	wxhdSelectionTool(wxhdDrawingEditor *owner);
	~wxhdSelectionTool();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseMove(wxhdMouseEvent &event);
	virtual void mouseDrag(wxhdMouseEvent &event);
	virtual void keyDown(wxhdKeyEvent &event);
	virtual void keyUp(wxhdKeyEvent &event);
	void setDelegateTool(wxhdITool *tool);
	wxhdITool *getDelegateTool();

protected:
	wxhdITool *_delegateTool;
private:
	void deleteAllFigures(wxhdDrawingView *view);
};
#endif
