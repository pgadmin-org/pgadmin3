//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCompositeFigure.cpp - Figure that draw a bitmap
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/figures/hdBitmapFigure.h"
#include "hotdraw/main/hdDrawingView.h"


hdBitmapFigure::hdBitmapFigure(wxBitmap image)
{
	imageToDraw = image;
	this->getBasicDisplayBox().width = imageToDraw.GetWidth();
	this->getBasicDisplayBox().height = imageToDraw.GetHeight();
}

hdBitmapFigure::~hdBitmapFigure()
{
}

void hdBitmapFigure::basicDraw(wxBufferedDC &context, hdDrawingView *view)
{
	hdRect copy = displayBox().gethdRect(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawBitmap(imageToDraw, copy.GetPosition(), true);
}

void hdBitmapFigure::basicDrawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	basicDraw(context, view);
}

void hdBitmapFigure::changeBitmap(wxBitmap image)
{
	imageToDraw = image;
	//Index doesn't care because width and height are shared in a MultiPosRect
	this->getBasicDisplayBox().width = imageToDraw.GetWidth();
	this->getBasicDisplayBox().height = imageToDraw.GetHeight();
}

int hdBitmapFigure::getWidth()
{
	return imageToDraw.GetWidth();
}

int hdBitmapFigure::getHeight()
{
	return imageToDraw.GetHeight();
}
