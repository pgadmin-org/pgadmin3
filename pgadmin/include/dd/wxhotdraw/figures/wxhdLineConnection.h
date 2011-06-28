//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLineConnection.h - Base class for line connection figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDLINECONNECTION_H
#define WXHDLINECONNECTION_H
#include "dd/wxhotdraw/figures/wxhdPolyLineFigure.h"
#include "dd/wxhotdraw/connectors/wxhdIConnector.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"

class wxhdLineConnection : public wxhdPolyLineFigure
{
public:
	wxhdLineConnection();
	wxhdLineConnection(wxhdIFigure *figure1, wxhdIFigure *figure2);
	~wxhdLineConnection();

	virtual void connectStart(wxhdIConnector *start, wxhdDrawingView *view = NULL);
	virtual void connectEnd(wxhdIConnector *end, wxhdDrawingView *view = NULL);
	virtual void disconnectStart(wxhdDrawingView *view = NULL);
	virtual void disconnectEnd(wxhdDrawingView *view = NULL);
	virtual void updateConnection();
	virtual bool canConnectStart(wxhdIFigure *figure);
	virtual bool canConnectEnd(wxhdIFigure *figure);
	virtual bool canConnect();
	virtual void setPointAt (int index, int x, int y);
	virtual wxhdCollection *handlesEnumerator();
	virtual void basicMoveBy(int x, int y);

	virtual void onFigureChanged(wxhdIFigure *figure);

	virtual wxhdIConnector *getStartConnector();
	virtual wxhdIConnector *getEndConnector();
	virtual void setStartConnector(wxhdIConnector *connector);
	virtual void setEndConnector(wxhdIConnector *connector);
	virtual wxhdIFigure *getStartFigure();
	virtual wxhdIFigure *getEndFigure();
	virtual wxhdIHandle *getStartHandle();
	virtual wxhdIHandle *getEndHandle();
	virtual void addPoint (int x, int y);
	virtual void insertPointAt (int index, int x, int y);

protected:
	virtual void updateHandlesIndexes();
	virtual void connectFigure (wxhdIConnector *connector);
	virtual void disconnectFigure (wxhdIConnector *connector);
	wxhdIConnector *startConnector;
	wxhdIConnector *endConnector;

private:
	wxhdIHandle *changeConnStartHandle;
	wxhdIHandle *changeConnEndHandle;
};
#endif
