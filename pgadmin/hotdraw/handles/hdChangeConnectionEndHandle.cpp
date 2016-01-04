//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdChangeConnectionEndHandle.cpp - Handle to allow change connected figure at end of connection figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/handles/hdChangeConnectionEndHandle.h"
#include "hotdraw/utilities/hdPoint.h"

hdChangeConnectionEndHandle::hdChangeConnectionEndHandle(hdLineConnection *owner):
	hdChangeConnectionHandle(owner)
{
}

hdChangeConnectionEndHandle::~hdChangeConnectionEndHandle()
{
}

hdPoint &hdChangeConnectionEndHandle::locate(int posIdx)
{
	return connection->getEndPoint(posIdx);
}

hdIConnector *hdChangeConnectionEndHandle::target()
{
	return connection->getEndConnector();
}

void hdChangeConnectionEndHandle::connect(hdIConnector *connector, hdDrawingView *view)
{
	connection->connectEnd(connector, view);
}

void hdChangeConnectionEndHandle::disconnect(hdDrawingView *view)
{
	connection->disconnectEnd(view);
}

bool hdChangeConnectionEndHandle::isConnectionPossible(hdIFigure *figure)
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

void hdChangeConnectionEndHandle::setPoint(int posIdx, hdPoint p)
{
	connection->setEndPoint(posIdx, p);
}
