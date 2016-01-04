//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdChangeConnectionEndHandle.h - Handle to allow change connected figure at end of connection figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCHANGECONNECTIONENDHANDLE_H
#define HDCHANGECONNECTIONENDHANDLE_H

#include "hotdraw/handles/hdChangeConnectionHandle.h"
#include "hotdraw/figures/hdLineConnection.h"

class hdChangeConnectionEndHandle : public hdChangeConnectionHandle
{
public:
	hdChangeConnectionEndHandle(hdLineConnection *owner);
	~hdChangeConnectionEndHandle();
	virtual hdPoint &locate(int posIdx);
	virtual hdIConnector *target();
	virtual void disconnect(hdDrawingView *view = NULL);
	virtual void connect(hdIConnector *connector, hdDrawingView *view = NULL);
	virtual bool isConnectionPossible(hdIFigure *figure);
	virtual void setPoint(int posIdx, hdPoint p);
};
#endif
