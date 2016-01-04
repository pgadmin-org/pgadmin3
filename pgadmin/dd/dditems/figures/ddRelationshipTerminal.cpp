//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRelationshipTerminal.cpp - Draw inverse arrows at fk terminal based on kind of relationship.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/dditems/figures/ddRelationshipTerminal.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/utilities/hdGeometry.h"

ddRelationshipTerminal::ddRelationshipTerminal(ddRelationshipFigure *owner, bool endFigureTerminal)
{
	ownerFigure = owner;
	endTerminal = endFigureTerminal;
}

ddRelationshipTerminal::~ddRelationshipTerminal()
{
}

hdPoint &ddRelationshipTerminal::draw (wxBufferedDC &context, hdPoint &a, hdPoint &b, hdDrawingView *view)
{
	hdGeometry g;
	hdPoint points[3];

	context.SetPen(terminalLinePen);

	hdPoint aCopy = a, bCopy = b;
	view->CalcScrolledPosition(aCopy.x, aCopy.y, &aCopy.x, &aCopy.y);
	view->CalcScrolledPosition(bCopy.x, bCopy.y, &bCopy.x, &bCopy.y);

	if(endTerminal)
	{
		//Calc a point very far away of center of table to intersect one of the sides lines of the table rectangle figure
		double X = aCopy.x + (bCopy.x - aCopy.x) * 0.9;
		double Y = aCopy.y + (bCopy.y - aCopy.y) * 0.9;

		if(ownerFigure->getEndFigure() && ownerFigure->getOneToMany())
		{
			hdRect r = ownerFigure->getEndFigure()->displayBox().gethdRect(view->getIdx());

			view->CalcScrolledPosition(r.x, r.y, &r.x, &r.y);

			int centerX = r.x + r.width / 2;
			int centerY = r.y + r.height / 2;

			context.SetPen(*wxBLACK_PEN);
			context.SetBrush(*wxBLACK_BRUSH);

			double XX, YY, distance;

			//Calculate a new point to a given distance from the end of the relationship to draw many ( ----<| ) connector
			//first calculate vector from point1 & point2
			double vectorx = aCopy.x - bCopy.x;
			double vectory = aCopy.y - bCopy.y;
			//calculate the length
			double length = sqrt(vectorx * vectorx + vectory * vectory);
			//normalize the vector to unit length
			double normalizevx = vectorx / length;
			double normalizevy = vectory / length;
			distance = -15;
			//calculate point a given distance
			XX = bCopy.x + normalizevx * (length + distance);
			YY = bCopy.y + normalizevy * (length + distance);

			wxPoint intersectionLine1(centerX, centerY);
			wxPoint intersectionLine2(X, Y);

			//TOP
			if(g.intersection(intersectionLine1, intersectionLine2, r.GetTopLeft(), r.GetTopRight()))
			{
				points[0] = wxPoint(XX, YY);
				points[1] = wxPoint(aCopy.x - 7, aCopy.y);
				points[2] = wxPoint(aCopy.x + 7, aCopy.y);
				context.DrawPolygon(3, points);

				if(ownerFigure->getIdentifying())
				{
					context.SetPen(wxPen(*wxBLACK, 2));
					context.DrawLine(wxPoint(XX - 7, YY), wxPoint(XX + 7, YY));
					context.SetPen(*wxBLACK_PEN);
				}

			}	//RIGHT
			else if(g.intersection(intersectionLine1, intersectionLine2, r.GetTopRight(), r.GetBottomRight()))
			{
				points[0] = wxPoint(XX, YY);
				points[1] = wxPoint(aCopy.x, aCopy.y - 7);
				points[2] = wxPoint(aCopy.x, aCopy.y + 7);
				context.DrawPolygon(3, points);

				if(ownerFigure->getIdentifying())
				{
					context.SetPen(wxPen(*wxBLACK, 2));
					context.DrawLine(wxPoint(XX, YY - 7), wxPoint(XX, YY + 7));
					context.SetPen(*wxBLACK_PEN);
				}
			}	//BOTTOM
			else if(g.intersection(intersectionLine1, intersectionLine2, r.GetBottomLeft(), r.GetBottomRight()))
			{
				points[0] = wxPoint(XX, YY);
				points[1] = wxPoint(aCopy.x - 7, aCopy.y);
				points[2] = wxPoint(aCopy.x + 7, aCopy.y);
				context.DrawPolygon(3, points);

				if(ownerFigure->getIdentifying())
				{
					context.SetPen(wxPen(*wxBLACK, 2));
					context.DrawLine(wxPoint(XX - 7, YY), wxPoint(XX + 7, YY));
					context.SetPen(*wxBLACK_PEN);
				}
			}	//LEFT
			else if(g.intersection(intersectionLine1, intersectionLine2, r.GetTopLeft(), r.GetBottomLeft()))
			{
				points[0] = wxPoint(XX, YY);
				points[1] = wxPoint(aCopy.x, aCopy.y - 7);
				points[2] = wxPoint(aCopy.x, aCopy.y + 7);
				context.DrawPolygon(3, points);

				if(ownerFigure->getIdentifying())
				{
					context.SetPen(wxPen(*wxBLACK, 2));
					context.DrawLine(wxPoint(XX, YY - 7), wxPoint(XX, YY + 7));
					context.SetPen(*wxBLACK_PEN);
				}
			}
			else
			{
				//CENTER of star figure or invalid place, do nothing
			}

			value = hdPoint(XX, YY);
			return value;
		}
		value = hdPoint(0, 0);
		return value;
	}
	value = hdPoint(0, 0);
	return value;
}
