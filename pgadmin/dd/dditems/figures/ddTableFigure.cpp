//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableFigure.cpp - Draw table figure of a model
//
////////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"
#include "dd/dditems/figures/ddColumnFigure.h"
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/main/hdDrawingEditor.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "dd/dditems/handles/ddAddColButtonHandle.h"
#include "dd/dditems/locators/ddAddColLocator.h"
#include "dd/dditems/handles/ddAddFkButtonHandle.h"
#include "dd/dditems/locators/ddAddFkLocator.h"
#include "dd/dditems/handles/ddRemoveTableButtonHandle.h"
#include "dd/dditems/locators/ddRemoveTableLocator.h"
#include "dd/dditems/handles/ddMinMaxTableButtonHandle.h"
#include "dd/dditems/locators/ddMinMaxTableLocator.h"
#include "dd/dditems/handles/ddScrollBarHandle.h"
#include "dd/dditems/locators/ddScrollBarTableLocator.h"
#include "dd/dditems/handles/ddSouthTableSizeHandle.h"
#include "dd/dditems/locators/ddTableBottomLocator.h"
#include "dd/ddmodel/ddDBReverseEngineering.h"
#include "hotdraw/utilities/hdGeometry.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "hotdraw/connectors/hdLocatorConnector.h"
#include "hotdraw/main/hdDrawing.h"
#include "dd/ddmodel/ddDatabaseDesign.h"

//Images
#include "images/ddAddColumn.pngc"
#include "images/ddRemoveColumn.pngc"
#include "images/ddAddForeignKey.pngc"
#include "images/ddMaximizeTable.pngc"
#include "images/ddMinimizeTable.pngc"
#include "images/ddRemoveTable.pngc"

/*
All figures title, colums, indexes are store at same array to improve performance in the following order:
	[0] = table border rect
	[1] = table title
	[2] = first column index
	[maxColIndex] = last column index
	[minIdxIndex] = first index index
	[maxIdxIndex] = last index index
*/

void ddTableFigure::Init(wxString tableName, int x, int y)
{
	setKindId(DDTABLEFIGURE);
	internalPadding = 2;
	externalPadding = 4;
	selectingFkDestination = false;

	//Set Value default Attributes
	fontColorAttribute->fontColor = wxColour(49, 79, 79);
	//Set Value default selected Attributes
	lineSelAttribute->pen().SetColour(wxColour(204, 0, 0));
	lineSelAttribute->pen().SetStyle(wxSOLID);
	lineSelAttribute->pen().SetWidth(1);
	fillSelAttribute->brush().SetColour(wxColour(255, 250, 205));
	fillAttribute->brush().SetColour(wxColour(248, 248, 255));
	fontSelColorAttribute->fontColor = wxColour(49, 79, 79);

	//Set table size, width and position
	rectangleFigure = new hdRectangleFigure();
	rectangleFigure->moveTo(0, x, y);
	add(rectangleFigure);

	tableTitle = new ddTextTableItemFigure(tableName, dt_null, NULL);
	tableTitle->setOwnerTable(this);
	tableTitle->setEditable(true);
	tableTitle->moveTo(0, x, y);
	tableTitle->disablePopUp();
	tableTitle->setShowDataType(false);
	add(tableTitle);
	tableTitle->moveTo(0, rectangleFigure->getBasicDisplayBox().x[0] + internalPadding * 2, rectangleFigure->getBasicDisplayBox().y[0] + internalPadding / 2);

	//Intialize handles
	wxBitmap image = wxBitmap(*ddAddColumn_png_img);
	wxSize valueSize = wxSize(8, 8);
	figureHandles->addItem(new ddAddColButtonHandle((hdIFigure *)this, (hdILocator *)new ddAddColLocator(), image, valueSize));
	image = wxBitmap(*ddAddForeignKey_png_img);
	figureHandles->addItem(new ddAddFkButtonHandle((hdIFigure *)this, (hdILocator *)new ddAddFkLocator(), image, valueSize));
	image = wxBitmap(*ddRemoveTable_png_img);
	figureHandles->addItem(new ddRemoveTableButtonHandle((hdIFigure *)this, (hdILocator *)new ddRemoveTableLocator(), image, valueSize));
	image = wxBitmap(*ddMinimizeTable_png_img);
	wxBitmap image2 = wxBitmap(*ddMaximizeTable_png_img);
	figureHandles->addItem(new ddMinMaxTableButtonHandle((hdIFigure *)this, (hdILocator *)new ddMinMaxTableLocator(), image, image2, valueSize));
	figureHandles->addItem(new ddSouthTableSizeHandle(this, (hdILocator *)new ddTableBottomLocator()));

	//Intialize special handle
	valueSize = wxSize(10, colsRect.GetSize().GetHeight());
	scrollbar = new ddScrollBarHandle(this, (hdILocator *)new ddScrollBarTableLocator(), valueSize);

	//Intialize columns window (min is always 1 in both, with or without cols & indxs)
	colsRowsSize = 0;
	colsWindow = 0;
	idxsRowsSize = 0;
	idxsWindow = 0;

	//Initialize indexes (pointers to array segments)
	maxColIndex = 2;
	minIdxIndex = 2;
	maxIdxIndex = 2;

	//Initialize position where start to draw columns & indexes, this is the value to allow scrollbars
	beginDrawCols = 2;
	beginDrawIdxs = 2;

	//Initialize
	pkName = wxEmptyString;
	ukNames.clear();

	updateTableSize();

	basicDisplayBox.x[0] = x;
	basicDisplayBox.y[0] = y;
	belongsToSchema = false;
}

ddTableFigure::ddTableFigure(wxString tableName, int x, int y):
	hdCompositeFigure()
{
	Init(tableName, x, y);
}

ddTableFigure::ddTableFigure(wxString tableName, int posIdx, int x, int y):
	hdCompositeFigure()
{
	Init(tableName, 0, 0);
	//Check figure available positions for diagrams, add at least needed to allow initialization of the class
	int i, start;
	start = basicDisplayBox.CountPositions();
	for(i = start; i < (posIdx + 1); i++)
	{
		AddPosForNewDiagram();
	}
	syncInternalsPosAt(posIdx, x, y);
}

//Used by persistence classes
void ddTableFigure::InitTableValues(wxArrayString UniqueKeysName, wxString primaryKeyName, int bdc, int bdi, int maxcolsi, int minidxsi, int maxidxsi, int colsrs, int colsw, int idxsrs, int idxsw)
{
	ukNames = UniqueKeysName;
	pkName = primaryKeyName;
	beginDrawCols = bdc;
	beginDrawIdxs = bdi;
	maxColIndex = maxcolsi;
	minIdxIndex = minidxsi;
	maxIdxIndex = maxidxsi;
	colsRowsSize = colsrs;
	colsWindow = colsw;
	idxsRowsSize = idxsrs;
	idxsWindow = idxsw;
	updateTableSize();
}

ddTableFigure::~ddTableFigure()
{
	if(scrollbar)
	{
		if(figureHandles->existsObject(scrollbar))
			figureHandles->removeItem(scrollbar);
		delete scrollbar;
	}
}

void ddTableFigure::AddPosForNewDiagram()
{
	//Add new position to internal calculations figure
	fullSizeRect.addNewXYPosition();
	titleRect.addNewXYPosition();
	titleColsRect.addNewXYPosition();
	colsRect.addNewXYPosition();
	titleIndxsRect.addNewXYPosition();
	indxsRect.addNewXYPosition();
	unScrolledColsRect.addNewXYPosition();
	unScrolledFullSizeRect.addNewXYPosition();
	unScrolledTitleRect.addNewXYPosition();
	//Add to all figure figures
	hdCompositeFigure::AddPosForNewDiagram();
}

void ddTableFigure::RemovePosOfDiagram(int posIdx)
{
	//Remove position for internal calculations figure
	fullSizeRect.removeXYPosition(posIdx);
	titleRect.removeXYPosition(posIdx);
	titleColsRect.removeXYPosition(posIdx);
	colsRect.removeXYPosition(posIdx);
	titleIndxsRect.removeXYPosition(posIdx);
	indxsRect.removeXYPosition(posIdx);
	unScrolledColsRect.removeXYPosition(posIdx);
	unScrolledFullSizeRect.removeXYPosition(posIdx);
	unScrolledTitleRect.removeXYPosition(posIdx);
	//remove position at all figure figures
	hdCompositeFigure::RemovePosOfDiagram(posIdx);
}

ddColumnFigure *ddTableFigure::getColByName(wxString name)
{
	ddColumnFigure *out = NULL;
	ddColumnFigure *f;

	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	iterator->Next(); //Second figure is main title

	while(iterator->HasNext())
	{
		f = (ddColumnFigure *) iterator->Next();
		if(f->getColumnName(false).IsSameAs(name))
		{
			out = f;
			break;
		}
	}
	delete iterator;

	return out;
}

//WARNING: Columns SHOULD BE ADDED only using this function to avoid strange behaviors
void ddTableFigure::addColumn(int posIdx, ddColumnFigure *column)
{
	column->setOwnerTable(this);
	add(column);
	//Update Indexes
	if(maxColIndex == minIdxIndex) //maxColIndex == minIdxIndex means not indexes at this table, then update too
	{
		minIdxIndex++;
		maxIdxIndex++;
	}
	maxColIndex++;
	colsWindow++;  //by default add a column increase initial window
	colsRowsSize++;

	updateTableSize(true);

	//Fix column position at all available positions (Diagrams)
	int i;
	for(i = 0; i < basicDisplayBox.CountPositions(); i++)
	{
		syncInternalsPosAt(i, basicDisplayBox.x[i], basicDisplayBox.y[i]);
	}
}

//WARNING: Function should be called on a table generated from a storage or to sync values after a big change at model (not derived from hotdraw events)
void ddTableFigure::syncInternalsPosAt(int posIdx, int x, int y)
{
	basicDisplayBox.x[posIdx] = x;
	basicDisplayBox.y[posIdx] = y;
	rectangleFigure->moveTo(posIdx, x, y);
	tableTitle->moveTo(posIdx, rectangleFigure->getBasicDisplayBox().x[posIdx] + internalPadding * 2, rectangleFigure->getBasicDisplayBox().y[posIdx] + internalPadding / 2);
	calcInternalSubAreas(posIdx);
	recalculateColsPos(posIdx);
}

//WARNING: Function should be called on a table generated from a storage or to sync values after a big change at model (not derived from hotdraw events)
void ddTableFigure::syncInternalsPosAt(wxArrayInt &x, wxArrayInt &y)
{
	unsigned int posIdx, pointsCount = tableTitle->getBasicDisplayBox().CountPositions(), finalValue = x.Count();
	//I need to check that figures inside figure have all points too
	while(pointsCount < finalValue)
	{
		AddPosForNewDiagram();
		pointsCount = tableTitle->getBasicDisplayBox().CountPositions();
	}

	//optimize this, because this is hack right now to avoid some weird problem when recreating figure status
	basicDisplayBox.x = x;
	basicDisplayBox.y = y;

	for(posIdx = 0; posIdx < finalValue; posIdx++)
	{
		rectangleFigure->moveTo(posIdx, x[posIdx], y[posIdx]);
		tableTitle->moveTo(posIdx, rectangleFigure->getBasicDisplayBox().x[posIdx] + internalPadding * 2, rectangleFigure->getBasicDisplayBox().y[posIdx] + internalPadding / 2);
		calcInternalSubAreas(posIdx);
		recalculateColsPos(posIdx);
	}
}

//WARNING: Columns SHOULD BE ADDED only using this columns if was created as an image from storage one
void ddTableFigure::addColumnFromStorage(ddColumnFigure *column)
{
	add(column);
}

void ddTableFigure::removeColumn(int posIdx, ddColumnFigure *column)
{
	//Hack to allow to remove Fk before delete it.
	if(column->isPrimaryKey() || column->isUniqueKey())
	{
		column->setColumnKindToNone();
	}

	column->setOwnerTable(NULL);
	remove(column);

	if(column)
		delete column;
	//Update Indexes
	if(maxColIndex == minIdxIndex) //means not indexes at this table, then update too
	{
		minIdxIndex--;
		maxIdxIndex--;
	}
	maxColIndex--;
	if(colsRowsSize == colsWindow) //only decrease if size of window and columns is the same
		colsWindow--;
	colsRowsSize--;
	if(beginDrawCols > 2)
		beginDrawCols--;
	calcInternalSubAreas(posIdx);
	recalculateColsPos(posIdx);
	if(colsWindow == colsRowsSize) //if handle need to be removed, remove it
	{
		if(figureHandles->existsObject(scrollbar))
			figureHandles->removeItem(scrollbar);
	}
	//hack to update relationship position when table size change
	manuallyNotifyChange(posIdx);
	column = NULL;
}

void ddTableFigure::recalculateColsPos(int posIdx)
{
	wxFont font = fontAttribute->font();
	int defaultHeight = getColDefaultHeight(font);

	hdIFigure *f = (hdIFigure *) figureFigures->getItemAt(0); //first figure is always Rect
	int horizontalPos = f->displayBox().x[posIdx] + 2;
	int verticalPos = 0;

	for(int i = 2; i < maxColIndex ; i++)
	{
		f = (hdIFigure *) figureFigures->getItemAt(i); //table title
		if( (i >= beginDrawCols) && (i <= (colsWindow + beginDrawCols)) ) //Visible to draw
		{
			verticalPos = colsRect.y[posIdx] + (defaultHeight * (i - beginDrawCols) + ((i - beginDrawCols) * internalPadding));
			f->moveTo(posIdx, horizontalPos, verticalPos);
		}
		else
			f->moveTo(posIdx, -65000, -65000); //any figure outside canvas (x<0 || y<0) is not draw & not used to calculate displaybox
	}
}



void ddTableFigure::basicDraw(wxBufferedDC &context, hdDrawingView *view)
{
	int idx = view->getIdx();
	calcInternalSubAreas(idx);

	if(calcScrolled) //Hack to avoid pass view as parameter to calcInternalSubAreas() because is sometimes called outside a paint event
	{
		view->CalcScrolledPosition(fullSizeRect.x[idx], fullSizeRect.y[idx], &fullSizeRect.x[idx], &fullSizeRect.y[idx]);
		view->CalcScrolledPosition(titleRect.x[idx], titleRect.y[idx], &titleRect.x[idx], &titleRect.y[idx]);
		view->CalcScrolledPosition(titleColsRect.x[idx], titleColsRect.y[idx], &titleColsRect.x[idx], &titleColsRect.y[idx]);
		view->CalcScrolledPosition(colsRect.x[idx], colsRect.y[idx], &colsRect.x[idx], &colsRect.y[idx]);
		view->CalcScrolledPosition(titleIndxsRect.x[idx], titleIndxsRect.y[idx], &titleIndxsRect.x[idx], &titleIndxsRect.y[idx]);
		view->CalcScrolledPosition(indxsRect.x[idx], indxsRect.y[idx], &indxsRect.x[idx], &indxsRect.y[idx]);
		calcScrolled = false;
	}

	hdIFigure *f = (hdIFigure *) figureFigures->getItemAt(0); //table rectangle
	f->draw(context, view);
	f = (hdIFigure *) figureFigures->getItemAt(1); //table title
	f->draw(context, view);

	for(int i = beginDrawCols; i < (colsWindow + beginDrawCols); i++)
	{
		f = (hdIFigure *) figureFigures->getItemAt(i); //table title
		if(f->displayBox().GetPosition(view->getIdx()).x > 0 && f->displayBox().GetPosition(view->getIdx()).y > 0)
		{
			f->draw(context, view);
		}
	}

	reapplyAttributes(context, view); //reset attributes to default of figure because can be modified at Draw functions.

	//Set Font for title "Columns"
	wxFont font = fontAttribute->font();
	int newSize = font.GetPointSize() * 0.7;
	font.SetPointSize(newSize);
	context.SetFont(font);

	//Draw Columns Title Line 1
	context.DrawLine(titleColsRect.GetTopLeft(idx), titleColsRect.GetTopRight(idx));
	//Draw Columns Title
	context.DrawText(wxT("Columns"), titleColsRect.x[idx] + 3, titleColsRect.y[idx]);
	//Draw Columns Title Line 2
	context.DrawLine(titleColsRect.GetBottomLeft(idx), titleColsRect.GetBottomRight(idx));
	//DrawVertical Lines
	context.DrawLine(titleColsRect.GetBottomLeft(idx).x + 11, titleColsRect.GetBottomLeft(idx).y, titleColsRect.GetBottomLeft(idx).x + 11, titleIndxsRect.GetTopLeft(idx).y);
	context.DrawLine(titleColsRect.GetBottomLeft(idx).x + 22, titleColsRect.GetBottomLeft(idx).y, titleColsRect.GetBottomLeft(idx).x + 22, titleIndxsRect.GetTopLeft(idx).y);
	//Draw Indexes Title Line 1
	context.DrawLine(titleIndxsRect.GetTopLeft(idx), titleIndxsRect.GetTopRight(idx));
	//Draw Indexes Title
	//disable until implemented in a future: context.DrawText(wxT("Indexes"),titleIndxsRect.x+3,titleIndxsRect.y);
	//Draw Indexes Title Line 2
	context.DrawLine(titleIndxsRect.GetBottomLeft(idx), titleIndxsRect.GetBottomRight(idx));

	context.SetFont(fontAttribute->font()); 		//after change font return always to initial one

	//Draw scrollbar is needed
	if(scrollbar && figureHandles->existsObject(scrollbar))
		scrollbar->draw(context, view);

	//Use this in a future
	//Hack to show message to select fk destination table
	if(selectingFkDestination)
	{
		context.SetTextForeground(*wxWHITE);
		wxBrush old = context.GetBrush();
		context.SetBrush(*wxBLACK_BRUSH);

		int w, h, x, y;
		context.GetTextExtent(wxString(wxT("Select Destination table of foreign key")), &w, &h);
		x = fullSizeRect.GetTopLeft(idx).x + (((fullSizeRect.GetTopRight(idx).x - fullSizeRect.GetTopLeft(idx).x) - w) / 2);
		y = fullSizeRect.GetTopLeft(idx).y - h - 2;
		context.DrawRectangle(wxRect(x, y, w, h));
		context.DrawText(wxString(wxT("Select Destination table of foreign key")), x, y);

		context.SetBrush(old);
		context.SetTextForeground(*wxBLACK);
		context.SetBackground(*wxWHITE);

		//don't draw anything else then don't reapply default attributes
	}
}

void ddTableFigure::basicDrawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	int idx = view->getIdx();
	calcInternalSubAreas(idx);

	if(calcScrolled) //Hack to avoid pass view as parameter to calcInternalSubAreas() because is sometimes called outside a paint event
	{
		view->CalcScrolledPosition(fullSizeRect.x[idx], fullSizeRect.y[idx], &fullSizeRect.x[idx], &fullSizeRect.y[idx]);
		view->CalcScrolledPosition(titleRect.x[idx], titleRect.y[idx], &titleRect.x[idx], &titleRect.y[idx]);
		view->CalcScrolledPosition(titleColsRect.x[idx], titleColsRect.y[idx], &titleColsRect.x[idx], &titleColsRect.y[idx]);
		view->CalcScrolledPosition(colsRect.x[idx], colsRect.y[idx], &colsRect.x[idx], &colsRect.y[idx]);
		view->CalcScrolledPosition(titleIndxsRect.x[idx], titleIndxsRect.y[idx], &titleIndxsRect.x[idx], &titleIndxsRect.y[idx]);
		view->CalcScrolledPosition(indxsRect.x[idx], indxsRect.y[idx], &indxsRect.x[idx], &indxsRect.y[idx]);
		calcScrolled = false;
	}

	hdIFigure *f = (hdIFigure *) figureFigures->getItemAt(0); //table rectangle
	f->drawSelected(context, view);
	f = (hdIFigure *) figureFigures->getItemAt(1); //table title
	f->drawSelected(context, view);

	for(int i = beginDrawCols; i < (colsWindow + beginDrawCols); i++)
	{
		f = (hdIFigure *) figureFigures->getItemAt(i); //table title
		if(f->displayBox().GetPosition(view->getIdx()).x > 0 && f->displayBox().GetPosition(view->getIdx()).y > 0)
		{
			f->drawSelected(context, view);
		}
	}

	reapplySelAttributes(context, view); //reset attributes to default of figure because can be modified at Draw functions.
	wxFont font = fontAttribute->font();
	float t = font.GetPointSize();
	int newSize = font.GetPointSize() * 0.7;
	font.SetPointSize(newSize);
	context.SetFont(font);

	//Draw Columns Title Line 1
	context.DrawLine(titleColsRect.GetTopLeft(idx), titleColsRect.GetTopRight(idx));
	//Draw Columns Title
	context.DrawText(wxT("Columns"), titleColsRect.x[idx] + 3, titleColsRect.y[idx]);
	//Draw Columns Title Line 2
	context.DrawLine(titleColsRect.GetBottomLeft(idx), titleColsRect.GetBottomRight(idx));
	//DrawVertical Lines
	context.DrawLine(titleColsRect.GetBottomLeft(idx).x + 11, titleColsRect.GetBottomLeft(idx).y, titleColsRect.GetBottomLeft(idx).x + 11, titleIndxsRect.GetTopLeft(idx).y);
	context.DrawLine(titleColsRect.GetBottomLeft(idx).x + 22, titleColsRect.GetBottomLeft(idx).y, titleColsRect.GetBottomLeft(idx).x + 22, titleIndxsRect.GetTopLeft(idx).y);
	//Draw Indexes Title Line 1
	context.DrawLine(titleIndxsRect.GetTopLeft(idx), titleIndxsRect.GetTopRight(idx));
	//Draw Indexes Title
	//disable until implemented in a future: context.DrawText(wxT("Indexes"),titleIndxsRect.x+3,titleIndxsRect.y);
	//Draw Indexes Title Line 2
	context.DrawLine(titleIndxsRect.GetBottomLeft(idx), titleIndxsRect.GetBottomRight(idx));
}

hdMultiPosRect &ddTableFigure::getBasicDisplayBox()
{
	return basicDisplayBox;
}

void ddTableFigure::setColsRowsWindow(int num)
{
	if(num > 0)
	{
		colsWindow = num;
		wxFont font = fontAttribute->font();
		colsRect.height = getColDefaultHeight(font) * colsWindow;
		colsRect.width = getFiguresMaxWidth();
	}
}

int ddTableFigure::getHeightFontMetric(wxString text, wxFont font)
{
	int width, height;
	wxBitmap emptyBitmap(*ddAddColumn_png_img);
	wxMemoryDC temp_dc;
	temp_dc.SelectObject(emptyBitmap);
	temp_dc.SetFont(font);
	temp_dc.GetTextExtent(text, &width, &height);
	return height;
}

int ddTableFigure::getColDefaultHeight(wxFont font)
{
	if(figureFigures->count() <= 0)
	{
		int width, height;
		wxBitmap emptyBitmap(*ddAddColumn_png_img);
		wxMemoryDC temp_dc;
		temp_dc.SelectObject(emptyBitmap);
		temp_dc.SetFont(font);
		temp_dc.GetTextExtent(wxT("NewColumn"), &width, &height);
		return height;
	}
	else
	{
		hdIFigure *f = (hdIFigure *) figureFigures->getItemAt(1); //table title
		return f->displayBox().height;
	}
}

//Show select fk destination Message Hack
void ddTableFigure::setSelectFkDestMode(bool value)
{
	selectingFkDestination = value;
}

int ddTableFigure::getFiguresMaxWidth()
{
	ddColumnFigure *cf;
	hdGeometry g;

	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	int maxWidth = 0;
	cf = (ddColumnFigure *) iterator->Next(); //Second figure is main title
	maxWidth = g.max(maxWidth, cf->displayBox().width + 20);
	while(iterator->HasNext())
	{
		cf = (ddColumnFigure *) iterator->Next();
		maxWidth = g.max(maxWidth, cf->displayBox().width);
	}
	delete iterator;
	if(figureHandles->existsObject(scrollbar))
		return maxWidth + 11;  //as defined at locator
	else
		return maxWidth;
}

void ddTableFigure::calcInternalSubAreas(int posIdx)
{
	calcScrolled = true;

	int maxWidth = getFiguresMaxWidth() + externalPadding;
	if(maxWidth < 100)
		maxWidth = 100;
	wxFont font = fontAttribute->font();
	int defaultHeight = getColDefaultHeight(font);

	hdRect db = basicDisplayBox.gethdRect(posIdx);

	//*** titleRect
	float t = font.GetPointSize();
	int newSize = font.GetPointSize() * 0.7;
	font.SetPointSize(newSize);
	int colsTitleHeight = getHeightFontMetric(wxT("Columns"), font);

	titleRect.x[posIdx] = db.x;
	titleRect.y[posIdx] = db.y;
	titleRect.width = maxWidth;
	titleRect.height = defaultHeight;

	titleColsRect.x[posIdx] = db.x;
	titleColsRect.y[posIdx] = titleRect.y[posIdx] + titleRect.height;
	titleColsRect.width = maxWidth;
	titleColsRect.height = colsTitleHeight;
	unScrolledTitleRect = titleColsRect;

	//*** colsRect
	colsRect.width = maxWidth;
	if(colsWindow > 0)
		colsRect.height = defaultHeight * colsWindow + (colsWindow * internalPadding);
	else
		colsRect.height = defaultHeight;
	colsRect.x[posIdx] = db.x;
	colsRect.y[posIdx] = titleRect.y[posIdx] + titleRect.height + titleColsRect.height;
	unScrolledColsRect = colsRect;

	//*** idxTitleRect
	titleIndxsRect.width = maxWidth;
	titleIndxsRect.height = colsTitleHeight;
	titleIndxsRect.x[posIdx] = db.x;
	titleIndxsRect.y[posIdx] = colsRect.y[posIdx] + colsRect.height;

	//*** indexesRect
	indxsRect.width = maxWidth;
	indxsRect.height = defaultHeight * idxsWindow + (idxsWindow * internalPadding);
	indxsRect.x[posIdx] = db.x;
	indxsRect.y[posIdx] = titleIndxsRect.y[posIdx] + titleIndxsRect.height;

	//*** FullTable Size
	fullSizeRect.width = maxWidth;
	fullSizeRect.height = titleRect.height + titleColsRect.height + colsRect.height + titleIndxsRect.height + indxsRect.height;
	fullSizeRect.x[posIdx] = db.x;
	fullSizeRect.y[posIdx] = titleRect.y[posIdx];
	unScrolledFullSizeRect = fullSizeRect;

	//Update size
	wxSize sizeValue = fullSizeRect.GetSize();
	rectangleFigure->setSize(sizeValue);
}

void ddTableFigure::updateTableSize(bool notifyChange)
{
	//Step 0: Recalculate displaybox size, in case of an external modification as change of datatype in a fk from a source (data is stored in original table)
	ddColumnFigure *cf;
	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	cf = (ddColumnFigure *) iterator->Next(); //Second figure is main title
	while(iterator->HasNext())
	{
		cf = (ddColumnFigure *) iterator->Next();
		cf->displayBoxUpdate();
	}
	delete iterator;

	//Step 1: Update table size
	calcInternalSubAreas(0);
	basicDisplayBox.SetSize(fullSizeRect.GetSize());
	if(notifyChange)
	{
		//hack to update relationship position when table size change, but need to be notified to all views only doing it right now for first view
		manuallyNotifyChange(0);
	}
}

hdMultiPosRect &ddTableFigure::getColsSpace()
{
	return unScrolledColsRect;
}

hdMultiPosRect &ddTableFigure::getFullSpace()
{
	return unScrolledFullSizeRect;
}

hdMultiPosRect &ddTableFigure::getTitleRect()
{
	return unScrolledTitleRect;
}


int ddTableFigure::getTotalColumns()
{
	return colsRowsSize;
}

int ddTableFigure::getColumnsWindow()
{
	return colsWindow;
}

void ddTableFigure::setColumnsWindow(int posIdx, int value, bool maximize)
{

	if(!maximize)
	{

		//if value >0 && <= max size table && table+offset < maxColIndex with window
		if( (value > 0) && (value <= colsRowsSize) && (maxColIndex >= ( beginDrawCols + value ) ) )
		{
			colsWindow = value;
			calcInternalSubAreas(posIdx);
			recalculateColsPos(posIdx);
		}

		//if special case of needing to modify beginDrawCols then do it
		if( (value > 0) && (value <= colsRowsSize) && (maxColIndex < ( beginDrawCols + value ) ) )
		{
			if( (beginDrawCols + colsWindow) == maxColIndex) //if index is at max
			{
				int diff = value - colsWindow; // value should be always higher tan colsWindows
				if(diff > 0 && (beginDrawCols - diff) >= 0 )
				{
					beginDrawCols -= diff;
					colsWindow = value;
					calcInternalSubAreas(posIdx);
					recalculateColsPos(posIdx);

				}
			}
		}
	}
	else
	{
		beginDrawCols = 2;
		colsWindow = value;
		calcInternalSubAreas(posIdx);
		recalculateColsPos(posIdx);
	}


	//Hide Scrollbar if needed
	if(colsWindow == colsRowsSize)
	{
		if(figureHandles->existsObject(scrollbar))
			figureHandles->removeItem(scrollbar);
	}
	else
	{
		if (!figureHandles->existsObject(scrollbar))
			figureHandles->addItem(scrollbar);
	}

}

void ddTableFigure::columnsWindowUp(int posIdx)  //move window from number to zero
{
	if( beginDrawCols > 2 )
	{
		beginDrawCols--;
		calcInternalSubAreas(posIdx);
		recalculateColsPos(posIdx);
	}
}

void ddTableFigure::columnsWindowDown(int posIdx)  //move window from number to maxcolumns
{
	if( (beginDrawCols + colsWindow) < maxColIndex)
	{
		beginDrawCols++;
		calcInternalSubAreas(posIdx);
		recalculateColsPos(posIdx);
	}
}

int ddTableFigure::getTopColWindowIndex()
{
	return (beginDrawCols - 2);
}

void ddTableFigure::setPkConstraintName(wxString name)
{
	pkName = name;
}

wxString ddTableFigure::getPkConstraintName()
{
	return pkName;
}

wxArrayString ddTableFigure::getAllColumnsNames()
{
	wxArrayString tmp;
	ddColumnFigure *f;
	tmp.Clear();
	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	iterator->Next(); //Second figure is main title

	while(iterator->HasNext())
	{
		f = (ddColumnFigure *) iterator->Next();
		tmp.Add(f->getColumnName(false));
	}
	delete iterator;
	return tmp;
}

wxArrayString ddTableFigure::getAllFkSourceColsNames(bool pk, int ukIndex)
{
	wxArrayString tmp;
	ddColumnFigure *f;
	tmp.Clear();
	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	iterator->Next(); //Second figure is main title

	while(iterator->HasNext())
	{
		f = (ddColumnFigure *) iterator->Next();
		if(pk)
		{
			if(f->isPrimaryKey())
				tmp.Add(f->getColumnName(false));
		}
		else
		{
			if(f->isUniqueKey(ukIndex))
				tmp.Add(f->getColumnName(false));
		}
	}
	delete iterator;
	return tmp;
}

ddColumnFigure *ddTableFigure::getColumnByName(wxString name)
{
	ddColumnFigure *f;
	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	iterator->Next(); //Second figure is main title

	while(iterator->HasNext())
	{
		f = (ddColumnFigure *) iterator->Next();
		if(f->getColumnName().IsSameAs(name))
		{
			return f;
		}
	}
	delete iterator;
	return NULL;
}

wxArrayString &ddTableFigure::getUkConstraintsNames()
{
	return ukNames;
}

wxString ddTableFigure::getTableName()
{
	ddTextTableItemFigure *c = (ddTextTableItemFigure *) figureFigures->getItemAt(1);
	c->setOneTimeNoAlias();
	return c->getText(false);
}

//set Null on all relationship items with a fk column to be delete or a pk to be removed (pk attribute)
void ddTableFigure::prepareForDeleteFkColumn(ddColumnFigure *column)
{
	hdIteratorBase *iterator = observersEnumerator();
	while(iterator->HasNext())
	{
		ddRelationshipFigure *r = (ddRelationshipFigure *) iterator->Next();
		if(r->getStartFigure() == this)	//Only update FK of connection with this table as source. source ---<| destination
			r->prepareFkForDelete(column);
	}
	delete iterator;

}

//	Note about observers:
//	A table is observed by several relationships at same time, where that observers
//	are just looking for changes that will affect relationship behavior.
//	Ex: if I delete a pk on observed table (source) all observers (destination)
//		should modify their columns to remove that fk created from that pk column.
// Warning: when a relationship is created an observer is added to both sides of relationship
// because this behavior (needed for update connection) to identify if is an observer
// of source table or destination table, should be check end figure, start!=end and end=this is end figure
// If start = and is recursive
void ddTableFigure::updateFkObservers()
{
	hdIteratorBase *iterator = observersEnumerator();
	while(iterator->HasNext())
	{
		ddRelationshipFigure *r = (ddRelationshipFigure *) iterator->Next();
		if(r->getStartFigure() == this)	//Only update FK of connection with this table as source. source ---<| destination
		{
			r->updateForeignKey();
		}
	}
	delete iterator;
}

//If a column change datatype, should alert all others table to adjust their size with new values
void ddTableFigure::updateSizeOfObservers()
{
	//For all tables that are observing this table, update their size
	hdIteratorBase *iterator = observersEnumerator();
	while(iterator->HasNext())
	{
		ddRelationshipFigure *r = (ddRelationshipFigure *) iterator->Next();
		ddTableFigure *destFkTable = (ddTableFigure *) r->getEndFigure();
		destFkTable->updateTableSize();
	}
	delete iterator;
}

//drop foreign keys with this table as origin or destination because table is going to be deleted
void ddTableFigure::processDeleteAlert(hdDrawing *drawing)
{
	hdIteratorBase *iterator = observersEnumerator();
	bool repeatFlag;
	do
	{
		repeatFlag = false;
		iterator->ResetIterator();
		while(iterator->HasNext())
		{
			ddRelationshipFigure *rel = (ddRelationshipFigure *) iterator->Next();
			rel->disconnectStart();
			rel->disconnectEnd();

			drawing->getOwnerEditor()->removeFromAllSelections(rel);
			drawing->getOwnerEditor()->deleteModelFigure(rel);
			repeatFlag = true;
			break;
		}
	}
	while(repeatFlag);

	delete iterator;
}

void ddTableFigure::basicMoveBy(int posIdx, int x, int y)
{

	hdIFigure *f = (hdIFigure *) figureFigures->getItemAt(0);
//Hack to avoid bug in if clause
	int width =  spaceForMovement.GetWidth();
	int height =  spaceForMovement.GetHeight();
	int bottom = f->displayBox().y[posIdx] + f->displayBox().height + y;
	int right = f->displayBox().x[posIdx] + f->displayBox().width + x;
	int left = f->displayBox().x[posIdx] + x;
	int top = f->displayBox().y[posIdx] + y;

//limit movemnt of table figures to canvas space
	if( (left > 0) && (top > 0) && (right < width) && (bottom < height) )
		hdCompositeFigure::basicMoveBy(posIdx, x, y);
}

//Validate status of table for SQL DDL generation
bool ddTableFigure::validateTable(wxString &errors)
{
	bool out = true;
	wxString tmp = wxEmptyString;
	ddColumnFigure *f;

	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	iterator->Next(); //Second figure is main title

	while(iterator->HasNext())
	{
		f = (ddColumnFigure *) iterator->Next();
		if(!f->validateColumn(tmp))
		{
			out = false;
		}
	}

	if(!out)
	{
		errors.Append(wxT("\n"));
		errors.Append(wxT("Errors detected at table") + this->getTableName() + wxT(" \n"));
		errors.Append(tmp);
		errors.Append(wxT("\n"));
	}

	delete iterator;

	return out;
}

//Using some options from http://www.postgresql.org/docs/8.1/static/sql-createtable.html, but new options can be added in a future.
wxString ddTableFigure::generateSQLCreate(wxString schemaName)
{
	//Columns and table
	wxString tmp(wxT("CREATE TABLE "));
	if(!schemaName.IsEmpty())
	{
		tmp += wxT("\"") + schemaName + wxT("\".\"") + getTableName() + wxT("\" (\n");
	}
	else
	{
		tmp += wxT("\"") + getTableName() + wxT("\" (\n");
	}
	hdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //Fixed Position for table rectangle
	iterator->Next(); //Fixed Position for table name
	while(iterator->HasNext())
	{
		ddColumnFigure *column = (ddColumnFigure *) iterator->Next();
		tmp += column->generateSQL();
		if(column->isNotNull())
		{
			tmp += wxT(" NOT NULL");
		}
		if(iterator->HasNext())
		{
			tmp += wxT(" , \n");
		}
	}
	tmp += wxT("\n ); ");

	return tmp;
}

wxString ddTableFigure::generateSQLAlterPks(wxString schemaName)
{
	wxString tmp;
	hdIteratorBase *iterator = figuresEnumerator();
	//Pk, Uk Constraints
	iterator->Next(); //Fixed Position for table rectangle
	iterator->Next(); //Fixed Position for table name
	int contPk = 0;
	while(iterator->HasNext())
	{
		ddColumnFigure *column = (ddColumnFigure *) iterator->Next();
		if(column->isPrimaryKey())
			contPk++;
	}
	if(contPk > 0)
	{
		tmp += wxT("\nALTER TABLE ");
		if(!schemaName.IsEmpty())
		{
			tmp += wxT("\"") + schemaName + wxT("\".\"") + getTableName() + wxT("\"");
		}
		else
		{
			tmp += wxT("\"") + getTableName() + wxT("\"") ;
		}

		tmp += wxT(" ADD ");

		if(!pkName.IsEmpty())
		{
			tmp += wxT("CONSTRAINT \"") + pkName + wxT("\" ");
		}
		tmp += wxT("PRIMARY KEY ( ");
		iterator->ResetIterator();
		iterator->Next(); //Fixed Position for table rectangle
		iterator->Next(); //Fixed Position for table name

		while(iterator->HasNext())
		{
			ddColumnFigure *column = (ddColumnFigure *) iterator->Next();
			if(column->isPrimaryKey())
			{
				tmp += wxT("\"") + column->getColumnName() + wxT("\"");
				contPk--;
				if(contPk > 0)
				{
					tmp += wxT(" , ");
				}
				else
				{
					tmp += wxT(" ); ");
				}
			}
		}
	}
	delete iterator;

	return tmp;
}

wxString ddTableFigure::generateSQLAlterFks(wxString schemaName)
{
	wxString tmp;
	hdIteratorBase *iterator = figuresEnumerator();
	//Fk Constraint
	iterator = observersEnumerator();
	if(!iterator->HasNext())
	{
		tmp = wxEmptyString;
	}
	else
	{
		while(iterator->HasNext())
		{
			ddRelationshipFigure *rel = (ddRelationshipFigure *) iterator->Next();
			if(rel->getStartFigure() != this)
			{
				tmp += rel->generateSQL(schemaName);
			}
		}
	}
	delete iterator;
	return tmp;
}

wxString ddTableFigure::generateSQLAlterUks(wxString schemaName)
{
	hdIteratorBase *iterator = figuresEnumerator();

	//Pk, Uk Constraints
	iterator->Next(); //Fixed Position for table rectangle
	iterator->Next(); //Fixed Position for table name
	int MaxUk = -1;
	while(iterator->HasNext())
	{
		ddColumnFigure *column = (ddColumnFigure *) iterator->Next();
		if(column->isUniqueKey() && column->getUniqueConstraintIndex() > MaxUk)
			MaxUk = column->getUniqueConstraintIndex();
	}

	wxString tmp = wxEmptyString;
	if(MaxUk >= 0)
	{
		int i;
		for(i = 0; i <= MaxUk; i++)
		{
			tmp += wxT("\nALTER TABLE ");

			if(!schemaName.IsEmpty())
				tmp += wxT("\"") + schemaName + wxT("\".");
			tmp += wxT("\"") + getTableName() + wxT("\"") ;

			tmp += wxT(" ADD ");

			if(!getUkConstraintsNames()[i].IsEmpty())
			{
				tmp += wxT("CONSTRAINT \"") + getUkConstraintsNames()[i] + wxT("\" ") ;
			}
			tmp += wxT("UNIQUE ( ");

			int countUk = 0;
			iterator->ResetIterator();
			iterator->Next(); //Fixed Position for table rectangle
			iterator->Next(); //Fixed Position for table name
			while(iterator->HasNext())
			{
				ddColumnFigure *column = (ddColumnFigure *) iterator->Next();
				if(column->getUniqueConstraintIndex() == i)
					countUk++;
			}

			iterator->ResetIterator();
			iterator->Next(); //Fixed Position for table rectangle
			iterator->Next(); //Fixed Position for table name
			while(iterator->HasNext())
			{
				ddColumnFigure *column = (ddColumnFigure *) iterator->Next();
				if(column->isUniqueKey() && column->getUniqueConstraintIndex() == i)
				{
					tmp += wxT("\"") + column->getColumnName() + wxT("\"");
					countUk--;
					if(countUk > 0)
					{
						tmp += wxT(", ");
					}
				}
			}
			tmp += wxT(" ); ");
		}
	}
	delete iterator;
	return tmp;
}

wxString ddTableFigure::generateAltersTable(pgConn *connection, wxString schemaName, ddDatabaseDesign *design)
{
	wxString out;

	OID oidTable = ddImportDBUtils::getTableOID(connection, schemaName, getTableName());
	if(oidTable == -1)
	{
		wxMessageBox(wxString::Format(_("Cannot build an ALTER TABLE statement for a non existing table (%s.%s)."),
		                              schemaName.c_str(), getTableName().c_str()), _("Error when trying to get table OID"),  wxICON_ERROR);
		return wxEmptyString;
	}

	ddStubTable *dbTable = ddImportDBUtils::getTable(connection, getTableName(), oidTable);
	if(!dbTable)
	{
		wxMessageBox(wxString::Format(_("Cannot reverse engineering table %s.%s."),
		                              schemaName.c_str(), getTableName().c_str()), _("Error when trying to get stub table"),  wxICON_ERROR);
		return wxEmptyString;
	}

	//--------------- DETECT TABLE LEVEL CHANGES

	// Check if TABLE was renamed
	// **Not supported yet**

	// Check if PRIMARY KEY constraint was renamed
	// Some models don't set a name for the primary key, so we ignore those models
	if(this->getPkConstraintName().Len() > 0 && !dbTable->PrimaryKeyName.IsSameAs(this->getPkConstraintName()))
	{
		out += wxT("\n");
		out += wxT("ALTER INDEX \"") + dbTable->PrimaryKeyName + wxT("\" RENAME TO \"") + this->getPkConstraintName() + wxT("\";");
		out += wxT("\n");
	}

	// Check if a UNIQUE KEY constraint was renamed
	// **Not supported yet** (is it possible?)

	//--------------- DETECT COLUMN LEVEL CHANGES

	// Check if A COLUMN was renamed
	// **Not supported yet**

	// Look for columns that exist in the database but not in the model
	bool pkRemovedflag = false;
	bool ukRemoved = false;
	stubColsHashMap::iterator it;
	ddStubColumn *item;

	for (it = dbTable->cols.begin(); it != dbTable->cols.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		ddColumnFigure *column = getColumnByName(key);

		// Check for changes at the column
		if(column)
		{
			// Generate ALTER COLUMN statement if needed
			// Datatype change, length, and precision are checked

			// Temporary conversion fix to datatype of designer should be improved in a future
			wxString dataType = item->typeColumn->Name();
			bool sameDatatype = true, sameScale = true, samePrecision = true;
			int s = -1, p = -1;
			bool useScale = true, needps = false;

			s = item->typeColumn->Length();
			p = item->typeColumn->Precision();

			if(dataType.IsSameAs(wxT("character varying"), false))
			{
				needps = true;
				dataType = wxT("varchar(n)");
			}

			else if(dataType.IsSameAs(wxT("numeric"), false))
			{
				needps = true;
				useScale = false;
				dataType = wxT("numeric(p,s)");
			}
			else if(dataType.IsSameAs(wxT("interval"), false))
			{
				needps = true;
				dataType = wxT("interval(n)");
			}
			else if(dataType.IsSameAs(wxT("bit"), false))
			{
				needps = true;
				dataType = wxT("bit(n)");
			}
			else if(dataType.IsSameAs(wxT("char"), false))
			{
				needps = true;
				dataType = wxT("char(n)");
			}
			else if(dataType.IsSameAs(wxT("varbit"), false))
			{
				needps = true;
				dataType = wxT("varbit(n)");
			}
			else if(dataType.IsSameAs(wxT("character"), false))
			{
				needps = true;
				dataType = wxT("char(n)");
			}

			if(needps)
			{
				if(useScale)
				{
					samePrecision = column->getPrecision() == s;
				}
				else
				{
					samePrecision = column->getPrecision() == s;
					sameScale = column->getScale() == p;
				}
			}


			sameDatatype = column->getRawDataType().IsSameAs(dataType, false);

			if(!samePrecision || !sameScale || !sameDatatype)
			{
				out += wxT("\n");
				out += wxT("ALTER TABLE ");

				if(!schemaName.IsEmpty())
					out += wxT("\"") + schemaName + wxT("\".");
				out += wxT("\"") + getTableName() + wxT("\"") ;

				out	+= wxT(" ALTER COLUMN ") + column->generateSQL(true) + wxT(";");
				out += wxT("\n");
			}
		}
		else  // DROP COLUMN because it doesn't exist in the model anymore
		{
			out += wxT("\n");
			out += wxT("ALTER TABLE ");

			if(!schemaName.IsEmpty())
				out += wxT("\"") + schemaName + wxT("\".");
			out += wxT("\"") + getTableName() + wxT("\"") ;

			out += wxT(" DROP COLUMN \"") + key + wxT("\";");
			out += wxT("\n");
			if(item->isPrimaryKey)
			{
				pkRemovedflag = true;
			}
		}
	}

	// Look for columns that exist in the model but not in the database
	hdIteratorBase *iteratorPK = figuresEnumerator();
	iteratorPK->Next(); //Fixed Position for table rectangle
	iteratorPK->Next(); //Fixed Position for table name

	bool pkAddedflag = false;
	while(iteratorPK->HasNext())
	{
		ddColumnFigure *column = (ddColumnFigure *) iteratorPK->Next();
		if(dbTable->cols.find(column->getColumnName()) == dbTable->cols.end())  //Exist in model but not in db
		{
			out += wxT("\n");
			out += wxT("ALTER TABLE ");
			if(!schemaName.IsEmpty())
				out += wxT("\"") + schemaName + wxT("\".");
			out += wxT("\"") + getTableName() + wxT("\"") ;
			out += wxT(" ADD COLUMN ") + column->generateSQL() + wxT(";");
			out += wxT("\n");
			if(column->isPrimaryKey())
			{
				pkAddedflag = true;
			}
		}
	}

	// Look for all PRIMARY columns in the database's tables
	bool pkChanged = false;
	iteratorPK->ResetIterator();
	iteratorPK->Next(); //Fixed Position for table rectangle
	iteratorPK->Next(); //Fixed Position for table name
	while(iteratorPK->HasNext())
	{
		ddColumnFigure *column = (ddColumnFigure *) iteratorPK->Next();
		if(column->isPrimaryKey())
		{
			if(dbTable->cols.find(column->getColumnName()) == dbTable->cols.end())
			{
				pkChanged = true;
			}
			else
			{
				if(!dbTable->cols[column->getColumnName()]->isPrimaryKey)
				{
					pkChanged = true;
				}
			}
		}
	}
	delete iteratorPK;

	// Look for all PRIMARY columns in the model
	stubColsHashMap::iterator itCol;
	ddStubColumn *itemCol;
	for (itCol = dbTable->cols.begin(); itCol != dbTable->cols.end(); ++itCol)
	{
		wxString colStubName = itCol->first;
		itemCol = itCol->second;
		if(itemCol->isPrimaryKey)
		{
			ddColumnFigure *col = getColumnByName(colStubName);
			if(col != NULL)
			{
				if(!col->isPrimaryKey())
				{
					pkChanged = true;
				}
			}
			else
			{
				pkChanged = true;
			}
		}
	}


	// Handle the addition of a new column to the primary key
	if(pkAddedflag || pkRemovedflag || pkChanged)
	{
		// Drop existing primary key
		out += wxT("\n");
		out += wxT("ALTER TABLE ");
		if(!schemaName.IsEmpty())
			out += wxT("\"") + schemaName + wxT("\".");
		out += wxT("\"") + getTableName() + wxT("\"") ;
		out += wxT(" DROP CONSTRAINT \"") + this->getPkConstraintName() + wxT("\";");
		// Create the new one
		out += generateSQLAlterPks(schemaName);
	}

	// Handle changes from NOT NULL to NULL (always after dropping PK)
	for (it = dbTable->cols.begin(); it != dbTable->cols.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		ddColumnFigure *column = getColumnByName(key);

		// Check for changes at the column level
		if(column)
		{
			// Generate ALTER COLUMN statement if needed
			// Datatype change, length and precision are handled.

			// Temporary conversion fix to datatype of designer should be improved in a future
			wxString dataType = item->typeColumn->Name();
			bool sameDatatype = true, sameScale = true, samePrecision = true;
			int s = -1, p = -1;

			// Model has now NULL constraint, so drop the NOT NULL constraint
			if(column->isNotNull())
			{
				if(!item->isNotNull)
				{
					out += wxT("\n");
					out += wxT("ALTER TABLE ");
					if(!schemaName.IsEmpty())
						out += wxT("\"") + schemaName + wxT("\".");
					out += wxT("\"") + getTableName() + wxT("\"") ;
					out += wxT(" ALTER COLUMN \"") + column->getColumnName() + _("\" SET NOT NULL;");
					out += wxT("\n");
				}
			}
			else if(!column->isNotNull())
			{
				if(item->isNotNull)
				{
					out += wxT("\n");
					out += wxT("ALTER TABLE \"");
					if(!schemaName.IsEmpty())
						out += wxT("\"") + schemaName + wxT("\".");
					out += wxT("\"") + getTableName() + wxT("\"") ;
					out += wxT("\" ALTER COLUMN \"") + column->getColumnName() + wxT("\" DROP NOT NULL;");
					out += wxT("\n");
				}
			}
		}
	}

	// Check UK conditions
	int i, maxUkn = this->getUkConstraintsNames().Count();
	for(i = 0; i < maxUkn; i++)
	{
		if(this->getUkConstraintsNames()[i].Len() == 0)
		{
			wxMessageBox(wxString::Format(_("Some UNIQUE keys on table %s have no name.\nYou should set a name for them, so that pgAdmin can check consistency with the already available database constraints."),
			                              getTableName().c_str()), _("Trying to build ALTER sentences"),  wxICON_ERROR);
			return wxEmptyString;
		}
	}

	// Search for UNIQUE key deleted from model, but available in the database
	// Two steps process: first drop then delete from wxarraystring
	int maxUkStub = dbTable->UniqueKeysNames.Count();
	for(i = 0; i < maxUkStub; i++)
	{
		// Drop UK [in db but not in model]
		if(this->getUkConstraintsNames().Index(dbTable->UniqueKeysNames[i]) == wxNOT_FOUND)
		{
			// Drop it
			out += wxT("\n");
			out += wxT("DROP INDEX \"") + dbTable->UniqueKeysNames[i] + wxT("\";");
			out += wxT("\n");

			// This index metadata is not useful anymore, so erase it at temporary stub table.
			stubColsHashMap::iterator itDelUkIdx;
			ddStubColumn *itemDelUkIdx;
			for (itDelUkIdx = dbTable->cols.begin(); itDelUkIdx != dbTable->cols.end(); ++itDelUkIdx)
			{
				wxString keyDelUkIdx = itDelUkIdx->first;
				itemDelUkIdx = itDelUkIdx->second;
				if(itemDelUkIdx->uniqueKeyIndex == i)
				{
					itemDelUkIdx->uniqueKeyIndex = -1;
				}
			}
		}
	}

	maxUkStub = dbTable->UniqueKeysNames.Count();
	for(i = 0; i < maxUkStub; i++)
	{
		// Drop UK [in db but not in model]
		if(this->getUkConstraintsNames().Index(dbTable->UniqueKeysNames[i]) == wxNOT_FOUND)
		{
			dbTable->UniqueKeysNames.RemoveAt(i);
			maxUkStub = dbTable->UniqueKeysNames.Count();
		}
	}

	// Search for new UK in model to add ADD CONSTRAINT clause
	int maxUkModel = this->getUkConstraintsNames().Count();
	for(i = 0; i < maxUkModel; i++)
	{
		// Add UK [in model but not in db]
		if(dbTable->UniqueKeysNames.Index(this->getUkConstraintsNames()[i]) == wxNOT_FOUND)
		{
			// Create it
			out += wxT("\nALTER TABLE ");
			if(!schemaName.IsEmpty())
				out += wxT("\"") + schemaName + wxT("\".");
			out += wxT("\"") + getTableName() + wxT("\"") ;
			out += wxT(" ADD ");
			if(!getUkConstraintsNames()[i].IsEmpty())
			{
				out += wxT(" CONSTRAINT \"") + getUkConstraintsNames()[i] + wxT("\" ");
			}
			out += wxT(" UNIQUE ( ");

			int countUk = 0;
			hdIteratorBase *iteratorUk = figuresEnumerator();
			iteratorUk->Next(); //Fixed Position for table rectangle
			iteratorUk->Next(); //Fixed Position for table name
			while(iteratorUk->HasNext())
			{
				ddColumnFigure *column = (ddColumnFigure *) iteratorUk->Next();
				if(column->getUniqueConstraintIndex() == i)
					countUk++;
			}

			iteratorUk->ResetIterator();
			iteratorUk->Next(); //Fixed Position for table rectangle
			iteratorUk->Next(); //Fixed Position for table name
			while(iteratorUk->HasNext())
			{
				ddColumnFigure *column = (ddColumnFigure *) iteratorUk->Next();
				if(column->isUniqueKey() && column->getUniqueConstraintIndex() == i)
				{
					out += wxT("\"") + column->getColumnName() + wxT("\"");
					countUk--;
					if(countUk > 0)
					{
						out += wxT(", ");
					}
					else
					{
						out += wxT(");");
					}
				}
			}
			delete iteratorUk;
		}
	}

	// After delete/create UK, look for changes at existing UK
	// Unified UK indexes at both dbtable and ddTableFigure [same index at dbtable that in tablefigure]
	// BUT in table figure exists indexes without an equivalent in dbtable
	maxUkStub = this->getUkConstraintsNames().Count();
	for(i = 0; i < maxUkStub; i++)
	{
		int oldIndex = dbTable->UniqueKeysNames.Index(this->getUkConstraintsNames()[i]);
		int newIndex = i;

		if(oldIndex != wxNOT_FOUND)
		{
			stubColsHashMap::iterator itDelUkIdx;
			ddStubColumn *itemDelUkIdx;
			for (itDelUkIdx = dbTable->cols.begin(); itDelUkIdx != dbTable->cols.end(); ++itDelUkIdx)
			{
				wxString keyDelUkIdx = itDelUkIdx->first;
				itemDelUkIdx = itDelUkIdx->second;
				if(item->uniqueKeyIndex == oldIndex)
				{
					item->uniqueKeyIndex = newIndex;
				}
			}
		}
	}

	// Generate again those UK that fill one of these conditions:
	// 1. UK at tablefigure have more columns that at stub
	// 2. Uk at stub have more column that at tablefigure
	// Right now, UK constraints have unified indexes (same index at tablefigure and stub)
	maxUkStub = this->getUkConstraintsNames().Count();

	// for each UK
	for(i = 0; i < maxUkStub; i++)
	{
		// Only for UKs with number unified that exists at both sides [db and tablefigure] [with boundaries check]
		int boundarie1 = getUkConstraintsNames().Count();
		int boundarie2 = dbTable->UniqueKeysNames.Count();
		if( (i < boundarie1) && (i < boundarie2) && getUkConstraintsNames().Index( dbTable->UniqueKeysNames[i]) != wxNOT_FOUND )
		{
			// Assumption
			bool createUkAgain = false;

			// CHECK FIRST CONDITION --->   1. UK at tablefigure have more columns that at stub?
			hdIteratorBase *iteratorUk = figuresEnumerator();
			iteratorUk->Next(); //Fixed Position for table rectangle
			iteratorUk->Next(); //Fixed Position for table name

			while(iteratorUk->HasNext())
			{
				ddColumnFigure *column = (ddColumnFigure *) iteratorUk->Next();
				// For each UK column of the constraint with same index (position i, constraint at getUkConstraintsNames()[] )
				if(column->getUniqueConstraintIndex() == i)
				{
					if(dbTable->cols.find(column->getColumnName()) == dbTable->cols.end())  //found at dbtable that uk column
					{
						createUkAgain = true;
					}
					else
					{
						ddStubColumn *stubCol = dbTable->cols[column->getColumnName()];
						if(!stubCol->isUniqueKey())
						{
							createUkAgain = true;
						}
					}
				}
			}
			delete iteratorUk;

			// CHECK SECOND CONDITION --->   UK at stub have more uk column of same index that at tablefigure?
			stubColsHashMap::iterator itCol;
			ddStubColumn *itemCol;
			for (itCol = dbTable->cols.begin(); itCol != dbTable->cols.end(); ++itCol)
			{
				bool isAtFigure = false;
				wxString colStubName = itCol->first;
				itemCol = itCol->second;
				if(itemCol->uniqueKeyIndex == i)
				{
					ddColumnFigure *colUk = getColumnByName(colStubName);
					if(colUk == NULL)
					{
						createUkAgain = true;
					}
					else
					{
						if(!colUk->isUniqueKey())
						{
							createUkAgain = true;
						}
					}
				}
			}

			if(createUkAgain)
			{
				// Drop it
				out += wxT("\n");
				out += wxT("DROP INDEX \"") + dbTable->UniqueKeysNames[i] + wxT("\";");
				out += wxT("\n");

				// Create it
				out += wxT("\nALTER TABLE ");
				if(!schemaName.IsEmpty())
					out += wxT("\"") + schemaName + wxT("\".");
				out += wxT("\"") + getTableName() + wxT("\"") ;
				out += wxT(" ADD ");
				if(!getUkConstraintsNames()[i].IsEmpty())
				{
					out += wxT(" CONSTRAINT \"") + getUkConstraintsNames()[i] + wxT("\" ");
				}
				out += wxT(" UNIQUE ( ");

				int countUk = 0;
				hdIteratorBase *iteratorUk = figuresEnumerator();
				iteratorUk->Next(); //Fixed Position for table rectangle
				iteratorUk->Next(); //Fixed Position for table name
				while(iteratorUk->HasNext())
				{
					ddColumnFigure *column = (ddColumnFigure *) iteratorUk->Next();
					if(column->getUniqueConstraintIndex() == i)
						countUk++;
				}

				iteratorUk->ResetIterator();
				iteratorUk->Next(); //Fixed Position for table rectangle
				iteratorUk->Next(); //Fixed Position for table name
				while(iteratorUk->HasNext())
				{
					ddColumnFigure *column = (ddColumnFigure *) iteratorUk->Next();
					if(column->isUniqueKey() && column->getUniqueConstraintIndex() == i)
					{
						out += wxT("\"") + column->getColumnName() + wxT("\"");
						countUk--;
						if(countUk > 0)
						{
							out += wxT(", ");
						}
						else
						{
							out += wxT(" );");
						}
					}
				}
				delete iteratorUk;
			}
		}
	}

	// Validate all FKs have a name defined at model
	hdIteratorBase *iteratorRelations = observersEnumerator();
	while(iteratorRelations->HasNext())
	{
		ddRelationshipFigure *r = (ddRelationshipFigure *) iteratorRelations->Next();
		if(r->getConstraintName().Len() == 0)	// Add to list, FKs with this table as destination. source ---<| destination
		{
			wxMessageBox(wxString::Format(_("Some foreign keys keys on table %s have no name.\nYou should set a name for them, so that pgAdmin can check consistency with the already available database constraints."),
			                              getTableName().c_str()), _("Trying to build ALTER sentences"),  wxICON_ERROR);
			return wxEmptyString;
		}
	}

	//Check Foreign Keys
	// FK at model are same (including attributes and columns) at db
	iteratorRelations->ResetIterator();
	while(iteratorRelations->HasNext())
	{
		ddRelationshipFigure *r = (ddRelationshipFigure *) iteratorRelations->Next();
		if(r->getEndFigure() == this)	// Only check FK this table as destination. source ---<| destination
		{
			// Check relationship from model exists a db?
			if(ddImportDBUtils::existsFk(connection, dbTable->OIDTable, schemaName, r->getConstraintName(), r->getStartTable()->getTableName()))
			{
				// Columns and other properties are exactly the same? if not, then drop and create again
				if(!ddImportDBUtils::isModelSameDbFk(connection, dbTable->OIDTable, schemaName, r->getConstraintName(), r->getStartTable()->getTableName(), r->getEndTable()->getTableName(), dbTable, r))
				{
					// Drop it first
					out += wxT("\n");
					out += wxT("ALTER TABLE ");
					if(!schemaName.IsEmpty())
						out += wxT("\"") + schemaName + wxT("\".");
					out += wxT("\"") + this->getTableName() + wxT("\"") ;
					out += wxT(" DROP CONSTRAINT \"") + r->getConstraintName() + wxT("\";");
					out += wxT("\n");

					// Then Add it again with changes
					out += r->generateSQL(schemaName);

				}
			}
			else //relationship only exists at model and not in db
			{
				//Create because it doesn't exists
				out += r->generateSQL(schemaName);
			}
		}
	}

	// Check foreign keys available in the database but not in the model
	// because we need to drop them
	// First, create a list of FKs at destination table in the model
	wxArrayString validFks;
	iteratorRelations->ResetIterator();
	while(iteratorRelations->HasNext())
	{
		ddRelationshipFigure *r = (ddRelationshipFigure *) iteratorRelations->Next();
		if(r->getEndFigure() == this)	// Add to list, FKs with this table as destination. source ---<| destination
		{
			validFks.Add(r->getConstraintName());
		}
	}
	delete iteratorRelations;

	wxArrayString fksToDelete = ddImportDBUtils::getFkAtDbNotInModel(connection, dbTable->OIDTable, schemaName, validFks , design);

	int max = fksToDelete.Count();
	for(i = 0; i < max; i++)
	{
		// Drop it
		out += wxT("\n");
		out += wxT("ALTER TABLE ");
		if(!schemaName.IsEmpty())
			out += wxT("\"") + schemaName + wxT("\".");
		out += wxT("\"") + this->getTableName() + wxT("\"") ;
		out += wxT(" DROP CONSTRAINT \"") + fksToDelete[i] + wxT("\";");
		out += wxT("\n");
	}

	if(dbTable)
		delete dbTable;
	return out;
}
