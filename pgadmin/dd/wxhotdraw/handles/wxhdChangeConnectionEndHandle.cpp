//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdChangeConnectionEndHandle.cpp - Handle to allow change connected figure at end of connection figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/handles/wxhdChangeConnectionEndHandle.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

wxhdChangeConnectionEndHandle::wxhdChangeConnectionEndHandle(wxhdLineConnection *owner):
	wxhdChangeConnectionHandle(owner)
{
}

wxhdChangeConnectionEndHandle::~wxhdChangeConnectionEndHandle()
{
}

wxhdPoint &wxhdChangeConnectionEndHandle::locate(int posIdx)
{
	return connection->getEndPoint(posIdx);
}

wxhdIConnector *wxhdChangeConnectionEndHandle::target()
{
	return connection->getEndConnector();
}

void wxhdChangeConnectionEndHandle::connect(wxhdIConnector *connector, wxhdDrawingView *view)
{
	connection->connectEnd(connector, view);
}

void wxhdChangeConnectionEndHandle::disconnect(wxhdDrawingView *view)
{
	connection->disconnectEnd(view);
}

bool wxhdChangeConnectionEndHandle::isConnectionPossible(wxhdIFigure *figure)
{
	if(!figure->includes(connection) && figure->canConnect() && connection->canConnectEnd(figure))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void wxhdChangeConnectionEndHandle::setPoint(int posIdx, wxhdPoint p)
{
	connection->setEndPoint(posIdx, p);
}