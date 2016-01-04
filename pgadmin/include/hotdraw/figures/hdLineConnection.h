//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLineConnection.h - Base class for line connection figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDLINECONNECTION_H
#define HDLINECONNECTION_H
#include "hotdraw/figures/hdPolyLineFigure.h"
#include "hotdraw/connectors/hdIConnector.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/handles/hdIHandle.h"

class hdLineConnection : public hdPolyLineFigure
{
public:
	hdLineConnection();
	hdLineConnection(int posIdx, hdIFigure *figure1, hdIFigure *figure2);
	~hdLineConnection();

	virtual void connectStart(hdIConnector *start, hdDrawingView *view = NULL);
	virtual void connectEnd(hdIConnector *end, hdDrawingView *view = NULL);
	virtual void disconnectStart(hdDrawingView *view = NULL);
	virtual void disconnectEnd(hdDrawingView *view = NULL);
	virtual void updateConnection(int posIdx);
	virtual bool canConnectStart(hdIFigure *figure);
	virtual bool canConnectEnd(hdIFigure *figure);
	virtual bool canConnect();
	virtual void setPointAt (int posIdx, int index, int x, int y);
	virtual hdCollection *handlesEnumerator();
	virtual void basicMoveBy(int posIdx, int x, int y);

	virtual void onFigureChanged(int posIdx, hdIFigure *figure);

	virtual hdIConnector *getStartConnector();
	virtual hdIConnector *getEndConnector();
	virtual void setStartConnector(hdIConnector *connector);
	virtual void setEndConnector(hdIConnector *connector);
	virtual hdIFigure *getStartFigure();
	virtual hdIFigure *getEndFigure();
	virtual hdIHandle *getStartHandle();
	virtual hdIHandle *getEndHandle();
	virtual void addPoint (int posIdx, int x, int y);
	virtual void insertPointAt (int posIdx, int index, int x, int y);

protected:
	virtual void updateHandlesIndexes();
	virtual void connectFigure (hdIConnector *connector);
	virtual void disconnectFigure (hdIConnector *connector);
	hdIConnector *startConnector;
	hdIConnector *endConnector;

private:
	hdIHandle *changeConnStartHandle;
	hdIHandle *changeConnEndHandle;
};
#endif
