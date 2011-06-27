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

#ifndef WXHDMENUTOOL_H
#define WXHDMENUTOOL_H

#include "dd/wxhotdraw/tools/wxhdFigureTool.h"
#include "dd/wxhotdraw/figures/wxhdSimpleTextFigure.h"


class wxhdMenuTool : public wxhdFigureTool
{

public:
	wxhdMenuTool(wxhdDrawingEditor *editor, wxhdIFigure *fig, wxhdITool *dt);
    ~wxhdMenuTool();
	virtual void mouseDown(wxhdMouseEvent& event);  //Mouse Right Click
	virtual void activate();
	virtual void deactivate();
	virtual void mouseDrag(wxhdMouseEvent& event);
	virtual void OnGenericPopupClick(wxCommandEvent& event, wxhdDrawingView *view);
	//Because a bug it was move to main View class as a hack. virtual void changeHandler(wxCommandEvent& event);
protected:
private:
	wxhdAbstractMenuFigure *menuFigure;
	//Because a bug in the way wxwidgets connect events I can't declare it here, wxTextCtrl *edit, instead I do it on the view.
};
#endif
