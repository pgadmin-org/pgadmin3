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
	ddColumnFigure(wxString& columnName, ddTableFigure *owner, ddRelationshipItem *sourceFk=NULL);
    ~ddColumnFigure();
	virtual void basicMoveBy(int x, int y);
	virtual void moveTo(int x, int y);
	virtual void setOwnerTable(ddTableFigure *table);
	virtual bool containsPoint(int x, int y);
	virtual wxhdRect& getBasicDisplayBox();
	virtual void basicDraw(wxBufferedDC& context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC& context, wxhdDrawingView *view);
	virtual wxhdIFigure* findFigure(int x, int y);
	virtual wxhdIFigure* getFigureAt(int pos);
	virtual wxhdITool* CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool);
	virtual ddTableFigure* getOwnerTable();
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
	void toggleColumnKind(ddColumnType type, wxhdDrawingView *view=NULL);
	void setColumnOption(ddColumnOptionType type);
	void setRightIconForColumn();
	ddColumnOptionType getColumnOption();
	ddDataType getDataType();
	void setDataType(ddDataType type);
	wxString& getColumnName(bool datatype=false);
	void setColumnName(wxString name);
	bool isForeignKey();
	bool isGeneratedForeignKey();
	bool isUserCreatedForeignKey();
	bool isForeignKeyFromPk();
	wxString generateSQL();
	bool isFkNameGenerated();
	void activateGenFkName();
	void deactivateGenFkName();
	ddRelationshipItem* getFkSource();
	int getPrecision();
	int getScale();
	void setAsUserCreatedFk(ddRelationshipItem *relatedFkItem);
	ddRelationshipItem* getRelatedFkItem();
	bool validateColumn(wxString& errors);
	void setTextColour(wxColour colour);



protected:
	ddColumnKindIcon *leftImage;
	ddColumnOptionIcon *centerImage;
	ddTextTableItemFigure *columnText;
	ddTableFigure *ownerTable;
	ddRelationshipItem *usedAsFkDestFor;
	bool generateFkName;

private:
	ddRelationshipItem *fkSource;

	
};
#endif
