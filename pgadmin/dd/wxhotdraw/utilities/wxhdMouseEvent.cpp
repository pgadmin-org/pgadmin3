//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdMouseEvent.cpp - Wrapper Class to integrate conversion CalcUnscrolledPosition in a mouse event.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/utilities/wxhdMouseEvent.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

wxhdMouseEvent::wxhdMouseEvent(wxMouseEvent& event, wxhdDrawingView *owner):
mouseEvent(event)
{
	view=owner;
	m_shiftDown = event.m_shiftDown;
}

wxhdPoint& wxhdMouseEvent::GetPosition()
{
	return getUnScrolledPosition();
}

wxhdPoint& wxhdMouseEvent::getUnScrolledPosition()
{
	unScrolled = mouseEvent.GetPosition();
	view->CalcUnscrolledPosition(unScrolled.x,unScrolled.y,&unScrolled.x,&unScrolled.y);
	return unScrolled;
}

wxhdPoint& wxhdMouseEvent::getScrolledPosition()
{
	scrolled = mouseEvent.GetPosition();
	return scrolled;
}

int wxhdMouseEvent::getScrolledPosX()
{
	return getScrolledPosition().x;
}

int wxhdMouseEvent::getScrolledPosY()
{
	return getScrolledPosition().y;
}

int wxhdMouseEvent::getUnScrolledPosX()
{
	return getUnScrolledPosition().x;
}

int wxhdMouseEvent::getUnScrolledPosY()
{
	return getUnScrolledPosition().y;
}

bool wxhdMouseEvent::LeftDClick()
{
	return mouseEvent.LeftDClick();
}

bool wxhdMouseEvent::ShiftDown()
{
	return mouseEvent.ShiftDown();
}

bool wxhdMouseEvent::RightDown()
{
	return mouseEvent.RightDown();
}

bool wxhdMouseEvent::LeftDown()
{
	return mouseEvent.LeftDown();
}

bool wxhdMouseEvent::LeftUp()
{
	return mouseEvent.LeftUp();
}

bool wxhdMouseEvent::LeftIsDown()
{
	return mouseEvent.LeftIsDown();
}

wxhdDrawingView* wxhdMouseEvent::getView()
{
	return view;
}
