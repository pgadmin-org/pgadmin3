//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdILocator.h - Base class for creation of a location for a ddHandle
//
//////////////////////////////////////////////////////////////////////////

#ifndef hdILocator_H
#define hdILocator_H

#include "hotdraw/main/hdObject.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/figures/hdIFigure.h"

class hdILocator : public hdObject
{
public:
	hdILocator();
	~hdILocator();

	virtual hdPoint &locate(int posIdx, hdIFigure *owner) = 0;

protected:
	hdPoint locatePoint;
private:

};
#endif
