//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdMouseEvent.h - Wrapper Class to integrate conversion CalcUnscrolledPosition in a mouse event.
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDMOUSEEVENT_H
#define HDMOUSEEVENT_H
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/main/hdObject.h"

class hdDrawingView;

class hdMouseEvent : public hdObject
{
public:
	hdMouseEvent(wxMouseEvent &event, hdDrawingView *owner);
	hdPoint &getUnScrolledPosition();
	hdPoint &getScrolledPosition();
	hdPoint &GetPosition();
	int getUnScrolledPosX();
	int getUnScrolledPosY();
	int getScrolledPosX();
	int getScrolledPosY();
	bool LeftDClick();
	bool LeftDown();
	bool LeftIsDown();
	bool LeftUp();
	bool ShiftDown();
	bool RightDown();
	bool m_shiftDown;
	hdDrawingView *getView();
private:
	hdDrawingView *view;
	wxMouseEvent &mouseEvent;
	hdPoint unScrolled, scrolled;
};
#endif
