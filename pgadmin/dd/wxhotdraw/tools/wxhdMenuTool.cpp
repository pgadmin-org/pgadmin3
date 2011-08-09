//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdMenuTool.cpp - Allow Edition of textTool (double click) or show a menu to modifiy in someway text (right click) a figure.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdMenuTool.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class wxhdDrawingEditor;

wxhdMenuTool::wxhdMenuTool(wxhdDrawingView *view, wxhdIFigure *fig, wxhdITool *dt):
	wxhdFigureTool(view, fig, dt)
{
	menuFigure = (wxhdAbstractMenuFigure *) this->getFigure();
	ownerView->setMenuToolFigure(NULL);
}

wxhdMenuTool::~wxhdMenuTool()
{
}

void wxhdMenuTool::mouseDown(wxhdMouseEvent &event)
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
		wxhdPoint p = event.GetPosition();
		event.getView()->CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		event.getView()->PopupMenu(&menu, p);
		return;
	}

	getDefaultTool()->mouseDown(event);
}

void wxhdMenuTool::activate(wxhdDrawingView *view)
{
	wxhdFigureTool::activate(view);
}

void wxhdMenuTool::deactivate(wxhdDrawingView *view)
{
	wxhdFigureTool::deactivate(view);
}

void wxhdMenuTool::mouseDrag(wxhdMouseEvent &event)
{
	getDefaultTool()->mouseDrag(event);
}

void wxhdMenuTool::OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view)
{
	menuFigure->OnGenericPopupClick(event, view);
}
