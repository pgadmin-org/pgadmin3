//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCompositeFigure.h - Figure that draw a bitmap
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDBITMAPFIGURE_H
#define WXHDBITMAPFIGURE_H

#include "dd/wxhotdraw/figures/wxhdAbstractFigure.h"

class wxhdBitmapFigure : public wxhdAbstractFigure
{
public:
	wxhdBitmapFigure(wxBitmap image);
    ~wxhdBitmapFigure();

	virtual void basicDraw(wxBufferedDC& context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC& context, wxhdDrawingView *view);		
	virtual void changeBitmap(wxBitmap image);
	virtual int getWidth();
	virtual int getHeight();

protected:

private:
	wxBitmap imageToDraw;
};
#endif
