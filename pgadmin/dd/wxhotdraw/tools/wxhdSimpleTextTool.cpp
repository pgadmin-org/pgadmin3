//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdSimpleTextTool.cpp - Tool to allow edition of textTool with a double click or show a menu with a right click.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>

// App headers
#include "dd/wxhotdraw/tools/wxhdSimpleTextTool.h"
#include "dd/wxhotdraw/figures/wxhdSimpleTextFigure.h"

class wxhdDrawingEditor;

wxhdSimpleTextTool::wxhdSimpleTextTool(wxhdDrawingEditor *editor, wxhdIFigure *fig, wxhdITool *dt, bool fastEdit , wxString dialogCaption, wxString dialogMessage):
wxhdFigureTool(editor,fig,dt)
{
	dlgMessage = dialogMessage;
	dlgCaption = dialogCaption;
	withoutDialog = fastEdit;
	showEdit = false;
	txtFigure = ((wxhdSimpleTextFigure *)this->getFigure());
	editor->view()->setSimpleTextToolFigure(NULL);
	edit = getDrawingEditor()->view()->getSimpleTextToolEdit();
	okButton = getDrawingEditor()->view()->getOkTxt();
	cancelButton = getDrawingEditor()->view()->getCancelTxt();
	calculateSizeEntry(editor->view());
}

wxhdSimpleTextTool::~wxhdSimpleTextTool()
{
}

void wxhdSimpleTextTool::calculateSizeEntry(wxhdDrawingView *view)
{
	if(edit)
	{
		wxhdPoint p=txtFigure->displayBox().GetPosition();
		view->CalcScrolledPosition(p.x,p.y,&p.x,&p.y);
		edit->SetPosition(p);
		edit->SetSize(txtFigure->displayBox().GetSize());
		okButton->SetPosition(wxPoint(p.x+edit->GetSize().GetWidth()+4,p.y));
		cancelButton->SetPosition(wxPoint(okButton->GetPosition().x+okButton->GetSize().GetWidth()+4,p.y));
		//Right now implemented with a hack (function at main view), but source of bug, probably can be tracked.
	}
}

void wxhdSimpleTextTool::mouseDown(wxhdMouseEvent& event)
{	
	setAnchorCoords(event.GetPosition().x,event.GetPosition().y);

    // Right click to get the contextual menu
	if(txtFigure->menuEnabled() && event.RightDown())
	{
        wxMenu menu;
        getDrawingEditor()->view()->setSimpleTextToolFigure(txtFigure,true);
        txtFigure->createMenu(menu);
        getDrawingEditor()->view()->connectPopUpMenu(menu);
        wxhdPoint p=event.GetPosition();
        event.getView()->CalcScrolledPosition(p.x,p.y,&p.x,&p.y);
        getDrawingEditor()->view()->PopupMenu(&menu, p);
		return;
	}

    // Double click to rename an object
	if(event.LeftDClick())
	{
		if(withoutDialog)
		{
			getDrawingEditor()->view()->setSimpleTextToolFigure(txtFigure);
			showEdit = true;
			edit->ChangeValue(txtFigure->getText()); //Same as SetValue but don't generated wxEVT_COMMAND_TEXT_UPDATED event
			calculateSizeEntry(event.getView());
			edit->SetFocus();
			edit->Show();
			okButton->Show();
			cancelButton->Show();
		}
		else
		{
			callDialog();
		}
		return;
	}
	getDefaultTool()->mouseDown(event);
}

void wxhdSimpleTextTool::activate()
{
	showEdit = false;
	wxhdFigureTool::activate();
}

void wxhdSimpleTextTool::deactivate()
{
	if(edit)
	{
        // Can't delete this objects because view is the owner of this objects
		edit->Hide();
		okButton->Hide();
		cancelButton->Hide();
		getDrawingEditor()->view()->setSimpleTextToolFigure(NULL);
	}
	wxhdFigureTool::deactivate();
}

void wxhdSimpleTextTool::mouseDrag(wxhdMouseEvent& event)
{
	if(!showEdit)
	{
		getDefaultTool()->mouseDrag(event);
	}
}

void wxhdSimpleTextTool::OnGenericPopupClick(wxCommandEvent& event, wxhdDrawingView *view)
{
	txtFigure->OnGenericPopupClick(event,view);
}

bool wxhdSimpleTextTool::callDialog()
{
			wxString sNewValue = wxGetTextFromUser(dlgMessage, dlgCaption, txtFigure->getText(),getDrawingEditor()->view());
			if (!sNewValue.IsEmpty())
			{
				txtFigure->setText(sNewValue);
				return true;
			}
			else
				return false;
}