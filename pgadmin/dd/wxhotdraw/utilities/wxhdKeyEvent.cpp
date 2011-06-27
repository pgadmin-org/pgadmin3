//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdKeyEvent.h - Wrapper Class to integrate wxKeyEvent inside library.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/utilities/wxhdKeyEvent.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

wxhdKeyEvent::wxhdKeyEvent(wxKeyEvent& event, wxhdDrawingView *owner):
keyEvent(event)
{
	view=owner;
	keyCode = event.GetKeyCode();
}

int wxhdKeyEvent::GetKeyCode()
{
	return keyCode;
}

wxhdDrawingView* wxhdKeyEvent::getView()
{
	return view;
}
