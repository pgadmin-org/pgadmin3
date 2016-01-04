//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdConnectionCreationTool.h - A Tool that allow to create a connection figure between two figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCONNECTIONCREATIONTOOL_H
#define HDCONNECTIONCREATIONTOOL_H

#include "hotdraw/tools/hdAbstractTool.h"
#include "hotdraw/main/hdDrawingEditor.h"
#include "hotdraw/figures/hdLineConnection.h"

class hdConnectionCreationTool : public hdAbstractTool
{
public:
	hdConnectionCreationTool(hdDrawingView *view, hdLineConnection *figure);
	~hdConnectionCreationTool();

	virtual void mouseDrag(hdMouseEvent &event);
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(hdMouseEvent &event);
	virtual void mouseMove(hdMouseEvent &event);
protected:

private:
	hdLineConnection *toolConnection;
	hdIHandle *handle;
	int numClicks;
	bool dragged;

};
#endif
