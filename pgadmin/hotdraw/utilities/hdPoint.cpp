//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPoint.cpp - wxPoint class to be used as wrapper and allow independence of point class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/utilities/hdPoint.h"

hdPoint::hdPoint():
	wxPoint()
{
}

hdPoint::hdPoint(int x, int y):
	wxPoint(x, y)
{
}

hdPoint::hdPoint(const hdPoint &p):
	wxPoint(p.x, p.y)
{
}

hdPoint::hdPoint(const wxPoint &p):
	wxPoint(p.x, p.y)
{
}
