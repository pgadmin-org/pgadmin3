//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddScrollBarHandle.cpp - A handle for a table figure that allow to scroll it when table is not in full size
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/handles/ddScrollBarHandle.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/utilities/hdGeometry.h"

//Images
#include "images/ddUp.pngc"
#include "images/ddDown.pngc"

ddScrollBarHandle::ddScrollBarHandle(ddTableFigure *owner, hdILocator *scrollBarLocator , wxSize &size):
	hdLocatorHandle(owner, scrollBarLocator)
{
	table = owner;
	scrollLocator = scrollBarLocator;
	displayBox.SetSize(size);
	upBitmap = wxBitmap(*ddUp_png_img);
	downBitmap = wxBitmap(*ddDown_png_img);
}

ddScrollBarHandle::~ddScrollBarHandle()
{
}


wxCursor ddScrollBarHandle::createCursor()
{
	return wxCursor(wxCURSOR_HAND);
}

//avoid to use inflate on this handle
hdRect &ddScrollBarHandle::getDisplayBox(int posIdx)
{
	hdPoint p = locate(posIdx);
	displayBox.width = 11; //as defined at locator
	displayBox.height = table->getColsSpace().height;
	displayBox.SetPosition(p);
	return displayBox;
}

void ddScrollBarHandle::draw(wxBufferedDC &context, hdDrawingView *view)
{
	int idx = view->getIdx();
	context.SetBrush(*wxWHITE_BRUSH);
	wxPoint copy = getDisplayBox(idx).GetPosition();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawRectangle(copy.x, copy.y, getDisplayBox(idx).width, getDisplayBox(idx).height);
	context.DrawBitmap(upBitmap, copy.x + 1, copy.y + 2, true);
	context.DrawBitmap(downBitmap, copy.x + 1, copy.y + getDisplayBox(idx).height - 2 - downBitmap.GetHeight(), true);

	barSize.SetHeight((getDisplayBox(idx).height - 12) * 0.45);
	barSize.SetWidth(getDisplayBox(idx).width - 4);

	int divBy = (table->getTotalColumns() - table->getColumnsWindow());
	if(divBy <= 0)
		divBy = table->getColumnsWindow();
	int colOffset = barSize.GetHeight() / divBy;
	int verticalPosBar = 3 + copy.y + downBitmap.GetHeight() + colOffset * table->getTopColWindowIndex();
	if(table->getColumnsWindow() > 1)
		context.DrawRectangle(wxPoint(copy.x + 2, verticalPosBar), barSize);

}

void ddScrollBarHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
	int idx = view->getIdx();
	int y = event.GetPosition().y;
	anchorY = y;
	if( (y > (getDisplayBox(idx).GetPosition().y + 2)) && (y <  (getDisplayBox(idx).GetPosition().y + 2 + 6)) )  //6 image height
		table->columnsWindowUp(idx);

	if( (y > (getDisplayBox(idx).GetPosition().y + getDisplayBox(idx).height - 2 - downBitmap.GetHeight()) ) && (y < (getDisplayBox(idx).GetPosition().y + getDisplayBox(idx).height - 2) ) )
		table->columnsWindowDown(idx);
	view->notifyChanged();
}

void ddScrollBarHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
	int y = event.GetPosition().y;
	int divBy = (table->getTotalColumns() - table->getColumnsWindow());
	if(divBy <= 0)
		divBy = table->getColumnsWindow();
	int colOffset = barSize.GetHeight() / divBy;

	hdGeometry g;
	if ( g.ddabs(anchorY - y) > colOffset)
	{
		if((anchorY - y) > 0)
		{
			table->columnsWindowUp(view->getIdx());
		}
		else
		{
			table->columnsWindowDown(view->getIdx());
		}
		anchorY = y;
	}
	view->notifyChanged();
}

void ddScrollBarHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
}

hdPoint &ddScrollBarHandle::locate(int posIdx)
{
	if(scrollLocator)
	{
		pointLocate = scrollLocator->locate(posIdx, getOwner());
		return pointLocate;
	}
	else
		pointLocate = hdPoint(0, 0);
	return pointLocate;
}
