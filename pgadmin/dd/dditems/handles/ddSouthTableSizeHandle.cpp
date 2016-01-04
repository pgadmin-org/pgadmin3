//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddSouthTableSizeHandle.cpp - Allow to change table size by using drag and drop from south side of table rectangle
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/dditems/handles/ddSouthTableSizeHandle.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "hotdraw/utilities/hdGeometry.h"
#include "hotdraw/figures/defaultAttributes/hdFontAttribute.h"
#include "hotdraw/main/hdDrawingView.h"

ddSouthTableSizeHandle::ddSouthTableSizeHandle(ddTableFigure *owner, hdILocator *locator):
	hdLocatorHandle(owner, locator)
{
}

hdRect &ddSouthTableSizeHandle::getDisplayBox(int posIdx)
{
	hdPoint p = locate(posIdx);
	ddTableFigure *table = (ddTableFigure *) getOwner();
	displayBox.width = table->getFullSpace().width * 0.5; //as defined at locator
	displayBox.height = 3;
	displayBox.SetPosition(p);
	return displayBox;
}


wxCursor ddSouthTableSizeHandle::createCursor()
{
	return wxCursor(wxCURSOR_SIZENS);
}

void ddSouthTableSizeHandle::draw(wxBufferedDC &context, hdDrawingView *view)
{
}

ddSouthTableSizeHandle::~ddSouthTableSizeHandle()
{
}

void ddSouthTableSizeHandle::invokeStart(hdMouseEvent &event, hdDrawingView *view)
{
	anchorY = event.GetPosition().y;
}

void ddSouthTableSizeHandle::invokeStep(hdMouseEvent &event, hdDrawingView *view)
{
	int y = event.GetPosition().y;
	ddTableFigure *table = (ddTableFigure *) getOwner();
	wxFont font = *hdFontAttribute::defaultFont;
	int colOffset = table->getColDefaultHeight(font);

	int divBy = (table->getTotalColumns() - table->getColumnsWindow());
	if(divBy <= 0)
		divBy = table->getColumnsWindow();

	hdGeometry g;
	if ( g.ddabs(anchorY - y) > colOffset)
	{
		if((anchorY - y) > 0)
		{
			table->setColumnsWindow(view->getIdx(), table->getColumnsWindow() - 1);
		}
		else
		{
			table->setColumnsWindow(view->getIdx(), table->getColumnsWindow() + 1);
		}
		anchorY = y;
	}

	//hack to update relationship position when table size change
	table->manuallyNotifyChange(view->getIdx());
	view->notifyChanged();
}

void ddSouthTableSizeHandle::invokeEnd(hdMouseEvent &event, hdDrawingView *view)
{
	//hack to update relationship position when table size change
	ddTableFigure *table = (ddTableFigure *) getOwner();
	table->manuallyNotifyChange(view->getIdx());
	view->notifyChanged();
}
