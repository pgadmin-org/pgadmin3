//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdRectangleFigure.h - A simple rectangle  figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDRECTANGLEFIGURE_H
#define WXHDRECTANGLEFIGURE_H

#include <wx/dcbuffer.h>

#include "dd/wxhotdraw/figures/wxhdAbstractFigure.h"


class wxhdDrawingView;

class wxhdRectangleFigure : public wxhdAbstractFigure
{
public:
	wxhdRectangleFigure();
	~wxhdRectangleFigure();
	void basicDraw(wxBufferedDC &context, wxhdDrawingView *view);
	void basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view);
	void setRectangle(wxhdRect &rect);
	void setSize(wxSize &size);

protected:
private:
};
#endif
