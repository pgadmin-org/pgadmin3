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
class wxhdDrawing;

class ddTableFigure : public wxhdCompositeFigure
{
public:
	ddTableFigure(wxString tableName, int x, int y, wxString shortName = wxEmptyString);
	ddTableFigure(wxString tableName, int posIdx, int x, int y, wxString shortName = wxEmptyString);
	void InitTableValues(wxArrayString UniqueKeysName, wxString primaryKeyName, int bdc, int bdi, int maxcolsi, int minidxsi, int maxidxsi, int colsrs, int colsw, int idxsrs, int idxsw);
	void Init(wxString tableName, int x, int y, wxString shortName = wxEmptyString);
	wxhdMultiPosRect &getBasicDisplayBox();
	~ddTableFigure();

	//Diagrams related functions
	virtual void AddPosForNewDiagram();
	virtual void RemovePosOfDiagram(int posIdx);

	//add remove items
	ddColumnFigure *getColByName(wxString name);
	void addColumn(int posIdx, ddColumnFigure *column);
	void addColumnFromStorage(ddColumnFigure *column);
	void syncInternalsPosAt(int posIdx, int x, int y);
	void syncInternalsPosAt(wxArrayInt &x, wxArrayInt &y);
	void removeColumn(int posIdx, ddColumnFigure *column);

	//movement
	void manuallyNotifyChange(int posIdx)
	{
		changed(posIdx);
	};
	virtual void basicMoveBy(int posIdx, int x, int y);

	//show messages to set fk destination
	void setSelectFkDestMode(bool value);

	//delete hack tables
	void processDeleteAlert(wxhdDrawing *drawing);

	//columns scrolls
	void updateTableSize(bool notifyChange = true);
	void recalculateColsPos(int posIdx);
	void setColsRowsWindow(int num);
	wxhdMultiPosRect &getColsSpace();
	wxhdMultiPosRect &getFullSpace();
	wxhdMultiPosRect &getTitleRect();
	int getTotalColumns();
	int getColumnsWindow();
	int getTopColWindowIndex();
	void setColumnsWindow(int posIdx, int value, bool maximize = false);
	void columnsWindowUp(int posIdx);
	void columnsWindowDown(int posIdx);
	int getColDefaultHeight(wxFont font);

	//metadata
	wxString getTableName();
	void setShortTableName(wxString shortName);
	wxString getShortTableName();
	wxString generateSQLCreate();
	wxString generateSQLAlterPks();
	wxString generateSQLAlterFks();
	static wxString generateShortName(wxString longName);
	wxArrayString getAllColumnsNames();
	wxArrayString getAllFkSourceColsNames(bool pk, int ukIndex = -1);
	ddColumnFigure *getColumnByName(wxString name);
	bool validateTable(wxString &errors);

	//uk pk constraints
	void setPkConstraintName(wxString name);
	wxString getPkConstraintName();
	wxArrayString &getUkConstraintsNames();
	bool disablePrimaryKey();
	bool enablePrimaryKey();

	//fk related
	void updateFkObservers();
	void updateSizeOfObservers();
	void prepareForDeleteFkColumn(ddColumnFigure *column);

	//ScrollBar persistence related
	int getBeginDrawCols()
	{
		return beginDrawCols;
	};
	int getBeginDrawIdxs()
	{
		return beginDrawIdxs;
	};
	int getMaxColIndex()
	{
		return maxColIndex;
	};
	int getMinIdxIndex()
	{
		return minIdxIndex;
	};
	int getMaxIdxIndex()
	{
		return maxIdxIndex;
	};
	int getColsRowsSize()
	{
		return colsRowsSize;
	};
	int getColsWindow()
	{
		return colsWindow;
	};
	int getIdxsRowsSize()
	{
		return idxsRowsSize;
	};
	int getIdxsWindow()
	{
		return idxsWindow;
	};

protected:
	//drawing
	virtual void basicDraw(wxBufferedDC &context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view);

private:
	//Main Rectangle Sizes
	wxhdMultiPosRect fullSizeRect, titleRect, titleColsRect, colsRect, titleIndxsRect, indxsRect;
	wxhdMultiPosRect unScrolledColsRect, unScrolledFullSizeRect, unScrolledTitleRect;

	//Rectangle item counters
	int colsRowsSize, colsWindow, idxsRowsSize, idxsWindow;

	//vector indexes
	int maxColIndex, minIdxIndex, maxIdxIndex;

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
	void calcInternalSubAreas(int posIdx);

	//pk uk(s)
	wxString pkName;
	wxArrayString ukNames;
};
#endif
