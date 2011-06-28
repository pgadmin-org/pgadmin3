//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddAddFkButtonHandle.cpp - A handle for a table figure that allow to graphically add relationships (fk)
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/handles/ddAddFkButtonHandle.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "dd/wxhotdraw/tools/wxhdConnectionCreationTool.h"
#include "dd/wxhotdraw/utilities/wxhdMouseEvent.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/figures/ddRelationshipTerminal.h"

//Images
#include "images/ddRelationshipCursor.pngc"

ddAddFkButtonHandle::ddAddFkButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size):
	wxhdButtonHandle(owner, buttonLocator, buttonImage, size)
{
	handleCursorImage = wxBitmap(*ddRelationshipCursor_png_img).ConvertToImage();
	handleCursor = wxCursor(handleCursorImage);
}

ddAddFkButtonHandle::~ddAddFkButtonHandle()
{
}

void ddAddFkButtonHandle::invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view)
{
	if(getOwner()->ms_classInfo.IsKindOf(&ddTableFigure::ms_classInfo))
	{
		ddRelationshipFigure *fkConnection = new ddRelationshipFigure();
		fkConnection->setStartTerminal(new ddRelationshipTerminal(fkConnection, false));
		fkConnection->setEndTerminal(new ddRelationshipTerminal(fkConnection, true));
		wxhdConnectionCreationTool *conn = new wxhdConnectionCreationTool(view->editor(), fkConnection);
		view->editor()->setTool(conn);
		// Simulate button down to start connection of foreign key
		wxMouseEvent e(wxEVT_LEFT_DOWN);
		e.m_x = event.GetPosition().x;
		e.m_y = event.GetPosition().y;
		e.SetEventObject(view);
		wxhdMouseEvent evento(e, view);
		conn->mouseDown(evento);
		ddTableFigure *table = (ddTableFigure *) getOwner();
		table->setSelectFkDestMode(true);
	}
}

void ddAddFkButtonHandle::invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

void ddAddFkButtonHandle::invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view)
{
}

wxCursor ddAddFkButtonHandle::createCursor()
{
	return handleCursor;
}
