//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgTable.cpp - Table class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgTable.h"
#include "pgCollection.h"
#include "pgConstraints.h"
#include "pgColumn.h"
#include "pgIndexConstraint.h"
#include "pgForeignKey.h"
#include "pgCheck.h"


pgTable::pgTable(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_TABLE, newName)
{
    rows=-1;
}

pgTable::~pgTable()
{
}

bool pgTable::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP TABLE ") + GetQuotedFullIdentifier());
}


wxString pgTable::GetAllConstraints(wxTreeCtrl *browser, wxTreeItemId collectionId, int type)
{
    wxString sql;

    long cookie;
    pgObject *data;
    wxTreeItemId item=browser->GetFirstChild(collectionId, cookie);
            
    while (item)
    {
        data=(pgObject*)browser->GetItemData(item);
        if (type < 0 || type == data->GetType())
        {
            sql += wxT(",\n  CONSTRAINT ") + data->GetQuotedIdentifier() 
                + wxT(" ") + data->GetTypeName().Upper() 
                + wxT(" ") ;
            data->ShowTreeDetail(browser);
            
            switch (data->GetType())
            {
                case PG_PRIMARYKEY:
                case PG_UNIQUE:
                    sql += ((pgIndexConstraint*)data)->GetDefinition();
                    break;
                case PG_FOREIGNKEY:
                    sql += ((pgForeignKey*)data)->GetDefinition();
                    break;
                case PG_CHECK:
                    sql += ((pgCheck*)data)->GetDefinition();
                    break;
            }
        }
        
        item=browser->GetNextChild(collectionId, cookie);
    }

    return sql;
}


wxString pgTable::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        // make sure all kids are appended
        ShowTreeDetail(browser);
        sql = wxT("-- Table: ") + GetQuotedFullIdentifier() + wxT("\n")
            + wxT("CREATE TABLE ") + GetQuotedFullIdentifier() + wxT("   (\n");

        pgObject *data;
        long cookie;
        wxTreeItemId item=browser->GetFirstChild(GetId(), cookie);
        wxTreeItemId columnsItem, constraintsItem;
        while (item)
        {
            data=(pgObject*)browser->GetItemData(item);
            if (data->GetType() == PG_COLUMNS)
                columnsItem = item;
            else if (data->GetType() == PG_CONSTRAINTS)
                constraintsItem = item;

            if (columnsItem && constraintsItem)
                break;

            item=browser->GetNextChild(GetId(), cookie);
        }
        if (columnsItem)
        {
            pgCollection *coll=(pgCollection*)browser->GetItemData(columnsItem);
            // make sure all columns are appended
            coll->ShowTreeDetail(browser);
            // this is the columns collection
            wxTreeItemId item=browser->GetFirstChild(columnsItem, cookie);

            // add columns
            int colCount=0;
            while (item)
            {
                data=(pgObject*)browser->GetItemData(item);
                if (data->GetType() == PG_COLUMN)
                {
                    pgColumn *column=(pgColumn*)data;
                    // make sure column details are read
                    column->ShowTreeDetail(browser);

                    if (column->GetColNumber() > 0)
                    {
                        if (colCount)
                            sql += wxT(",\n");

                        sql += wxT("  ") + column->GetQuotedIdentifier() + wxString(" ")
                            + column->GetDefinition();

                        colCount++;
                    }
                }
                
                item=browser->GetNextChild(columnsItem, cookie);
            }
        }

        /*
        // add primary key
        if (!GetPrimaryKey().IsNull())
        {
            sql += wxT(",\n  CONSTRAINT ") + GetPrimaryKeyName() + wxT(" PRIMARY KEY (");
            wxStringTokenizer collist(GetPrimaryKeyColNumbers(), ',');
            long cn;
            int pkcolcount=0;

            while (collist.HasMoreTokens())
            {
                cn=StrToLong(collist.GetNextToken());
                wxTreeItemId item=browser->GetFirstChild(columnsItem, cookie);
                while (item)
                {
                    data=(pgObject*)browser->GetItemData(item);
                    if (data->GetType() == PG_COLUMN)
                    {
                        pgColumn *column=(pgColumn*)data;
                        if (column->GetColNumber() == cn)
                        {
                            if (pkcolcount)
                                sql += wxT(", ");
                            sql += column->GetQuotedIdentifier();
                            pkcolcount++;
                            break;
                        }
                    }

                    item=browser->GetNextChild(columnsItem, cookie);
                }
            }
            sql += wxT(")");
        }

        // add checks
        if (constraintsItem)
        {
            // this is the checks collection
            pgCollection *coll=(pgCollection*)data;
            // make sure all kids are read
            coll->ShowTreeDetail(browser);

            wxTreeItemId item=browser->GetFirstChild(constraintsItem, cookie);
            
            while (item)
            {
                data=(pgObject*)browser->GetItemData(item);
                int colCount=0;
                if (data->GetType() == PG_CHECK)
                {
                    sql += wxT(",\n");

                    pgCheck *check=(pgCheck *)data;
                    check->ShowTreeDetail(browser);
                    sql += wxT("  CONSTRAINT ") + check->GetConstraint();
                }
                
                item=browser->GetNextChild(constraintsItem, cookie);
            }
        }

        // add foreign keys
        if (constraintsItem)
        {
            // this is the foreign keys collection
            pgCollection *coll=(pgCollection*)data;
            coll->ShowTreeDetail(browser);

            wxTreeItemId item=browser->GetFirstChild(constraintsItem, cookie);
            
            while (item)
            {
                data=(pgObject*)browser->GetItemData(item);
                int colCount=0;
                if (data->GetType() == PG_FOREIGNKEY)
                {
                    sql += wxT(",\n");

                    pgForeignKey *foreignKey=(pgForeignKey *)data;
                    foreignKey->ShowTreeDetail(browser);
                    sql += wxT("  CONSTRAINT ") + foreignKey->GetConstraint();
                }                
                item=browser->GetNextChild(constraintsItem, cookie);
            }
        }
        */

        if (constraintsItem)
        {
            pgCollection *coll=(pgCollection*)browser->GetItemData(constraintsItem);
            // make sure all kids are read
            coll->ShowTreeDetail(browser);

            sql += GetAllConstraints(browser, constraintsItem, PG_PRIMARYKEY);
            sql += GetAllConstraints(browser, constraintsItem, PG_FOREIGNKEY);
            sql += GetAllConstraints(browser, constraintsItem, PG_UNIQUE);
            sql += GetAllConstraints(browser, constraintsItem, PG_CHECK);
        }
        sql += wxT("\n) ");
        if (GetInheritedTableCount())
        {
            sql += wxT("INHERITS (") + GetQuotedInheritedTables() + wxT(") ");
        }

        if (GetHasOids())
            sql += wxT("WITH OIDS;\n");
        else
            sql += wxT("WITHOUT OIDS;\n")
                + GetGrant(wxT("arwdRxt")) 
                + GetCommentSql()
                + wxT("\n\n");

        // add indexes here

        // add triggers here 


    }
    return sql;
}


void pgTable::UpdateRows()
{
    pgSet *props = ExecuteSet(wxT("SELECT count(*) AS rows FROM ") + GetQuotedFullIdentifier());
    if (props)
    {
        rows = StrToDouble(props->GetVal(0));
        delete props;
    }
}

void pgTable::UpdateInheritance()
{
    // not checked so far
    pgSet *props=ExecuteSet(wxT(
        "SELECT c.relname , nspname\n"
        "  FROM pg_inherits i\n"
        "  JOIN pg_class c ON c.oid = i.inhparent\n"
        "  JOIN pg_namespace n ON n.oid=c.relnamespace\n"
        " WHERE i.inhrelid = ") +GetOidStr() + wxT("\n"
        " ORDER BY inhseqno"));
    if (props)
    {
        inheritedTableCount=0;
        inheritedTables=wxT("");
        while (!props->Eof())
        {
            if (inheritedTables != wxT(""))
                inheritedTables += wxT(", ");
            inheritedTables += props->GetVal(wxT("relname"));
            quotedInheritedTables += qtIdent(props->GetVal(wxT("nspname")))
                    +wxT(".")+qtIdent(props->GetVal(wxT("relname")));
            props->MoveNext();
            inheritedTableCount++;
        }
        delete props;
    }
}





void pgTable::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        // Log
        wxLogInfo(wxT("Adding child object to table ") + GetIdentifier());

        pgCollection *collection;

        // Columns
        collection = new pgCollection(PG_COLUMNS);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Constraints
        collection = new pgConstraints();
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Indexes
        collection = new pgCollection(PG_INDEXES);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Rules
        collection = new pgCollection(PG_RULES);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Triggers
        collection = new pgCollection(PG_TRIGGERS);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // convert list of columns numbers to column names
        wxStringTokenizer collist(GetPrimaryKeyColNumbers(), ',');
        wxString cn;

        while (collist.HasMoreTokens())
        {
            cn=collist.GetNextToken();
            pgSet *set=ExecuteSet(wxT(
                "SELECT attname\n"
                "  FROM pg_attribute\n"
                " WHERE attrelid=") + GetOidStr() + wxT(" AND attnum IN (") + cn + wxT(")"));
            if (set)
            {
                if (!primaryKey.IsNull())
                    primaryKey += wxT(", ");

                primaryKey += set->GetVal(0);
                delete set;
            }
        }
    }

    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
        if (GetPrimaryKey().IsNull())
            InsertListItem(properties, pos++, wxT("Primary Key"), wxT("<none>"));
        else
            InsertListItem(properties, pos++, wxT("Primary Key"), GetPrimaryKey());

        // this might be on demand later, if too slow
        UpdateRows();

        double rows=GetRows();
        if (rows < 0)
            InsertListItem(properties, pos++, wxT("Rows"), wxT("?"));
        else
            InsertListItem(properties, pos++, wxT("Rows"), rows);

        InsertListItem(properties, pos++, wxT("Rows estimated"), GetEstimatedRows());
        InsertListItem(properties, pos++, wxT("Inherits Tables"), GetHasSubclass());
        InsertListItem(properties, pos++, wxT("Inherited Tables Count"), GetInheritedTableCount());
        InsertListItem(properties, pos++, wxT("Inherited Tables"), GetInheritedTables());
        InsertListItem(properties, pos++, wxT("Has OIDs?"), GetHasOids());
        InsertListItem(properties, pos++, wxT("System Table?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }

    DisplayStatistics(statistics, wxT(
        "SELECT seq_scan AS \"Sequential Scans\", seq_tup_read AS \"Sequential Tuples Read\", "
                "idx_scan AS \"Index Scans\", idx_tup_fetch AS \"Index Tuples Fetched\", "
                "n_tup_ins AS \"Tuples Inserted\", n_tup_upd AS \"Tuples Updated\", n_tup_del AS \"Tuples Deleted\","
                "heap_blks_read AS \"Heap Blocks Read\", heap_blks_hit AS \"Heap Blocks Hit\", "
                "idx_blks_read AS \"Index Blocks Read\", idx_blks_hit AS \"Index Blocks Hit\", "
                "toast_blks_read AS \"Toast Blocks Read\", toast_blks_hit AS \"Toast Blocks Hit\", "
                "tidx_blks_read AS \"Toast Index Blocks Read\", tidx_blks_hit AS \"Toast Index Blocks Hit\"\n"
        "  FROM pg_stat_all_tables stat, pg_statio_all_tables statio\n"
        " WHERE stat.relid = statio.relid\n"
        "   AND stat.relid = ") + GetOidStr());
}




pgObject *pgTable::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *table=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_TABLES)
            table = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND rel.oid=") + GetOidStr());
    }
    return table;
}



pgObject *pgTable::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgTable *table=0;

    pgSet *tables= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT rel.oid, relname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, "
                "relhassubclass, reltuples, description, conname, conkey\n"
        "  FROM pg_class rel\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=rel.oid AND des.objsubid=0\n"
        "  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n"
        " WHERE ((relkind = 'r') OR (relkind = 's')) AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n"
        + restriction + 
        " ORDER BY relname"));

    if (tables)
    {
        int nr=tables->NumRows();

        while (!tables->Eof())
        {
            table = new pgTable(collection->GetSchema(), tables->GetVal(wxT("relname")));

            table->iSetOid(tables->GetOid(wxT("oid")));
            table->iSetOwner(tables->GetVal(wxT("relowner")));
            table->iSetAcl(tables->GetVal(wxT("relacl")));
            table->iSetComment(tables->GetVal(wxT("description")));
            table->iSetHasOids(tables->GetBool(wxT("relhasoids")));
            table->iSetEstimatedRows(tables->GetDouble(wxT("reltuples")));
            table->iSetHasSubclass(tables->GetBool(wxT("relhassubclass")));
            table->iSetPrimaryKeyName(tables->GetVal(wxT("conname")));
            wxString cn=tables->GetVal(wxT("conkey"));
            cn=cn.Mid(1, cn.Length()-2);
            table->iSetPrimaryKeyColNumbers(cn);

            if (browser)
            {
                collection->AppendBrowserItem(browser, table);
                tables->MoveNext();
            }
            else
                break;
        }

		delete tables;
    }
    return table;
}


void pgTable::ShowStatistics(pgCollection *collection, wxListCtrl *statistics)
{
    wxLogInfo(wxT("Displaying statistics for tables on ")+ collection->GetSchema()->GetIdentifier());

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Table"), wxLIST_FORMAT_LEFT, 150);
    statistics->InsertColumn(1, wxT("Tuples inserted"), wxLIST_FORMAT_LEFT, 80);
    statistics->InsertColumn(2, wxT("Tuples updated"), wxLIST_FORMAT_LEFT, 80);
    statistics->InsertColumn(3, wxT("Tuples deleted"), wxLIST_FORMAT_LEFT, 80);

    pgSet *stats = collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT relname, n_tup_ins, n_tup_upd, n_tup_del FROM pg_stat_all_tables ORDER BY relname"));

    if (stats)
    {
        long pos=0;
        while (!stats->Eof())
        {
            statistics->InsertItem(pos, stats->GetVal(wxT("relname")), PGICON_STATISTICS);
            statistics->SetItem(pos, 1, stats->GetVal(wxT("n_tup_ins")));
            statistics->SetItem(pos, 2, stats->GetVal(wxT("n_tup_upd")));
            statistics->SetItem(pos, 3, stats->GetVal(wxT("n_tup_del")));
            stats->MoveNext();
            pos++;
        }

	    delete stats;
    }
}
