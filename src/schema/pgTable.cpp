//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "sysSettings.h"
#include "pgfeatures.h"

pgTable::pgTable(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_TABLE, newName)
{
    inheritedTableCount=0;
    rowsCounted = false;
}

pgTable::~pgTable()
{
}


wxMenu *pgTable::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();
    if (schema->GetCreatePrivilege())
    {
        AppendMenu(menu, PG_COLUMN);
        if (GetPrimaryKey().IsEmpty())      // Will not notice if pk has been added after last refresh
            AppendMenu(menu, PG_PRIMARYKEY);
        AppendMenu(menu, PG_FOREIGNKEY);
        AppendMenu(menu, PG_UNIQUE);
        AppendMenu(menu, PG_CHECK);
        AppendMenu(menu, PG_INDEX);
        AppendMenu(menu, PG_RULE);
        AppendMenu(menu, PG_TRIGGER);
    }
    return menu;
}


wxString pgTable::GetHelpPage(bool forCreate) const
{ 
    if (forCreate)
        return wxT("sql-createtable");
    else
        return wxT("sql-altertable");
}


bool pgTable::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP TABLE ") + GetQuotedFullIdentifier() + wxT(";"));
}


wxString pgTable::GetAllConstraints(wxTreeCtrl *browser, wxTreeItemId collectionId, int type)
{
    wxString sql;
    wxCookieType cookie;
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
				  sql += wxT("(") + ((pgCheck*)data)->GetDefinition() + wxT(")");
                    break;
            }
        }
        
        item=browser->GetNextChild(collectionId, cookie);
    }

    return sql;
}


wxString pgTable::GetSql(wxTreeCtrl *browser)
{
	wxString colDetails;
    if (sql.IsNull())
    {
        // make sure all kids are appended
        ShowTreeDetail(browser);
        sql = wxT("-- Table: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP TABLE ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE TABLE ") + GetQuotedFullIdentifier() + wxT("\n(\n");

        pgObject *data;
        wxCookieType cookie;
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

                        sql += wxT("  ") + column->GetQuotedIdentifier() + wxT(" ")
                            + column->GetDefinition();

						// Whilst we are looping round the columns, grab their comments as well.
						// Perhaps we should also get storage types here?
						colDetails += column->GetCommentSql();
						if (colDetails.Length() > 0)
							if (colDetails.Last() != '\n') colDetails += wxT("\n");

                        colCount++;
                    }
                }
                
                item=browser->GetNextChild(columnsItem, cookie);
            }
        }

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
            sql += wxT("\nWITH OIDS");
        else
            sql += wxT("\nWITHOUT OIDS");

        AppendIfFilled(sql, wxT(" TABLESPACE "), qtIdent(tablespace));

        sql += wxT(";\n")
            + GetOwnerSql(7, 3)
            + GetGrant(wxT("arwdRxt")) 
            + GetCommentSql();

		// Column comments
		sql += colDetails;

        // add indexes here

        // add triggers here 

		if (sql.Length() > 0)
			if (sql.Last() != '\n') sql += wxT("\n");

    }
    return sql;
}


wxString pgTable::GetCoveringIndex(wxTreeCtrl *browser, const wxString &collist)
{
    // delivers the name of the index which covers the named columns
    wxCookieType cookie;

    wxTreeItemId collItem=browser->GetFirstChild(GetId(), cookie);
    while (collItem)
    {
        pgObject *data=(pgObject*)browser->GetItemData(collItem);
        if (data && (data->GetType() == PG_CONSTRAINTS || data->GetType() == PG_INDEXES))
        {
            wxCookieType cookie2;
            wxTreeItemId item=browser->GetFirstChild(collItem, cookie2);
            while (item)
            {
                pgIndex *index=(pgIndex*)browser->GetItemData(item);
                if (index && (index->GetType() == PG_INDEX || index->GetType() == PG_PRIMARYKEY || index->GetType() == PG_UNIQUE))
                {
                    index->ShowTreeDetail(browser);
                    if (collist == index->GetColumns() || 
                        collist + wxT(",") == index->GetColumns().Left(collist.Length()+1))
                        return index->GetName();
                }
                item=browser->GetNextChild(collItem, cookie2);
            }
        }
        collItem=browser->GetNextChild(GetId(), cookie);
    }

    return wxEmptyString;
}


void pgTable::UpdateRows()
{
    pgSet *props = ExecuteSet(wxT("SELECT count(*) AS rows FROM ") + GetQuotedFullIdentifier());
    if (props)
    {
        rows = props->GetLongLong(0);
        delete props;
        rowsCounted = true;
    }
}

void pgTable::UpdateInheritance()
{
    // not checked so far
    pgSet *props=ExecuteSet(
        wxT("SELECT c.relname , nspname\n")
        wxT("  FROM pg_inherits i\n")
        wxT("  JOIN pg_class c ON c.oid = i.inhparent\n")
        wxT("  JOIN pg_namespace n ON n.oid=c.relnamespace\n")
        wxT(" WHERE i.inhrelid = ") +GetOidStr() + wxT("\n")
        wxT(" ORDER BY inhseqno"));
    if (props)
    {
        inheritedTableCount=0;
        inheritedTables=wxT("");
        while (!props->Eof())
        {
            if (inheritedTableCount)
            {
                inheritedTables += wxT(", ");
                quotedInheritedTables += wxT(", ");
            }
            inheritedTables += props->GetVal(wxT("relname"));
            quotedInheritedTables += GetQuotedSchemaPrefix(props->GetVal(wxT("nspname")))
                    + qtIdent(props->GetVal(wxT("relname")));
            quotedInheritedTablesList.Add(GetQuotedSchemaPrefix(props->GetVal(wxT("nspname")))
                    + qtIdent(props->GetVal(wxT("relname"))));
            props->MoveNext();
            inheritedTableCount++;
        }
        delete props;
    }
}





void pgTable::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);

        // Log
        wxLogInfo(wxT("Adding child object to table ") + GetIdentifier());

        pgCollection *collection;

        // Columns
        collection = new pgCollection(PG_COLUMNS, GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Constraints
        collection = new pgConstraints(GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Indexes
        collection = new pgCollection(PG_INDEXES, GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Rules
        collection = new pgCollection(PG_RULES, GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // Triggers
        collection = new pgCollection(PG_TRIGGERS, GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);

        // convert list of columns numbers to column names
        wxStringTokenizer collist(GetPrimaryKeyColNumbers(), wxT(","));
        wxString cn;

        while (collist.HasMoreTokens())
        {
            cn=collist.GetNextToken();
            pgSet *set=ExecuteSet(
                wxT("SELECT attname\n")
                wxT("  FROM pg_attribute\n")
                wxT(" WHERE attrelid=") + GetOidStr() + wxT(" AND attnum IN (") + cn + wxT(")"));
            if (set)
            {
                if (!primaryKey.IsNull())
                {
                    quotedPrimaryKey += wxT(", ");
                    primaryKey += wxT(", ");
                }
                primaryKey += set->GetVal(0);
                quotedPrimaryKey += qtIdent(set->GetVal(0));
                delete set;
            }
        }

        if (settings->GetAutoRowCountThreshold() >= GetEstimatedRows())
            UpdateRows();

        UpdateInheritance();
    }

    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        if (!tablespace.IsEmpty())
            properties->AppendItem(_("Tablespace"), tablespace);
        properties->AppendItem(_("ACL"), GetAcl());
        if (GetPrimaryKey().IsNull())
            properties->AppendItem(_("Primary key"), _("<no primary key>"));
        else
            properties->AppendItem(_("Primary key"), GetPrimaryKey());

        properties->AppendItem(_("Rows (estimated)"), GetEstimatedRows());

        if (rowsCounted)
            properties->AppendItem(_("Rows (counted)"), rows);
        else
            properties->AppendItem(_("Rows (counted)"), _("not counted"));

        properties->AppendItem(_("Inherits tables"), GetHasSubclass());
        properties->AppendItem(_("Inherited tables count"), GetInheritedTableCount());
        if (GetInheritedTableCount())
            properties->AppendItem(_("Inherited tables"), GetInheritedTables());
        properties->AppendItem(_("Has OIDs?"), GetHasOids());
        properties->AppendItem(_("System table?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}


void pgTable::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    wxString sql =
        wxT("SELECT seq_scan AS ") + qtIdent(_("Sequential Scans")) +
             wxT(", seq_tup_read AS ") + qtIdent(_("Sequential Tuples Read")) +
             wxT(", idx_scan AS ") + qtIdent(_("Index Scans")) +
             wxT(", idx_tup_fetch AS ") + qtIdent(_("Index Tuples Fetched"))+
             wxT(", n_tup_ins AS ") + qtIdent(_("Tuples Inserted"))+
             wxT(", n_tup_upd AS ") + qtIdent(_("Tuples Updated")) +
             wxT(", n_tup_del AS ") + qtIdent(_("Tuples Deleted")) +
             wxT(", heap_blks_read AS ") + qtIdent(_("Heap Blocks Read")) +
             wxT(", heap_blks_hit AS ") + qtIdent(_("Heap Blocks Hit")) +
             wxT(", idx_blks_read AS ") + qtIdent(_("Index Blocks Read")) +
             wxT(", idx_blks_hit AS ") + qtIdent(_("Index Blocks Hit")) +
             wxT(", toast_blks_read AS ") + qtIdent(_("Toast Blocks Read")) +
             wxT(", toast_blks_hit AS ") + qtIdent(_("Toast Blocks Hit")) +
             wxT(", tidx_blks_read AS ") + qtIdent(_("Toast Index Blocks Read")) +
             wxT(", tidx_blks_hit AS ") + qtIdent(_("Toast Index Blocks Hit"));
    
    if (GetConnection()->HasFeature(FEATURE_SIZE))
    {
        sql += wxT(", pg_size_pretty(pg_relation_size(stat.relid)) AS ") + qtIdent(_("Table Size"))
            +  wxT(", CASE WHEN cl.reltoastrelid = 0 THEN ") + qtString(_("none")) + wxT(" ELSE pg_size_pretty(pg_relation_size(cl.reltoastrelid)) END AS ") + qtIdent(_("Toast Table Size"))
            +  wxT(", pg_size_pretty(COALESCE((SELECT SUM(pg_relation_size(indexrelid)) FROM pg_index WHERE indrelid=stat.relid)::int8, 0)) AS ") + qtIdent(_("Indexes Size"));
    }
    sql +=  wxT("\n")
        wxT("  FROM pg_stat_all_tables stat\n")
        wxT("  JOIN pg_statio_all_tables statio ON stat.relid = statio.relid\n")
        wxT("  JOIN pg_class cl ON cl.oid=stat.relid\n")
        wxT(" WHERE stat.relid = ") + GetOidStr();

    DisplayStatistics(statistics, sql);
}



pgObject *pgTable::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgTable *table=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_TABLES)
            table = (pgTable*)ReadObjects((pgCollection*)obj, 0, wxT("\n   AND rel.oid=") + GetOidStr());
    }
    return table;
}



pgObject *pgTable::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgTable *table=0;

    pgSet *tables;
    if (collection->GetConnection()->BackendMinimumVersion(7, 5))
    {
        tables= collection->GetDatabase()->ExecuteSet(
            wxT("SELECT rel.oid, relname, spcname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
                    wxT("relhassubclass, reltuples, description, conname, conkey\n")
            wxT("  FROM pg_class rel\n")
            wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=rel.reltablespace\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=rel.oid AND des.objsubid=0\n")
            wxT("  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n")
            wxT(" WHERE relkind IN ('r','s','t') AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
            + restriction + 
            wxT(" ORDER BY relname"));
    }
    else
    {
        tables= collection->GetDatabase()->ExecuteSet(
            wxT("SELECT rel.oid, relname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
                    wxT("relhassubclass, reltuples, description, conname, conkey\n")
            wxT("  FROM pg_class rel\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=rel.oid AND des.objsubid=0\n")
            wxT("  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n")
            wxT(" WHERE relkind IN ('r','s','t') AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
            + restriction + 
            wxT(" ORDER BY relname"));
    }
    if (tables)
    {
        while (!tables->Eof())
        {
            table = new pgTable(collection->GetSchema(), tables->GetVal(wxT("relname")));

            table->iSetOid(tables->GetOid(wxT("oid")));
            table->iSetOwner(tables->GetVal(wxT("relowner")));
            table->iSetAcl(tables->GetVal(wxT("relacl")));
            if (collection->GetConnection()->BackendMinimumVersion(7, 5))
                table->iSetTablespace(tables->GetVal(wxT("spcname")));
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


void pgTable::ShowStatistics(pgCollection *collection, ctlListView *statistics)
{
    wxLogInfo(wxT("Displaying statistics for tables on ")+ collection->GetSchema()->GetIdentifier());

    bool hasSize=collection->GetConnection()->HasFeature(FEATURE_SIZE);

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->AddColumn(_("Table"), 100);
    statistics->AddColumn(_("Tuples inserted"), 50);
    statistics->AddColumn(_("Tuples updated"), 50);
    statistics->AddColumn(_("Tuples deleted"), 50);
    if (hasSize)
        statistics->AddColumn(_("Size"), 60);

    wxString sql=wxT("SELECT st.relname, n_tup_ins, n_tup_upd, n_tup_del");
    if (hasSize)
        sql += wxT(", pg_size_pretty(pg_relation_size(st.relid)")
               wxT(" + CASE WHEN cl.reltoastrelid = 0 THEN 0 ELSE pg_relation_size(cl.reltoastrelid) END")
               wxT(" + COALESCE((SELECT SUM(pg_relation_size(indexrelid)) FROM pg_index WHERE indrelid=st.relid)::int8, 0)) AS size");
    
    sql += wxT("\n  FROM pg_stat_all_tables st")
           wxT("  JOIN pg_class cl on cl.oid=st.relid\n")
	       wxT(" WHERE schemaname = ") + qtString(collection->GetSchema()->GetName())
	    +  wxT("\n ORDER BY relname");

    pgSet *stats = collection->GetDatabase()->ExecuteSet(sql);

    if (stats)
    {
        long pos=0;
        while (!stats->Eof())
        {
            statistics->InsertItem(pos, stats->GetVal(wxT("relname")), PGICON_STATISTICS);
            statistics->SetItem(pos, 1, stats->GetVal(wxT("n_tup_ins")));
            statistics->SetItem(pos, 2, stats->GetVal(wxT("n_tup_upd")));
            statistics->SetItem(pos, 3, stats->GetVal(wxT("n_tup_del")));
            if (hasSize)
                statistics->SetItem(pos, 4, stats->GetVal(wxT("size")));
            stats->MoveNext();
            pos++;
        }

	    delete stats;
    }
}

