//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlResultGrid.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "debugger/ctlResultGrid.h"

IMPLEMENT_CLASS( ctlResultGrid, wxGrid )

////////////////////////////////////////////////////////////////////////////////
// ctlResultGrid constructor
//
//    We use a ctlResultGrid to display the result set from a query.  This class
//  is a minor extension of the wxGrid class.

ctlResultGrid::ctlResultGrid( wxWindow *parent, wxWindowID id )
	: wxGrid( parent, id )
{
	SetFont(settings->GetSystemFont());

	CreateGrid( 0, 0 );
}

////////////////////////////////////////////////////////////////////////////////
// fillGrid()
//
//    Given a result set handle, this function copies the values in that result
//  set into the grid.

void ctlResultGrid::FillResult(pgSet *set)
{
	// Clear out the old results (if any) and resize
	// grid to match the result set
	if( GetNumberRows())
		DeleteRows( 0, GetNumberRows());
	if( GetNumberCols())
		DeleteCols( 0, GetNumberCols());

	if (!set)
		return;

	int rowCount = set->NumRows();
	int colCount = set->NumCols();

	// If this PGresult represents a non-query command
	// (like an INSERT), there won't be any columns in
	// the result set - just return
	if( colCount == 0 )
		return;

	// Disable repaints to we don't flicker too much

	BeginBatch();

	AppendRows(rowCount);
	AppendCols(colCount);

	EnableEditing(false);

	// Copy the column names from the result set into the column headers
	int row = 0,
	    col;
	for(col = 0; col < colCount; ++col)
		SetColLabelValue(col, set->ColName(col));

	// Now copy each value from the result set into the grid
	while(!set->Eof())
	{
		for(col = 0; col < colCount; ++col)
		{
			if(set->IsNull(col))
				SetCellValue(row, col, wxT(""));
			else
				SetCellValue(row, col, set->GetVal(col));
		}
		row++;
		set->MoveNext();
	}

	// Resize each column to fit its content
	AutoSizeColumns(false);

	// Enable repaints
	EndBatch();
}
