//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbTable.cpp - Table object for GQB
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbObject.h"
#include "gqb/gqbTable.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbArrayCollection.h"

gqbTable::gqbTable(gqbObject *parent, wxString name, pgConn *connection, type_gqbObject type, OID oid)
	: gqbObjectCollection(name, parent, connection, oid)
{
	setType(type);
}


gqbIteratorBase *gqbTable::createColumnsIterator()
{
	return createIterator();
}


void gqbTable::addColumn(gqbColumn *column)
{
	this->addObject(column);
}


void gqbTable::createObjects(gqbBrowser *_tablesBrowser,  pgConn *_conn, OID oidVal,  wxTreeItemId parentNode)
{
	createColumns(_conn, _tablesBrowser, parentNode, oidVal);
}

void gqbTable::createColumns(pgConn *conn, gqbBrowser *tablesBrowser, wxTreeItemId parentNode,  OID oidVal)
{

	wxString systemRestriction;
	if (!settings->GetShowSystemObjects())
		systemRestriction = wxT("\n   AND attnum > 0");

	wxString sql =
	    wxT("SELECT attname FROM pg_attribute att\n")
	    wxT(" WHERE attrelid = ")
	    + NumToStr(oidVal)
	    + systemRestriction + wxT("\n")
	    wxT("   AND attisdropped IS FALSE\n")
	    wxT(" ORDER BY attnum");

	pgSet *columns = conn->ExecuteSet(sql);
	if (columns)
	{
		while (!columns->Eof())
		{
			if (tablesBrowser)
			{
				//Disable, Column SHOULDN'T be added to tree only use for debug purposes tablesBrowser->AppendItem(parentNode, columns->GetVal(wxT("attname")) , -1, -1);
				wxString tmpname = wxString(columns->GetVal(wxT("attname")));
				gqbColumn *column = new gqbColumn(this, tmpname, conn);
				this->addColumn(column);
				columns->MoveNext();
			}
			else
				break;
		}

		delete columns;
	}
}


//work as a synonym for function
int gqbTable::countCols()
{
	return this->countObjects();
}


//work as a synonym for function & return correct type
gqbColumn *gqbTable::getColumnAtIndex(int index)
{
	return (gqbColumn *)this->getObjectAtIndex(index);
}


int gqbTable::indexColumn(gqbColumn *col)
{
	return this->indexObject(col);
}
