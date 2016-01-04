//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridOrderTable.cpp - Table implementation for Order By Panel Grid
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbGridOrderTable.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbArrayCollection.h"
#include "gqb/gqbModel.h"

gqbGridOrderTable::gqbGridOrderTable(int numColumns, gqbObjsArray *cols, gqbObjsArray *parent, charArray *orderBy)
{
	numberColumns = numColumns;
	columns = cols;
	colsParents = parent;
	kindOfOrder = orderBy;
}


gqbGridOrderTable::~gqbGridOrderTable()
{
}


int gqbGridOrderTable::GetNumberRows()
{
	return (columns->GetCount());
}


int gqbGridOrderTable::GetNumberCols()
{
	return numberColumns;
}


bool gqbGridOrderTable::IsEmptyCell( int row, int col )
{
	int count = columns->GetCount();
	if(row + 1 <= count)
		return false;
	else
		return true;
}


wxString gqbGridOrderTable::GetValue( int row, int col )
{
	if(col == 0)
	{
		wxString col = wxT("");
		if(((gqbQueryObject *)colsParents->Item(row))->getAlias().length() > 0)
		{
			col += ((gqbQueryObject *)colsParents->Item(row))->getAlias() + wxT(".");
		}
		else
		{
			col += ((gqbQueryObject *)colsParents->Item(row))->getName() + wxT(".");
		}
		col += ((gqbColumn *)columns->Item(row))->getName();
		return col;
	}

	if(numberColumns == 2)
	{
		if(col == 1)
		{
			wxString ord = wxT("");
			if(kindOfOrder->Item(row) == 'A')
				ord += wxT("ASC");
			else
				ord += wxT("DESC");

			return ord;
		}
	}
	return wxT("");
}


wxString gqbGridOrderTable::GetColLabelValue(int col)
{
	switch(col)
	{
		case 0:
			if(numberColumns == 2)
			{
				return _("Column");
			}
			else
			{
				return _("Available Columns");
			}
			break;
		case 1:
			return _("Order");
			break;
	};
	return wxT("");
}


void gqbGridOrderTable::SetValue( int row, int col, const wxString &value )
{
	if(col == 1 && numberColumns == 2)
	{
		if(value.Contains(wxT("ASC")))
		{
			kindOfOrder->Item(row) = 'A';
		}
		else
		{
			kindOfOrder->Item(row) = 'D';
		}
	}
}


void gqbGridOrderTable::AppendItem(gqbColumn *column, gqbQueryObject *parent, char kindOrder)
{
	columns->Add(column);
	colsParents->Add(parent);
	if(numberColumns == 2)
	{
		kindOfOrder->Add(kindOrder);
	}

	if (GetView() )
	{
		wxGridTableMessage msg( this,
		                        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
		                        (columns->GetCount() - 1),
		                        1 );
		GetView()->ProcessTableMessage( msg );
	}
}


bool gqbGridOrderTable::removeFirstRow(gqbObject *itemTable)
{
	bool found = false;
	int i, size = colsParents->GetCount();

	for(i = 0; i < size; i++)
	{
		if (colsParents->Item(i) == itemTable)
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		columns->RemoveAt(i);
		colsParents->RemoveAt(i);
		if(numberColumns == 2)
		{
			kindOfOrder->RemoveAt(i);
		}
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


void gqbGridOrderTable::emptyTableData(gqbQueryObject *object)
{
	// Because items positions on array changes when I delete one, I have to do the remove in this way
	while(removeFirstRow(object));
}


void gqbGridOrderTable::removeRowAt(int i)
{
	columns->RemoveAt(i);
	colsParents->RemoveAt(i);
	if(numberColumns == 2)
	{
		kindOfOrder->RemoveAt(i);
	}
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


gqbObject *gqbGridOrderTable::getObjectAt(int pos, int col)
{
	gqbObject *value = NULL;
	switch(col)
	{
		case 0:
			value = columns->Item(pos);
			break;
		case 1:
			value = colsParents->Item(pos);
			break;
	};
	return value;
}


void gqbGridOrderTable::changesPositions(int sPos, int dPos)
{

	int size = columns->GetCount();
	gqbObject *tmpTable = NULL, *tmpColumn = NULL;
	char tmpKind = 'N';

	if( (sPos >= 0 && sPos < size) && (dPos >= 0 && dPos < size) )
	{
		tmpTable = colsParents->Item(sPos);
		tmpColumn = columns->Item(sPos);
		tmpKind = kindOfOrder->Item(sPos);

		colsParents->Item(sPos) = colsParents->Item(dPos);
		columns->Item(sPos) = columns->Item(dPos);
		kindOfOrder->Item(sPos) = kindOfOrder->Item(dPos);
		colsParents->Item(dPos) = tmpTable;
		columns->Item(dPos) = tmpColumn;
		kindOfOrder->Item(dPos) = tmpKind;
	}

	wxGridTableMessage msg( this,
	                        wxGRIDTABLE_REQUEST_VIEW_GET_VALUES,
	                        sPos,
	                        1 );
	GetView()->ProcessTableMessage( msg );

}


// GQB-TODO: optimize this functions & related buttons events at gqbView because works but are a mess.
// Change a single row or a range to one pos up or down (but no more than one position)
void gqbGridOrderTable::changesRangeOnePos(int topPos, int bottomPos, int newTop)
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
