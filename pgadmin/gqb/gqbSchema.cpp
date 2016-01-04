//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbSchema.cpp - Schema object for GQB
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbSchema.h"
#include "gqb/gqbObject.h"
#include "gqb/gqbTable.h"
#include "gqb/gqbBrowser.h"

gqbSchema::gqbSchema(gqbObject *parent, wxString name, pgConn *connection, OID oid)
	: gqbObject(name, parent, connection, oid)
{
	setType(GQB_SCHEMA);
}


// GQB-TODO: don't declare OID inside gqbBrowsear instead use the pgadmin one
void gqbSchema::createObjects(gqbBrowser *tablesBrowser, OID oidVal, wxTreeItemId parentNode, int tableImage, int viewImage, int xTableImage)
{
	createTables(tablesBrowser, parentNode, oidVal, tableImage, viewImage, xTableImage);
}

void gqbSchema::createTables(gqbBrowser *tablesBrowser, wxTreeItemId parentNode, OID oidVal, int tableImage, int viewImage, int xTableImage)
{
	wxString query;

	// Get the child objects.
	query = wxT("SELECT oid, relname, relkind\n")
	        wxT("  FROM pg_class\n")
	        wxT(" WHERE relkind IN ('r','v','x','m') AND relnamespace = ") + NumToStr(oidVal) + wxT(";");

	pgSet *tables = conn->ExecuteSet(query);
	wxTreeItemId parent;

	if (tables)
	{
		while (!tables->Eof())
		{
			gqbTable *table = 0;
			wxString tmpname = tables->GetVal(wxT("relname"));
			wxString relkind = tables->GetVal(wxT("relkind"));

			if (relkind == wxT("r")) // Table
			{
				table = new gqbTable(this, tmpname, conn, GQB_TABLE, tables->GetOid(wxT("oid")));
				parent = tablesBrowser->AppendItem(parentNode, tables->GetVal(wxT("relname")) , tableImage, tableImage, table);
			}
			else if (relkind == wxT("v") || relkind == wxT("m"))
			{
				table = new gqbTable(this, tmpname, conn, GQB_VIEW, tables->GetOid(wxT("oid")));
				parent = tablesBrowser->AppendItem(parentNode, tables->GetVal(wxT("relname")) , viewImage, viewImage, table);
			}
			else if (relkind == wxT("x"))  // Greenplum external table
			{
				table = new gqbTable(this, tmpname, conn, GQB_TABLE, tables->GetOid(wxT("oid")));
				parent = tablesBrowser->AppendItem(parentNode, tables->GetVal(wxT("relname")), xTableImage, xTableImage, table);
			}

			// Create columns inside this table.
			if (table)
				table->createObjects(tablesBrowser, conn, tables->GetOid(wxT("oid")), parent);

			tables->MoveNext();
		}

		delete tables;
	}

	tablesBrowser->SortChildren(parentNode);
}
