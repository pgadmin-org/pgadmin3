//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLocatorConnector.h - class that puts connects at locator position
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDLOCATORCONNECTOR_H
#define HDLOCATORCONNECTOR_H

#include "hotdraw/main/hdObject.h"
#include "hotdraw/figures/hdLineConnection.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/locators/hdILocator.h"

class hdLocatorConnector : public hdIConnector
{
public:
	hdLocatorConnector(hdIFigure *owner, hdILocator *locator);
	~hdLocatorConnector();
	virtual hdPoint findStart(int posIdx, hdLineConnection *connection);
	virtual hdPoint findEnd(int posIdx, hdLineConnection *connection);
	virtual bool containsPoint(int posIdx, int x, int y);
	virtual void draw(wxBufferedDC &context);
	virtual hdPoint locate(int posIdx);
	virtual hdRect &getDisplayBox(int posIdx);
protected:
	int size; //standard size connector
	hdRect displayBox;
private:
	hdILocator *figureLocator;
	hdIFigure *figureOwner;

};
#endif
