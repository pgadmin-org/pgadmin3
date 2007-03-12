//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsresultgrid.cpp 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsresultgrid.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsResultGrid.h"

IMPLEMENT_CLASS( wsResultGrid, wxGrid )

////////////////////////////////////////////////////////////////////////////////
// wsResultGrid constructor
//
//	We use a wsResultGrid to display the result set from a query.  This class 
//  is a minor extension of the wxGrid class.

wsResultGrid::wsResultGrid( wxWindow * parent, wxWindowID id )
  : wxGrid( parent, id )
{
    CreateGrid( 0, 0 );
}

////////////////////////////////////////////////////////////////////////////////
// fillGrid()
//
//	Given a result set handle, this function copies the values in that result 
//  set into the grid.

void wsResultGrid::fillGrid( PGresult * result )
{
    int	rowCount = PQntuples( result );
    int	colCount = PQnfields( result );

	// If this PGresult represents a non-query command 
	// (like an INSERT), there won't be any columns in 
	// the result set - just return

    if( colCount == 0 )
		return;

	// Disable repaints to we don't flicker too much

    BeginBatch();

	// Clear out the old results (if any) and resize 
	// grid to match the result set

	if( GetNumberRows())
		DeleteRows( 0, GetNumberRows());
	if( GetNumberCols())
		DeleteCols( 0, GetNumberCols());

    AppendRows( rowCount );
    AppendCols( colCount );

    EnableEditing( false );

	// Copy the column names from the result set into the column headers

    for( int col = 0; col < colCount; ++col )
		SetColLabelValue( col, wxString( PQfname( result, col ), wxConvUTF8 ));

	// Now copy each value from the result set into the grid

    for( int row = 0; row < rowCount; ++row )
    {
		for( int col = 0; col < colCount; ++col )
		{
			if( PQgetisnull( result, row, col ))
				SetCellValue( row, col, wxT( "" ));
			else
				SetCellValue( row, col, wxString( PQgetvalue( result, row, col ), wxConvUTF8 ));
		}
    }

	// Resize each column to fit its content

    AutoSizeColumns( false );

	// Enable repaints

    EndBatch();
}
