//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddColLocator.h - Locate table add column button inside a table.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDADDCOLLOCATOR_H
#define DDADDCOLLOCATOR_H

#include "dd/wxhotdraw/locators/wxhdILocator.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class ddAddColLocator : public wxhdILocator
{
public:
	ddAddColLocator();
    ~ddAddColLocator();

	virtual wxhdPoint& locate(wxhdIFigure *owner);

protected:

private:
};
#endif
