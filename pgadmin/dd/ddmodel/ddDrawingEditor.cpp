//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.cpp - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/ddmodel/ddDrawingEditor.h"
#include "dd/dditems/utilities/ddTableNameDialog.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/ddmodel/ddDrawingView.h"


ddDrawingEditor::ddDrawingEditor(wxWindow *owner, ddDatabaseDesign *design)
	: wxhdDrawingEditor(owner, false)
{
	databaseDesign = design;
	createView(owner);
}

void ddDrawingEditor::createView(wxWindow *owner)
{
	_view = new ddDrawingView(owner, this, wxSize(1200, 1200), _model);
	// Set Scroll Bar & split
	_view->SetScrollbars( 10, 10, 127, 80 );
	_view->EnableScrolling(true, true);
	_view->AdjustScrollbars();
}

void ddDrawingEditor::createMenu(wxMenu &mnu)
{
	mnu.Append(MNU_NEWTABLE, _("Add new Table"));
}

void ddDrawingEditor::OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view)
{
	switch(event.GetId())
	{
		case MNU_NEWTABLE:
			ddTableNameDialog *newTableDialog = new ddTableNameDialog(
			    view,
			    databaseDesign->getNewTableName(),
			    wxEmptyString,
			    NULL
			);
			int answer = newTableDialog->ShowModal();
			if (answer == wxID_OK && !newTableDialog->GetValue1().IsEmpty())
			{
				ddTableFigure *newTable = new ddTableFigure(newTableDialog->GetValue1(),
				        rand() % 90 + 200,
				        rand() % 90 + 140,
				        newTableDialog->GetValue2()
				                                           );
				databaseDesign->addTable(newTable);
				databaseDesign->refreshDraw();
			}
			delete newTableDialog;
			break;
	}
}
