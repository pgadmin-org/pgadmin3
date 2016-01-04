//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdChangeConnectionStartHandle.cpp - Handle to allow change connected figure at start of connection figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/handles/hdChangeConnectionStartHandle.h"
#include "hotdraw/utilities/hdPoint.h"

hdChangeConnectionStartHandle::hdChangeConnectionStartHandle(hdLineConnection *owner):
	hdChangeConnectionHandle(owner)
{
}

hdChangeConnectionStartHandle::~hdChangeConnectionStartHandle()
{
}

hdPoint &hdChangeConnectionStartHandle::locate(int posIdx)
{
	return connection->getStartPoint(posIdx);
}

hdIConnector *hdChangeConnectionStartHandle::target()
{
	return connection->getStartConnector();
}

void hdChangeConnectionStartHandle::connect(hdIConnector *connector, hdDrawingView *view)
{
	connection->connectStart(connector, view);
}

void hdChangeConnectionStartHandle::disconnect(hdDrawingView *view)
{
	connection->disconnectStart(view);
}

bool hdChangeConnectionStartHandle::isConnectionPossible(hdIFigure *figure)
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

void hdChangeConnectionStartHandle::setPoint(int posIdx, hdPoint p)
{
	connection->setStartPoint(posIdx, p);
}
