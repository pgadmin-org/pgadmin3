//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddFkLocator.h - Locate table add fk relationship button inside a table.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDADDFKLOCATOR_H
#define DDADDFKLOCATOR_H

#include "hotdraw/locators/hdILocator.h"
#include "hotdraw/figures/hdIFigure.h"

class ddAddFkLocator : public hdILocator
{
public:
	ddAddFkLocator();
	~ddAddFkLocator();

	virtual hdPoint &locate(int posIdx, hdIFigure *owner);

protected:

private:
};
#endif
