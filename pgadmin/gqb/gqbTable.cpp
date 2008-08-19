//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

gqbTable::gqbTable(gqbObject *parent, wxString name, type_gqbObject type=_gqbTable):
gqbObjectCollection(name,type)
{
    this->setType(_gqbTable);
    this->setName(name);
    this->setOwner(parent);
}


gqbIteratorBase* gqbTable::createColumnsIterator()
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


wxString gqbTable::NumToStr(OID value)
{
    wxString result;
    result.Printf(wxT("%lu"), (long)value);
    return result;
}


void gqbTable::createColumns(pgConn *conn, gqbBrowser *tablesBrowser, wxTreeItemId parentNode,  OID oidVal)
{

    wxString systemRestriction;
    if (!settings->GetShowSystemObjects())
        systemRestriction = wxT("\n   AND attnum > 0");

    wxString sql=
        wxT("SELECT att.*, def.*, pg_catalog.pg_get_expr(def.adbin, def.adrelid) AS defval, CASE WHEN attndims > 0 THEN 1 ELSE 0 END AS isarray, format_type(ty.oid,NULL) AS typname, tn.nspname as typnspname, et.typname as elemtypname,\n")
        wxT("  cl.relname, na.nspname, att.attstattarget, description, cs.relname AS sername, ns.nspname AS serschema,\n")
        wxT("  (SELECT count(1) FROM pg_type t2 WHERE t2.typname=ty.typname) > 1 AS isdup, indkey");

    if (conn->BackendMinimumVersion(7, 4))
        sql +=
            wxT(",\n")
            wxT("  EXISTS(SELECT 1 FROM  pg_constraint WHERE conrelid=att.attrelid AND contype='f'")
            wxT(" AND att.attnum=ANY(conkey)) As isfk");

    sql += wxT("\n")
        wxT("  FROM pg_attribute att\n")
        wxT("  JOIN pg_type ty ON ty.oid=atttypid\n")
        wxT("  JOIN pg_namespace tn ON tn.oid=ty.typnamespace\n")
        wxT("  JOIN pg_class cl ON cl.oid=attrelid\n")
        wxT("  JOIN pg_namespace na ON na.oid=cl.relnamespace\n")
        wxT("  LEFT OUTER JOIN pg_type et ON et.oid=ty.typelem\n")
        wxT("  LEFT OUTER JOIN pg_attrdef def ON adrelid=attrelid AND adnum=attnum\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=attrelid AND des.objsubid=attnum\n")
        wxT("  LEFT OUTER JOIN (pg_depend JOIN pg_class cs ON objid=cs.oid AND cs.relkind='S') ON refobjid=attrelid AND refobjsubid=attnum\n")
        wxT("  LEFT OUTER JOIN pg_namespace ns ON ns.oid=cs.relnamespace\n")
        wxT("  LEFT OUTER JOIN pg_index pi ON pi.indrelid=attrelid AND indisprimary\n")
        wxT(" WHERE attrelid = ")
        + NumToStr(oidVal)
        + systemRestriction + wxT("\n")
        wxT("   AND attisdropped IS FALSE\n")
        wxT(" ORDER BY attnum");

    pgSet *columns= conn->ExecuteSet(sql);
    if (columns)
    {
        while (!columns->Eof())
        {
            if (tablesBrowser)
            {
                //Disable, Column SHOULDN'T be added to tree only use for debug purposes tablesBrowser->AppendItem(parentNode, columns->GetVal(wxT("attname")) , -1, -1);
                wxString tmpname = wxString(columns->GetVal(wxT("attname")));
                gqbColumn *column = new gqbColumn(this,tmpname,_gqbColumn);
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
gqbColumn* gqbTable::getColumnAtIndex(int index)
{
    return (gqbColumn *)this->getObjectAtIndex(index);
}


int gqbTable::indexColumn(gqbColumn *col)
{
    return this->indexObject(col);
}
