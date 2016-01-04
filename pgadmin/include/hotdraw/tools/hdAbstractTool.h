//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractTool.h - An abstract tool to allow creation of all tools
//
//////////////////////////////////////////////////////////////////////////

#ifndef hdAbstractTool_H
#define hdAbstractTool_H

#include "hotdraw/main/hdObject.h"
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/main/hdDrawingEditor.h"


class hdAbstractTool : public hdITool
{
public:
	hdAbstractTool(hdDrawingView *view);
	~hdAbstractTool();

	void setAnchorCoords(int x, int y);

	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(hdMouseEvent &event);
	virtual void mouseMove(hdMouseEvent &event);
	virtual void mouseDrag(hdMouseEvent &event);
	virtual void keyDown(hdKeyEvent &event);
	virtual void keyUp(hdKeyEvent &event);
	hdDrawingView *getDrawingView();

protected:
	int anchorX, anchorY;
	hdDrawingView *ownerView;
private:

};
#endif
