//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddScrollBarTableLocator.h - Locate table scrollbar inside a table.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDSCROLLBARTABLELOCATOR_H
#define DDSCROLLBARTABLELOCATOR_H

#include "dd/wxhotdraw/locators/wxhdILocator.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class ddScrollBarTableLocator : public wxhdILocator
{
public:
	ddScrollBarTableLocator();
    ~ddScrollBarTableLocator();

	virtual wxhdPoint& locate(wxhdIFigure *owner);

protected:

private:

};
#endif
