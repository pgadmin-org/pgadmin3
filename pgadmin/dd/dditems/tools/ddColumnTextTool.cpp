//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnTextTool.cpp - Modification of simple text tool for editing composite figure columns
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>

// App headers
#include "dd/dditems/tools/ddColumnTextTool.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/utilities/ddTableNameDialog.h"

class wxhdDrawingEditor;


ddColumnTextTool::ddColumnTextTool(wxhdDrawingView *view, wxhdIFigure *fig, wxhdITool *dt, bool fastEdit , wxString dialogCaption, wxString dialogMessage):
	wxhdSimpleTextTool(view, fig, dt, fastEdit, dialogCaption, dialogMessage)
{
	if(colTextFigure->ms_classInfo.IsKindOf(&ddTextTableItemFigure::ms_classInfo))
		colTextFigure = (ddTextTableItemFigure *) fig;
	else
		colTextFigure = NULL;
}

ddColumnTextTool::~ddColumnTextTool()
{
}

void ddColumnTextTool::mouseDown(wxhdMouseEvent &event)
{
	wxhdSimpleTextTool::mouseDown(event);
}

bool ddColumnTextTool::callDialog(wxhdDrawingView *view)
{
	if(colTextFigure->getOwnerColumn() == NULL)
	{
		wxString colName = colTextFigure->getText();
		wxString colShortName = colTextFigure->getAlias();
		ddTableNameDialog *nameAliasDialog = new ddTableNameDialog(
		    view,
		    colName,
		    colShortName,
		    colTextFigure
		);

		int answer =	nameAliasDialog->ShowModal();
		bool change = false;

		if(answer = wxOK)
		{
			//check if names changed
			change =  ! (colShortName.IsSameAs(nameAliasDialog->GetValue1()) && colShortName.IsSameAs(nameAliasDialog->GetValue2()));
			if(change)
			{
				colTextFigure->setText(nameAliasDialog->GetValue1());
				colTextFigure->setAlias(nameAliasDialog->GetValue2());
				view->notifyChanged();
			}
		}
		delete nameAliasDialog;
		return change;
	}
	else
	{
		bool change = wxhdSimpleTextTool::callDialog(view);
		if(  change && colTextFigure->getOwnerColumn()->isGeneratedForeignKey()) //after a manual user column rename, deactivated automatic generation of fk name.
			colTextFigure->getOwnerColumn()->deactivateGenFkName();
		return change;
	}
}
