//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRemoveTableButtonHandle.cpp - A handle for a table figure that allow to delete it
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/handles/ddRemoveTableButtonHandle.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/utilities/hdRemoveDeleteDialog.h"
#include "dd/ddmodel/ddDrawingEditor.h"
#include "dd/ddmodel/ddDatabaseDesign.h"

//Images
#include "images/ddDeleteTableCursor.pngc"

ddRemoveTableButtonHandle::ddRemoveTableButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size):
	hdButtonHandle(owner, buttonLocator, buttonImage, size)
{
}

ddRemoveTableButtonHandle::~ddRemoveTableButtonHandle()
{
}

void ddRemoveTableButtonHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
}

void ddRemoveTableButtonHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
}

void ddRemoveTableButtonHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
	if(view && getOwner())
	{
		ddTableFigure *table = (ddTableFigure *) getOwner();
		hdRemoveDeleteDialog dialog(_("Are you sure you wish to delete table ") + table->getTableName() + wxT("?"), _("Delete table?"), view);
		int answer = dialog.ShowModal();
		if (answer == DD_DELETE)
		{
			ddDrawingEditor *editor = (ddDrawingEditor *) view->editor();
			//Unselect table at all diagrams
			editor->removeFromAllSelections(table);
			//Drop foreign keys with this table as origin or destination
			table->processDeleteAlert(view->getDrawing());
			//Drop table
			editor->deleteModelFigure(table);
			editor->getDesign()->refreshBrowser();
			editor->checkRelationshipsConsistency(view->getIdx());
			view->notifyChanged();
		}
		else if(answer == DD_REMOVE)
		{
			ddDrawingEditor *editor = (ddDrawingEditor *) view->editor();
			editor->getExistingDiagram(view->getIdx())->removeFromSelection(table);
			editor->getExistingDiagram(view->getIdx())->remove(table);
			editor->checkRelationshipsConsistency(view->getIdx());
			view->notifyChanged();
		}

	}
}
