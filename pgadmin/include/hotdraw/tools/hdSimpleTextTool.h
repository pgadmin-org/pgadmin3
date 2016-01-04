//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdSimpleTextTool.h - Tool to allow edition of textTool with a double click or show a menu with a right click.
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDSIMPLETEXTTOOL_H
#define HDSIMPLETEXTTOOL_H

#include "hotdraw/tools/hdFigureTool.h"
#include "hotdraw/figures/hdSimpleTextFigure.h"

class hdSimpleTextTool : public hdFigureTool
{

public:
	hdSimpleTextTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt, bool fastEdit = true, wxString dialogCaption = wxEmptyString, wxString dialogMessage = wxEmptyString);
	~hdSimpleTextTool();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual void activate(hdDrawingView *view);
	virtual void deactivate(hdDrawingView *view);
	virtual void mouseDrag(hdMouseEvent &event);
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view);
	virtual bool callDialog(hdDrawingView *view);
	//Because a bug it was move to main View class instance as a hack. virtual void changeHandler(wxCommandEvent& event);
protected:
	bool withoutDialog;
	wxString dlgMessage, dlgCaption;
private:
	hdSimpleTextFigure *txtFigure;
	void calculateSizeEntry(hdDrawingView *view);
	bool showEdit;
	//Because a bug in the way wxwidgets connect events I can't declare it here, wxTextCtrl *edit, instead I do it on the view instance.
};

#endif
