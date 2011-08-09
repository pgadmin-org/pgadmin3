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

#ifndef DDDRAWINGVIEW_H
#define DDDRAWINGVIEW_H

#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/ddmodel/ddDrawingEditor.h"
#include "dd/ddmodel/ddDatabaseDesign.h"

class ddDrawingView : public wxhdDrawingView
{
public:
	ddDrawingView(int diagram, wxWindow *ddParent, ddDrawingEditor *editor , wxSize size, wxhdDrawing *drawing);
	//Hack To allow right click menu at canvas without a figure
	virtual void createViewMenu(wxMenu &mnu);
	virtual void OnGenericViewPopupClick(wxCommandEvent &event);
protected:
private:
};

// A drop target that do nothing only accept text, if accept then tree add table to model
class ddDropTarget : public wxTextDropTarget
{
public:
	ddDropTarget(ddDatabaseDesign *sourceDesign, wxhdDrawing *targetDrawing);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString &text);
private:
	wxhdDrawing *target;
	ddDatabaseDesign *source;
};
#endif
