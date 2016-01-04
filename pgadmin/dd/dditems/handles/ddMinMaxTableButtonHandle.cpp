//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "hotdraw/main/hdDrawingView.h"

//Images
#include "images/ddMinMaxCursor.pngc"

ddMinMaxTableButtonHandle::ddMinMaxTableButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxBitmap &buttonSecondImage, wxSize &size):
	hdButtonHandle(owner, buttonLocator, buttonImage, size)
{
	buttonMaximizeImage = buttonSecondImage;
	tmpImage = buttonImage;
	showFirst = true;
}

ddMinMaxTableButtonHandle::~ddMinMaxTableButtonHandle()
{
}

void ddMinMaxTableButtonHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
}

void ddMinMaxTableButtonHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
}

void ddMinMaxTableButtonHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
	ddTableFigure *table = (ddTableFigure *) getOwner();

	if(showFirst)
	{
		buttonIcon = buttonMaximizeImage;
		table->setColumnsWindow(view->getIdx(), 1);
	}
	else
	{
		buttonIcon = tmpImage;
		table->setColumnsWindow(table->getTotalColumns(), true);
	}
	showFirst = !showFirst;
	view->notifyChanged();
}
