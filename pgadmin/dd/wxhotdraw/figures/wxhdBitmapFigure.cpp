//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCompositeFigure.cpp - Figure that draw a bitmap
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdBitmapFigure.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"


wxhdBitmapFigure::wxhdBitmapFigure(wxBitmap image)
{
	imageToDraw = image;
	this->getBasicDisplayBox().width = imageToDraw.GetWidth();
	this->getBasicDisplayBox().height = imageToDraw.GetHeight();
}

wxhdBitmapFigure::~wxhdBitmapFigure()
{
}

void wxhdBitmapFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = displayBox();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawBitmap(imageToDraw, copy.GetPosition(), true);
}

void wxhdBitmapFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	basicDraw(context, view);
}

void wxhdBitmapFigure::changeBitmap(wxBitmap image)
{
	imageToDraw = image;
	this->getBasicDisplayBox().width = imageToDraw.GetWidth();
	this->getBasicDisplayBox().height = imageToDraw.GetHeight();
}

int wxhdBitmapFigure::getWidth()
{
	return imageToDraw.GetWidth();
}

int wxhdBitmapFigure::getHeight()
{
	return imageToDraw.GetHeight();
}
