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
#include "pgColumn.h"
#include "pgCheck.h"
#include "pgForeignKey.h"


pgTable::pgTable(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_TABLE, newName)
{
    rows=-1;
}

pgTable::~pgTable()
{
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
        wxTreeItemId columnsItem=browser->GetFirstChild(GetId(), cookie);
        while (columnsItem)
        {
            data=(pgObject*)browser->GetItemData(columnsItem);
            if (data->GetType() == PG_COLUMNS)
                break;
            columnsItem=browser->GetNextChild(GetId(), cookie);
        }
        if (columnsItem)
        {
            pgCollection *coll=(pgCollection*)data;
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
                            + column->GetFullType();

                        AppendIfFilled(sql,  wxT(" DEFAULT "), column->GetDefault());

                        if (column->GetNotNull())
                            sql += wxT(" NOT NULL");
                        colCount++;
                    }
                }
                
                item=browser->GetNextChild(columnsItem, cookie);
            }
        }

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
        wxTreeItemId checkItem=browser->GetFirstChild(GetId(), cookie);
        while (checkItem)
        {
            data=(pgObject*)browser->GetItemData(checkItem);
            if (data->GetType() == PG_CHECKS)
                break;
            checkItem=browser->GetNextChild(GetId(), cookie);
        }
        if (checkItem)
        {
            // this is the checks collection
            pgCollection *coll=(pgCollection*)data;
            // make sure all kids are read
            coll->ShowTreeDetail(browser);

            wxTreeItemId item=browser->GetFirstChild(checkItem, cookie);
            
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
                
                item=browser->GetNextChild(checkItem, cookie);
            }
        }

        // add foreign keys
        wxTreeItemId fkItem=browser->GetFirstChild(GetId(), cookie);
        while (fkItem)
        {
            data=(pgObject*)browser->GetItemData(fkItem);
            if (data->GetType() == PG_FOREIGNKEYS)
                break;
            fkItem=browser->GetNextChild(GetId(), cookie);
        }
        if (fkItem)
        {
            // this is the foreign keys collection
            pgCollection *coll=(pgCollection*)data;
            coll->ShowTreeDetail(browser);

            wxTreeItemId item=browser->GetFirstChild(fkItem, cookie);
            
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
                item=browser->GetNextChild(fkItem, cookie);
            }
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
    pgSet *props = ExecuteSet(wxT("SELECT count(*) AS rows FROM ") + GetQuotedIdentifier());
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
        inheritedTableCount=props->NumRows();
        inheritedTables=wxT("");
        while (!props->Eof())
        {
            if (inheritedTables != wxT(""))
                inheritedTables += wxT(", ");
            inheritedTables += props->GetVal(wxT("relname"));
            quotedInheritedTables += qtIdent(props->GetVal(wxT("nspname")))
                    +wxT(".")+qtIdent(props->GetVal(wxT("relname")));
            props->MoveNext();
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

        // Checks
        collection = new pgCollection(PG_CHECKS);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_CHECK, -1, collection);

        // Columns
        collection = new pgCollection(PG_COLUMNS);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_COLUMN, -1, collection);

        // Foreign Keys
        collection = new pgCollection(PG_FOREIGNKEYS);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_KEY, -1, collection);

        // Indexes
        collection = new pgCollection(PG_INDEXES);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_INDEX, -1, collection);

        // Rules
        collection = new pgCollection(PG_RULES);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_RULE, -1, collection);

        // Triggers
        collection = new pgCollection(PG_TRIGGERS);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_TRIGGER, -1, collection);

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
                browser->AppendItem(collection->GetId(), table->GetIdentifier(), PGICON_TABLE, -1, table);
                tables->MoveNext();
            }
            else
                break;
        }

		delete tables;
    }
    return table;
}


void pgTable::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {

        // Log
        wxLogInfo(wxT("Adding tables to schema ")+ collection->GetSchema()->GetIdentifier());

        // Get the tables
        ReadObjects(collection, browser);
    }

    if (statistics)
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
                statistics->InsertItem(pos, stats->GetVal(wxT("relname")), 0);
                statistics->SetItem(pos, 1, stats->GetVal(wxT("n_tup_ins")));
                statistics->SetItem(pos, 2, stats->GetVal(wxT("n_tup_upd")));
                statistics->SetItem(pos, 3, stats->GetVal(wxT("n_tup_del")));
                stats->MoveNext();
                pos++;
            }

	        delete stats;
        }
    }
}
