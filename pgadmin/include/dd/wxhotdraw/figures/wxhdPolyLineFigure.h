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


WX_DEFINE_ARRAY(wxhdArrayCollection *, pointsCollections);

class wxhdPolyLineFigure : public wxhdAbstractMenuFigure
{
public:
	wxhdPolyLineFigure();
	~wxhdPolyLineFigure();

	virtual wxhdMultiPosRect &getBasicDisplayBox();
	virtual int pointCount(int posIdx);
	virtual int pointLinesCount();
	virtual void AddPosForNewDiagram();
	virtual void RemovePosOfDiagram(int posIdx);
	virtual wxhdPoint &getStartPoint(int posIdx);
	virtual void setStartPoint(int posIdx, wxhdPoint point);
	virtual wxhdPoint &getEndPoint(int posIdx);
	virtual void setEndPoint(int posIdx, wxhdPoint point);
	virtual void setStartTerminal(wxhdLineTerminal *terminal);
	virtual wxhdLineTerminal *getStartTerminal();
	virtual void setEndTerminal(wxhdLineTerminal *terminal);
	virtual wxhdLineTerminal *getEndTerminal();
	wxhdCollection *handlesEnumerator();
	virtual int findSegment (int posIdx, int x, int y);
	virtual void splitSegment(int posIdx, int x, int y);
	virtual void changed(int posIdx);

	virtual void addPoint (int posIdx, int x, int y);
	virtual void clearPoints (int posIdx);
	virtual void insertPointAt (int posIdx, int index, int x, int y);
	virtual void setPointAt (int posIdx, int index, int x, int y);
	virtual void removePointAt (int posIdx, int index);
	virtual void basicMoveBy(int posIdx, int x, int y);
	virtual wxhdITool *CreateFigureTool(wxhdDrawingView *view, wxhdITool *defaultTool);

	virtual wxhdPoint &pointAt(int posIdx, int index);
	virtual bool containsPoint (int posIdx, int x, int y);
	virtual void setLinePen(wxPen pen);
	int countPointsAt(int posIdx);

protected:
	virtual void basicDraw (wxBufferedDC &context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view);

	int getMaximunIndex();
	virtual void updateHandlesIndexes();
	pointsCollections points;
	wxhdPoint endPoint, startPoint, pointAtPos;

private:
	wxhdLineTerminal *startTerminal, *endTerminal;
	bool handlesChanged;
	wxPen linePen;

};
#endif
