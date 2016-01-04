//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdMenuTool.cpp - Allow Edition of textTool (double click) or show a menu to modifiy in someway text (right click) a figure.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>

// App headers
#include "hotdraw/tools/hdMenuTool.h"
#include "hotdraw/figures/hdIFigure.h"

class hdDrawingEditor;

hdMenuTool::hdMenuTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt):
	hdFigureTool(view, fig, dt)
{
	menuFigure = (hdAbstractMenuFigure *) this->getFigure();
	ownerView->setMenuToolFigure(NULL);
}

hdMenuTool::~hdMenuTool()
{
}

void hdMenuTool::mouseDown(hdMouseEvent &event)
{
	//Linux hack for bug
	int x = event.GetPosition().x, y = event.GetPosition().y;
	setAnchorCoords(x, y);

	if(menuFigure->menuEnabled() && event.RightDown())
	{
		wxMenu menu;
		event.getView()->setMenuToolFigure(menuFigure);
		menuFigure->createMenu(menu);
		event.getView()->connectPopUpMenu(menu);
		hdPoint p = event.GetPosition();
		event.getView()->CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		event.getView()->PopupMenu(&menu, p);
		return;
	}

	getDefaultTool()->mouseDown(event);
}

void hdMenuTool::activate(hdDrawingView *view)
{
	hdFigureTool::activate(view);
}

void hdMenuTool::deactivate(hdDrawingView *view)
{
	hdFigureTool::deactivate(view);
}

void hdMenuTool::mouseDrag(hdMouseEvent &event)
{
	getDefaultTool()->mouseDrag(event);
}

void hdMenuTool::OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view)
{
	menuFigure->OnGenericPopupClick(event, view);
}
