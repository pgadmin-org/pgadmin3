//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableFigure.h - Draw table figure of a model
//
//////////////////////////////////////////////////////////////////////////// 

#ifndef DDTABLEFIGURE_H
#define DDTABLEFIGURE_H
#include "dd/wxhotdraw/figures/wxhdCompositeFigure.h"
#include "dd/wxhotdraw/figures/wxhdRectangleFigure.h"
#include "dd/wxhotdraw/figures/wxhdSimpleTextFigure.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"
#include "dd/dditems/figures/ddColumnFigure.h"

class ddScrollBarHandle;
class ddRelationshipFigure;

class ddTableFigure : public wxhdCompositeFigure
{
public:
	ddTableFigure(wxString tableName, int x, int y, wxString shortName=wxEmptyString);
    ~ddTableFigure();
	
	//add remove items
	bool colNameAvailable(wxString name);
	void addColumn(ddColumnFigure *column);
	void removeColumn(ddColumnFigure *column);
	
	//movement
	virtual void basicMoveBy(int x, int y);
	
	//show messages to set fk destination
	void setSelectFkDestMode(bool value);

	//delete hack tables
	void processDeleteAlert(wxhdDrawingView *view);
	
	//columns scrolls
	void updateTableSize();
	void recalculateColsPos();
	void setColsRowsWindow(int num);
	wxhdRect& getColsSpace();
	wxhdRect& getFullSpace();
	wxhdRect& getTitleRect();
	int getTotalColumns();
	int getColumnsWindow();
	int getTopColWindowIndex();
	void setColumnsWindow(int value, bool maximize=false);
	void columnsWindowUp();
	void columnsWindowDown();
	int getColDefaultHeight(wxFont font);
	
	//metadata
	wxString getTableName();
	void setShortTableName(wxString shortName);
	wxString getShortTableName();
	wxString generateSQL();
	static wxString generateShortName(wxString longName);
	wxArrayString getAllColumnsNames();
	wxArrayString getAllFkSourceColsNames(bool pk, int ukIndex=-1);
	ddColumnFigure *getColumnByName(wxString name);
	bool validateTable(wxString& errors);

	//uk pk constraints
	void setPkConstraintName(wxString name);
	wxString getPkConstraintName();
	wxArrayString& getUkConstraintsNames();
	bool disablePrimaryKey();
	bool enablePrimaryKey();
		
	//fk related
	void updateFkObservers();
	void updateSizeOfObservers();
	void prepareForDeleteFkColumn(ddColumnFigure *column);

protected:
	//drawing
	virtual void basicDraw(wxBufferedDC& context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC& context, wxhdDrawingView *view);

private:
	//Main Rectangle Sizes
	wxhdRect fullSizeRect, titleRect, titleColsRect, colsRect, titleIndxsRect, indxsRect;
	wxhdRect unScrolledColsRect, unScrolledFullSizeRect, unScrolledTitleRect;
	
	//Rectangle item counters
	int colsRowsSize, colsWindow, idxsRowsSize, idxsWindow;
	
	//vector indexes
	int maxColIndex,minIdxIndex,maxIdxIndex;
	
	//position
	int beginDrawCols, beginDrawIdxs;
	
	//Default Figures
	wxhdRectangleFigure *rectangleFigure;
	ddTextTableItemFigure *tableTitle;

	//helper variables
	bool selectingFkDestination;
	int internalPadding, externalPadding;
	bool calcScrolled;
	
	//specials handles
	ddScrollBarHandle *scrollbar;

	//methods
	int getHeightFontMetric(wxString text, wxFont font);
	int getFiguresMaxWidth();
	void calcRectsAreas();

	//pk uk(s)
	wxString pkName;
	wxArrayString ukNames;
};
#endif
