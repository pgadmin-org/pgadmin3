//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPolyLineLocator.h - Return multiple location at same time for a PolyLine
//
//////////////////////////////////////////////////////////////////////////

#ifndef XWHDPOLYLINELOCATOR_H
#define XWHDPOLYLINELOCATOR_H

#include "dd/wxhotdraw/locators/wxhdILocator.h"

class wxhdPolyLineLocator : public wxhdILocator
{
public:
	wxhdPolyLineLocator(int index);
	~wxhdPolyLineLocator();

	virtual wxhdPoint &locate(wxhdIFigure *owner);
	virtual void setIndex(int index);

protected:

private:
	int indx;
};
#endif
