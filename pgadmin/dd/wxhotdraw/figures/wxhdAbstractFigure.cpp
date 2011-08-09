//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractFigure.cpp - Base class for all figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdAbstractFigure.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

wxhdAbstractFigure::wxhdAbstractFigure()
{
	spaceForMovement.SetHeight(0);
	spaceForMovement.SetWidth(0);
}

wxhdAbstractFigure::~wxhdAbstractFigure()
{
}

bool wxhdAbstractFigure::canConnect ()
{
	return true;
}

bool wxhdAbstractFigure::includes(wxhdIFigure *figure)
{
	return (this == figure);
}

void wxhdAbstractFigure::draw(wxBufferedDC &context, wxhdDrawingView *view)
{
	//Hack to Allow creations of limits for figures movements, but what to do should be defined at derivated classes
	spaceForMovement = view->canvasSize;
	wxhdIFigure::draw(context, view);
	basicDraw(context, view);
}

void wxhdAbstractFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = displayBox().getwxhdRect(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

	context.SetPen(*wxGREEN_PEN);
	context.SetBrush(wxBrush (wxColour(208, 208, 208), wxSOLID));
	context.DrawRectangle(copy);
}

void wxhdAbstractFigure::drawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdIFigure::drawSelected(context, view);
	basicDrawSelected(context, view);
}

void wxhdAbstractFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = displayBox().getwxhdRect(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);

	context.SetPen(*wxRED_PEN);
	context.SetBrush(wxBrush (wxColour(133, 133, 133), wxSOLID));
	context.DrawRectangle(copy);
}


wxhdITool *wxhdAbstractFigure::CreateFigureTool(wxhdDrawingView *view, wxhdITool *defaultTool)
{
	return defaultTool;
}

void wxhdAbstractFigure::moveBy(int posIdx, int x, int y)
{
	willChange();
	basicMoveBy(posIdx, x, y);
	changed(posIdx);
}

void wxhdAbstractFigure::basicMoveBy(int posIdx, int x, int y)
{
	basicDisplayBox.x[posIdx] += x;
	basicDisplayBox.y[posIdx] += y;
}


void wxhdAbstractFigure::moveTo(int posIdx, int x, int y)
{
	basicDisplayBox.x[posIdx] = x;
	basicDisplayBox.y[posIdx] = y;
}

void wxhdAbstractFigure::willChange()
{
	invalidate();
}

void wxhdAbstractFigure::changed(int posIdx)
{
	invalidate();
	onFigureChanged(posIdx, this);
}

void wxhdAbstractFigure::invalidate()
{

}

bool wxhdAbstractFigure::containsPoint(int posIdx, int x, int y)
{
	return basicDisplayBox.Contains(posIdx, x, y);
}

void wxhdAbstractFigure::onFigureChanged(int posIdx, wxhdIFigure *figure)
{
	//go to figure procedure to alert observers of changes on this figure
	wxhdIFigure::onFigureChanged(posIdx, figure);
}