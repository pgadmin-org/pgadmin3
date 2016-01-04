//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGraphsimple.h - A simple Graphic Interface for GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBGRAPHBEHAVIOR_H
#define GQBGRAPHBEHAVIOR_H

#include <wx/dcbuffer.h>

// App headers
#include "gqb/gqbQueryObjs.h"

// Interface class for drawing of objects in canvas
class gqbGraphBehavior : public wxObject
{
public:
	// Important: The drawTable function always should store the width & height of the graphic
	// representation of the table inside the gqbQueryObject for use of controller.
	virtual void drawTable(wxMemoryDC &bdc, wxPoint *origin, gqbQueryObject *queryTable) = 0;
	virtual void drawTempJoinLine(wxMemoryDC &bdc, wxPoint &origin, wxPoint &end) = 0;
	virtual void drawJoin(wxMemoryDC &bdc, wxPoint &origin, wxPoint &dest, wxPoint &anchorUsed, bool selected, type_Join joinKind) = 0;
	virtual void calcAnchorPoint(gqbQueryJoin *join) = 0;
	virtual void UpdatePosObject(gqbQueryObject *queryTable, int x, int y, int cursorAdjustment) = 0;

	// GQB-TODO find a way to not hard code the 17 default value
	virtual gqbColumn *getColumnAtPosition(wxPoint *clickPoint, gqbQueryObject *queryTable, int sensibility = 17) = 0;
	virtual bool clickOnJoin(gqbQueryJoin *join, wxPoint &pt, wxPoint &origin, wxPoint &dest) = 0;
	virtual int getTitleRowHeight() = 0;
private:

};
#endif
