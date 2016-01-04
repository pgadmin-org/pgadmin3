//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLineTerminal.h - Base class for line terminal figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDLINETERMINAL_H
#define HDLINETERMINAL_H
#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/utilities/hdPoint.h"

class hdLineTerminal : public hdObject
{
public:
	hdLineTerminal();
	~hdLineTerminal();

	virtual hdPoint &draw (wxBufferedDC &context, hdPoint &a, hdPoint &b, hdDrawingView *view);
	virtual void setLinePen(wxPen pen);
protected:
	wxPen terminalLinePen;
private:
	hdPoint middle;

};
#endif
