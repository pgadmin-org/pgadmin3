//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
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

gqbDatabase::gqbDatabase(wxString name, type_gqbObject type=GQB_DATABASE):
gqbObject(name,type)
{
    this->setType(GQB_DATABASE);
    this->setName(name);
    this->setOwner(NULL);
    conn=NULL;
}


void gqbDatabase::createObjects(gqbBrowser *_tablesBrowser,  pgConn *_conn)
{

    wxString rootNodeString = wxString(_conn->GetDbname());
    // Create Root Node
    _tablesBrowser->createRoot(rootNodeString);

    // FillBrowser
    createSchemas(_conn,_tablesBrowser,_tablesBrowser->getCatalogRootNode(),GQB_CATALOG,5);
    createSchemas(_conn,_tablesBrowser,_tablesBrowser->getTablesRootNode(),GQB_OTHER,1);
}


// Use database connection to create all objects inside tree
void gqbDatabase::createSchemas(pgConn *conn,  gqbBrowser *tablesBrowser, wxTreeItemId parentNode,typeSchema MetaType, int indexImage)
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
        restr += wxT("  AND nspparent = 0\n");

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
            wxString name=schemas->GetVal(wxT("nspname"));
            long nsptyp=schemas->GetLong(wxT("nsptyp"));

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

            int tableImage = 2, viewImage = 7;
            gqbSchema *schema;

            if (MetaType == GQB_CATALOG)
            {

                // Create Schema Object
                schema = new gqbSchema(this, name, GQB_SCHEMA);
                parent=tablesBrowser->AppendItem(parentNode, name , indexImage, indexImage, schema);

                if(name != wxT("pg_catalog") && name != wxT("pgagent"))
				{
                    tableImage=5;
					viewImage=5;
				}
            }
            else
            {

                // Create Schema Object
                schema = new gqbSchema(this, name, GQB_SCHEMA);
                parent=tablesBrowser->AppendItem(parentNode, name , indexImage, indexImage, schema);
            }

            schema->createObjects(tablesBrowser, conn, schemas->GetOid(wxT("oid")), parent, tableImage, viewImage, 8);
            schemas->MoveNext();
        }

        delete schemas;
    }
}
