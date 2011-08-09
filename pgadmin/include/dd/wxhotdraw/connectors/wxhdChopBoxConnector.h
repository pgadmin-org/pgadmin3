//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdChopBoxConnector.h - Connector for center of figure to line crossing one limit line of rect
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCHOPBOXCONNECTOR_H
#define WXHDCHOPBOXCONNECTOR_H


#include "dd/wxhotdraw/connectors/wxhdIConnector.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class wxhdChopBoxConnector : public wxhdIConnector
{
public:
	wxhdChopBoxConnector(wxhdIFigure *owner);
	~wxhdChopBoxConnector();
	virtual wxhdPoint findStart(int posIdx, wxhdLineConnection *connFigure);
	virtual wxhdPoint findEnd(int posIdx, wxhdLineConnection *connFigure);
protected:
	virtual wxhdPoint chop(int posIdx, wxhdIFigure *target, wxhdPoint point);
private:
	wxhdPoint point;
	wxhdRect rect;

};
#endif
