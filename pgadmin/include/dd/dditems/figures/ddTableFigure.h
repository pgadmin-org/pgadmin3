//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableFigure.h - Draw table figure of a model
//
////////////////////////////////////////////////////////////////////////////

#ifndef DDTABLEFIGURE_H
#define DDTABLEFIGURE_H
#include "hotdraw/figures/hdCompositeFigure.h"
#include "hotdraw/figures/hdRectangleFigure.h"
#include "hotdraw/figures/hdSimpleTextFigure.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"
#include "dd/dditems/figures/ddColumnFigure.h"
#include "dd/ddmodel/ddDatabaseDesign.h"

class ddScrollBarHandle;
class ddRelationshipFigure;
class hdDrawing;

class ddTableFigure : public hdCompositeFigure
{
public:
	ddTableFigure(wxString tableName, int x, int y);
	ddTableFigure(wxString tableName, int posIdx, int x, int y);
	void InitTableValues(wxArrayString UniqueKeysName, wxString primaryKeyName, int bdc, int bdi, int maxcolsi, int minidxsi, int maxidxsi, int colsrs, int colsw, int idxsrs, int idxsw);
	void Init(wxString tableName, int x, int y);
	hdMultiPosRect &getBasicDisplayBox();
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
	void processDeleteAlert(hdDrawing *drawing);

	//columns scrolls
	void updateTableSize(bool notifyChange = true);
	void recalculateColsPos(int posIdx);
	void setColsRowsWindow(int num);
	hdMultiPosRect &getColsSpace();
	hdMultiPosRect &getFullSpace();
	hdMultiPosRect &getTitleRect();
	int getTotalColumns();
	int getColumnsWindow();
	int getTopColWindowIndex();
	void setColumnsWindow(int posIdx, int value, bool maximize = false);
	void columnsWindowUp(int posIdx);
	void columnsWindowDown(int posIdx);
	int getFiguresMaxWidth();
	int getColDefaultHeight(wxFont font);

	//metadata
	wxString getTableName();
	wxString getShortTableName();
	wxString generateSQLCreate(wxString schemaName);
	wxString generateSQLAlterPks(wxString schemaName);
	wxString generateSQLAlterFks(wxString schemaName);
	wxString generateSQLAlterUks(wxString schemaName);
	wxString generateAltersTable(pgConn *connection, wxString schemaName, ddDatabaseDesign *design);
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

	//Temporary alter table instead of create Helper
	bool getBelongsToSchema()
	{
		return belongsToSchema;
	};
	void setBelongsToSchema(bool value)
	{
		belongsToSchema = value;
	};

protected:
	//drawing
	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);

private:
	//Temporary alter table instead of create Helper
	bool belongsToSchema;

	//Main Rectangle Sizes
	hdMultiPosRect fullSizeRect, titleRect, titleColsRect, colsRect, titleIndxsRect, indxsRect;
	hdMultiPosRect unScrolledColsRect, unScrolledFullSizeRect, unScrolledTitleRect;

	//Rectangle item counters
	int colsRowsSize, colsWindow, idxsRowsSize, idxsWindow;

	//vector indexes
	int maxColIndex, minIdxIndex, maxIdxIndex;

	//position
	int beginDrawCols, beginDrawIdxs;

	//Default Figures
	hdRectangleFigure *rectangleFigure;
	ddTextTableItemFigure *tableTitle;

	//helper variables
	bool selectingFkDestination;
	int internalPadding, externalPadding;
	bool calcScrolled;

	//specials handles
	ddScrollBarHandle *scrollbar;

	//methods
	int getHeightFontMetric(wxString text, wxFont font);
	void calcInternalSubAreas(int posIdx);

	//pk uk(s)
	wxString pkName;
	wxArrayString ukNames;
};
#endif
