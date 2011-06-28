//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCreationTool.h - A Tool that create a figure by just click on view
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCREATIONTOOL_H
#define WXHDCREATIONTOOL_H

#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"


class wxhdCreationTool : public wxhdAbstractTool
{
public:
	wxhdCreationTool(wxhdDrawingEditor *editor, wxhdIFigure *prototype);
	~wxhdCreationTool();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void activate();
	virtual void deactivate();
	virtual void setPrototype(wxhdIFigure *prototype);
	virtual wxhdIFigure *getPrototype();

protected:
	wxhdIFigure *figurePrototype;


};
#endif
