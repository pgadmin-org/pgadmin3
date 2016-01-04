//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddMinMaxTableLocator.cpp - Locate table minimize/maximize button inside a table.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/locators/ddMinMaxTableLocator.h"
#include "dd/dditems/figures/ddTableFigure.h"

ddMinMaxTableLocator::ddMinMaxTableLocator()
{
}

ddMinMaxTableLocator::~ddMinMaxTableLocator()
{
}

hdPoint &ddMinMaxTableLocator::locate(int posIdx, hdIFigure *owner)
{
	if(owner)
	{
		ddTableFigure *table = (ddTableFigure *) owner;
		int x = table->displayBox().x[posIdx] + table->displayBox().width - 20; //(8+2)
		int y = table->displayBox().y[posIdx] + 6;

		locatePoint.x = x;
		locatePoint.y = y;
		return locatePoint;
	}
	locatePoint.x = 0;
	locatePoint.y = 0;
	return locatePoint;
}
