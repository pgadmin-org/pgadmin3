//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnFigureTool.h - Improvement to hdFigureTool to work with composite table figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDCOLUMNFIGURETOOL_H
#define DDCOLUMNFIGURETOOL_H

#include "hotdraw/tools/hdFigureTool.h"


class ddColumnFigureTool : public hdFigureTool
{
public:
	ddColumnFigureTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt);
	~ddColumnFigureTool();
	virtual void setDefaultTool(hdITool *dt);
	virtual hdITool *getDefaultTool();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void activate(hdDrawingView *view);
	virtual void deactivate(hdDrawingView *view);
	virtual void setDelegateTool(hdDrawingView *view, hdITool *tool);
	virtual hdITool *getDelegateTool();
protected:
	hdITool *delegateTool;
private:
};
#endif
