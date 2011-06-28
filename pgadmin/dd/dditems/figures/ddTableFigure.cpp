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

ddTableFigure::ddTableFigure(wxString tableName, int x, int y, wxString shortName):
	wxhdCompositeFigure()
{
	setKindId(DDTABLEFIGURE);
	internalPadding = 2;
	externalPadding = 4;
	selectingFkDestination = false;

	//Set Value default Attributes
	fontAttribute->font().SetPointSize(8);
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
	rectangleFigure->moveTo(x, y);
	add(rectangleFigure);

	tableTitle = new ddTextTableItemFigure(tableName, dt_null, NULL);
	tableTitle->setOwnerTable(this);
	tableTitle->setEditable(true);
	tableTitle->moveTo(x, y);
	tableTitle->disablePopUp();
	tableTitle->setShowDataType(false);
	add(tableTitle);
	tableTitle->setAlias(shortName);  //Should be here to avoid a null pointer bug
	tableTitle->moveTo(rectangleFigure->getBasicDisplayBox().x + internalPadding * 2, rectangleFigure->getBasicDisplayBox().y + internalPadding / 2);

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
	pkName = wxT("NewTable_pk");
	ukNames.clear();

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

bool ddTableFigure::colNameAvailable(wxString name)
{
	bool out = true;
	ddColumnFigure *f;

	wxhdIteratorBase *iterator = figuresEnumerator();
	iterator->Next(); //First figure is main rect
	iterator->Next(); //Second figure is main title

	while(iterator->HasNext())
	{
		f = (ddColumnFigure *) iterator->Next();
		if(f->getColumnName(false).IsSameAs(name))
		{
			out = false;
			break;
		}
	}
	delete iterator;

	return out;
}

//WARNING: Columns SHOULD BE ADDED only using this function to avoid strange behaviors
void ddTableFigure::addColumn(ddColumnFigure *column)
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
	calcRectsAreas();
	recalculateColsPos();
}

void ddTableFigure::removeColumn(ddColumnFigure *column)
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
	calcRectsAreas();
	recalculateColsPos();
	if(colsWindow == colsRowsSize) //if handle need to be removed, remove it
	{
		if(figureHandles->existsObject(scrollbar))
			figureHandles->removeItem(scrollbar);
	}
	//hack to update relationship position when table size change
	moveBy(-1, 0);
	moveBy(1, 0);
	column = NULL;
}

void ddTableFigure::recalculateColsPos()
{
	wxFont font = settings->GetSystemFont();
	int defaultHeight = getColDefaultHeight(font);

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0); //first figure is always Rect
	int horizontalPos = f->displayBox().x + 2;
	int verticalPos = 0;

	for(int i = 2; i < maxColIndex ; i++)
	{
		f = (wxhdIFigure *) figureFigures->getItemAt(i); //table title
		if( (i >= beginDrawCols) && (i <= (colsWindow + beginDrawCols)) ) //Visible to draw
		{
			verticalPos = colsRect.y + (defaultHeight * (i - beginDrawCols) + ((i - beginDrawCols) * internalPadding));
			f->moveTo(horizontalPos, verticalPos);
		}
		else
			f->moveTo(-65000, -65000); //any figure outside canvas (x<0 || y<0) is not draw & not used to calculate displaybox
	}
}



void ddTableFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	calcRectsAreas();

	if(calcScrolled) //Hack to avoid pass view as parameter to calcRectsAreas() because is sometimes called outside a paint event
	{
		view->CalcScrolledPosition(fullSizeRect.x, fullSizeRect.y, &fullSizeRect.x, &fullSizeRect.y);
		view->CalcScrolledPosition(titleRect.x, titleRect.y, &titleRect.x, &titleRect.y);
		view->CalcScrolledPosition(titleColsRect.x, titleColsRect.y, &titleColsRect.x, &titleColsRect.y);
		view->CalcScrolledPosition(colsRect.x, colsRect.y, &colsRect.x, &colsRect.y);
		view->CalcScrolledPosition(titleIndxsRect.x, titleIndxsRect.y, &titleIndxsRect.x, &titleIndxsRect.y);
		view->CalcScrolledPosition(indxsRect.x, indxsRect.y, &indxsRect.x, &indxsRect.y);
		calcScrolled = false;
	}

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0); //table rectangle
	f->draw(context, view);
	f = (wxhdIFigure *) figureFigures->getItemAt(1); //table title
	f->draw(context, view);

	for(int i = beginDrawCols; i < (colsWindow + beginDrawCols); i++)
	{
		f = (wxhdIFigure *) figureFigures->getItemAt(i); //table title
		if(f->displayBox().GetPosition().x > 0 && f->displayBox().GetPosition().y > 0)
		{
			f->draw(context, view);
		}
	}

	reapplyAttributes(context, view); //reset attributes to default of figure because can be modified at Draw functions.

	//Set Font for title "Columns"
	wxFont font = settings->GetSystemFont();
	font.SetPointSize(7);
	context.SetFont(font);

	//Draw Columns Title Line 1
	context.DrawLine(titleColsRect.GetTopLeft(), titleColsRect.GetTopRight());
	//Draw Columns Title
	context.DrawText(wxT("Columns"), titleColsRect.x + 3, titleColsRect.y);
	//Draw Columns Title Line 2
	context.DrawLine(titleColsRect.GetBottomLeft(), titleColsRect.GetBottomRight());
	//DrawVertical Lines
	context.DrawLine(titleColsRect.GetBottomLeft().x + 11, titleColsRect.GetBottomLeft().y, titleColsRect.GetBottomLeft().x + 11, titleIndxsRect.GetTopLeft().y);
	context.DrawLine(titleColsRect.GetBottomLeft().x + 22, titleColsRect.GetBottomLeft().y, titleColsRect.GetBottomLeft().x + 22, titleIndxsRect.GetTopLeft().y);
	//Draw Indexes Title Line 1
	context.DrawLine(titleIndxsRect.GetTopLeft(), titleIndxsRect.GetTopRight());
	//Draw Indexes Title
	//disable until implemented in a future: context.DrawText(wxT("Indexes"),titleIndxsRect.x+3,titleIndxsRect.y);
	//Draw Indexes Title Line 2
	context.DrawLine(titleIndxsRect.GetBottomLeft(), titleIndxsRect.GetBottomRight());

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
		x = fullSizeRect.GetTopLeft().x + (((fullSizeRect.GetTopRight().x - fullSizeRect.GetTopLeft().x) - w) / 2);
		y = fullSizeRect.GetTopLeft().y - h - 2;
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
	calcRectsAreas();

	if(calcScrolled) //Hack to avoid pass view as parameter to calcRectsAreas() because is sometimes called outside a paint event
	{
		view->CalcScrolledPosition(fullSizeRect.x, fullSizeRect.y, &fullSizeRect.x, &fullSizeRect.y);
		view->CalcScrolledPosition(titleRect.x, titleRect.y, &titleRect.x, &titleRect.y);
		view->CalcScrolledPosition(titleColsRect.x, titleColsRect.y, &titleColsRect.x, &titleColsRect.y);
		view->CalcScrolledPosition(colsRect.x, colsRect.y, &colsRect.x, &colsRect.y);
		view->CalcScrolledPosition(titleIndxsRect.x, titleIndxsRect.y, &titleIndxsRect.x, &titleIndxsRect.y);
		view->CalcScrolledPosition(indxsRect.x, indxsRect.y, &indxsRect.x, &indxsRect.y);
		calcScrolled = false;
	}

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0); //table rectangle
	f->drawSelected(context, view);
	f = (wxhdIFigure *) figureFigures->getItemAt(1); //table title
	f->drawSelected(context, view);

	for(int i = beginDrawCols; i < (colsWindow + beginDrawCols); i++)
	{
		f = (wxhdIFigure *) figureFigures->getItemAt(i); //table title
		if(f->displayBox().GetPosition().x > 0 && f->displayBox().GetPosition().y > 0)
		{
			f->drawSelected(context, view);
		}
	}

	reapplySelAttributes(context, view); //reset attributes to default of figure because can be modified at Draw functions.
	wxFont font = settings->GetSystemFont();
	font.SetPointSize(7);
	context.SetFont(font);

	//Draw Columns Title Line 1
	context.DrawLine(titleColsRect.GetTopLeft(), titleColsRect.GetTopRight());
	//Draw Columns Title
	context.DrawText(wxT("Columns"), titleColsRect.x + 3, titleColsRect.y);
	//Draw Columns Title Line 2
	context.DrawLine(titleColsRect.GetBottomLeft(), titleColsRect.GetBottomRight());
	//DrawVertical Lines
	context.DrawLine(titleColsRect.GetBottomLeft().x + 11, titleColsRect.GetBottomLeft().y, titleColsRect.GetBottomLeft().x + 11, titleIndxsRect.GetTopLeft().y);
	context.DrawLine(titleColsRect.GetBottomLeft().x + 22, titleColsRect.GetBottomLeft().y, titleColsRect.GetBottomLeft().x + 22, titleIndxsRect.GetTopLeft().y);
	//Draw Indexes Title Line 1
	context.DrawLine(titleIndxsRect.GetTopLeft(), titleIndxsRect.GetTopRight());
	//Draw Indexes Title
	//disable until implemented in a future: context.DrawText(wxT("Indexes"),titleIndxsRect.x+3,titleIndxsRect.y);
	//Draw Indexes Title Line 2
	context.DrawLine(titleIndxsRect.GetBottomLeft(), titleIndxsRect.GetBottomRight());
}

void ddTableFigure::setColsRowsWindow(int num)
{
	if(num > 0)
	{
		colsWindow = num;
		wxFont font = settings->GetSystemFont();
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


void ddTableFigure::calcRectsAreas()
{
	calcScrolled = true;

	int maxWidth = getFiguresMaxWidth() + externalPadding;
	if(maxWidth < 100)
		maxWidth = 100;
	wxFont font = settings->GetSystemFont();
	int defaultHeight = getColDefaultHeight(font);

	//cache displayBox()
	wxhdRect db = displayBox();

	//*** titleRect
	font.SetPointSize(7);
	int colsTitleHeight = getHeightFontMetric(wxT("Columns"), font);
	titleRect.x = db.x;
	titleRect.y = db.y;
	titleRect.width = maxWidth;
	titleRect.height = defaultHeight;

	titleColsRect.x = db.x;
	titleColsRect.y = titleRect.y + titleRect.height;
	titleColsRect.width = maxWidth;
	titleColsRect.height = colsTitleHeight;
	unScrolledTitleRect = titleColsRect;

	//*** colsRect
	colsRect.width = maxWidth;
	if(colsWindow > 0)
		colsRect.height = defaultHeight * colsWindow + (colsWindow * internalPadding);
	else
		colsRect.height = defaultHeight;
	colsRect.x = db.x;
	colsRect.y = titleRect.y + titleRect.height + titleColsRect.height;
	unScrolledColsRect = colsRect;

	//*** idxTitleRect
	titleIndxsRect.width = maxWidth;
	titleIndxsRect.height = colsTitleHeight;
	titleIndxsRect.x = db.x;
	titleIndxsRect.y = colsRect.y + colsRect.height;

	//*** indexesRect
	indxsRect.width = maxWidth;
	indxsRect.height = defaultHeight * idxsWindow + (idxsWindow * internalPadding);
	indxsRect.x = db.x;
	indxsRect.y = titleIndxsRect.y + titleIndxsRect.height;

	//*** FullTable Size
	fullSizeRect.width = maxWidth;
	fullSizeRect.height = titleRect.height + titleColsRect.height + colsRect.height + titleIndxsRect.height + indxsRect.height;
	fullSizeRect.x = db.x;
	fullSizeRect.y = titleRect.y;
	unScrolledFullSizeRect = fullSizeRect;

	//Update sizes
	wxSize value = fullSizeRect.GetSize();
	rectangleFigure->setSize(value);
}

void ddTableFigure::updateTableSize()
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
	calcRectsAreas();
	wxSize value = fullSizeRect.GetSize();
	rectangleFigure->setSize(value);
	//hack to update relationship position when table size change
	moveBy(-1, 0);
	moveBy(1, 0);
}

wxhdRect &ddTableFigure::getColsSpace()
{
	return unScrolledColsRect;
}

wxhdRect &ddTableFigure::getFullSpace()
{
	return unScrolledFullSizeRect;
}

wxhdRect &ddTableFigure::getTitleRect()
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

void ddTableFigure::setColumnsWindow(int value, bool maximize)
{

	if(!maximize)
	{

		//if value >0 && <= max size table && table+offset < maxColIndex with window
		if( (value > 0) && (value <= colsRowsSize) && (maxColIndex >= ( beginDrawCols + value ) ) )
		{
			colsWindow = value;
			calcRectsAreas();
			recalculateColsPos();
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
					calcRectsAreas();
					recalculateColsPos();

				}
			}
		}
	}
	else
	{
		beginDrawCols = 2;
		colsWindow = value;
		calcRectsAreas();
		recalculateColsPos();
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

void ddTableFigure::columnsWindowUp()  //move window from number to zero
{
	if( beginDrawCols > 2 )
	{
		beginDrawCols--;
		calcRectsAreas();
		recalculateColsPos();
	}
}

void ddTableFigure::columnsWindowDown()  //move window from number to maxcolumns
{
	if( (beginDrawCols + colsWindow) < maxColIndex)
	{
		beginDrawCols++;
		calcRectsAreas();
		recalculateColsPos();
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
4. first word of a syllabe will be defined as first letter before vowels (a,e,i,o,u).
3. Tables with only one word:
		1st char of first syllabe + 2nd char of second syllabe + last char.
		ex: EMPLOYEES will be EPE
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
		c = 0;
		if(numVowels >= 3)
		{
			//word have at least 3 vowels tokens
			while( vowelsTokens.HasMoreTokens() )
			{
				wxString token = vowelsTokens.GetNextToken();
				if(c == 0)
					f = token.GetChar(0);
				if(c == 1)
					s = token.GetChar(0);
				if(((c + 1) - numVowels) == 0)
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

//drop foreign keys with this table as origin or destination
void ddTableFigure::processDeleteAlert(wxhdDrawingView *view)
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
			if(view->isFigureSelected(rel))
				view->removeFromSelection(rel);
			view->remove(rel);
			repeatFlag = true;
			delete rel;
			break;
		}
	}
	while(repeatFlag);

	delete iterator;
}

void ddTableFigure::basicMoveBy(int x, int y)
{

	wxhdIFigure *f = (wxhdIFigure *) figureFigures->getItemAt(0);
//Hack to avoid bug in if clause
	int width =  spaceForMovement.GetWidth();
	int height =  spaceForMovement.GetHeight();
	int bottom = f->displayBox().y + f->displayBox().height + y;
	int right = f->displayBox().x + f->displayBox().width + x;
	int left = f->displayBox().x + x;
	int top = f->displayBox().y + y;

//limit movemnt of table figures to canvas space
	if( (left > 0) && (top > 0) && (right < width) && (bottom < height) )
		wxhdCompositeFigure::basicMoveBy(x, y);
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
wxString ddTableFigure::generateSQL()
{
	//Columns and table
	wxString tmp(wxT("CREATE TABLE "));
	tmp += getTableName() + wxT(" (\n");
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
	//Pk, Uk Constraints
	iterator->ResetIterator();
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
		tmp += wxT(", \nPRIMARY KEY ( ");
		iterator->ResetIterator();
		iterator->Next(); //Fixed Position for table rectangle
		iterator->Next(); //Fixed Position for table name

		while(iterator->HasNext())
		{
			ddColumnFigure *column = (ddColumnFigure *) iterator->Next();
			if(column->isPrimaryKey())
			{
				tmp += column->getColumnName();
				contPk--;
				if(contPk > 0)
				{
					tmp += wxT(" , ");
				}
				else
				{
					tmp += wxT(" ) ");
				}
			}
		}
	}
	delete iterator;
	//Fk Constraint
	iterator = observersEnumerator();
	if(!iterator->HasNext())
	{
		tmp += wxT("\n ); ");
	}
	else
	{
		while(iterator->HasNext())
		{
			ddRelationshipFigure *rel = (ddRelationshipFigure *) iterator->Next();
			if(rel->getStartFigure() != this)
			{
				wxString tmp2 = rel->generateSQL();
				if(tmp2.length() > 0)
				{
					tmp += wxT(" , \n");
					tmp += tmp2;
				}
			}
		}
		tmp += wxT("\n ); ");
	}
	delete iterator;
	return tmp;
}
