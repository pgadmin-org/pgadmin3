//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

gqbSchema::gqbSchema(gqbObject *parent, wxString name, type_gqbObject type=GQB_SCHEMA):
gqbObject(name,type)
{
    this->setType(GQB_SCHEMA);
    this->setName(name);
    this->setOwner(parent);
}


// GQB-TODO: don't declare OID inside gqbBrowsear instead use the pgadmin one
void gqbSchema::createObjects(gqbBrowser *_tablesBrowser,  pgConn *_conn, OID oidVal, wxTreeItemId parentNode, int _tableImage, int _viewImage)
{
    createTables(_conn, _tablesBrowser, parentNode, oidVal, _tableImage, _viewImage);
}


wxString gqbSchema::NumToStr(OID value)
{
    wxString result;
    result.Printf(wxT("%lu"), (long)value);
    return result;
}


void gqbSchema::createTables(pgConn *conn, gqbBrowser *tablesBrowser, wxTreeItemId parentNode, OID oidVal, int tableImage, int viewImage)
{
	
    wxString query;
    wxString restriction=wxT("");
	
	// First Add Tables;
    if (conn->BackendMinimumVersion(8, 0))
    {
        query= wxT("SELECT rel.oid, relname, rel.reltablespace AS spcoid, spcname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
            wxT("relhassubclass, reltuples, description, conname, conkey,\n")
            wxT("       EXISTS(select 1 FROM pg_trigger\n")
            wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
            wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
            wxT("                     WHERE tgrelid=rel.oid) AS isrepl\n");
        if (conn->BackendMinimumVersion(8, 2))
            query += wxT(", substring(array_to_string(reloptions, ',') from 'fillfactor=([0-9]*)') AS fillfactor \n");
        query += wxT("  FROM pg_class rel\n")
            wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=rel.reltablespace\n")
            wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0)\n")
            wxT("  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n")
            wxT(" WHERE relkind IN ('r','s','t') AND relnamespace = ") + NumToStr(oidVal) + wxT("\n")
            + restriction +
            wxT(" ORDER BY relname");
    }
    else
    {
        query= wxT("SELECT rel.oid, relname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
            wxT("relhassubclass, reltuples, description, conname, conkey,\n")
            wxT("       EXISTS(select 1 FROM pg_trigger\n")
            wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
            wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
            wxT("                     WHERE tgrelid=rel.oid) AS isrepl\n")
            wxT("  FROM pg_class rel\n")
            wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0)\n")
            wxT("  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n")
            wxT(" WHERE relkind IN ('r','s','t') AND relnamespace = ") + NumToStr(oidVal)  + wxT("\n")
            + restriction +
            wxT(" ORDER BY relname");
    }

    pgSet *tables = conn->ExecuteSet(query);
    wxTreeItemId parent;

    if (tables)
    {
        while (!tables->Eof())
        {
            wxString tmpname = wxString(tables->GetVal(wxT("relname")));
            gqbTable *table = new gqbTable(this, tmpname, GQB_TABLE);
            parent=tablesBrowser->AppendItem(parentNode, tables->GetVal(wxT("relname")) , tableImage, tableImage, table);

			// Create columns inside this table.
            table->createObjects(tablesBrowser, conn, tables->GetOid(wxT("oid")), parent);

            tables->MoveNext();
        }

        delete tables;
    }

	// Later Add Views;
	query=wxT("SELECT c.oid, c.xmin, c.relname, pg_get_userbyid(c.relowner) AS viewowner, c.relacl, description")
        wxT("  FROM pg_class c\n")
        wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid and des.objsubid=0)\n")
        wxT(" WHERE ((c.relhasrules AND (EXISTS (\n")
        wxT("           SELECT r.rulename FROM pg_rewrite r\n")
        wxT("            WHERE ((r.ev_class = c.oid)\n")
        wxT("              AND (bpchar(r.ev_type) = '1'::bpchar)) ))) OR (c.relkind = 'v'::char))\n")
        wxT("   AND relnamespace = ") + NumToStr(oidVal) + wxT("\n")
        + restriction
        + wxT(" ORDER BY relname");

    pgSet *views = conn->ExecuteSet(query);
	  if (views)
    {
        while (!views->Eof())
        {
            wxString tmpname = wxString(views->GetVal(wxT("relname")));
            gqbTable *table = new gqbTable(this, tmpname, GQB_VIEW);
            parent=tablesBrowser->AppendItem(parentNode, views->GetVal(wxT("relname")) , viewImage, viewImage, table);

			// Create columns inside this view.
            table->createObjects(tablesBrowser,conn,views->GetOid(wxT("oid")),parent);

            views->MoveNext();
			}
	  }

	  tablesBrowser->SortChildren(parentNode);
}
