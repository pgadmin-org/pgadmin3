//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdSelectionTool.h - Tool to allow selection of figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDSELECTIONTOOL_H
#define HDSELECTIONTOOL_H

#include "hotdraw/tools/hdITool.h"
#include "hotdraw/tools/hdAbstractTool.h"

class hdSelectionTool : public hdAbstractTool
{
public:
	hdSelectionTool(hdDrawingView *view);
	~hdSelectionTool();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(hdMouseEvent &event);
	virtual void mouseMove(hdMouseEvent &event);
	virtual void mouseDrag(hdMouseEvent &event);
	virtual void keyDown(hdKeyEvent &event);
	virtual void keyUp(hdKeyEvent &event);
	void setDelegateTool(hdDrawingView *view, hdITool *tool);
	hdITool *getDelegateTool();

protected:
	hdITool *_delegateTool;
private:
	void deleteAllFigures(hdDrawingView *view);
};
#endif
