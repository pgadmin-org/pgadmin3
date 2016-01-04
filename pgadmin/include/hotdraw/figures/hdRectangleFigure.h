//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdRectangleFigure.h - A simple rectangle  figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDRECTANGLEFIGURE_H
#define HDRECTANGLEFIGURE_H

#include <wx/dcbuffer.h>

#include "hotdraw/figures/hdAbstractFigure.h"


class hdDrawingView;

class hdRectangleFigure : public hdAbstractFigure
{
public:
	hdRectangleFigure();
	~hdRectangleFigure();
	void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	void setRectangle(hdMultiPosRect &rect);
	void setSize(wxSize &size);

protected:
private:
};
#endif
