//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "hotdraw/tools/hdConnectionCreationTool.h"
#include "hotdraw/utilities/hdMouseEvent.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/figures/ddRelationshipTerminal.h"

//Images
#include "images/ddRelationshipCursor.pngc"

ddAddFkButtonHandle::ddAddFkButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size):
	hdButtonHandle(owner, buttonLocator, buttonImage, size)
{
}

ddAddFkButtonHandle::~ddAddFkButtonHandle()
{
}

void ddAddFkButtonHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
	if(getOwner()->ms_classInfo.IsKindOf(&ddTableFigure::ms_classInfo))
	{
		ddRelationshipFigure *fkConnection = new ddRelationshipFigure();
		//Check figure available positions for diagrams, add at least needed to allow initialization of the class
		int i, start;
		start = fkConnection->displayBox().CountPositions();
		for(i = start; i < (view->getIdx() + 1); i++)
		{
			fkConnection->AddPosForNewDiagram();
		}
		fkConnection->setStartTerminal(new ddRelationshipTerminal(fkConnection, false));
		fkConnection->setEndTerminal(new ddRelationshipTerminal(fkConnection, true));
		hdConnectionCreationTool *conn = new hdConnectionCreationTool(view, fkConnection);
		view->setTool(conn);
		// Simulate button down to start connection of foreign key
		wxMouseEvent e(wxEVT_LEFT_DOWN);
		e.m_x = event.GetPosition().x;
		e.m_y = event.GetPosition().y;
		e.SetEventObject(view);
		hdMouseEvent evento(e, view);
		conn->mouseDown(evento);
		ddTableFigure *table = (ddTableFigure *) getOwner();
		table->setSelectFkDestMode(true);
		view->notifyChanged();
	}
}

void ddAddFkButtonHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
}

void ddAddFkButtonHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
}
