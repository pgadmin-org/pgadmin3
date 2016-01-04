//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdMenuTool.h - Allow Edition of textTool (double click) or show a menu to modifiy in someway text (right click).
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCANVASMENUTOOL_H
#define HDCANVASMENUTOOL_H

#include "hotdraw/tools/hdAbstractTool.h"

class hdCanvasMenuTool : public hdAbstractTool
{

public:
	hdCanvasMenuTool(hdDrawingView *view, hdITool *dt);
	~hdCanvasMenuTool();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void mouseDrag(hdMouseEvent &event);
	virtual void mouseUp(hdMouseEvent &event);
	virtual void mouseMove(hdMouseEvent &event);
	virtual void createViewMenu(hdDrawingView *view, wxMenu &mnu);
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view);
	//Because a bug it was move to main View class as a hack. virtual void changeHandler(wxCommandEvent& event);
protected:
	hdITool *defaultTool;
private:
};
#endif
