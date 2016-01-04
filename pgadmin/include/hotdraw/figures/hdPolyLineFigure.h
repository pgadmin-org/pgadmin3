//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPolyLineFigure.h - A simple line figure that can be split on several lines joined by flexibility points
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDPOLYLINEFIGURE_H
#define HDPOLYLINEFIGURE_H

#include "hotdraw/figures/hdAbstractMenuFigure.h"
#include "hotdraw/figures/hdLineTerminal.h"
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/utilities/hdPoint.h"


WX_DEFINE_ARRAY(hdArrayCollection *, pointsCollections);

class hdPolyLineFigure : public hdAbstractMenuFigure
{
public:
	hdPolyLineFigure();
	~hdPolyLineFigure();

	virtual hdMultiPosRect &getBasicDisplayBox();
	virtual int pointCount(int posIdx);
	virtual int pointLinesCount();
	virtual void AddPosForNewDiagram();
	virtual void RemovePosOfDiagram(int posIdx);
	virtual hdPoint &getStartPoint(int posIdx);
	virtual void setStartPoint(int posIdx, hdPoint point);
	virtual hdPoint &getEndPoint(int posIdx);
	virtual void setEndPoint(int posIdx, hdPoint point);
	virtual void setStartTerminal(hdLineTerminal *terminal);
	virtual hdLineTerminal *getStartTerminal();
	virtual void setEndTerminal(hdLineTerminal *terminal);
	virtual hdLineTerminal *getEndTerminal();
	hdCollection *handlesEnumerator();
	virtual int findSegment (int posIdx, int x, int y);
	virtual void splitSegment(int posIdx, int x, int y);
	virtual void changed(int posIdx);

	virtual void addPoint (int posIdx, int x, int y);
	virtual void clearPoints (int posIdx);
	virtual void insertPointAt (int posIdx, int index, int x, int y);
	virtual void setPointAt (int posIdx, int index, int x, int y);
	virtual void removePointAt (int posIdx, int index);
	virtual void basicMoveBy(int posIdx, int x, int y);
	virtual hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);

	virtual hdPoint &pointAt(int posIdx, int index);
	virtual bool containsPoint (int posIdx, int x, int y);
	virtual void setLinePen(wxPen pen);
	int countPointsAt(int posIdx);

protected:
	virtual void basicDraw (wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);

	int getMaximunIndex();
	virtual void updateHandlesIndexes();
	pointsCollections points;
	hdPoint endPoint, startPoint, pointAtPos;

private:
	hdLineTerminal *startTerminal, *endTerminal;
	bool handlesChanged;
	wxPen linePen;

};
#endif
