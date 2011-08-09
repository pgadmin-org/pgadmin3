//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdChangeConnectionHandle.h - Base Handle to allow change connected figures at connection figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCHANGECONNECTIONHANDLE_H
#define WXHDCHANGECONNECTIONHANDLE_H

#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/figures/wxhdLineConnection.h"
#include "dd/wxhotdraw/connectors/wxhdIConnector.h"
#include "dd/wxhotdraw/main/wxhdDrawing.h"

class wxhdChangeConnectionHandle : public wxhdIHandle
{
public:
	wxhdChangeConnectionHandle(wxhdLineConnection *owner);
	~wxhdChangeConnectionHandle();

	virtual void draw(wxBufferedDC &context, wxhdDrawingView *view);
	virtual wxCursor createCursor();
	virtual void invokeStart(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeStep(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual void invokeEnd(wxhdMouseEvent &event, wxhdDrawingView *view);
	virtual wxhdIConnector *target() = 0;
	virtual void disconnect(wxhdDrawingView *view = NULL) = 0;
	virtual void connect(wxhdIConnector *connector, wxhdDrawingView *view = NULL) = 0;
	virtual void setPoint(int posIdx, wxhdPoint p) = 0;
	virtual bool isConnectionPossible(wxhdIFigure *figure) = 0;
	wxhdIFigure *findConnectableFigure(int posIdx, int x, int y, wxhdDrawing *drawing);
	wxhdIConnector *findConnectionTarget(int posIdx, int x, int y, wxhdDrawing *drawing);
protected:
	wxhdLineConnection *connection;
	wxhdIFigure *targetFigure;
private:
	wxhdIConnector *originalTarget;
};
#endif
