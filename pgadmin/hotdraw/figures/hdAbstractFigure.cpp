//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractFigure.cpp - Base class for all figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "hotdraw/figures/hdAbstractFigure.h"
#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/main/hdDrawingView.h"

hdAbstractFigure::hdAbstractFigure()
{
	spaceForMovement.SetHeight(0);
	spaceForMovement.SetWidth(0);
}

hdAbstractFigure::~hdAbstractFigure()
{
}

bool hdAbstractFigure::canConnect ()
{
	return true;
}

bool hdAbstractFigure::includes(hdIFigure *figure)
{
	return (this == figure);
}

void hdAbstractFigure::draw(wxBufferedDC &context, hdDrawingView *view)
{
	//Hack to Allow creations of limits for figures movements, but what to do should be defined at derivated classes
	spaceForMovement = view->canvasSize;
	hdIFigure::draw(context, view);
	basicDraw(context, view);
}

void hdAbstractFigure::basicDraw(wxBufferedDC &context, hdDrawingView *view)
{
	hdRect copy = displayBox().gethdRect(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

	context.SetPen(*wxGREEN_PEN);
	context.SetBrush(wxBrush (wxColour(208, 208, 208), wxSOLID));
	context.DrawRectangle(copy);
}

void hdAbstractFigure::drawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	hdIFigure::drawSelected(context, view);
	basicDrawSelected(context, view);
}

void hdAbstractFigure::basicDrawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	hdRect copy = displayBox().gethdRect(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

	context.SetPen(*wxRED_PEN);
	context.SetBrush(wxBrush (wxColour(133, 133, 133), wxSOLID));
	context.DrawRectangle(copy);
}


hdITool *hdAbstractFigure::CreateFigureTool(hdDrawingView *view, hdITool *defaultTool)
{
	return defaultTool;
}

void hdAbstractFigure::moveBy(int posIdx, int x, int y)
{
	willChange();
	basicMoveBy(posIdx, x, y);
	changed(posIdx);
}

void hdAbstractFigure::basicMoveBy(int posIdx, int x, int y)
{
	basicDisplayBox.x[posIdx] += x;
	basicDisplayBox.y[posIdx] += y;
}


void hdAbstractFigure::moveTo(int posIdx, int x, int y)
{
	basicDisplayBox.x[posIdx] = x;
	basicDisplayBox.y[posIdx] = y;
}

void hdAbstractFigure::willChange()
{
	invalidate();
}

void hdAbstractFigure::changed(int posIdx)
{
	invalidate();
	onFigureChanged(posIdx, this);
}

void hdAbstractFigure::invalidate()
{

}

bool hdAbstractFigure::containsPoint(int posIdx, int x, int y)
{
	return basicDisplayBox.Contains(posIdx, x, y);
}

void hdAbstractFigure::onFigureChanged(int posIdx, hdIFigure *figure)
{
	//go to figure procedure to alert observers of changes on this figure
	hdIFigure::onFigureChanged(posIdx, figure);
}
