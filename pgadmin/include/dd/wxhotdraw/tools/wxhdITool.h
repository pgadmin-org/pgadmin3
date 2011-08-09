//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdITool.h - Base class for all tools
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDITOOL_H
#define WXHDITOOL_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/utilities/wxhdMouseEvent.h"
#include "dd/wxhotdraw/utilities/wxhdKeyEvent.h"


class wxhdITool : public wxhdObject
{
public:
	wxhdITool();
	~wxhdITool();
	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseMove(wxhdMouseEvent &event);
	virtual void mouseDrag(wxhdMouseEvent &event);
	virtual void keyDown(wxhdKeyEvent &event);
	virtual void keyUp(wxhdKeyEvent &event);
	virtual void activate(wxhdDrawingView *view);
	virtual void deactivate(wxhdDrawingView *view);
	virtual bool activated();
	virtual bool undoable();

protected:

	bool activatedValue;
	bool undoableValue;

};
#endif
