//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPolyLineLocator.h - Return multiple location at same time for a PolyLine
//
//////////////////////////////////////////////////////////////////////////

#ifndef XWHDPOLYLINELOCATOR_H
#define XWHDPOLYLINELOCATOR_H

#include "hotdraw/locators/hdILocator.h"

class hdPolyLineLocator : public hdILocator
{
public:
	hdPolyLineLocator(int index);
	~hdPolyLineLocator();

	virtual hdPoint &locate(int posIdx, hdIFigure *owner);
	virtual void setIndex(int index);

protected:

private:
	int indx;
};
#endif
