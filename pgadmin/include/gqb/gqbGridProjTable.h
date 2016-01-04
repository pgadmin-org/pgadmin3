//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridProjTable.h - Table implementation for Projection Panel Grid
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBGRIDPROJTABLE_H
#define GQBGRIDPROJTABLE_H

#include <wx/grid.h>

// App headers
#include "gqb/gqbModel.h"
#include "gqb/gqbArrayCollection.h"

// GQB-TODO: don't use gqbObjsArray, use a new one in the model because violating MVC Pattern

// GQB-TODO: this is not needed the one in gqbArrayCollections works her ?????
// WX_DEFINE_ARRAY_PTR(gqbObject *, gqbObjsArray); this is not

// Create the Data Model that will be used by wxGrid Component
class gqbGridProjTable : public wxGridTableBase
{
public:
	gqbGridProjTable(gqbObjsArray *position, gqbObjsArray *parent, wxArrayString *alias);
	virtual ~gqbGridProjTable();
	int GetNumberRows();
	int GetNumberCols();
	bool IsEmptyCell( int row, int col );
	wxString GetValue( int row, int col );
	void SetValue( int row, int col, const wxString &value );
	void *GetValueAsCustom( int row, int col, const wxString &typeName );
	void  SetValueAsCustom( int row, int col, const wxString &typeName, void *value );
	wxString GetColLabelValue( int col);
	bool removeRow(gqbObject *itemTable, gqbObject *itemColumn);
	void removeAllRows(gqbObject *itemTable);
	void changesPositions(int spos, int dpos);
	void changesRangeOnePos(int topPos, int bottomPos, int newTop);
	void AppendItem(int col, gqbObject *item);
	void emptyTableData();

private:
	gqbObjsArray *colsPosition;       // Here store position of the columns at Select projection clause
	// [Select c1,c2,c3...,cn from...]
	gqbObjsArray *colsParents;        // Because above array only store a column object cannot be recovered
	// the object that store it (gqbQueryObject) [remember can be use
	// same table twice on a query].
	wxArrayString *columnsAlias;      // GQB-TODO: find a better solution than this
};
#endif
