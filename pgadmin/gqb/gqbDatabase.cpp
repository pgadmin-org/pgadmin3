//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbDatabase.cpp - Database object for GQB.
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/regex.h>

// App headers
#include "gqb/gqbDatabase.h"
#include "gqb/gqbObject.h"
#include "gqb/gqbSchema.h"
#include "schema/pgSchema.h"

gqbDatabase::gqbDatabase(wxString name, pgConn *connection)
	: gqbObject(name, NULL, connection)
{
	setType(GQB_DATABASE);
}


void gqbDatabase::createObjects(gqbBrowser *_tablesBrowser)
{

	wxString rootNodeString = wxString(conn->GetDbname());
	// Create Root Node
	_tablesBrowser->createRoot(rootNodeString);

	// FillBrowser
	createSchemas(_tablesBrowser, _tablesBrowser->getCatalogRootNode(), GQB_CATALOG, GQB_IMG_CATALOG);
	createSchemas(_tablesBrowser, _tablesBrowser->getTablesRootNode(), GQB_OTHER, GQB_IMG_NAMESPACE);
}


// Use database connection to create all objects inside tree
void gqbDatabase::createSchemas(gqbBrowser *tablesBrowser, wxTreeItemId parentNode, typeSchema MetaType, int indexImage)
{

	// Search Schemas and insert it
	wxString restr =  wxT(" WHERE ");

	if (MetaType != GQB_CATALOG)
	{
		restr += wxT("NOT ");
	}
	restr += wxT("((nspname = 'pg_catalog' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pg_class' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'pgagent' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pga_job' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'information_schema' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'tables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname LIKE '_%' AND EXISTS (SELECT 1 FROM pg_proc WHERE proname='slonyversion' AND pronamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'dbo' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'systables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'sys' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'all_tables' AND relnamespace = nsp.oid LIMIT 1)))\n");

	if (conn->EdbMinimumVersion(8, 2))
	{
		restr += wxT("  AND nsp.nspparent = 0\n");
		// Do not show dbms_job_procedure in schemas
		if (!settings->GetShowSystemObjects())
			restr += wxT("AND NOT (nspname = 'dbms_job_procedure' AND EXISTS(SELECT 1 FROM pg_proc WHERE pronamespace = nsp.oid and proname = 'run_job' LIMIT 1))\n");
	}

	wxString sql;

	if (MetaType == GQB_CATALOG)
	{
		sql = wxT("SELECT 2 AS nsptyp, nspname, nsp.oid")
		      wxT("  FROM pg_namespace nsp\n")
		      + restr +
		      wxT(" ORDER BY 1, nspname");
	}
	else
	{
		if (conn->BackendMinimumVersion(8, 1))
		{
			sql = wxT("SELECT CASE WHEN nspname LIKE E'pg\\\\_temp\\\\_%' THEN 1\n")
			      wxT("            WHEN (nspname LIKE E'pg\\\\_%') THEN 0\n");
		}
		else
		{
			sql = wxT("SELECT CASE WHEN nspname LIKE 'pg\\\\_temp\\\\_%' THEN 1\n")
			      wxT("            WHEN (nspname LIKE 'pg\\\\_%') THEN 0\n");
		}
		sql += wxT("            ELSE 3 END AS nsptyp, nspname, nsp.oid\n")
		       wxT("  FROM pg_namespace nsp\n")
		       + restr +
		       wxT(" ORDER BY 1, nspname");
	}

	pgSet *schemas = conn->ExecuteSet(sql);
	wxTreeItemId parent;

	if (schemas)
	{
		while (!schemas->Eof())
		{
			wxString name = schemas->GetVal(wxT("nspname"));
			long nsptyp = schemas->GetLong(wxT("nsptyp"));

			wxStringTokenizer tokens(settings->GetSystemSchemas(), wxT(","));
			while (tokens.HasMoreTokens())
			{
				wxRegEx regex(tokens.GetNextToken());
				if (regex.Matches(name))
				{
					nsptyp = SCHEMATYP_USERSYS;
					break;
				}
			}

			if (nsptyp <= SCHEMATYP_USERSYS && MetaType != GQB_CATALOG && !settings->GetShowSystemObjects())
			{
				schemas->MoveNext();
				continue;
			}

			int tableImage = GQB_IMG_TABLE, viewImage = GQB_IMG_VIEW;
			gqbSchema *schema;

			if (MetaType == GQB_CATALOG)
			{

				// Create Schema Object
				schema = new gqbSchema(this, name, conn, schemas->GetOid(wxT("oid")));
				parent = tablesBrowser->AppendItem(parentNode, name, indexImage, indexImage, schema);
				schema->createObjects(tablesBrowser, schema->getOid(), parent, GQB_IMG_TABLE, GQB_IMG_VIEW, GQB_IMG_EXTTABLE);

				if(name != wxT("pg_catalog") && name != wxT("pgagent"))
				{
					tableImage = GQB_IMG_CATALOG_OBJ;
					viewImage = GQB_IMG_CATALOG_OBJ;
				}
			}
			else
			{

				// Create Schema Object
				// Note that the schema will be populated when the node is expanded.
				schema = new gqbSchema(this, name, conn, schemas->GetOid(wxT("oid")));
				parent = tablesBrowser->AppendItem(parentNode, name , indexImage, indexImage, schema);
				schema->createObjects(tablesBrowser, schema->getOid(), parent, GQB_IMG_TABLE, GQB_IMG_VIEW, GQB_IMG_EXTTABLE);
			}

			schemas->MoveNext();
		}

		delete schemas;
	}
}
