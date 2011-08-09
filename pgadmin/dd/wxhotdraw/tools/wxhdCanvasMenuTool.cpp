//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCanvasMenuTool.cpp - Allow to set up a menu at main canvas in case of not finding a tool or handle that manages an event
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdCanvasMenuTool.h"

class wxhdDrawingEditor;

wxhdCanvasMenuTool::wxhdCanvasMenuTool(wxhdDrawingView *view, wxhdITool *dt):
	wxhdAbstractTool(view)
{
	defaultTool = dt;
	ownerView->setCanvasMenuTool(NULL);
}

wxhdCanvasMenuTool::~wxhdCanvasMenuTool()
{
	ownerView->setCanvasMenuTool(NULL);
	if(defaultTool)
		delete defaultTool;
}

void wxhdCanvasMenuTool::mouseDown(wxhdMouseEvent &event)
{
	//Linux hack for bug
	int x = event.GetPosition().x, y = event.GetPosition().y;
	setAnchorCoords(x, y);

	if(event.RightDown())
	{
		wxMenu menu;
		event.getView()->setCanvasMenuTool(this);
		createViewMenu(event.getView(), menu);
		event.getView()->connectPopUpMenu(menu);
		wxhdPoint p = event.GetPosition();
		event.getView()->CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		event.getView()->PopupMenu(&menu, p);
		return;
	}

	defaultTool->mouseDown(event);
}

void wxhdCanvasMenuTool::mouseDrag(wxhdMouseEvent &event)
{
	defaultTool->mouseDrag(event);
}

void wxhdCanvasMenuTool::mouseUp(wxhdMouseEvent &event)
{
	defaultTool->mouseUp(event);
}

void wxhdCanvasMenuTool::mouseMove(wxhdMouseEvent &event)
{
	defaultTool->mouseMove(event);
}

void wxhdCanvasMenuTool::createViewMenu(wxhdDrawingView *view, wxMenu &mnu)
{
	view->createViewMenu(mnu);
}

void wxhdCanvasMenuTool::OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view)
{
	view->OnGenericViewPopupClick(event);
}