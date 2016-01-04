//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGraphsimple.cpp - A simple Implementation of the Graphic Interface for GQB
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "utils/sysSettings.h"
#include "gqb/gqbGraphSimple.h"
#include "gqb/gqbQueryObjs.h"
// GQB Images
#include "images/gqbColNotSel.pngc"
#include "images/gqbColSel.pngc"

gqbGraphSimple::gqbGraphSimple()
{
	normalFont = settings->GetSystemFont();
	TableTitleFont = settings->GetSystemFont();
	TableTitleFont.SetWeight(wxFONTWEIGHT_BOLD);
	BackgroundLayer1 = wxBrush(wxColour(112, 112, 112), wxSOLID);
	BackgroundLayer2 = wxBrush (wxColour(208, 208, 208), wxSOLID);
	BackgroundTitle = wxBrush (wxColour(245, 245, 245), wxSOLID);
	minTableWidth = 80;
	minTableHeight = 54;
	rowHeight = 0;                                //By default but this it's replaced by font metrics value
	rowLeftMargin = 14;
	rowRightMargin = 5;
	rowTopMargin = 1;
	lineClickThreshold = 7;
	selectedPen = wxPen(wxColour(0, 146, 195), 2, wxSOLID);
	selectedBrush = wxBrush(wxColour(0, 146, 195), wxSOLID);
	imgSelBoxEmpty = *gqbColNotSel_png_bmp;
	imgSelBoxSelected = *gqbColSel_png_bmp;

}


// NOTES:(1) store values of width & height at queryTable.
// (2)Need to set a font for the device context before get font metrics with GetTextExtent
void gqbGraphSimple::drawTable(wxMemoryDC &bdc, wxPoint *origin, gqbQueryObject *queryTable)
{

#if wxCHECK_VERSION(2, 9, 0)
	wxCoord  w = 0, h = 0, height = 0, width = 0, margin = 5;
#else
	long  w = 0, h = 0, height = 0, width = 0, margin = 5;
#endif

	// Get Value for row Height
	if(!rowHeight)
	{
		bdc.SetFont(TableTitleFont);
		bdc.GetTextExtent(wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxtz"), &w, &h);
		rowHeight = h;
	}

	// Get Title Metrics
	bdc.SetFont(TableTitleFont);
	height += rowHeight + rowTopMargin;

	// Calculate font metrics for table title with/without alias
	if(queryTable->getAlias().length() > 0)
		bdc.GetTextExtent(queryTable->getName() + wxT(" (") + queryTable->getAlias() + wxT(")"), &w, &h);
	else
		bdc.GetTextExtent(queryTable->getName(), &w, &h);
	width = rowLeftMargin + w + rowRightMargin;

	// Get Columns Metrics
	bdc.SetFont(normalFont);

	// Don't use h value from font metrics to get consistency between columns vertical separation (height)
	height += rowHeight * queryTable->parent->countCols() + rowTopMargin * queryTable->parent->countCols();
	gqbIteratorBase *iterator = queryTable->parent->createColumnsIterator();
	while(iterator->HasNext())
	{
		gqbColumn *tmp = (gqbColumn *)iterator->Next();
		bdc.GetTextExtent(tmp->getName(), &w, &h);
		if((rowLeftMargin + w + rowRightMargin) > width)
			width = rowLeftMargin + w + rowRightMargin;
	}

	//Don't delete iterator because will be use below;

	// Set table Size in ObjectModel (Temporary Values for object representation,
	// and for this reason the view can modified model without using the controller
	// because this values are used by controller when use object's size in internal operations)
	if( (height + 2) < minTableHeight) // +2 from BackgroundLayers addition
	{
		queryTable->setHeight(minTableHeight);
		height = minTableHeight;
	}
	else
		queryTable->setHeight(height + 2);

	if( (width + 2) < minTableWidth)
	{
		queryTable->setWidth(minTableWidth);
		width = minTableWidth;
	}
	else
		queryTable->setWidth(width + 2);

	//Decorate Table
	bdc.SetPen(*wxTRANSPARENT_PEN);

	//draw second Layer
	bdc.SetBrush(BackgroundLayer2);
	bdc.DrawRectangle(wxRect(wxPoint(origin->x, origin->y), wxSize(width + 2, height + 2)));

	//draw third Layer
	bdc.SetBrush(BackgroundLayer1);
	bdc.DrawRectangle(wxRect(wxPoint(origin->x, origin->y), wxSize(width + 1, height + 1)));

	//draw real frame layer
	bdc.SetBrush(*wxWHITE_BRUSH);
	if(queryTable->getSelected())
	{
		bdc.SetPen(selectedPen);
	}
	else
	{
		bdc.SetPen(*wxBLACK_PEN);
	}
	bdc.DrawRectangle(wxRect(wxPoint(origin->x, origin->y), wxSize(width, height)));

	//draw title layer
	bdc.SetBrush(BackgroundTitle);
	bdc.DrawRectangle(wxRect(wxPoint(origin->x, origin->y), wxSize(width, rowHeight + rowTopMargin)));
	bdc.SetFont(TableTitleFont);
	if(queryTable->getAlias().length() > 0)
		bdc.DrawText(queryTable->getName() + wxT(" (") + queryTable->getAlias() + wxT(")"), origin->x + margin, origin->y + rowTopMargin);
	else
		bdc.DrawText(queryTable->getName(), origin->x + margin, origin->y + rowTopMargin);
	bdc.SetFont(normalFont);

	// GQB-TODO: in a future reuse a little more the iterator creating it inside the Query or Table Object
	// and only delete it when delete the query object.

	// Draw Columns
	height = rowHeight + rowTopMargin;
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		gqbColumn *tmp = (gqbColumn *)iterator->Next();
		if(queryTable->existsColumn(tmp))
		{
			bdc.SetTextForeground(* wxRED);
			bdc.SetFont(normalFont);
			bdc.DrawBitmap(imgSelBoxSelected, origin->x + 3, origin->y + height, true);
		}
		else
		{
			bdc.SetFont(normalFont);
			bdc.DrawBitmap(imgSelBoxEmpty, origin->x + 3, origin->y + height, true);
		}
		bdc.DrawText(tmp->getName(), origin->x + rowLeftMargin, origin->y + height);
		bdc.SetTextForeground( *wxBLACK);
		height += rowHeight + rowTopMargin;
	}
	delete iterator;                              //now if delete because it's not needed anymore

}


// return a column when a user click on a checkbox [0->16] x point
gqbColumn *gqbGraphSimple::getColumnAtPosition(wxPoint *clickPoint, gqbQueryObject *queryTable, int sensibility)
{
	int countCols = queryTable->parent->countCols(), colPos = -1;
	if(countCols > 0)                             //exists any column
	{
		colPos = (clickPoint->y - queryTable->position.y) / (rowHeight + rowTopMargin);
	}

	int x = clickPoint->x - queryTable->position.x;
	if( (x > 0 && x < sensibility) && colPos > 0)

		// Because 0 is title
		return queryTable->parent->getColumnAtIndex(colPos - 1);
	else
		return NULL;
}


void gqbGraphSimple::drawTempJoinLine(wxMemoryDC &bdc, wxPoint &origin, wxPoint &end)
{
	wxPoint anchorsUsed = wxPoint(0, 0);

	if(origin.x < end.x)
	{
		anchorsUsed.x = 1;
		anchorsUsed.y = -1;
	}
	else
	{
		anchorsUsed.x = -1;
		anchorsUsed.y = 1;
	}

	drawJoin(bdc, origin, end, anchorsUsed, true, _equally);
}


void gqbGraphSimple::drawJoin(wxMemoryDC &bdc, wxPoint &origin, wxPoint &dest, wxPoint &anchorUsed, bool selected = false, type_Join joinKind = _equally)
{
	wxPoint origin2 = origin;
	wxPoint dest2 = dest;

	if(selected)
	{
		bdc.SetPen(selectedPen);
		bdc.SetBrush(selectedBrush);
	}
	else
	{
		bdc.SetPen(*wxBLACK_PEN);
		bdc.SetBrush(*wxBLACK_BRUSH);
	}

	// GQB-TODO: optimize this if possible, I know one other can be the same?

	// getAnchorsUsed() [-1==left]   [1==right] x->origin y->destination
	if(anchorUsed.x == 1)
	{
		bdc.DrawRectangle(origin.x, origin.y - 4, 8, 8);
		origin2.x += 20;
	}
	else
	{
		bdc.DrawRectangle(origin.x - 8, origin.y - 4, 8, 8);
		origin2.x -= 20;
	}

	if(anchorUsed.y == 1)
	{
		bdc.DrawRectangle(dest.x, dest.y - 4, 8, 8);
		dest2.x += 20;
	}
	else
	{
		bdc.DrawRectangle(dest.x - 8, dest.y - 4, 8, 8);
		dest2.x -= 20;
	}

	bdc.DrawLine(origin, origin2);
	bdc.DrawLine(dest, dest2);
	bdc.DrawLine(origin2, dest2);

	// Draw type of join
	switch(joinKind)
	{
		case _equally:
			bdc.DrawText(wxT("="), findLineMiddle(origin2, dest2));
			break;
		case _lesser:
			bdc.DrawText(wxT("<"), findLineMiddle(origin2, dest2));
			break;
		case _greater:
			bdc.DrawText(wxT(">"), findLineMiddle(origin2, dest2));
			break;
		case _equlesser:
			bdc.DrawText(wxT("<="), findLineMiddle(origin2, dest2));
			break;
		case _equgreater:
			bdc.DrawText(wxT(">="), findLineMiddle(origin2, dest2));
			break;
	};
}


// Return true if pt click over a threshold of the join, false if not
bool gqbGraphSimple::clickOnJoin(gqbQueryJoin *join, wxPoint &pt, wxPoint &origin, wxPoint &dest)
{

	wxPoint origin2 = origin;
	wxPoint dest2 = dest;

	if(join->getAnchorsUsed().x == 1)
	{
		origin2.x += 20;
	}
	else
	{
		origin2.x -= 20;
	}

	if(join->getAnchorsUsed().y == 1)
	{
		dest2.x += 20;
	}
	else
	{
		dest2.x -= 20;
	}

	// Check origin anchor
	bool value1 = insideLine(pt, origin, origin2, lineClickThreshold);

	// Check dest anchor
	bool value2 = insideLine(pt, dest, dest2, lineClickThreshold);

	// Check line between both tables
	bool value3 = insideLine(pt, origin2, dest2, lineClickThreshold);

	if(value1 || value2 || value3)
		return true;
	else
		return false;
}


bool gqbGraphSimple::insideLine(wxPoint &pt, wxPoint &p1, wxPoint &p2, int threshold = 7)
{
	bool value = false;
	if(distanceToLine(pt, p1, p2) < threshold)
	{
		value = true;
	}
	return value;
}


wxPoint gqbGraphSimple::findLineMiddle(wxPoint p1, wxPoint p2)
{
	int middleX = -1, middleY = -1;

	int dx =  p2.x - p1.x;
	if(dx > 0)                                    // p1 at left
	{
		middleX = dx / 2 + p1.x;
	}                                             // p1 at right
	else
	{
		middleX = p1.x + dx / 2;
	}

	int dy =  p2.y - p1.y;
	if(dy > 0)                                    // p1 is above
	{
		middleY = dy / 2 + p1.y;
	}                                             // p1 is below
	else
	{
		middleY = p1.y + dy / 2;
	}

	if(dy == 0)
		middleY = p1.y;
	if(dx == 0)
		middleX = p1.x;

	return wxPoint(middleX, middleY);
}


double gqbGraphSimple::distanceToLine(wxPoint pt, wxPoint p1, wxPoint p2)
{
	p2.x -= p1.x;
	p2.y -= p1.y;

	pt.x -= p1.x;
	pt.y -= p1.y;

	double dprod = pt.x * p2.x + pt.y * p2.y;
	double pLenSq;

	if(dprod <= 0.0)
	{
		pLenSq = 0.0;
	}
	else
	{
		pt.x = p2.x - pt.x;
		pt.y = p2.y - pt.y;
		dprod = pt.x * p2.x + pt.y * p2.y;
		if(dprod <= 0.0)
		{
			pLenSq = 0.0;
		}
		else
		{
			pLenSq = dprod * dprod / (p2.x * p2.x + p2.y * p2.y);
		}
	}

	double lengthSq = pt.x * pt.x + pt.y * pt.y - pLenSq;

	if(lengthSq < 0)
	{
		lengthSq = 0;
	}

	double length = sqrt(lengthSq);

	return length;
}


// Set the anchors points [source, destination] for a join
void gqbGraphSimple::calcAnchorPoint(gqbQueryJoin *join)
{
	int index, x, y;
	wxPoint use;                                  // [-1==left]   [1==right] x->origin y->destination
	int sx = join->getSourceQTable()->position.x;
	int sy = join->getSourceQTable()->position.y;
	int dx = join->getDestQTable()->position.x;
	int dy = join->getDestQTable()->position.y;

	// Source
	index = join->getSourceQTable()->getColumnIndex(join->getSCol()) + 1;
	if(sx < dx)
	{
		x = sx + join->getSourceQTable()->getWidth();
		use.x = 1;
	}
	else
	{
		x = sx;
		use.x = -1;
	}
	y = sy + index * (rowHeight + rowTopMargin) + ((rowHeight + rowTopMargin) / 2);
	join->setSourceAnchor(wxPoint(x, y));

	// Destination
	index = join->getDestQTable()->getColumnIndex(join->getDCol()) + 1;
	if(dx < sx)
	{
		x = dx + join->getDestQTable()->getWidth();
		use.y = 1;
	}
	else
	{
		x = dx;
		use.y = -1;
	}
	y = dy + index * (rowHeight + rowTopMargin) + ((rowHeight + rowTopMargin) / 2);
	join->setDestAnchor(wxPoint(x, y));

	join->setAnchorsUsed(use);
}


// Update position of Object in the query if move table & adjust all other items like joins (own & registered)
void  gqbGraphSimple::UpdatePosObject(gqbQueryObject *queryTable, int x, int y, int cursorAdjustment)
{
	x -= cursorAdjustment;                        // Move Pointer to a better Position;
	y -= rowHeight / 2;

	// Update position of table
	// Do not allow table/view moved/repositioned less than (0, 0) cordinates
	queryTable->position.x = x > 0 ? x : 0;
	queryTable->position.y = y > 0 ? y : 0;

	// Update position of anchor points of Joins that origin from this table
	if(queryTable->getHaveJoins())
	{
		gqbIteratorBase *j = queryTable->createJoinsIterator();
		while(j->HasNext())
		{
			gqbQueryJoin *tmp = (gqbQueryJoin *)j->Next();
			calcAnchorPoint(tmp);
		}
		delete j;
	}

	// Update position of anchor points of Joins that come from others tables
	if(queryTable->getHaveRegJoins())
	{
		gqbIteratorBase *r = queryTable->createRegJoinsIterator();
		while(r->HasNext())
		{
			gqbQueryJoin *tmp = (gqbQueryJoin *)r->Next();
			calcAnchorPoint(tmp);
		}
		delete r;
	}
}


int gqbGraphSimple::getTitleRowHeight()
{
	return rowHeight;
}
