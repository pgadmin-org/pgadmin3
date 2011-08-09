//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdHandleTrackerTool.h - A Tool that allow to use multiple handles
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDHANDLETRACKERTOOL_H
#define WXHDHANDLETRACKERTOOL_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/tools/wxhdAbstractTool.h"


class wxhdHandleTrackerTool : public wxhdAbstractTool
{
public:
	wxhdHandleTrackerTool(wxhdDrawingView *view, wxhdIHandle *anchor);
	~wxhdHandleTrackerTool();

	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseDrag(wxhdMouseEvent &event);

protected:
	wxhdIHandle *anchorHandle;

private:

};
#endif
