//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCreationTool.h - A Tool that create a figure by just click on view
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCREATIONTOOL_H
#define HDCREATIONTOOL_H

#include "hotdraw/tools/hdAbstractTool.h"
#include "hotdraw/figures/hdIFigure.h"


class hdCreationTool : public hdAbstractTool
{
public:
	hdCreationTool(hdDrawingView *view, hdIFigure *prototype);
	~hdCreationTool();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(hdMouseEvent &event);
	virtual void activate(hdDrawingView *view);
	virtual void deactivate(hdDrawingView *view);
	virtual void setPrototype(hdIFigure *prototype);
	virtual hdIFigure *getPrototype();

protected:
	hdIFigure *figurePrototype;


};
#endif
