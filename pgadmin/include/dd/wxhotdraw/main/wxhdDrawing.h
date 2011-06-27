//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawing.h - Main storage class for all objects of the model
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDDRAWING_H
#define WXHDDRAWING_H

#include "dd/wxhotdraw/figures/wxhdIFigure.h"

// Main model of drawing
class wxhdDrawing : public wxObject
{
public:
	wxhdDrawing();
	virtual ~wxhdDrawing();
	virtual void add(wxhdIFigure *figure);
	virtual void remove(wxhdIFigure *figure);
	virtual bool includes(wxhdIFigure *figure);
	virtual wxhdIFigure* findFigure(int x, int y);
	virtual void recalculateDisplayBox();
	virtual void bringToFront(wxhdIFigure *figure);
	virtual void sendToBack(wxhdIFigure *figure);
	virtual wxhdRect& DisplayBox();
	virtual wxhdIteratorBase* figuresEnumerator();
	virtual wxhdIteratorBase* figuresInverseEnumerator();
	virtual void deleteAllFigures();

protected:

private:
	wxhdCollection *figures;
	wxhdCollection *handles;
	wxhdRect displayBox;
};
#endif
