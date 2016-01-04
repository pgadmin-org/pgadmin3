//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdSimpleTextTool.cpp - Tool to allow edition of textTool with a double click or show a menu with a right click.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>

// App headers
#include "hotdraw/tools/hdSimpleTextTool.h"
#include "hotdraw/figures/hdSimpleTextFigure.h"

class hdDrawingEditor;

hdSimpleTextTool::hdSimpleTextTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt, bool fastEdit , wxString dialogCaption, wxString dialogMessage):
	hdFigureTool(view, fig, dt)
{
	dlgMessage = dialogMessage;
	dlgCaption = dialogCaption;
	withoutDialog = fastEdit;
	showEdit = false;
	txtFigure = ((hdSimpleTextFigure *)this->getFigure());
	ownerView->setSimpleTextToolFigure(NULL);

	calculateSizeEntry(ownerView);
}

hdSimpleTextTool::~hdSimpleTextTool()
{
}

void hdSimpleTextTool::calculateSizeEntry(hdDrawingView *view)
{
	if(view->getSimpleTextToolEdit())
	{
		hdPoint p = txtFigure->displayBox().GetPosition(view->getIdx());
		view->CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		view->getSimpleTextToolEdit()->SetPosition(p);
		view->getSimpleTextToolEdit()->SetSize(txtFigure->displayBox().GetSize());
		view->getOkTxt()->SetPosition(wxPoint(p.x + view->getSimpleTextToolEdit()->GetSize().GetWidth() + 4, p.y));
		view->getCancelTxt()->SetPosition(wxPoint(view->getOkTxt()->GetPosition().x + view->getOkTxt()->GetSize().GetWidth() + 4, p.y));
		//Right now implemented with a hack (function at main view), but source of bug, probably can be tracked.
	}
}

void hdSimpleTextTool::mouseDown(hdMouseEvent &event)
{
	setAnchorCoords(event.GetPosition().x, event.GetPosition().y);

	// Right click to get the contextual menu
	if(txtFigure->menuEnabled() && event.RightDown())
	{
		wxMenu menu;
		event.getView()->setSimpleTextToolFigure(txtFigure, true);
		txtFigure->createMenu(menu);
		event.getView()->connectPopUpMenu(menu);
		hdPoint p = event.GetPosition();
		event.getView()->CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		event.getView()->PopupMenu(&menu, p);
		return;
	}

	// Double click to rename an object
	if(event.LeftDClick())
	{
		if(withoutDialog)
		{
			event.getView()->setSimpleTextToolFigure(txtFigure);
			showEdit = true;
			event.getView()->getSimpleTextToolEdit()->ChangeValue(txtFigure->getText()); //Same as SetValue but don't generated wxEVT_COMMAND_TEXT_UPDATED event
			calculateSizeEntry(event.getView());
			event.getView()->getSimpleTextToolEdit()->SetFocus();
			event.getView()->getSimpleTextToolEdit()->Show();
			event.getView()->getOkTxt()->Show();
			event.getView()->getCancelTxt()->Show();
		}
		else
		{
			callDialog(event.getView());
		}
		return;
	}
	getDefaultTool()->mouseDown(event);
}

void hdSimpleTextTool::activate(hdDrawingView *view)
{
	showEdit = false;
	hdFigureTool::activate(view);
}

void hdSimpleTextTool::deactivate(hdDrawingView *view)
{
	if(view->getSimpleTextToolEdit())
	{
		// Can't delete this objects because view is the owner of this objects
		view->getSimpleTextToolEdit()->Hide();
		view->getOkTxt()->Hide();
		view->getCancelTxt()->Hide();
		view->setSimpleTextToolFigure(NULL);
	}
	hdFigureTool::deactivate(view);
}

void hdSimpleTextTool::mouseDrag(hdMouseEvent &event)
{
	if(!showEdit)
	{
		getDefaultTool()->mouseDrag(event);
	}
}

void hdSimpleTextTool::OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view)
{
	txtFigure->OnGenericPopupClick(event, view);
}

bool hdSimpleTextTool::callDialog(hdDrawingView *view)
{
	wxString sNewValue = wxGetTextFromUser(dlgMessage, dlgCaption, txtFigure->getText(), view);
	if (!sNewValue.IsEmpty())
	{
		txtFigure->setText(sNewValue);
		view->notifyChanged();
		return true;
	}
	else
		return false;
}
