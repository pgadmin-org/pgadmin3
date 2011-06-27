//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineFigure.h - A simple line figure that can be split on several lines joined by flexibility points
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDPOLYLINEFIGURE_H
#define WXHDPOLYLINEFIGURE_H

#include "dd/wxhotdraw/figures/wxhdAbstractMenuFigure.h"
#include "dd/wxhotdraw/figures/wxhdLineTerminal.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

class wxhdPolyLineFigure : public wxhdAbstractMenuFigure
{
public:
	wxhdPolyLineFigure();
    ~wxhdPolyLineFigure();

	virtual wxhdRect& getBasicDisplayBox();
	virtual int pointCount();
	virtual wxhdPoint& getStartPoint();
	virtual void setStartPoint(wxhdPoint point);
	virtual wxhdPoint& getEndPoint();
	virtual void setEndPoint(wxhdPoint point);
	virtual void setStartTerminal(wxhdLineTerminal *terminal);
	virtual wxhdLineTerminal* getStartTerminal();
	virtual void setEndTerminal(wxhdLineTerminal *terminal);
	virtual wxhdLineTerminal* getEndTerminal();
	wxhdCollection* handlesEnumerator();
	virtual int findSegment (int x, int y);
	virtual void splitSegment(int x, int y);
	virtual void changed();

	virtual void addPoint (int x, int y);
	virtual void clearPoints ();
	virtual void insertPointAt (int index, int x, int y);
	virtual void setPointAt (int index, int x, int y);
	virtual void removePointAt (int index);
	virtual void basicMoveBy(int x, int y);
	virtual wxhdITool* CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool);

	virtual wxhdPoint& pointAt(int index);
	virtual bool containsPoint (int x, int y);
	virtual void setLinePen(wxPen pen);

protected:
	virtual void basicDraw (wxBufferedDC& context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC& context, wxhdDrawingView *view);

	virtual void updateHandlesIndexes();
	wxhdArrayCollection *points;
	wxhdPoint endPoint, startPoint, pointAtPos;

private:
	wxhdLineTerminal *startTerminal, *endTerminal;
	bool handlesChanged;
	wxPen linePen;

};
#endif
