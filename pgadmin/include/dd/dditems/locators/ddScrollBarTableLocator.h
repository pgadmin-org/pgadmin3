//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddScrollBarTableLocator.h - Locate table scrollbar inside a table.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDSCROLLBARTABLELOCATOR_H
#define DDSCROLLBARTABLELOCATOR_H

#include "hotdraw/locators/hdILocator.h"
#include "hotdraw/figures/hdIFigure.h"

class ddScrollBarTableLocator : public hdILocator
{
public:
	ddScrollBarTableLocator();
	~ddScrollBarTableLocator();

	virtual hdPoint &locate(int posIdx, hdIFigure *owner);

protected:

private:

};
#endif
