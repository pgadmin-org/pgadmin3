//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.h - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDDRAWINGEDITOR_H
#define WXHDDRAWINGEDITOR_H

#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"

class wxhdDrawingEditor : public wxhdObject
{
public:
	wxhdDrawingEditor(wxWindow *owner, bool defaultView = true );
    ~wxhdDrawingEditor();
	wxhdDrawingView* view();
	wxhdDrawing* model();
	wxhdITool* tool();
	void setTool(wxhdITool* tool);
	virtual void createView(wxWindow *owner);
	//Hack To allow right click menu at canvas without a figure
	virtual void createMenu(wxMenu &mnu);
	virtual void OnGenericPopupClick(wxCommandEvent& event, wxhdDrawingView *view);

protected:
	wxhdDrawingView *_view;
	wxhdDrawing *_model;
	wxhdITool *_tool;
private:
};
#endif
