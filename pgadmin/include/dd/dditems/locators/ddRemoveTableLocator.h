//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRemoveColLocator.cpp - Locate table delete button inside a table.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDREMOVETABLELOCATOR_H
#define DDREMOVETABLELOCATOR_H

#include "dd/wxhotdraw/locators/wxhdILocator.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class ddRemoveTableLocator : public wxhdILocator
{
public:
	ddRemoveTableLocator();
	~ddRemoveTableLocator();

	virtual wxhdPoint &locate(wxhdIFigure *owner);

protected:

private:
};
#endif
