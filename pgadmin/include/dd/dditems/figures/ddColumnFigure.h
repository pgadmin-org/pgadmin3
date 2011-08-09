//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnFigure.h - Minimal Composite Figure for a column of a table
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDCOLUMNFIGURE_H
#define DDCOLUMNFIGURE_H
#include "dd/wxhotdraw/figures/wxhdAttributeFigure.h"
#include "dd/dditems/figures/ddColumnKindIcon.h"
#include "dd/dditems/figures/ddColumnOptionIcon.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"

class ddTableFigure;
class ddRelationshipItem;

//Minimal overhead composite figure
class ddColumnFigure : public wxhdAttributeFigure
{
public:
	ddColumnFigure(wxString &columnName, ddTableFigure *owner, ddRelationshipItem *sourceFk = NULL);
	ddColumnFigure(wxString &columnName, ddTableFigure *owner, ddColumnOptionType option, bool isGenFk, bool isPkColumn, wxString colDataType, int p = -1, int s = -1, int ukIdx = -1, ddRelationshipItem *sourceFk = NULL, ddRelationshipItem *usedAsFkDestFor = NULL );
	void Init(wxString &columnName, ddTableFigure *owner, ddRelationshipItem *sourceFk = NULL);
	~ddColumnFigure();
	virtual void AddPosForNewDiagram();
	virtual void RemovePosOfDiagram(int posIdx);
	virtual void basicMoveBy(int posIdx, int x, int y);
	virtual void moveTo(int posIdx, int x, int y);
	virtual void setOwnerTable(ddTableFigure *table);
	virtual bool containsPoint(int posIdx, int x, int y);
	virtual wxhdMultiPosRect &getBasicDisplayBox();
	virtual void basicDraw(wxBufferedDC &context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view);
	virtual wxhdIFigure *findFigure(int posIdx, int x, int y);
	virtual wxhdIFigure *getFigureAt(int pos);
	virtual wxhdITool *CreateFigureTool(wxhdDrawingView *view, wxhdITool *defaultTool);
	virtual ddTableFigure *getOwnerTable();
	void displayBoxUpdate();
	bool isNull();
	bool isNotNull();
	bool isNone();
	bool isPrimaryKey();
	void disablePrimaryKey();
	void enablePrimaryKey();
	bool isUniqueKey();
	bool isUniqueKey(int uniqueIndex);
	int getUniqueConstraintIndex();
	void setUniqueConstraintIndex(int i);
	bool isPlain();
	void setColumnKindToNone();
	void toggleColumnKind(ddColumnType type, wxhdDrawingView *view = NULL);
	void setColumnOption(ddColumnOptionType type);
	void setRightIconForColumn();
	ddColumnOptionType getColumnOption();
	ddDataType getDataType();
	void setDataType(ddDataType type);
	wxString &getColumnName(bool datatype = false);
	void setColumnName(wxString name);
	bool isForeignKey();
	bool isGeneratedForeignKey();
	bool isUserCreatedForeignKey();
	bool isForeignKeyFromPk();
	wxString generateSQL();
	bool isFkNameGenerated();
	void activateGenFkName();
	void deactivateGenFkName();
	ddRelationshipItem *getFkSource();
	void setFkSource(ddRelationshipItem *newColumn);
	int getPrecision();
	int getScale();
	void setAsUserCreatedFk(ddRelationshipItem *relatedFkItem);
	ddRelationshipItem *getRelatedFkItem();
	bool validateColumn(wxString &errors);
	void setTextColour(wxColour colour);
	ddColumnKindIcon *getKindImage()
	{
		return kindImage;
	};
	ddColumnOptionIcon *getOptionImage()
	{
		return optionImage;
	};
	ddTextTableItemFigure *getColumnText()
	{
		return columnText;
	};

protected:
	ddColumnKindIcon *kindImage;
	ddColumnOptionIcon *optionImage;
	ddTextTableItemFigure *columnText;
	ddTableFigure *ownerTable;
	ddRelationshipItem *usedAsFkDestFor;
	bool generateFkName;

private:
	ddRelationshipItem *fkSource;


};
#endif
