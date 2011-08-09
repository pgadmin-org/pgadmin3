//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdMenuTool.h - Allow Edition of textTool (double click) or show a menu to modifiy in someway text (right click).
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCANVASMENUTOOL_H
#define WXHDCANVASMENUTOOL_H

#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"

class wxhdCanvasMenuTool : public wxhdAbstractTool
{

public:
	wxhdCanvasMenuTool(wxhdDrawingView *view, wxhdITool *dt);
	~wxhdCanvasMenuTool();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseDrag(wxhdMouseEvent &event);
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseMove(wxhdMouseEvent &event);
	virtual void createViewMenu(wxhdDrawingView *view, wxMenu &mnu);
	virtual void OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view);
	//Because a bug it was move to main View class as a hack. virtual void changeHandler(wxCommandEvent& event);
protected:
	wxhdITool *defaultTool;
private:
};
#endif
