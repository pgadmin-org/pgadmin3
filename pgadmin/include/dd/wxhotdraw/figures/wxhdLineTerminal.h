//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLineTerminal.h - Base class for line terminal figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDLINETERMINAL_H
#define WXHDLINETERMINAL_H
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

class wxhdLineTerminal : public wxhdObject
{
public:
	wxhdLineTerminal();
	~wxhdLineTerminal();

	virtual wxhdPoint &draw (wxBufferedDC &context, wxhdPoint &a, wxhdPoint &b, wxhdDrawingView *view);
	virtual void setLinePen(wxPen pen);
protected:
	wxPen terminalLinePen;
private:
	wxhdPoint middle;

};
#endif
