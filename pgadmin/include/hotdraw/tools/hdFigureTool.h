//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdFigureTool.h - Base class for all figure tools
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDFIGURETOOL_H
#define HDFIGURETOOL_H

#include "hotdraw/tools/hdAbstractTool.h"


class hdFigureTool : public hdAbstractTool
{
public:
	hdFigureTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt);
	~hdFigureTool();
	void setDefaultTool(hdITool *dt);
	hdITool *getDefaultTool();
	void setFigure(hdIFigure *fig);
	hdIFigure *getFigure();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(hdMouseEvent &event);
	virtual void mouseMove(hdMouseEvent &event);
	virtual void mouseDrag(hdMouseEvent &event);
	virtual void keyDown(hdKeyEvent &event);
	virtual void keyUp(hdKeyEvent &event);
protected:
	hdITool *defaultTool;
	hdIFigure *figure;
private:
};
#endif
