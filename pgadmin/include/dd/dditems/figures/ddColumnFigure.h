//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnFigure.h - Minimal Composite Figure for a column of a table
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDCOLUMNFIGURE_H
#define DDCOLUMNFIGURE_H
#include "hotdraw/figures/hdAttributeFigure.h"
#include "dd/dditems/figures/ddColumnKindIcon.h"
#include "dd/dditems/figures/ddColumnOptionIcon.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"

class ddTableFigure;
class ddRelationshipItem;

//Minimal overhead composite figure
class ddColumnFigure : public hdAttributeFigure
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
	virtual hdMultiPosRect &getBasicDisplayBox();
	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	virtual hdIFigure *findFigure(int posIdx, int x, int y);
	virtual hdIFigure *getFigureAt(int pos);
	virtual hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);
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
	void toggleColumnKind(ddColumnType type, hdDrawingView *view = NULL);
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
	wxString generateSQL(bool forAlterColumn = false);
	bool isFkNameGenerated();
	void activateGenFkName();
	void deactivateGenFkName();
	ddRelationshipItem *getFkSource();
	void setFkSource(ddRelationshipItem *newColumn);
	int getPrecision();
	void setPrecision(int n);
	int getScale();
	void setScale(int n);
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

	/*	Disable right now, it can be useful at the future when db designer will be improved again
		int getPgAttNumCol() { return pgAttNumColNumber;};
		void setPgAttNumCol(int attnum) { pgAttNumColNumber = attnum; };
	*/
	wxString getRawDataType();

protected:
	ddColumnKindIcon *kindImage;
	ddColumnOptionIcon *optionImage;
	ddTextTableItemFigure *columnText;
	ddTableFigure *ownerTable;
	ddRelationshipItem *usedAsFkDestFor;
	bool generateFkName;
	int pgAttNumColNumber;

private:
	ddRelationshipItem *fkSource;


};
#endif
