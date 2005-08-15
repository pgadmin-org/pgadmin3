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


#include "pgAdmin3.h"
#include "misc.h"
#include "frmHint.h"
#include "frmMain.h"
#include "frmMaintenance.h"
#include "pgTable.h"
#include "pgColumn.h"
#include "pgIndexConstraint.h"
#include "pgForeignKey.h"
#include "pgCheck.h"
#include "sysSettings.h"
#include "pgfeatures.h"
#include "pgRule.h"
#include "pgTrigger.h"
#include "pgConstraints.h"

// App headers

pgTable::pgTable(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, tableFactory, newName)
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
        columnFactory.AppendMenu(menu);
        if (GetPrimaryKey().IsEmpty())      // Will not notice if pk has been added after last refresh
            primaryKeyFactory.AppendMenu(menu);
        foreignKeyFactory.AppendMenu(menu);
        uniqueFactory.AppendMenu(menu);
        checkFactory.AppendMenu(menu);
        indexFactory.AppendMenu(menu);
        ruleFactory.AppendMenu(menu);
        triggerFactory.AppendMenu(menu);
    }
    return menu;
}


wxString pgTable::GetHelpPage(bool forCreate) const
{ 
    if (forCreate)
        return wxT("pg/sql-createtable");
    else
        return wxT("pg/sql-altertable");
}


bool pgTable::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql = wxT("DROP TABLE ") + GetQuotedFullIdentifier();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}


wxString pgTable::GetAllConstraints(ctlTree *browser, wxTreeItemId collectionId, int metaType)
{
    wxString sql;
    wxCookieType cookie;
    pgObject *data;
    wxTreeItemId item=browser->GetFirstChild(collectionId, cookie);
            
    while (item)
    {
        data=(pgObject*)browser->GetItemData(item);
        if (metaType < 0 || metaType == data->GetMetaType())
        {
            sql += wxT(",\n  CONSTRAINT ") + data->GetQuotedIdentifier() 
                + wxT(" ") + data->GetTypeName().Upper() 
                + wxT(" ") ;
            data->ShowTreeDetail(browser);
            
            switch (metaType)
            {
                case PGM_PRIMARYKEY:
                case PGM_UNIQUE:
                    sql += ((pgIndexConstraint*)data)->GetDefinition();
                    break;
                case PGM_FOREIGNKEY:
                    sql += ((pgForeignKey*)data)->GetDefinition();
                    break;
                case PGM_CHECK:
				  sql += wxT("(") + ((pgCheck*)data)->GetDefinition() + wxT(")");
                    break;
            }
        }
        
        item=browser->GetNextChild(collectionId, cookie);
    }

    return sql;
}


wxString pgTable::GetSql(ctlTree *browser)
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
            pgaFactory *factory=data->GetFactory();
            if (data->GetMetaType() == PGM_COLUMN)
                columnsItem = item;
            else if (data->GetMetaType() == PGM_CONSTRAINT)
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
                if (data->GetMetaType() == PGM_COLUMN)
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

            sql += GetAllConstraints(browser, constraintsItem, PGM_PRIMARYKEY);
            sql += GetAllConstraints(browser, constraintsItem, PGM_FOREIGNKEY);
            sql += GetAllConstraints(browser, constraintsItem, PGM_UNIQUE);
            sql += GetAllConstraints(browser, constraintsItem, PGM_CHECK);
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


wxString pgTable::GetCoveringIndex(ctlTree *browser, const wxString &collist)
{
    // delivers the name of the index which covers the named columns
    wxCookieType cookie;

    wxTreeItemId collItem=browser->GetFirstChild(GetId(), cookie);
    while (collItem)
    {
        pgObject *data=(pgObject*)browser->GetItemData(collItem);
        if (data && data->IsCollection() && (data->GetMetaType() == PGM_CONSTRAINT || data->GetMetaType() == PGM_INDEX))
        {
            wxCookieType cookie2;
            wxTreeItemId item=browser->GetFirstChild(collItem, cookie2);
            while (item)
            {
                pgIndex *index=(pgIndex*)browser->GetItemData(item);
                if (index && (index->GetMetaType() == PGM_INDEX || index->GetMetaType() == PGM_PRIMARYKEY || index->GetMetaType() == PGM_UNIQUE))
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





void pgTable::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        browser->RemoveDummyChild(this);

        // Log
        wxLogInfo(wxT("Adding child object to table ") + GetIdentifier());


        browser->AppendCollection(this, columnFactory);
        browser->AppendCollection(this, constraintFactory);
        browser->AppendCollection(this, indexFactory);
        browser->AppendCollection(this, ruleFactory);
        browser->AppendCollection(this, triggerFactory);

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

        if (form && GetVacuumHint() && !hintShown)
        {
            ShowHint(form, false);
        }
    }
}


bool pgTable::GetCanHint()
{
    return (GetVacuumHint() || primaryKey.IsEmpty());
}


bool pgTable::GetVacuumHint()
{
    bool canHint=false;

    if (rowsCounted)
    {
        if (!estimatedRows || (estimatedRows == 1000 && rows != 1000))
            canHint = (rows >= 20);
        else
        {
            wxULongLong quot = rows*10 / estimatedRows;
            canHint = ((quot > 12 || quot < 8) && (rows+20 < estimatedRows || rows > estimatedRows+20));
        }
    }
    else if (estimatedRows == 1000)
    {
        canHint = true;
    }
    return canHint;
}


void pgTable::ShowHint(frmMain *form, bool force)
{
    hintShown = true;
    int rc;
    
    if (force)
    {
        wxArrayString hints;
        if (GetVacuumHint())
            hints.Add(HINT_VACUUM);
        if (primaryKey.IsEmpty())
            hints.Add(HINT_PRIMARYKEY);

        rc=frmHint::ShowHint(form, hints, GetFullIdentifier());
    }
    else
        rc=frmHint::ShowHint(form, HINT_VACUUM, GetFullIdentifier(), force);

    if (rc == HINT_RC_FIX)
    {
        frmMaintenance *frm=new frmMaintenance(form, this);
        frm->Go();
    }
}




pgObject *pgTable::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgTable *table=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->IsCollection())
            table = (pgTable*)tableFactory.CreateObjects((pgCollection*)obj, 0, wxT("\n   AND rel.oid=") + GetOidStr());
    }
    return table;
}


///////////////////////////////////////////////////////////


pgTableCollection::pgTableCollection(pgaFactory *factory, pgSchema *sch)
: pgSchemaObjCollection(factory, sch)
{
}

    
void pgTableCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    wxLogInfo(wxT("Displaying statistics for tables on ")+ GetSchema()->GetIdentifier());

    bool hasSize=GetConnection()->HasFeature(FEATURE_SIZE);

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
               wxT(" + CASE WHEN cl.reltoastrelid = 0 THEN 0 ELSE pg_relation_size(cl.reltoastrelid) + COALESCE((SELECT SUM(pg_relation_size(indexrelid)) FROM pg_index WHERE indrelid=cl.reltoastrelid)::int8, 0) END")
               wxT(" + COALESCE((SELECT SUM(pg_relation_size(indexrelid)) FROM pg_index WHERE indrelid=st.relid)::int8, 0)) AS size");
    
    sql += wxT("\n  FROM pg_stat_all_tables st")
           wxT("  JOIN pg_class cl on cl.oid=st.relid\n")
	       wxT(" WHERE schemaname = ") + qtString(GetSchema()->GetName())
	    +  wxT("\n ORDER BY relname");

    pgSet *stats = GetDatabase()->ExecuteSet(sql);

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


///////////////////////////////////////////////////////////


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
            +  wxT(", CASE WHEN cl.reltoastrelid = 0 THEN ") + qtString(_("none")) + wxT(" ELSE pg_size_pretty(pg_relation_size(cl.reltoastrelid)+ COALESCE((SELECT SUM(pg_relation_size(indexrelid)) FROM pg_index WHERE indrelid=cl.reltoastrelid)::int8, 0)) END AS ") + qtIdent(_("Toast Table Size"))
            +  wxT(", pg_size_pretty(COALESCE((SELECT SUM(pg_relation_size(indexrelid)) FROM pg_index WHERE indrelid=stat.relid)::int8, 0)) AS ") + qtIdent(_("Indexes Size"));
    }
    sql +=  wxT("\n")
        wxT("  FROM pg_stat_all_tables stat\n")
        wxT("  JOIN pg_statio_all_tables statio ON stat.relid = statio.relid\n")
        wxT("  JOIN pg_class cl ON cl.oid=stat.relid\n")
        wxT(" WHERE stat.relid = ") + GetOidStr();

    DisplayStatistics(statistics, sql);
}


pgObject *pgTableFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
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


#include "images/table.xpm"
#include "images/tables.xpm"

pgTableFactory::pgTableFactory() 
: pgSchemaObjFactory(__("Table"), _("New Table"), _("Create a new Table."), table_xpm)
{
    metaType = PGM_TABLE;
}

pgCollection *pgTableFactory::CreateCollection(pgObject *obj)
{
    return new pgTableCollection(GetCollectionFactory(), (pgSchema*)obj);
}

pgTableFactory tableFactory;
static pgaCollectionFactory cf(&tableFactory, __("Tables"), tables_xpm);


pgCollection *pgTableObjFactory::CreateCollection(pgObject *obj)
{
    return new pgTableObjCollection(GetCollectionFactory(), (pgTable*)obj);
}
