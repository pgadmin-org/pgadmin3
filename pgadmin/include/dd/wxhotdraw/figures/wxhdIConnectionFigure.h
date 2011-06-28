//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdIConnectionFigure.h - Base class for all connection figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDICONNECTIONFIGURE_H
#define WXHDICONNECTIONFIGURE_H
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/connectors/wxhdIConnector.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"


class wxhdIConnector;

class wxhdIConnectionFigure : public wxhdIFigure
{
public:

	virtual void connectStart(wxhdIConnector *start) = 0;
	virtual void connectEnd(wxhdIConnector *end) = 0;
	virtual void updateConnection() = 0;
	virtual void disconnectStart() = 0;
	virtual void disconnectEnd() = 0;
	virtual bool canConnectStart(wxhdIFigure *figure) = 0;
	virtual bool canConnectEnd(wxhdIFigure *figure) = 0;
	virtual wxhdPoint *pointAt(int index) = 0;
	virtual void splitSegment(int x, int y) = 0;

	virtual int pointCount() = 0;
	virtual wxhdIConnector *getStartConnector() = 0;
	virtual wxhdIConnector *getEndConnector() = 0;
	virtual void setStartConnector(wxhdIConnector *connector) = 0;
	virtual void setEndConnector(wxhdIConnector *connector) = 0;
	virtual wxhdPoint *getStartPoint() = 0;
	virtual void setStartPoint(wxhdPoint *point) = 0;
	virtual wxhdPoint *getEndPoint() = 0;
	virtual void setEndPoint(wxhdPoint *point) = 0;
	virtual wxhdIFigure *getStartFigure() = 0;
	virtual wxhdIFigure *getEndFigure() = 0;
	virtual wxhdIHandle *getStartHandle() = 0;
	virtual wxhdIHandle *getEndHandle() = 0;

protected:

private:

};
#endif
