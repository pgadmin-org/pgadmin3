//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCompositeFigure.h - Base class for all figures composite with figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCOMPOSITEFIGURE_H
#define HDCOMPOSITEFIGURE_H
#include "hotdraw/figures/hdAttributeFigure.h"

class hdCompositeFigure : public hdAttributeFigure
{
public:
	hdCompositeFigure();
	~hdCompositeFigure();
	virtual void AddPosForNewDiagram();
	virtual void RemovePosOfDiagram(int posIdx);
	virtual void basicMoveBy(int posIdx, int x, int y);
	virtual bool containsPoint(int posIdx, int x, int y);
	virtual hdIteratorBase *figuresEnumerator();
	virtual hdIteratorBase *figuresInverseEnumerator();
	virtual hdMultiPosRect &getBasicDisplayBox();
	virtual hdCollection *handlesEnumerator();
	virtual void add(hdIFigure *figure);
	virtual void remove(hdIFigure *figure);
	virtual bool includes(hdIFigure *figure);
	virtual hdIFigure *findFigure(int posIdx, int x, int y);
	virtual hdIFigure *getFigureAt(int indexOfCollection);
	virtual hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);
protected:
	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	hdCollection *figureFigures;
	hdCollection *figureHandles;

private:

};
#endif
