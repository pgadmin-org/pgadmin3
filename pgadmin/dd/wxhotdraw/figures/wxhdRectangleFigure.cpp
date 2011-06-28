//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdRectangleFigure.cpp - A simple rectangle  figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdRectangleFigure.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

wxhdRectangleFigure::wxhdRectangleFigure()
{
}

wxhdRectangleFigure::~wxhdRectangleFigure()
{
}

void wxhdRectangleFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = displayBox();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawRectangle(copy);
}

void wxhdRectangleFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = displayBox();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawRectangle(copy);
}

void wxhdRectangleFigure::setRectangle(wxhdRect &rect)
{
	basicDisplayBox = rect;
}

void wxhdRectangleFigure::setSize(wxSize &size)
{
	basicDisplayBox.SetSize(size);
}
