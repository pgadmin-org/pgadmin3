//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdChangeConnectionHandle.h - Base Handle to allow change connected figures at connection figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCHANGECONNECTIONHANDLE_H
#define HDCHANGECONNECTIONHANDLE_H

#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/figures/hdLineConnection.h"
#include "hotdraw/connectors/hdIConnector.h"
#include "hotdraw/main/hdDrawing.h"

class hdChangeConnectionHandle : public hdIHandle
{
public:
	hdChangeConnectionHandle(hdLineConnection *owner);
	~hdChangeConnectionHandle();

	virtual void draw(wxBufferedDC &context, hdDrawingView *view);
	virtual wxCursor createCursor();
	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view);
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view);
	virtual hdIConnector *target() = 0;
	virtual void disconnect(hdDrawingView *view = NULL) = 0;
	virtual void connect(hdIConnector *connector, hdDrawingView *view = NULL) = 0;
	virtual void setPoint(int posIdx, hdPoint p) = 0;
	virtual bool isConnectionPossible(hdIFigure *figure) = 0;
	hdIFigure *findConnectableFigure(int posIdx, int x, int y, hdDrawing *drawing);
	hdIConnector *findConnectionTarget(int posIdx, int x, int y, hdDrawing *drawing);
protected:
	hdLineConnection *connection;
	hdIFigure *targetFigure;
private:
	hdIConnector *originalTarget;
};
#endif
