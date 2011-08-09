//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdILocator.h - Base class for creation of a location for a ddHandle
//
//////////////////////////////////////////////////////////////////////////

#ifndef wxhdILocator_H
#define wxhdILocator_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class wxhdILocator : public wxhdObject
{
public:
	wxhdILocator();
	~wxhdILocator();

	virtual wxhdPoint &locate(int posIdx, wxhdIFigure *owner) = 0;

protected:
	wxhdPoint locatePoint;
private:

};
#endif
