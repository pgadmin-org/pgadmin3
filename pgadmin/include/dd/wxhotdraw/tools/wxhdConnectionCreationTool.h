//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdConnectionCreationTool.h - A Tool that allow to create a connection figure between two figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCONNECTIONCREATIONTOOL_H
#define WXHDCONNECTIONCREATIONTOOL_H

#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/figures/wxhdLineConnection.h"

class wxhdConnectionCreationTool : public wxhdAbstractTool
{
public:
	wxhdConnectionCreationTool(wxhdDrawingView *view, wxhdLineConnection *figure);
	~wxhdConnectionCreationTool();

	virtual void mouseDrag(wxhdMouseEvent &event);
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseMove(wxhdMouseEvent &event);
protected:

private:
	wxhdLineConnection *toolConnection;
	wxhdIHandle *handle;
	int numClicks;
	bool dragged;

};
#endif
