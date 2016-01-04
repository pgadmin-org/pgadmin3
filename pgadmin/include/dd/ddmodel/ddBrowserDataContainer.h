//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddBrowserDataContainer.h - Item to contain data for each treview child.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDBROWSERDATACONTAINER_H
#define DDBROWSERDATACONTAINER_H

#include "hotdraw/figures/hdIFigure.h"

class ddBrowserDataContainer : public wxTreeItemData
{
public:
	ddBrowserDataContainer(hdIFigure *data);
	~ddBrowserDataContainer();
	int getFigureKindId();
	hdIFigure *getFigure();

private:
	hdIFigure *figure;
};
#endif
