//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddBrowserDataContainer.h - Item to contain data for each treview child.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDBROWSERDATACONTAINER_H
#define DDBROWSERDATACONTAINER_H

#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class ddBrowserDataContainer : public wxTreeItemData
{
public:
	ddBrowserDataContainer(wxhdIFigure *data);
	~ddBrowserDataContainer();
	int getFigureKindId();
	wxhdIFigure *getFigure();

private:
	wxhdIFigure *figure;
};
#endif
