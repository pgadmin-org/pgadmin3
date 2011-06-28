//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractTool.h - An abstract tool to allow creation of all tools
//
//////////////////////////////////////////////////////////////////////////

#ifndef wxhdAbstractTool_H
#define wxhdAbstractTool_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"


class wxhdAbstractTool : public wxhdITool
{
public:
	wxhdAbstractTool(wxhdDrawingEditor *editor);
	~wxhdAbstractTool();

	void setAnchorCoords(int x, int y);

	virtual void mouseDown(wxhdMouseEvent &event);  //Mouse Right Click
	virtual void mouseUp(wxhdMouseEvent &event);
	virtual void mouseMove(wxhdMouseEvent &event);
	virtual void mouseDrag(wxhdMouseEvent &event);
	virtual void keyDown(wxhdKeyEvent &event);
	virtual void keyUp(wxhdKeyEvent &event);
	wxhdDrawingEditor *getDrawingEditor();

protected:
	int anchorX, anchorY;
private:
	wxhdDrawingEditor *ownerEditor;

};
#endif
