//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGraphsimple.h - A simple Implementation of the Graphic Interface for GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBGRAPHSIMPLE_H
#define GQBGRAPHSIMPLE_H

#include <wx/dcbuffer.h>

// App headers
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbGraphBehavior.h"

// Create Array Objects used as base for gqbCollections
class gqbGraphSimple : public gqbGraphBehavior
{
public:
	gqbGraphSimple();
	void drawTable(wxMemoryDC &bdc, wxPoint *origin, gqbQueryObject *queryTable);
	void drawTempJoinLine(wxMemoryDC &bdc, wxPoint &origin, wxPoint &end);
	void calcAnchorPoint(gqbQueryJoin *join);
	void drawJoin(wxMemoryDC &bdc, wxPoint &origin, wxPoint &dest, wxPoint &anchorUsed, bool selected, type_Join joinKind);
	void UpdatePosObject(gqbQueryObject *queryTable, int x, int y, int cursorAdjustment);
	gqbColumn *getColumnAtPosition(wxPoint *clickPoint, gqbQueryObject *queryTable, int sensibility = 17);
	bool clickOnJoin(gqbQueryJoin *join, wxPoint &pt, wxPoint &origin, wxPoint &dest);
	int getTitleRowHeight();

private:
	wxFont normalFont, TableTitleFont;
	wxBrush BackgroundLayer1, BackgroundLayer2, BackgroundTitle, selectedBrush;
	int minTableWidth, minTableHeight;
	int rowHeight, rowLeftMargin, rowRightMargin, rowTopMargin, lineClickThreshold;
	wxPen selectedPen;
	wxBitmap imgSelBoxEmpty, imgSelBoxSelected;
	bool insideLine(wxPoint &pt, wxPoint &p1, wxPoint &p2, int threshold);
	double distanceToLine(wxPoint pt, wxPoint p1, wxPoint p2);
	wxPoint findLineMiddle(wxPoint p1, wxPoint p2);
};
#endif
