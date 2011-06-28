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

#ifndef WXHDKEYEVENT_H
#define WXHDKEYEVENT_H
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/main/wxhdObject.h"

class wxhdDrawingView;

class wxhdKeyEvent : public wxhdObject
{
public:
	wxhdKeyEvent(wxKeyEvent &event, wxhdDrawingView *owner);
	wxhdDrawingView *getView();
	int GetKeyCode();
private:
	wxhdDrawingView *view;
	wxKeyEvent &keyEvent;
	int keyCode;
};
#endif
