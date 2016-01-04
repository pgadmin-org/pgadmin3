//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdChangeConnectionStartHandle.h - Handle to allow change connected figure at start of connection figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCHANGECONNECTIONSTARTHANDLE_H
#define HDCHANGECONNECTIONSTARTHANDLE_H

#include "hotdraw/handles/hdChangeConnectionHandle.h"
#include "hotdraw/figures/hdLineConnection.h"

class hdChangeConnectionStartHandle : public hdChangeConnectionHandle
{
public:
	hdChangeConnectionStartHandle(hdLineConnection *owner);
	~hdChangeConnectionStartHandle();
	virtual hdPoint &locate(int posIdx);
	virtual hdIConnector *target();
	virtual void disconnect(hdDrawingView *view = NULL);
	virtual void connect(hdIConnector *connector, hdDrawingView *view = NULL);
	virtual bool isConnectionPossible(hdIFigure *figure);
	virtual void setPoint(int posIdx, hdPoint p);
};
#endif
