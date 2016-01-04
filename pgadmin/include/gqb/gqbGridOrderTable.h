//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridOrderTable.h - Table implementation for Order By Panel Grid
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBGRIDORDERTABLE_H
#define GQBGRIDORDERTABLE_H

#include <wx/grid.h>

// App headers
#include "gqb/gqbArrayCollection.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbModel.h"

class gqbGridOrderTable : public wxGridTableBase
{
public:
	gqbGridOrderTable(int numColumns, gqbObjsArray *cols, gqbObjsArray *parent, charArray *orderBy);
	~gqbGridOrderTable();
	int GetNumberRows();
	int GetNumberCols();
	bool IsEmptyCell( int row, int col );
	wxString GetValue( int row, int col );
	wxString GetColLabelValue( int col);
	void SetValue( int row, int col, const wxString &value );
	void AppendItem(gqbColumn *column, gqbQueryObject *parent, char kindOrder);
	void emptyTableData(gqbQueryObject *object);
	gqbObject *getObjectAt(int pos, int col);
	bool removeFirstRow(gqbObject *itemTable);
	void removeRowAt(int i);
	void changesRangeOnePos(int topPos, int bottomPos, int newTop);
	void changesPositions(int sPos, int dPos);

private:
	int numberColumns;					// GQB-TODO: replace this with grid cols number function if possible
	gqbObjsArray *columns;				// Here store position of the columns at Select projection clause
	// [Select c1,c2,c3...,cn from...]
	gqbObjsArray *colsParents;			// Because above array only store a column object cannot be recovered
	// the object that store it (gqbQueryObject) [remember can be use same
	// table twice on a query].
	charArray *kindOfOrder;				// A [Asc] D [Desc]
};
#endif
