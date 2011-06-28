//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdChangeConnectionStartHandle.cpp - Handle to allow change connected figure at start of connection figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/handles/wxhdChangeConnectionStartHandle.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

wxhdChangeConnectionStartHandle::wxhdChangeConnectionStartHandle(wxhdLineConnection *owner):
	wxhdChangeConnectionHandle(owner)
{
}

wxhdChangeConnectionStartHandle::~wxhdChangeConnectionStartHandle()
{
}

wxhdPoint &wxhdChangeConnectionStartHandle::locate()
{
	return connection->getStartPoint();
}

wxhdIConnector *wxhdChangeConnectionStartHandle::target()
{
	return connection->getStartConnector();
}

void wxhdChangeConnectionStartHandle::connect(wxhdIConnector *connector, wxhdDrawingView *view)
{
	connection->connectStart(connector, view);
}

void wxhdChangeConnectionStartHandle::disconnect(wxhdDrawingView *view)
{
	connection->disconnectStart(view);
}

bool wxhdChangeConnectionStartHandle::isConnectionPossible(wxhdIFigure *figure)
{
	if(!figure->includes(connection) && figure->canConnect() && connection->canConnectStart(figure))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void wxhdChangeConnectionStartHandle::setPoint(wxhdPoint p)
{
	connection->setStartPoint(p);
}