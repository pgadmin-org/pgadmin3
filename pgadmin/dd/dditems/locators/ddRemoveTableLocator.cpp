//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRemoveColLocator.cpp - Locate table delete button inside a table.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/locators/ddRemoveTableLocator.h"
#include "dd/dditems/figures/ddTableFigure.h"

ddRemoveTableLocator::ddRemoveTableLocator()
{
}

ddRemoveTableLocator::~ddRemoveTableLocator()
{
}

hdPoint &ddRemoveTableLocator::locate(int posIdx, hdIFigure *owner)
{
	if(owner)
	{
		ddTableFigure *table = (ddTableFigure *) owner;
		int x = table->displayBox().x[posIdx] + table->displayBox().width - 10; //(8+2)
		int y = table->displayBox().y[posIdx] + 6;

		locatePoint.x = x;
		locatePoint.y = y;
		return locatePoint;
	}
	locatePoint.x = 0;
	locatePoint.y = 0;
	return locatePoint;
}
