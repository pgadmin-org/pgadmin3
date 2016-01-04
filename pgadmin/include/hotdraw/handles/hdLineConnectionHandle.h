//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLineConnectionHandle.h - Base class for Handles that are located at locator position
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDLINECONNECTIONHANDLE
#define HDLINECONNECTIONHANDLE

#include "hotdraw/handles/hdPolyLineHandle.h"

class hdLineConnectionHandle : public hdPolyLineHandle
{
public:
	hdLineConnectionHandle(hdPolyLineFigure *figure, hdILocator *loc, int index);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
private:

};
#endif
