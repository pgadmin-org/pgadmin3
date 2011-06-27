//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdMouseEvent.h - Wrapper Class to integrate conversion CalcUnscrolledPosition in a mouse event.
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDMOUSEEVENT_H
#define WXHDMOUSEEVENT_H
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/main/wxhdObject.h"

class wxhdDrawingView;

class wxhdMouseEvent : public wxhdObject
{
	public:
		wxhdMouseEvent(wxMouseEvent& event, wxhdDrawingView *owner);
		wxhdPoint& getUnScrolledPosition();
		wxhdPoint& getScrolledPosition();
		wxhdPoint& GetPosition();
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
		wxhdDrawingView* getView();
private:
		wxhdDrawingView *view;
		wxMouseEvent& mouseEvent;
		wxhdPoint unScrolled, scrolled;
};
#endif
