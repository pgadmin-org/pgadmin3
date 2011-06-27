//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractFigure.h - A StickyRectangleConnector locates connection points by choping
// the connection between the centers of the two figures at the display box. The location 
// of the connection point is computed once, when the user connects the figure. 
// Moving the figure around will not change the location.
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDSTICKYRECTANGLECONNECTOR_H
#define WXHDSTICKYRECTANGLECONNECTOR_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/figures/wxhdLineConnection.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/connectors/wxhdChopBoxConnector.h"

class wxhdLineConnection;
class wxhdIFigure;

class wxhdStickyRectangleConnector : public wxhdChopBoxConnector
{
public:
	wxhdStickyRectangleConnector(wxhdIFigure *owner, wxhdPoint p);
 	~wxhdStickyRectangleConnector();
	virtual void setAngle(float newAngle);
	virtual void updateAnchor(wxhdPoint p);
	virtual wxhdPoint getAnchor();
	virtual wxhdPoint chop(wxhdIFigure *target, wxhdPoint point);
	virtual void draw(wxBufferedDC& context);
protected:
	float angle;
private:

};
#endif
