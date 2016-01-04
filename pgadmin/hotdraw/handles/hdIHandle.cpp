//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdIHandle.cpp - Base class for all Handles
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/utilities/hdPoint.h"

hdIHandle::hdIHandle(hdIFigure *owner)
{
	figureOwner = owner;
}
hdIHandle::~hdIHandle()
{
}

hdIFigure *hdIHandle::getOwner()
{
	return figureOwner;
}

hdRect &hdIHandle::getDisplayBox(int posIdx)
{
	hdPoint p = locate(posIdx);
	displayBox.width = 0;
	displayBox.height = 0;
	displayBox.SetPosition(p);
	displayBox.Inflate(size, size);
	return displayBox;
}

bool hdIHandle::containsPoint(int posIdx, int x, int y)
{
	return getDisplayBox(posIdx).Contains(x, y);
}
