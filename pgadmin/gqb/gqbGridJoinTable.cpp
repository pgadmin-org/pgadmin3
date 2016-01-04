//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridJoinTable.cpp - Table implementation for Joins Panel Grid
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
#include "gqb/gqbGridJoinTable.h"
#include "gqb/gqbGridRestTable.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbViewController.h"

gqbGridJoinTable::gqbGridJoinTable(gqbController *_controller):
	wxGridTableBase()
{
	controller = _controller;
}


gqbGridJoinTable::~gqbGridJoinTable()
{
	emptyTableData();
}


int gqbGridJoinTable::GetNumberRows()
{
	return joins.count();
}


int gqbGridJoinTable::GetNumberCols()
{
	return 3;
}


bool gqbGridJoinTable::IsEmptyCell( int row, int col )
{
	if (joins.count() == 0)
		return true;
	return false;
}


wxString gqbGridJoinTable::GetValue( int row, int col )
{
	if (row >= joins.count())
		return wxEmptyString;
	gqbQueryJoin *obj = (gqbQueryJoin *)joins.getItemAt(row);

	switch(col)
	{
		case 0:
		{
			gqbQueryObject *srcTbl = obj->getSourceQTable();
			wxString sStr = srcTbl ? (srcTbl->getAlias().IsEmpty() ? qtIdent(srcTbl->getName()) : qtIdent(srcTbl->getAlias())) : wxString(wxEmptyString);
			if ( !sStr.IsEmpty() )
				sStr += wxT(".") + qtIdent(obj->getSourceCol());
			return sStr;
		}
		case 1:
			switch(obj->getKindofJoin())
			{
				case _equally:
					return wxT("=");
				case _lesser:
					return wxT("<");
				case _greater:
					return wxT(">");
				case _equlesser:
					return wxT("<=");
				case _equgreater:
					return wxT(">=");
			}
			return wxEmptyString;
		case 2:
		{
			gqbQueryObject *destTbl = obj->getDestQTable();
			wxString dStr = destTbl ? (destTbl->getAlias().IsEmpty() ? qtIdent(destTbl->getName()) : qtIdent(destTbl->getAlias())) : wxString(wxEmptyString);
			if ( !dStr.IsEmpty() )
				dStr += wxT(".") + obj->getDestCol();
			return dStr;
		}
		break;
	};
	return wxT("");
}


void gqbGridJoinTable::SetValue( int row, int col, const wxString &value )
{
	if (col == 1)
	{
		gqbQueryJoin *join = (gqbQueryJoin *)joins.getItemAt(row);
		if (value == wxT("="))
			join->setKindofJoin(_equally);
		else if (value == wxT("<"))
			join->setKindofJoin(_lesser);
		else if (value == wxT(">"))
			join->setKindofJoin(_greater);
		else if (value == wxT("<="))
			join->setKindofJoin(_equlesser);
		else if (value == wxT(">="))
			join->setKindofJoin(_equgreater);
		controller->getView()->Refresh();
	}
}

bool gqbGridJoinTable::ReplaceJoin( gqbQueryJoin *orig, gqbQueryJoin *newVal )
{
	int rowCount = joins.count();
	for ( int index = 0; index < rowCount; index++ )
	{
		if ( joins[ index ] == orig )
		{
			joins[ index ] = newVal;
			return true;
		}
	}
	return false;
}

gqbQueryJoin *gqbGridJoinTable::GetJoin( int row )
{
	if (row >= joins.count())
		return NULL;
	return (gqbQueryJoin *)joins.getItemAt(row);
}


void gqbGridJoinTable::removeJoin(gqbQueryJoin *item)
{
	if (item == NULL || this->joins.count() == 0)
		return;
	int index = joins.getIndex( item );
	if (index == -1)
		return;
	joins.removeItem( item );
	if (GetView())
	{
		wxGridTableMessage msg( this,
		                        wxGRIDTABLE_NOTIFY_ROWS_DELETED,
		                        index + 1,
		                        1 );
		GetView()->ProcessTableMessage( msg );
	}
}

void gqbGridJoinTable::removeJoins(gqbQueryObject *obj)
{
	if (!obj)
		return;

	if (obj->getHaveJoins())
	{
		gqbIteratorBase *itrJoins = obj->createJoinsIterator();
		if (itrJoins)
		{
			while (itrJoins->HasNext())
			{
				gqbQueryJoin *tmp = (gqbQueryJoin *)itrJoins->Next();
				removeJoin(tmp);
			}
		}
	}

	if (obj->getHaveRegJoins())
	{
		gqbIteratorBase *itrRegJoins = obj->createRegJoinsIterator();
		if (itrRegJoins)
		{
			while (itrRegJoins->HasNext())
			{
				gqbQueryJoin *tmp = (gqbQueryJoin *)itrRegJoins->Next();
				removeJoin(tmp);
			}
		}
	}
}

void gqbGridJoinTable::AppendJoin(gqbQueryJoin *item)
{
	bool notify = true;
	if ( item == NULL )
	{
		item = new gqbQueryJoin(NULL, NULL, NULL, NULL, _equally);
	}
	joins.addItem( item );
	if (notify && GetView() )
	{
		wxGridTableMessage msg( this,
		                        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
		                        joins.count(),
		                        1 );
		GetView()->ProcessTableMessage( msg );

		int row = GetView()->GetNumberRows() - 1;

		wxString strChoices[] = {wxT("="), wxT("<"), wxT("<="), wxT(">"), wxT(">=")};

		GetView()->SetCellRenderer(row, 0, new wxGridCellButtonRenderer);
		GetView()->SetCellRenderer(row, 1, new wxGridCellComboBoxRenderer);
		GetView()->SetCellEditor(row, 1, new dxGridCellSizedChoiceEditor(WXSIZEOF(strChoices), strChoices));
		GetView()->SetCellRenderer(row, 2, new wxGridCellButtonRenderer);

		GetView()->SetReadOnly( row, 0 );
		GetView()->SetReadOnly( row, 2 );
	}
}


// Removes all items from gqbGridJoinTable
void gqbGridJoinTable::emptyTableData()
{
	for (int index = joins.count() - 1; index >= 0; index--)
	{
		gqbQueryJoin *join = (gqbQueryJoin *)joins[index];
		joins.removeItem(join);

		// Join with either source or destination not present needs to be removed here
		if (!(join->getSourceQTable() && join->getDestQTable()))
		{
			delete join;
			join = NULL;
		}
	}
}

gqbQueryObject *gqbGridJoinTable::DeleteRow(size_t pos)
{
	gqbQueryJoin *join = (gqbQueryJoin *)joins.getItemAt(pos);
	gqbQueryObject *srcTbl = NULL;
	if (join)
	{
		this->removeJoin(join);
		if (join->getSourceQTable() && join->getDestQTable())
		{
			srcTbl = join->getSourceQTable();
			srcTbl->removeJoin(join, true);
		}
	}

	return srcTbl;
}

wxString gqbGridJoinTable::GetColLabelValue(int col)
{
	switch(col)
	{
		case 0:
			return _("Source Column");
		case 1:
			return _("Join Type");
		case 2:
			return _("Destination Column");
	}
	return wxEmptyString;
}


void gqbGridJoinTable::selectJoin(gqbQueryJoin *join)
{
	int indexRow = joins.getIndex(join);
	if (indexRow != -1 && GetView())
	{
		GetView()->ClearSelection();
		GetView()->SelectRow(indexRow);
	}
}

