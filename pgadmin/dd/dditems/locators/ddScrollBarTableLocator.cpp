//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddScrollBarTableLocator.cpp - Locate table scrollbar inside a table.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/locators/ddScrollBarTableLocator.h"
#include "dd/dditems/figures/ddTableFigure.h"

ddScrollBarTableLocator::ddScrollBarTableLocator()
{
}

ddScrollBarTableLocator::~ddScrollBarTableLocator()
{
}

wxhdPoint &ddScrollBarTableLocator::locate(wxhdIFigure *owner)
{
	if(owner)
	{
		ddTableFigure *table = (ddTableFigure *) owner;
		;
		int x = table->getColsSpace().GetTopRight().x - 11; //scrollwidth
		int y = table->getColsSpace().y;

		locatePoint.x = x;
		locatePoint.y = y;
		return locatePoint;
	}
	locatePoint.x = 0;
	locatePoint.y = 0;
	return locatePoint;
}