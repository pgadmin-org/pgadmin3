//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDrawingEditor.h - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDDRAWINGVIEW_H
#define DDDRAWINGVIEW_H

#include "hotdraw/main/hdDrawingView.h"
#include "dd/ddmodel/ddDrawingEditor.h"
#include "dd/ddmodel/ddDatabaseDesign.h"

class ddDrawingView : public hdDrawingView
{
public:
	ddDrawingView(int diagram, wxWindow *ddParent, ddDrawingEditor *editor , wxSize size, hdDrawing *drawing);
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
	ddDropTarget(ddDatabaseDesign *sourceDesign, hdDrawing *targetDrawing);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString &text);
private:
	hdDrawing *target;
	ddDatabaseDesign *source;
};
#endif
