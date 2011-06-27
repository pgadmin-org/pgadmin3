//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLineConnectionHandle.h - Base class for Handles that are located at locator position
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDLINECONNECTIONHANDLE
#define WXHDLINECONNECTIONHANDLE

#include "dd/wxhotdraw/handles/wxhdPolyLineHandle.h"

class wxhdLineConnectionHandle : public wxhdPolyLineHandle
{
public:
	wxhdLineConnectionHandle(wxhdPolyLineFigure *figure, wxhdILocator *loc, int index);
	virtual void invokeEnd(wxhdMouseEvent& event, wxhdDrawingView *view);
private:

};
#endif
