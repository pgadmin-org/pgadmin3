//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCompositeFigure.h - Figure that draw a bitmap
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDBITMAPFIGURE_H
#define HDBITMAPFIGURE_H

#include "hotdraw/figures/hdAbstractFigure.h"

class hdBitmapFigure : public hdAbstractFigure
{
public:
	hdBitmapFigure(wxBitmap image);
	~hdBitmapFigure();

	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	virtual void changeBitmap(wxBitmap image);
	virtual int getWidth();
	virtual int getHeight();

protected:

private:
	wxBitmap imageToDraw;
};
#endif
