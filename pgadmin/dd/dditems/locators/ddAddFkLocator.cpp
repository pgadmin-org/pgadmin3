//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddFkLocator.cpp - Locate table add fk relationship button inside a table.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/locators/ddAddFkLocator.h"
#include "dd/dditems/figures/ddTableFigure.h"

ddAddFkLocator::ddAddFkLocator()
{
}

ddAddFkLocator::~ddAddFkLocator()
{
}

hdPoint &ddAddFkLocator::locate(int posIdx, hdIFigure *owner)
{
	if(owner)
	{
		ddTableFigure *table = (ddTableFigure *) owner;
		locatePoint.x = table->getTitleRect().GetBottomRight(posIdx).x - 10;
		locatePoint.y = table->getTitleRect().GetBottomRight(posIdx).y - 9;
		return locatePoint;
	}
	locatePoint.x = 0;
	locatePoint.y = 0;
	return locatePoint;
}
