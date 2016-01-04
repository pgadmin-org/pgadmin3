//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractFigure.h - A StickyRectangleConnector locates connection points by choping
// the connection between the centers of the two figures at the display box. The location
// of the connection point is computed once, when the user connects the figure.
// Moving the figure around will not change the location.
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDSTICKYRECTANGLECONNECTOR_H
#define HDSTICKYRECTANGLECONNECTOR_H

#include "hotdraw/main/hdObject.h"
#include "hotdraw/figures/hdLineConnection.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/connectors/hdChopBoxConnector.h"

class hdLineConnection;
class hdIFigure;

class hdStickyRectangleConnector : public hdChopBoxConnector
{
public:
	hdStickyRectangleConnector(hdIFigure *owner, hdPoint p);
	~hdStickyRectangleConnector();
	virtual void setAngle(float newAngle);
	virtual void updateAnchor(int posIdx, hdPoint p);
	virtual hdPoint getAnchor(int posIdx);
	virtual hdPoint chop(int posIdx, hdIFigure *target, hdPoint point);
	virtual void draw(wxBufferedDC &context);
protected:
	float angle;
private:

};
#endif
