//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdILocator.cpp - Base class for creation of a location for a ddHandle
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/locators/hdILocator.h"

hdILocator::hdILocator()
{
	locatePoint = hdPoint(0, 0);
}

hdILocator::~hdILocator()
{
}
