//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
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
void gqbSchema::createObjects(gqbBrowser *tablesBrowser,  pgConn *conn, OID oidVal, wxTreeItemId parentNode, int tableImage, int viewImage, int xTableImage)
{
    createTables(conn, tablesBrowser, parentNode, oidVal, tableImage, viewImage, xTableImage);
}


wxString gqbSchema::NumToStr(OID value)
{
    wxString result;
    result.Printf(wxT("%lu"), (long)value);
    return result;
}


void gqbSchema::createTables(pgConn *conn, gqbBrowser *tablesBrowser, wxTreeItemId parentNode, OID oidVal, int tableImage, int viewImage, int xTableImage)
{ 
    wxString query;
    
    // Get the child objects.
    query = wxT("SELECT oid, relname, relkind\n")
            wxT("  FROM pg_class\n")
            wxT(" WHERE relkind IN ('r','v','x') AND relnamespace = ") + NumToStr(oidVal) + wxT(";");

    pgSet *tables = conn->ExecuteSet(query);
    wxTreeItemId parent;

    if (tables)
    {
        while (!tables->Eof())
        {
            gqbTable *table;
            wxString tmpname = tables->GetVal(wxT("relname"));
            wxString relkind = tables->GetVal(wxT("relkind"));
            
            if (relkind == wxT("r")) // Table
            {
                table = new gqbTable(this, tmpname, GQB_TABLE);
                parent=tablesBrowser->AppendItem(parentNode, tables->GetVal(wxT("relname")) , tableImage, tableImage, table);
            }
            else if (relkind == wxT("v"))
            {
                table = new gqbTable(this, tmpname, GQB_VIEW);
                parent=tablesBrowser->AppendItem(parentNode, tables->GetVal(wxT("relname")) , viewImage, viewImage, table);
            }
            else if (relkind == wxT("x"))  // Greenplum external table
            {
                table = new gqbTable(this, tmpname, GQB_TABLE);
                parent=tablesBrowser->AppendItem(parentNode, tables->GetVal(wxT("relname")) , xTableImage, xTableImage, table);
            }

            // Create columns inside this table.
            table->createObjects(tablesBrowser, conn, tables->GetOid(wxT("oid")), parent);
            tables->MoveNext();
        }

        delete tables;
    }

    tablesBrowser->SortChildren(parentNode);
}
