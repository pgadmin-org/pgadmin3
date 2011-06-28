//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
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
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

//Images
#include "images/ddDeleteTableCursor.pngc"

ddRemoveTableButtonHandle::ddRemoveTableButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size):
	wxhdButtonHandle(owner, buttonLocator, buttonImage, size)
{
	handleCursorImage = wxBitmap(*ddDeleteTableCursor_png_img).ConvertToImage();
	handleCursor = wxCursor(handleCursorImage);
}

ddRemoveTableButtonHandle::~ddRemoveTableButtonHandle()
{
}

void ddRemoveTableButtonHandle::invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

void ddRemoveTableButtonHandle::invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

void ddRemoveTableButtonHandle::invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view)
{

	if(view && getOwner())
	{
		ddTableFigure *table = (ddTableFigure *) getOwner();
		int answer = wxMessageBox(_("Are you sure you wish to delete table ") + table->getTableName() + wxT("?"), _("Delete table?"), wxYES_NO | wxNO_DEFAULT, view);
		if (answer == wxYES)
		{
			//unselect table
			if(view->isFigureSelected(table))
			{
				view->removeFromSelection(table);
			}
			//drop foreign keys with this table as origin or destination
			table->processDeleteAlert(view);
			//drop table
			view->remove(table);
			if(table)
			{
				delete table;
			}
		}
	}
}

wxCursor ddRemoveTableButtonHandle::createCursor()
{
	return handleCursor;
}
