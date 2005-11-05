//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slTable.cpp PostgreSQL Slony-I table
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "slTable.h"
#include "frmMain.h"


slTable::slTable(slSet *s, const wxString& newName)
: slSetObject(s, slTableFactory, newName)
{
    wxLogInfo(wxT("Creating a slTable object"));
}

slTable::~slTable()
{
    wxLogInfo(wxT("Destroying a slTable object"));
}


bool slTable::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("setdroptable(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slTable::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Register table ") + GetName() + wxT(" for replication.\n\n")
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("setaddtable(") 
                    + NumToStr(GetSet()->GetSlId()) + wxT(", ") 
                    + NumToStr(GetSlId()) + wxT(", ")
                    + qtString(GetName()) + wxT(", ")
                    + qtString(GetIndexName()) + wxT(", ")
                    + qtString(GetComment()) + wxT(");\n");

        
        size_t i;
        for (i=0 ; i < triggers.GetCount() ; i++)
        {
            sql += wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("storetrigger(") 
                    + NumToStr(GetSlId()) + wxT(", ")
                    + qtString(triggers[i]) + wxT(");\n");
        }
    }
    return sql;
}


void slTable::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;
        pgSet *set=GetConnection()->ExecuteSet(
            wxT("SELECT trig_tgname\n")
            wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_trigger\n")
            wxT(" WHERE trig_tabid = ") + NumToStr(GetSlId()));
        if (set)
        {
            while (!set->Eof())
            {
                triggers.Add(set->GetVal(wxT("trig_tgname")));
                set->MoveNext();
            }
            delete set;
        }
    }

    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for replicated table ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("ID"), GetSlId());
        properties->AppendItem(_("Index Name"), GetIndexName());
        properties->AppendItem(_("Altered"), GetAltered());
        properties->AppendItem(_("Comment"), GetComment());
        if (triggers.GetCount() > 0)
        {
            properties->AppendItem(_("Triggers"), triggers[0]);

            size_t i;
            for (i=1 ; i < triggers.GetCount() ; i++)
                properties->AppendItem(wxEmptyString, triggers[i]);
        }
    }
}



pgObject *slTable::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *table=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        table = slTableFactory.CreateObjects(coll, 0, wxT(" WHERE tab_id=") + NumToStr(GetSlId()) + wxT("\n"));

    return table;
}



pgObject *slSlTableFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restr)
{
    slSetObjCollection *collection=(slSetObjCollection*)coll;
    slTable *table=0;
    wxString restriction;
    if (restr.IsEmpty())
        restriction = wxT(" WHERE tab_set = ") + NumToStr(collection->GetSlId());
    else
        restriction = restr;

    pgSet *tables = collection->GetDatabase()->ExecuteSet(
        wxT("SELECT tab_id, tab_reloid, tab_set, nspname, relname, tab_idxname, tab_altered, tab_comment")
        wxT("  FROM ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_table\n")
        wxT("  JOIN ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_set ON set_id=tab_set\n")
        wxT("  JOIN pg_class cl ON cl.oid=tab_reloid\n")
        wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
         + restriction +
        wxT(" ORDER BY tab_id"));

    if (tables)
    {
        while (!tables->Eof())
        {
            table = new slTable(collection->GetSet(), tables->GetVal(wxT("nspname")) + wxT(".") + tables->GetVal(wxT("relname")));
            table->iSetSlId(tables->GetLong(wxT("tab_id")));
            table->iSetIndexName(tables->GetVal(wxT("tab_idxname")));
            table->iSetComment(tables->GetVal(wxT("tab_comment")));
            table->iSetAltered(tables->GetBool(wxT("tab_altered")));
            table->iSetOid(tables->GetOid(wxT("tab_reloid")));

            if (browser)
            {
                browser->AppendObject(collection, table);
				tables->MoveNext();
            }
            else
                break;
        }

		delete tables;
    }
    return table;
}


///////////////////////////////////////////////////

#include "images/table-repl.xpm"
#include "images/table-repl-sm.xpm"
#include "images/tables.xpm"

slSlTableFactory::slSlTableFactory() 
: slSetObjFactory(__("Table"), __("New Table"), __("Create a new Table."), table_repl_xpm, table_repl_sm_xpm)
{
    metaType = SLM_TABLE;
}


slSlTableFactory slTableFactory;
static pgaCollectionFactory cf(&slTableFactory, __("Tables"), tables_xpm);
