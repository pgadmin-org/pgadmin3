//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRelationshipTerminal.h - Draw inverse arrows at fk terminal based on kind of relationship.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDRELATIONSHIPTERMINAL_H
#define DDRELATIONSHIPTERMINAL_H
#include "dd/wxhotdraw/figures/wxhdLineTerminal.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"



class ddRelationshipTerminal : public wxhdLineTerminal
{
public:
	ddRelationshipTerminal(ddRelationshipFigure *owner, bool endFigureTerminal);
	~ddRelationshipTerminal();
	virtual wxhdPoint &draw (wxBufferedDC &context, wxhdPoint &a, wxhdPoint &b, wxhdDrawingView *view);
protected:

private:
	double lastFactor;
	ddRelationshipFigure *ownerFigure;
	bool endTerminal;
	wxhdPoint value;
};
#endif
