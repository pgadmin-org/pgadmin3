//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRelationshipTerminal.h - Draw inverse arrows at fk terminal based on kind of relationship.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDRELATIONSHIPTERMINAL_H
#define DDRELATIONSHIPTERMINAL_H
#include "hotdraw/figures/hdLineTerminal.h"
#include "hotdraw/utilities/hdPoint.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"



class ddRelationshipTerminal : public hdLineTerminal
{
public:
	ddRelationshipTerminal(ddRelationshipFigure *owner, bool endFigureTerminal);
	~ddRelationshipTerminal();
	virtual hdPoint &draw (wxBufferedDC &context, hdPoint &a, hdPoint &b, hdDrawingView *view);
protected:

private:
	double lastFactor;
	ddRelationshipFigure *ownerFigure;
	bool endTerminal;
	hdPoint value;
};
#endif
