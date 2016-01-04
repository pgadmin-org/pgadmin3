//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridProjTable.cpp - Table implementation for Projection Panel Grid
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/utils.h>
#include <wx/notebook.h>
#include <wx/regex.h>

// App headers
#include "gqb/gqbGridProjTable.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbQueryObjs.h"

gqbGridProjTable::gqbGridProjTable(gqbObjsArray *position, gqbObjsArray *parent, wxArrayString *alias):
	wxGridTableBase()
{
	colsPosition = position;
	colsParents = parent;
	columnsAlias = alias;
	// GQB-TODO: replace above pointers array with local variable if possible or research what it's causing bug in destructor???
}


gqbGridProjTable::~gqbGridProjTable()
{
}


int gqbGridProjTable::GetNumberRows()
{
	return (colsPosition->GetCount());
}


int gqbGridProjTable::GetNumberCols()
{

	return 3;
}


bool gqbGridProjTable::IsEmptyCell( int row, int col )
{

	int count = colsParents->GetCount();
	if(row + 1 <= count)
		return false;
	else
		return true;
}


wxString gqbGridProjTable::GetValue( int row, int col )
{
	switch(col)
	{
		case 0:
			if(((gqbQueryObject *)colsParents->Item(row))->getAlias().length() > 0)
			{
				return ((gqbQueryObject *)colsParents->Item(row))->getAlias();
			}
			else
			{
				return ((gqbQueryObject *)colsParents->Item(row))->getName();
			}
			break;
		case 1:
			return ((gqbColumn *)colsPosition->Item(row))->getName();
			break;
		case 2:
			return columnsAlias->Item(row);
			break;
	};
	return wxT("");
}


wxString gqbGridProjTable::GetColLabelValue( int col)
{
	switch(col)
	{
		case 0:
			return _("Relation");
			break;
		case 1:
			return _("Column");
			break;
		case 2:
			return _("Alias");
			break;
	};
	return wxT("");
}


void gqbGridProjTable::SetValue( int row, int col, const wxString &value )
{
	// Do nothing on values that cannot be edited on this model [Column & Relation Name]
	switch(col)
	{
		case 2:
			columnsAlias->Item(row) = value;
			break;
	};
}

void *gqbGridProjTable::GetValueAsCustom( int row, int col, const wxString &typeName )
{
	switch(col)
	{
		case 0:
			return (void *)&colsParents->Item(row);
			break;
		case 1:
			return (void *)&colsPosition->Item(row);
			break;
		case 2:
			break;
	};
	return NULL;
}


void  gqbGridProjTable::SetValueAsCustom( int row, int col, const wxString &typeName, void *value )
{
	switch(col)
	{
		case 0:
			colsParents->Add(((gqbQueryObject *)value));
			break;
		case 1:
			colsPosition->Add(((gqbColumn *)value));
	};
}


void gqbGridProjTable::AppendItem(int col, gqbObject *item)
{
	bool notify = false;
	switch(col)
	{
		case 0:
			colsParents->Add(item);
			break;
		case 1:
			colsPosition->Add(item);
			notify = true;
			break;
		case 2:
			columnsAlias->Add(wxT(""));
			break;
	};

	if (notify && GetView() )
	{
		wxGridTableMessage msg( this,
		                        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
		                        (colsParents->GetCount() - 1),
		                        1 );
		GetView()->ProcessTableMessage( msg );

		// Set the cells read-only
		GetView()->SetReadOnly(GetView()->GetNumberRows() - 1, 0);
		GetView()->SetReadOnly(GetView()->GetNumberRows() - 1, 1);
	}

}

// Remove a column at the grid
bool gqbGridProjTable::removeRow(gqbObject *itemTable, gqbObject *itemColumn)
{
	bool found = false;
	int i, size = colsPosition->GetCount();

	for(i = 0; i < size; i++)
	{
		if (colsParents->Item(i) == itemTable && colsPosition->Item(i) == itemColumn)
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		colsParents->RemoveAt(i);
		colsPosition->RemoveAt(i);
		columnsAlias->RemoveAt(i);

		if ( GetView() )                          // Notify Grid about the change
		{
			wxGridTableMessage msg( this,
			                        wxGRIDTABLE_NOTIFY_ROWS_DELETED,
			                        i + 1,
			                        1 );
			GetView()->ProcessTableMessage( msg );
		}
	}

	return found;
}


void gqbGridProjTable::removeAllRows(gqbObject *itemTable)
{

	int size = colsParents->GetCount();
	for(int i = (size - 1); i >= 0; i--)
	{
		if (colsParents->Item(i) == itemTable)
		{
			colsParents->RemoveAt(i);
			colsPosition->RemoveAt(i);
			columnsAlias->RemoveAt(i);

			// Notify Grid about the change
			if ( GetView() )
			{
				wxGridTableMessage msg( this,
				                        wxGRIDTABLE_NOTIFY_ROWS_DELETED,
				                        i + 1,
				                        1 );
				GetView()->ProcessTableMessage( msg );
			}
		}
	}
}


void gqbGridProjTable::changesPositions(int sPos, int dPos)
{

	int size = colsPosition->GetCount();
	gqbObject *tmpTable = NULL, *tmpColumn = NULL;
	wxString tmpAlias = wxT("");

	if( (sPos >= 0 && sPos < size) && (dPos >= 0 && dPos < size) )
	{
		tmpTable = colsParents->Item(sPos);
		tmpColumn = colsPosition->Item(sPos);
		tmpAlias = columnsAlias->Item(sPos);

		colsParents->Item(sPos) = colsParents->Item(dPos);
		colsPosition->Item(sPos) = colsPosition->Item(dPos);
		columnsAlias->Item(sPos) = columnsAlias->Item(dPos);
		colsParents->Item(dPos) = tmpTable;
		colsPosition->Item(dPos) = tmpColumn;
		columnsAlias->Item(dPos) = tmpAlias;
	}

	wxGridTableMessage msg( this,
	                        wxGRIDTABLE_REQUEST_VIEW_GET_VALUES,
	                        sPos,
	                        1 );
	GetView()->ProcessTableMessage( msg );

}


// GQB-TODO: optimize this functions & related buttons events at gqbView because works but are a mess.
// Change a single row or a range to one pos up or down (but no more than one position)
void gqbGridProjTable::changesRangeOnePos(int topPos, int bottomPos, int newTop)
{
	// Eliminate side effect of zero base array on calculations, but careful newTop still it's zero based
	topPos++;
	bottomPos++;
	int sizeRange = bottomPos - (topPos - 1), size = GetNumberRows();
	if(topPos > newTop)                           // Go Down
	{
		// Only if the movement don't create an overflow
		if( (topPos > 1) && ((newTop + sizeRange) <  size)  )
		{
			for(int i = newTop ; i < (newTop + sizeRange) ; i++)
			{
				changesPositions(i, i + 1);
			}
		}

	}                                             // Go Up
	else
	{
		// Only if the movement don't create an overflow
		if( (bottomPos < size) && ((newTop + sizeRange) <=  size)  )
		{
			// Go Up Down
			for(int i = (newTop + sizeRange - 1) ; i >= newTop  ; i--)
			{
				changesPositions(i - 1, i);
			}
		}
	}
}


// Removes all items from gqbGridProjTable
void gqbGridProjTable::emptyTableData()
{

	int count = colsPosition->GetCount();
	colsPosition->Empty();
	colsParents->Empty();
	columnsAlias->Empty();

	// Notify Grid about the change
	if ( GetView() )
	{
		wxGridTableMessage msg( this,
		                        wxGRIDTABLE_NOTIFY_ROWS_DELETED,
		                        1,
		                        count);
		GetView()->ProcessTableMessage( msg );
	}
}

