//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdIHandle.cpp - Base class for all Handles
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDIHANDLE_H
#define HDIHANDLE_H

#include "wx/dcbuffer.h"
#include "hotdraw/main/hdObject.h"
#include "hotdraw/utilities/hdMultiPosRect.h"
#include "hotdraw/utilities/hdPoint.h"

class hdDrawingView;
class hdIFigure;
class hdMouseEvent;

class hdIHandle : public hdObject
{
public:
	hdIHandle(hdIFigure *owner);
	~hdIHandle();

	static const int size = 4;

	virtual bool containsPoint(int posIdx, int x, int y);
	virtual void draw(wxBufferedDC &context, hdDrawingView *view) = 0;
	virtual hdPoint &locate(int posIdx) = 0;
	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view) = 0;
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view) = 0;
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view) = 0;
	virtual wxCursor createCursor() = 0;
	virtual hdRect &getDisplayBox(int posIdx);
protected:
	virtual hdIFigure *getOwner();
	hdRect displayBox;
private:
	hdIFigure *figureOwner;
	double lineWidth;

};
#endif
