//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdMouseEvent.cpp - Wrapper Class to integrate conversion CalcUnscrolledPosition in a mouse event.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/utilities/hdMouseEvent.h"
#include "hotdraw/main/hdDrawingView.h"

hdMouseEvent::hdMouseEvent(wxMouseEvent &event, hdDrawingView *owner):
	mouseEvent(event)
{
	view = owner;
	m_shiftDown = event.m_shiftDown;
}

hdPoint &hdMouseEvent::GetPosition()
{
	return getUnScrolledPosition();
}

hdPoint &hdMouseEvent::getUnScrolledPosition()
{
	unScrolled = mouseEvent.GetPosition();
	view->CalcUnscrolledPosition(unScrolled.x, unScrolled.y, &unScrolled.x, &unScrolled.y);
	return unScrolled;
}

hdPoint &hdMouseEvent::getScrolledPosition()
{
	scrolled = mouseEvent.GetPosition();
	return scrolled;
}

int hdMouseEvent::getScrolledPosX()
{
	return getScrolledPosition().x;
}

int hdMouseEvent::getScrolledPosY()
{
	return getScrolledPosition().y;
}

int hdMouseEvent::getUnScrolledPosX()
{
	return getUnScrolledPosition().x;
}

int hdMouseEvent::getUnScrolledPosY()
{
	return getUnScrolledPosition().y;
}

bool hdMouseEvent::LeftDClick()
{
	return mouseEvent.LeftDClick();
}

bool hdMouseEvent::ShiftDown()
{
	return mouseEvent.ShiftDown();
}

bool hdMouseEvent::RightDown()
{
	return mouseEvent.RightDown();
}

bool hdMouseEvent::LeftDown()
{
	return mouseEvent.LeftDown();
}

bool hdMouseEvent::LeftUp()
{
	return mouseEvent.LeftUp();
}

bool hdMouseEvent::LeftIsDown()
{
	return mouseEvent.LeftIsDown();
}

hdDrawingView *hdMouseEvent::getView()
{
	return view;
}
