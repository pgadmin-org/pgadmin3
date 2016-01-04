//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCanvasMenuTool.cpp - Allow to set up a menu at main canvas in case of not finding a tool or handle that manages an event
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>

// App headers
#include "hotdraw/tools/hdCanvasMenuTool.h"

class hdDrawingEditor;

hdCanvasMenuTool::hdCanvasMenuTool(hdDrawingView *view, hdITool *dt):
	hdAbstractTool(view)
{
	defaultTool = dt;
	ownerView->setCanvasMenuTool(NULL);
}

hdCanvasMenuTool::~hdCanvasMenuTool()
{
	ownerView->setCanvasMenuTool(NULL);
	if(defaultTool)
		delete defaultTool;
}

void hdCanvasMenuTool::mouseDown(hdMouseEvent &event)
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
		hdPoint p = event.GetPosition();
		event.getView()->CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		event.getView()->PopupMenu(&menu, p);
		return;
	}

	defaultTool->mouseDown(event);
}

void hdCanvasMenuTool::mouseDrag(hdMouseEvent &event)
{
	defaultTool->mouseDrag(event);
}

void hdCanvasMenuTool::mouseUp(hdMouseEvent &event)
{
	defaultTool->mouseUp(event);
}

void hdCanvasMenuTool::mouseMove(hdMouseEvent &event)
{
	defaultTool->mouseMove(event);
}

void hdCanvasMenuTool::createViewMenu(hdDrawingView *view, wxMenu &mnu)
{
	view->createViewMenu(mnu);
}

void hdCanvasMenuTool::OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view)
{
	view->OnGenericViewPopupClick(event);
}
