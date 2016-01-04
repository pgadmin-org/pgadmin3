//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdIConnectionFigure.h - Base class for all connection figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDICONNECTIONFIGURE_H
#define HDICONNECTIONFIGURE_H
#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/connectors/hdIConnector.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/handles/hdIHandle.h"


class hdIConnector;

class hdIConnectionFigure : public hdIFigure
{
public:

	virtual void connectStart(hdIConnector *start) = 0;
	virtual void connectEnd(hdIConnector *end) = 0;
	virtual void updateConnection() = 0;
	virtual void disconnectStart() = 0;
	virtual void disconnectEnd() = 0;
	virtual bool canConnectStart(hdIFigure *figure) = 0;
	virtual bool canConnectEnd(hdIFigure *figure) = 0;
	virtual hdPoint *pointAt(int index) = 0;
	virtual void splitSegment(int x, int y) = 0;

	virtual int pointCount() = 0;
	virtual hdIConnector *getStartConnector() = 0;
	virtual hdIConnector *getEndConnector() = 0;
	virtual void setStartConnector(hdIConnector *connector) = 0;
	virtual void setEndConnector(hdIConnector *connector) = 0;
	virtual hdPoint *getStartPoint() = 0;
	virtual void setStartPoint(hdPoint *point) = 0;
	virtual hdPoint *getEndPoint() = 0;
	virtual void setEndPoint(hdPoint *point) = 0;
	virtual hdIFigure *getStartFigure() = 0;
	virtual hdIFigure *getEndFigure() = 0;
	virtual hdIHandle *getStartHandle() = 0;
	virtual hdIHandle *getEndHandle() = 0;

protected:

private:

};
#endif
