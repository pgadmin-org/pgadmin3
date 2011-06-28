//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddMinMaxTableButtonHandle.cpp - A handle for a table figure that allow to graphically minimize or maximize table window size
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/handles/ddMinMaxTableButtonHandle.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/utilities/ddDataType.h"

//Images
#include "images/ddMinMaxCursor.pngc"

ddMinMaxTableButtonHandle::ddMinMaxTableButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator , wxBitmap &buttonImage, wxBitmap &buttonSecondImage, wxSize &size):
	wxhdButtonHandle(owner, buttonLocator, buttonImage, size)
{
	handleCursorImage = wxBitmap(*ddMinMaxCursor_png_img).ConvertToImage();
	handleCursor = wxCursor(handleCursorImage);
	buttonMaximizeImage = buttonSecondImage;
	tmpImage = buttonImage;
	showFirst = true;
}

ddMinMaxTableButtonHandle::~ddMinMaxTableButtonHandle()
{
}

void ddMinMaxTableButtonHandle::invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

void ddMinMaxTableButtonHandle::invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

void ddMinMaxTableButtonHandle::invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	ddTableFigure *table = (ddTableFigure *) getOwner();

	if(showFirst)
	{
		buttonIcon = buttonMaximizeImage;
		table->setColumnsWindow(1);
	}
	else
	{
		buttonIcon = tmpImage;
		table->setColumnsWindow(table->getTotalColumns(), true);
	}
	showFirst = !showFirst;
}


wxCursor ddMinMaxTableButtonHandle::createCursor()
{
	return handleCursor;
}
