//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdKeyEvent.h - Wrapper Class to integrate wxKeyEvent inside library.
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDKEYEVENT_H
#define HDKEYEVENT_H
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/main/hdObject.h"

class hdDrawingView;

class hdKeyEvent : public hdObject
{
public:
	hdKeyEvent(wxKeyEvent &event, hdDrawingView *owner);
	hdDrawingView *getView();
	int GetKeyCode();
private:
	hdDrawingView *view;
	wxKeyEvent &keyEvent;
	int keyCode;
};
#endif
