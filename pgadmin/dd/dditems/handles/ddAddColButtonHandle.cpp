//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddColButtonHandle.cpp - A handle for a table figure that allow to graphically add columns
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/handles/ddAddColButtonHandle.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

//Images
#include "images/ddAddColumnCursor.pngc"

ddAddColButtonHandle::ddAddColButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size):
	wxhdButtonHandle(owner, buttonLocator, buttonImage, size)
{
	handleCursorImage = wxBitmap(*ddAddColumnCursor_png_img).ConvertToImage();
	handleCursor = wxCursor(handleCursorImage);
}

ddAddColButtonHandle::~ddAddColButtonHandle()
{
}

void ddAddColButtonHandle::invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	ddTableFigure *table = (ddTableFigure *) getOwner();
	wxTextEntryDialog nameDialog(view, wxT("New column name"), wxT("Add a column"), wxT("NewColumn"));
	bool again;
	do
	{
		again = false;
		int answer = nameDialog.ShowModal();
		if (answer == wxID_OK)
		{
			wxString name = nameDialog.GetValue();
			if(table->colNameAvailable(name))
				table->addColumn(new ddColumnFigure(name, table));
			else
			{
				wxString msg(wxT("Error trying to add new column '"));
				msg.Append(name);
				msg.Append(wxT("' column name already in use"));
				wxMessageDialog info( view, msg ,
				                      wxT("Column name already in use"),
				                      wxNO_DEFAULT | wxOK | wxICON_EXCLAMATION);
				again = true;
				info.ShowModal();
			}
		}

	}
	while(again);
}

void ddAddColButtonHandle::invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

void ddAddColButtonHandle::invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

wxCursor ddAddColButtonHandle::createCursor()
{
	return handleCursor;
}
