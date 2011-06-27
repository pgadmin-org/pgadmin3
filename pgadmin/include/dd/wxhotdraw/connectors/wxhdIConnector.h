//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdIConnector.cpp - Base class for all connectors
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDICONNECTOR_H
#define WXHDICONNECTOR_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/figures/wxhdLineConnection.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"

class wxhdLineConnection;
class wxhdIFigure;

class wxhdIConnector : public wxhdObject
{
public:
	wxhdIConnector(wxhdIFigure *owner);
 	~wxhdIConnector();
	virtual wxhdPoint findStart(wxhdLineConnection *connection);
	virtual wxhdPoint findEnd(wxhdLineConnection *connection);
	virtual bool containsPoint(int x, int y);
	virtual void draw(wxBufferedDC& context);
	virtual wxhdIFigure* getOwner();
	virtual wxhdRect& getDisplayBox();
protected:
	virtual void setOwner(wxhdIFigure *owner);
	wxhdRect displayBox;
private:
	wxhdIFigure *figureOwner; 

};
#endif
