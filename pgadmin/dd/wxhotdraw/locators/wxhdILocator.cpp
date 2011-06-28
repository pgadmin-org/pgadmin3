//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdILocator.cpp - Base class for creation of a location for a ddHandle
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/locators/wxhdILocator.h"

wxhdILocator::wxhdILocator()
{
	locatePoint = wxhdPoint(0, 0);
}

wxhdILocator::~wxhdILocator()
{
}
