//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
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
#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
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
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/wxhotdraw/connectors/wxhdLocatorConnector.h"
#include "dd/wxhotdraw/main/wxhdDrawing.h"
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

void ddTableFigure::Init(wxString tableName, int x, int y, wxString shortName)
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
	rectangleFigure = new wxhdRectangleFigure();
	rectangleFigure->moveTo(0, x, y);
	add(rectangleFigure);

	tableTitle = new ddTextTableItemFigure(tableName, dt_null, NULL);
	tableTitle->setOwnerTable(this);
	tableTitle->setEditable(true);
	tableTitle->moveTo(0, x, y);
	tableTitle->disablePopUp();
	tableTitle->setShowDataType(false);
	add(tableTitle);
	tableTitle->setAlias(shortName);  //Should be here to avoid a null pointer bug
	tableTitle->moveTo(0, rectangleFigure->getBasicDisplayBox().x[0] + internalPadding * 2, rectangleFigure->getBasicDisplayBox().y[0] + internalPadding / 2);

	//Intialize handles
	wxBitmap image = wxBitmap(*ddAddColumn_png_img);
	wxSize valueSize = wxSize(8, 8);
	figureHandles->addItem(new ddAddColButtonHandle((wxhdIFigure *)this, (wxhdILocator *)new ddAddColLocator(), image, valueSize));
	image = wxBitmap(*ddAddForeignKey_png_img);
	figureHandles->addItem(new ddAddFkButtonHandle((wxhdIFigure *)this, (wxhdILocator *)new ddAddFkLocator(), image, valueSize));
	image = wxBitmap(*ddRemoveTable_png_img);
	figureHandles->addItem(new ddRemoveTableButtonHandle((wxhdIFigure *)this, (wxhdILocator *)new ddRemoveTableLocator(), image, valueSize));
	image = wxBitmap(*ddMinimizeTable_png_img);
	wxBitmap image2 = wxBitmap(*ddMaximizeTable_png_img);
	figureHandles->addItem(new ddMinMaxTableButtonHandle((wxhdIFigure *)this, (wxhdILocator *)new ddMinMaxTableLocator(), image, image2, valueSize));
	figureHandles->addItem(new ddSouthTableSizeHandle(this, (wxhdILocator *)new ddTableBottomLocator()));

	//Intialize special handle
	valueSize = wxSize(10, colsRect.GetSize().GetHeight());
	scrollbar = new ddScrollBarHandle(this, (wxhdILocator *)new ddScrollBarTableLocator(), valueSize);

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
}

ddTableFigure::ddTableFigure(wxString tableName, int x, int y, wxString shortName):
	wxhdCompositeFigure()
{
	Init(tableName, x, y, shortName);
}

ddTableFigure::ddTableFigure(wxString tableName, int posIdx, int x, int y, wxString shortName):
	wxhdCompositeFigure()
{
	Init(tableName, 0, 0, shortName);
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
	wxhdCompositeFigure::AddPosForNewDiagram();
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
	wxhdCompositeFigure::RemovePosOfDiagram(posIdx);
}

ddColumnFigure *ddTableFigure::getColByName(wxString name)
{
	ddColumnFigure *out = NULL;
	ddColumnFigure *f;

	wxhdIteratorBase *iterator = figuresEnumerator();
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

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0); //first figure is always Rect
	int horizontalPos = f->displayBox().x[posIdx] + 2;
	int verticalPos = 0;

	for(int i = 2; i < maxColIndex ; i++)
	{
		f = (wxhdIFigure *) figureFigures->getItemAt(i); //table title
		if( (i >= beginDrawCols) && (i <= (colsWindow + beginDrawCols)) ) //Visible to draw
		{
			verticalPos = colsRect.y[posIdx] + (defaultHeight * (i - beginDrawCols) + ((i - beginDrawCols) * internalPadding));
			f->moveTo(posIdx, horizontalPos, verticalPos);
		}
		else
			f->moveTo(posIdx, -65000, -65000); //any figure outside canvas (x<0 || y<0) is not draw & not used to calculate displaybox
	}
}



void ddTableFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
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

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0); //table rectangle
	f->draw(context, view);
	f = (wxhdIFigure *) figureFigures->getItemAt(1); //table title
	f->draw(context, view);

	for(int i = beginDrawCols; i < (colsWindow + beginDrawCols); i++)
	{
		f = (wxhdIFigure *) figureFigures->getItemAt(i); //table title
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

void ddTableFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
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

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0); //table rectangle
	f->drawSelected(context, view);
	f = (wxhdIFigure *) figureFigures->getItemAt(1); //table title
	f->drawSelected(context, view);

	for(int i = beginDrawCols; i < (colsWindow + beginDrawCols); i++)
	{
		f = (wxhdIFigure *) figureFigures->getItemAt(i); //table title
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

wxhdMultiPosRect &ddTableFigure::getBasicDisplayBox()
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
		wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(1); //table title
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
	wxhdGeometry g;

	wxhdIteratorBase *iterator = figuresEnumerator();
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

	wxhdRect db = basicDisplayBox.getwxhdRect(posIdx);

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
	wxhdIteratorBase *iterator = figuresEnumerator();
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

wxhdMultiPosRect &ddTableFigure::getColsSpace()
{
	return unScrolledColsRect;
}

wxhdMultiPosRect &ddTableFigure::getFullSpace()
{
	return unScrolledFullSizeRect;
}

wxhdMultiPosRect &ddTableFigure::getTitleRect()
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
	wxhdIteratorBase *iterator = figuresEnumerator();
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
	wxhdIteratorBase *iterator = figuresEnumerator();
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
	wxhdIteratorBase *iterator = figuresEnumerator();
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

/*
Rules to auto generate short names:
0. Table name delimiters are white space (quoted names) or _
1. if last char in a word is "s" is ignored, ex: employees -> last char will be e not s.
2. for quoted table names, quotes are ignored for short name purposes.
4. first char of a syllabe will be defined as first letter before vowels (a,e,i,o,u).
3. Tables with only one word:
		1st char first char of word + 1 char second/third or four syllabe + last char.
		ex: EMPLOYEES will be EME
			PRODUCT	will be PDT
4. Tables with more than one words:
		1st char of first word, first char of second word, last char of last word.
		ex: ITEM DESCRIPTIONS will be IDN
			ITEMS FOR SALE will be IFE
5. Tables with one word but in non latin characters as vowels (a,e,i,o,u) o less than 2 vowels
		first 3 letters of the word
*/

wxString ddTableFigure::generateShortName(wxString longName)
{
	wxString nameT = longName;
	//filter not desiree characters
	nameT.Replace(wxT("\""), wxT(""), true);
	//start to build short name based on rules
	wxStringTokenizer tokens(nameT, wxT(" _"), wxTOKEN_DEFAULT);
	wxChar f, s, l;
	int num = tokens.CountTokens(), c = 0;

	if(num > 1)
	{
		while( tokens.HasMoreTokens() )
		{
			wxString token = tokens.GetNextToken();
			if(c == 0)
				f = token.GetChar(0);
			if(c == 1)
				s = token.GetChar(0);
			if(((c + 1) - num) == 0)
			{
				l = token.GetChar(token.length() - 1);
				if(l == 's')
					l = token.GetChar(token.length() - 2);
			}
			c++;
		}
	}
	else
	{
		//Look for vowels
		wxStringTokenizer vowelsTokens(nameT, wxT("aeiou"), wxTOKEN_DEFAULT);
		int numVowels = vowelsTokens.CountTokens();
		if(numVowels >= 3)
		{
			//word have at least 3 vowels tokens
			f = nameT.GetChar(0);
			wxString token;
			token = vowelsTokens.GetNextToken();
			token = vowelsTokens.GetNextToken();
			s = token.GetChar(0);
			if(s == 0)
			{
				token = vowelsTokens.GetNextToken();
				s = token.GetChar(0);
			}
			if(s == 0)
			{
				token = vowelsTokens.GetNextToken();
				s = token.GetChar(0);
			}
			l = nameT.Last();
			if(l == 's')
				l = nameT.GetChar(nameT.length() - 2);
		}
		else
		{
			//Less than two vowels languages or non latin languages
			if(nameT.length() >= 3) // but a least 3 letters
			{
				f = nameT.GetChar(0);
				s = nameT.GetChar(1);
				l = nameT.GetChar(2);
			}
			else  //less three letters
			{
				f = nameT.GetChar(0);
				if(nameT.length() == 2)
					s = nameT.GetChar(1);
				else
					s = ' ';
				l = ' ';
			}

		}
	}
	wxString out = wxString::Format(wxT("%c%c%c"), f, s, l);
	out.UpperCase();
	return out;
}

wxString ddTableFigure::getShortTableName()
{
	ddTextTableItemFigure *c = (ddTextTableItemFigure *) figureFigures->getItemAt(1);
	return c->getAlias();
}

void ddTableFigure::setShortTableName(wxString shortName)
{
	ddTextTableItemFigure *c = (ddTextTableItemFigure *) figureFigures->getItemAt(1);
	c->setAlias(shortName);
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
	wxhdIteratorBase *iterator = observersEnumerator();
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
	wxhdIteratorBase *iterator = observersEnumerator();
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
	wxhdIteratorBase *iterator = observersEnumerator();
	while(iterator->HasNext())
	{
		ddRelationshipFigure *r = (ddRelationshipFigure *) iterator->Next();
		ddTableFigure *destFkTable = (ddTableFigure *) r->getEndFigure();
		destFkTable->updateTableSize();
	}
	delete iterator;
}

//drop foreign keys with this table as origin or destination because table is going to be deleted
void ddTableFigure::processDeleteAlert(wxhdDrawing *drawing)
{
	wxhdIteratorBase *iterator = observersEnumerator();
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

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0);
//Hack to avoid bug in if clause
	int width =  spaceForMovement.GetWidth();
	int height =  spaceForMovement.GetHeight();
	int bottom = f->displayBox().y[posIdx] + f->displayBox().height + y;
	int right = f->displayBox().x[posIdx] + f->displayBox().width + x;
	int left = f->displayBox().x[posIdx] + x;
	int top = f->displayBox().y[posIdx] + y;

//limit movemnt of table figures to canvas space
	if( (left > 0) && (top > 0) && (right < width) && (bottom < height) )
		wxhdCompositeFigure::basicMoveBy(posIdx, x, y);
}

//Validate status of table for SQL DDL generation
bool ddTableFigure::validateTable(wxString &errors)
{
	bool out = true;
	wxString tmp = wxEmptyString;
	ddColumnFigure *f;

	wxhdIteratorBase *iterator = figuresEnumerator();
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
wxString ddTableFigure::generateSQLCreate()
{
	//Columns and table
	wxString tmp(wxT("CREATE TABLE "));
	tmp += +wxT("\"") + getTableName() + wxT("\" (\n");
	wxhdIteratorBase *iterator = figuresEnumerator();
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

wxString ddTableFigure::generateSQLAlterPks()
{
	wxString tmp;
	wxhdIteratorBase *iterator = figuresEnumerator();
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
		tmp += wxT("\n ALTER TABLE ") + getTableName() + _(" ADD ");
		if(!pkName.IsEmpty())
		{
			tmp += _(" CONSTRAINT ") + pkName;
		}
		tmp += _(" PRIMARY KEY ( ");
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

wxString ddTableFigure::generateSQLAlterFks()
{
	wxString tmp;
	wxhdIteratorBase *iterator = figuresEnumerator();
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
				tmp += rel->generateSQL();
			}
		}
	}
	delete iterator;
	return tmp;
}