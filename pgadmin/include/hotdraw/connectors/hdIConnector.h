//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdIConnector.cpp - Base class for all connectors
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDICONNECTOR_H
#define HDICONNECTOR_H

#include "hotdraw/main/hdObject.h"
#include "hotdraw/figures/hdLineConnection.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/utilities/hdRect.h"

class hdLineConnection;
class hdIFigure;

class hdIConnector : public hdObject
{
public:
	hdIConnector(hdIFigure *owner);
	~hdIConnector();
	virtual hdPoint findStart(int posIdx, hdLineConnection *connection);
	virtual hdPoint findEnd(int posIdx, hdLineConnection *connection);
	virtual bool containsPoint(int posIdx, int x, int y);
	virtual void draw(wxBufferedDC &context);
	virtual hdIFigure *getOwner();
	virtual hdMultiPosRect &getDisplayBox();
protected:
	virtual void setOwner(hdIFigure *owner);
private:
	hdIFigure *figureOwner;

};
#endif
