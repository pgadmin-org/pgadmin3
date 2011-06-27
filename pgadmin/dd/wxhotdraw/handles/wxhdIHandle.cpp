//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdIHandle.cpp - Base class for all Handles
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

wxhdIHandle::wxhdIHandle(wxhdIFigure *owner)
{
	figureOwner=owner;
}
wxhdIHandle::~wxhdIHandle()
{
}

wxhdIFigure* wxhdIHandle::getOwner()
{
	return figureOwner;
}

wxhdRect& wxhdIHandle::getDisplayBox()
{
	wxhdPoint p = locate();
	displayBox.width=0;
	displayBox.height=0;
	displayBox.SetPosition(p);
	displayBox.Inflate(size,size);
	return displayBox;
}

bool wxhdIHandle::containsPoint(int x, int y)
{
	return getDisplayBox().Contains(x,y);
}
