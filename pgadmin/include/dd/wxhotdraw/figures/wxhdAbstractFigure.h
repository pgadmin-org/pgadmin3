//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractFigure.h - Base class for all figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDABSTRACTFIGURE_H
#define WXHDABSTRACTFIGURE_H
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/utilities/wxhdCollection.h"

class wxhdAbstractFigure : public wxhdIFigure
{
public:
	wxhdAbstractFigure();
    ~wxhdAbstractFigure();
	
	virtual bool canConnect ();
	virtual void draw(wxBufferedDC& context, wxhdDrawingView *view);
	virtual void drawSelected(wxBufferedDC& context, wxhdDrawingView *view);
	virtual bool includes(wxhdIFigure *figure);
	virtual wxhdITool* CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool);
	virtual void moveBy(int x, int y);
	virtual void basicMoveBy(int x, int y);
	virtual void moveTo(int x, int y);
	virtual bool containsPoint(int x, int y);
	virtual void onFigureChanged(wxhdIFigure *figure);

protected:
	virtual void basicDraw(wxBufferedDC& context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC& context, wxhdDrawingView *view);		
	void willChange();
	void changed();
	void invalidate();
	wxColour fillColor, lineColor;
	double lineWidth;
	wxSize spaceForMovement;

private:

};
#endif
