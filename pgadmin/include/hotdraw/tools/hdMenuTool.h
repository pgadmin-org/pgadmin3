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

#ifndef HDMENUTOOL_H
#define HDMENUTOOL_H

#include "hotdraw/tools/hdFigureTool.h"
#include "hotdraw/figures/hdSimpleTextFigure.h"


class hdMenuTool : public hdFigureTool
{

public:
	hdMenuTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt);
	~hdMenuTool();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void activate(hdDrawingView *view);
	virtual void deactivate(hdDrawingView *view);
	virtual void mouseDrag(hdMouseEvent &event);
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view);
	//Because a bug it was move to main View class as a hack. virtual void changeHandler(wxCommandEvent& event);
protected:
private:
	hdAbstractMenuFigure *menuFigure;
	//Because a bug in the way wxwidgets connect events I can't declare it here, wxTextCtrl *edit, instead I do it on the view.
};
#endif
