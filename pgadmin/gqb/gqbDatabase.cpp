//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

gqbDatabase::gqbDatabase(wxString name, type_gqbObject type=_gqbDatabase):
gqbObject(name,type)
{
    this->setType(_gqbDatabase);
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
    restr += wxT("((nspname = 'pg_catalog' and (SELECT count(*) FROM pg_class WHERE relname = 'pg_class' AND relnamespace = nsp.oid) > 0) OR\n");
    restr += wxT("(nspname = 'pgagent' and (SELECT count(*) FROM pg_class WHERE relname = 'pga_job' AND relnamespace = nsp.oid) > 0) OR\n");
    restr += wxT("(nspname = 'information_schema' and (SELECT count(*) FROM pg_class WHERE relname = 'tables' AND relnamespace = nsp.oid) > 0) OR\n");
    restr += wxT("(nspname LIKE '_%' and (SELECT count(*) FROM pg_proc WHERE proname='slonyversion' AND pronamespace = nsp.oid) > 0) OR\n");
    restr += wxT("(nspname = 'dbo' and (SELECT count(*) FROM pg_class WHERE relname = 'systables' AND relnamespace = nsp.oid) > 0) OR\n");
    restr += wxT("(nspname = 'sys' and (SELECT count(*) FROM pg_class WHERE relname = 'all_tables' AND relnamespace = nsp.oid) > 0)) AND nspname!='information_schema'\n");

    if (conn->EdbMinimumVersion(8, 2))
        restr += wxT("  AND nsp.nspparent = 0\n");

    wxString sql;

    if (MetaType == GQB_CATALOG)
    {
        sql = wxT("SELECT 2 AS nsptyp,\n")
            wxT("       nsp.nspname, nsp.oid, pg_get_userbyid(nspowner) AS namespaceowner, nspacl, description,")
            wxT("       FALSE as cancreate\n")
            wxT("  FROM pg_namespace nsp\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
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
        sql += wxT("            ELSE 3 END AS nsptyp,\n")
            wxT("       nsp.nspname, nsp.oid, pg_get_userbyid(nspowner) AS namespaceowner, nspacl, description,")
            wxT("       has_schema_privilege(nsp.oid, 'CREATE') as cancreate\n")
            wxT("  FROM pg_namespace nsp\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
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

            if (MetaType == PGM_CATALOG)
            {

                // Create Schema Object
                wxString tmpname = wxString(name);
                gqbSchema *schema = new gqbSchema(this, tmpname, _gqbSchema);
                parent=tablesBrowser->AppendItem(parentNode, name , indexImage, indexImage,schema);
                schema->createObjects(tablesBrowser, conn, schemas->GetOid(wxT("oid")), parent, 5, 5);
                schemas->MoveNext();
            }
            else
            {

                // Create Schema Object
                wxString tmpname= wxString(name);
                gqbSchema *schema = new gqbSchema(this, tmpname, _gqbSchema);
                parent=tablesBrowser->AppendItem(parentNode, name , indexImage, indexImage,schema);
                int tableImage=-1, viewImage=-1;

                //GQB-TODO: temporary fix replace this with a better one option
                if(schema->getName().Contains(wxT("pg_catalog")))
				{
                    tableImage=6;
					viewImage=6;
				}
                else
				{
                    tableImage=2;
					viewImage=7;
				}

                //Create tables inside this schema.
                schema->createObjects(tablesBrowser, conn, schemas->GetOid(wxT("oid")), parent, tableImage, viewImage);

                schemas->MoveNext();
            }
        }

        delete schemas;
    }
}
