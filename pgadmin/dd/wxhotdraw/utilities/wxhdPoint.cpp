//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPoint.cpp - wxPoint class to be used as wrapper and allow independence of point class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

wxhdPoint::wxhdPoint():
	wxPoint()
{
}

wxhdPoint::wxhdPoint(int x, int y):
	wxPoint(x, y)
{
}

wxhdPoint::wxhdPoint(const wxhdPoint &p):
	wxPoint(p.x, p.y)
{
}

wxhdPoint::wxhdPoint(const wxPoint &p):
	wxPoint(p.x, p.y)
{
}