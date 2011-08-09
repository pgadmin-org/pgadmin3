//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddBrowserDataContainer.cpp - Item to contain data for each treview child.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/ddmodel/ddBrowserDataContainer.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"


ddBrowserDataContainer::ddBrowserDataContainer(wxhdIFigure *data)
{
	figure = data;
}


// Destructor
ddBrowserDataContainer::~ddBrowserDataContainer()
{

}

int ddBrowserDataContainer::getFigureKindId()
{
	return figure->getKindId();
}

wxhdIFigure *ddBrowserDataContainer::getFigure()
{
	return figure;
}