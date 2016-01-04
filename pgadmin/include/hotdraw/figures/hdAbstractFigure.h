//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractFigure.h - Base class for all figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDABSTRACTFIGURE_H
#define HDABSTRACTFIGURE_H
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/utilities/hdCollection.h"

class hdAbstractFigure : public hdIFigure
{
public:
	hdAbstractFigure();
	~hdAbstractFigure();

	virtual bool canConnect ();
	virtual void draw(wxBufferedDC &context, hdDrawingView *view);
	virtual void drawSelected(wxBufferedDC &context, hdDrawingView *view);
	virtual bool includes(hdIFigure *figure);
	virtual hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);
	virtual void moveBy(int posIdx, int x, int y);
	virtual void basicMoveBy(int posIdx, int x, int y);
	virtual void moveTo(int posIdx, int x, int y);
	virtual bool containsPoint(int posIdx, int x, int y);
	virtual void onFigureChanged(int posIdx, hdIFigure *figure);

protected:
	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	void willChange();
	void changed(int posIdx);
	void invalidate();
	wxColour fillColor, lineColor;
	double lineWidth;
	wxSize spaceForMovement;

private:

};
#endif
