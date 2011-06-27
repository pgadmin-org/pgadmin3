//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdChangeConnectionStartHandle.h - Handle to allow change connected figure at start of connection figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCHANGECONNECTIONSTARTHANDLE_H
#define WXHDCHANGECONNECTIONSTARTHANDLE_H

#include "dd/wxhotdraw/handles/wxhdChangeConnectionHandle.h"
#include "dd/wxhotdraw/figures/wxhdLineConnection.h"

class wxhdChangeConnectionStartHandle : public wxhdChangeConnectionHandle
{
public:
	wxhdChangeConnectionStartHandle(wxhdLineConnection *owner);
    ~wxhdChangeConnectionStartHandle();
	virtual wxhdPoint& locate();
	virtual wxhdIConnector* target();
	virtual void disconnect(wxhdDrawingView *view=NULL);
	virtual void connect(wxhdIConnector *connector, wxhdDrawingView *view=NULL);
	virtual bool isConnectionPossible(wxhdIFigure *figure);
	virtual void setPoint(wxhdPoint p);
};
#endif
