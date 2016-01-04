//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableBottomLocator.cpp - Locate bottom (south) of table for use of south table size handle.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/locators/ddTableBottomLocator.h"
#include "dd/dditems/figures/ddTableFigure.h"

ddTableBottomLocator::ddTableBottomLocator()
{
}

ddTableBottomLocator::~ddTableBottomLocator()
{
}

hdPoint &ddTableBottomLocator::locate(int posIdx, hdIFigure *owner)
{
	if(owner)
	{
		ddTableFigure *table = (ddTableFigure *) owner;

		int x = table->getFullSpace().GetLeftBottom(posIdx).x + table->getFullSpace().width * 0.25;
		int y = table->getFullSpace().GetLeftBottom(posIdx).y - 2;

		locatePoint.x = x;
		locatePoint.y = y;
		return locatePoint;
	}
	locatePoint.x = 0;
	locatePoint.y = 0;
	return locatePoint;
}
