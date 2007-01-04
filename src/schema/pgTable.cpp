//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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


int pgTable::GetIconId()
{
    if (isReplicated)
        return tableFactory.GetReplicatedIconId();
    else
        return tableFactory.GetIconId();
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


int pgTable::GetReplicationStatus(ctlTree *browser, wxString *clusterName, long *setId)
{
    wxArrayString clusters=GetDatabase()->GetSlonyClusters(browser);

    bool isSubscribed=false;

    size_t i;
    for (i=0 ; i < clusters.GetCount() ; i++)
    {
        wxString nsp=qtIdent(wxT("_") + clusters.Item(i));

        pgSetIterator sets(GetConnection(),
            wxT("SELECT tab_set, sub_provider, ") + nsp + wxT(".getlocalnodeid(") + qtDbString(wxT("_") + clusters.Item(i)) + wxT(") AS localnode\n")
            wxT("  FROM ") + nsp + wxT(".sl_table\n")
            wxT("  LEFT JOIN ") + nsp + wxT(".sl_subscribe ON sub_set=tab_set\n")
            wxT(" WHERE tab_reloid = ") + GetOidStr());

        if (sets.RowsLeft())
        {
            if (clusterName)
                *clusterName = clusters.Item(i);
            if (setId)
                *setId = sets.GetLong(wxT("tab_set"));
            if (isSubscribed)
                return REPLICATIONSTATUS_MULTIPLY_PUBLISHED;

            long provider=sets.GetLong(wxT("sub_provider"));
            if (provider)
            {
                if (provider != sets.GetLong(wxT("localnode")))
                    return REPLICATIONSTATUS_REPLICATED;

                isSubscribed=true;

            }
        }
    }
    if (isSubscribed)
        return REPLICATIONSTATUS_SUBSCRIBED;

    return REPLICATIONSTATUS_NONE;
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


void pgTable::AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory)
{
    pgCollection *collection=browser->FindCollection(factory, GetId());
    if (collection)
    {
		sql += wxT("\n");
        collection->ShowTreeDetail(browser);

        treeObjectIterator idxIt(browser, collection);
        pgObject *obj;
        while ((obj = idxIt.GetNextObject()) != 0)
        {
            obj->ShowTreeDetail(browser);

            sql += obj->GetSql(browser) + wxT("\n");
        }
    }
}


wxString pgTable::GetSql(ctlTree *browser)
{
	wxString colDetails;
    wxString prevComment;

    if (sql.IsNull())
    {
        // make sure all kids are appended
        ShowTreeDetail(browser);
        sql = wxT("-- Table: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP TABLE ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE TABLE ") + GetQuotedFullIdentifier() + wxT("\n(\n");

        pgCollection *columns=browser->FindCollection(columnFactory, GetId());
        if (columns)
        {
            columns->ShowTreeDetail(browser);
            treeObjectIterator colIt(browser, columns);

            int colCount=0;
            pgColumn *column;
            while ((column = (pgColumn*)colIt.GetNextObject()) != 0)
            {
                column->ShowTreeDetail(browser);
                if (column->GetColNumber() > 0)
                {
                    if (colCount)
                    {
                        sql += wxT(",");
                        if (!prevComment.IsEmpty())
                        {
                            sql += wxT(" -- ") + prevComment.BeforeFirst('\n');
                            if (prevComment.BeforeFirst('\n').Length() != prevComment.Length())
                                sql += wxT("...");
                        }

                        sql += wxT("\n");
                    }

                    if (column->GetInheritedCount() > 0)
                        sql += wxString::Format(wxT("-- %s: "), _("Inherited"));

                    sql += wxT("  ") + column->GetQuotedIdentifier() + wxT(" ")
                        + column->GetDefinition();

                    prevComment = column->GetComment();

					// Whilst we are looping round the columns, grab their comments as well.
					// Perhaps we should also get storage types here?
					colDetails += column->GetCommentSql();
					if (colDetails.Length() > 0)
						if (colDetails.Last() != '\n') colDetails += wxT("\n");

                    colCount++;
                }
            }
        }

        pgCollection *constraints=browser->FindCollection(primaryKeyFactory, GetId());
        if (constraints)
        {
            constraints->ShowTreeDetail(browser);
            treeObjectIterator consIt(browser, constraints);

            pgObject *data;
            
            while ((data=consIt.GetNextObject()) != 0)
            {
                data->ShowTreeDetail(browser);

                sql += wxT(",");

                if (!prevComment.IsEmpty())
                {
                    sql += wxT(" -- ") + prevComment.BeforeFirst('\n');
                    if (prevComment.BeforeFirst('\n').Length() != prevComment.Length())
                        sql += wxT("...");
                }


                sql += wxT("\n  CONSTRAINT ") + data->GetQuotedIdentifier() 
                    + wxT(" ") + data->GetTypeName().Upper() 
                    + wxT(" ") ;

                prevComment = data->GetComment();
            
                switch (data->GetMetaType())
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
        }
        if (!prevComment.IsEmpty())
       {
            sql += wxT(" -- ") + prevComment.BeforeFirst('\n');
            if (prevComment.BeforeFirst('\n').Length() != prevComment.Length())
                sql += wxT("...");
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
            + GetOwnerSql(7, 3);

        if (GetConnection()->BackendMinimumVersion(8, 2))
            sql += GetGrant(wxT("arwdxt"));
        else
            sql += GetGrant(wxT("arwdRxt"));

        sql += GetCommentSql();

		// Column comments
		sql += colDetails + wxT("\n");

        AppendStuff(sql, browser, indexFactory);
        AppendStuff(sql, browser, ruleFactory);
        AppendStuff(sql, browser, triggerFactory);
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
        pgObject *data=browser->GetObject(collItem);
        if (data && data->IsCollection() && (data->GetMetaType() == PGM_CONSTRAINT || data->GetMetaType() == PGM_INDEX))
        {
            wxCookieType cookie2;
            wxTreeItemId item=browser->GetFirstChild(collItem, cookie2);
            while (item)
            {
                pgIndex *index=(pgIndex*)browser->GetObject(item);
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


wxString pgTable::GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM)
{
	wxString sql;
	wxString line;
	
	int colcount=0;
    pgCollection *columns=browser->FindCollection(columnFactory, GetId());
    if (columns)
    {
        columns->ShowTreeDetail(browser);
        treeObjectIterator colIt(browser, columns);

        pgColumn *column;
        while ((column = (pgColumn*)colIt.GetNextObject()) != 0)
        {
            column->ShowTreeDetail(browser);
            if (column->GetColNumber() > 0)
            {
                if (colcount++)
				{
					line += wxT(", ");
					QMs += wxT(", ");
				}
				if (line.Length() > 60)
				{
					if (!sql.IsEmpty())
					{
						sql += wxT("\n") + wxString(' ', indent);
					}
					sql += line;
					line = wxEmptyString;
					QMs += wxT("\n") + wxString(' ', indent);
				}

				line += column->GetQuotedIdentifier();
				if (withQM)
					line += wxT("=?");
				QMs += wxT("?");
            }
        }
    }

	if (!line.IsEmpty())
	{
		if (!sql.IsEmpty())
			sql += wxT("\n") + wxString(' ', indent);
		sql += line;
	}
	return sql;
}

pgCollection *pgTable::GetColumnCollection(ctlTree *browser)
{
    pgCollection *columns=browser->FindCollection(columnFactory, GetId());
    return columns;
}

pgCollection *pgTable::GetConstraintCollection(ctlTree *browser)
{
    pgCollection *constraints=browser->FindCollection(constraintFactory, GetId());
    return constraints;
}

wxString pgTable::GetSelectSql(ctlTree *browser)
{
	wxString qms;
	wxString sql=
		wxT("SELECT ") + GetCols(browser, 7, qms, false) + wxT("\n")
		wxT("  FROM ") + GetQuotedFullIdentifier() + wxT(";\n");
	return sql;
}


wxString pgTable::GetInsertSql(ctlTree *browser)
{
	wxString qms;
	wxString sql = 
		wxT("INSERT INTO ") + GetQuotedFullIdentifier() + wxT("(\n")
		wxT("            ") + GetCols(browser, 12, qms, false) + wxT(")\n")
		wxT("    VALUES (") + qms + wxT(");\n");
	return sql;
}


wxString pgTable::GetUpdateSql(ctlTree *browser)
{
	wxString qms;
	wxString sql = 
		wxT("UPDATE ") + GetQuotedFullIdentifier() + wxT("\n")
		wxT("   SET ") + GetCols(browser, 7, qms, true) + wxT("\n")
		wxT(" WHERE <condition>;\n");
	return sql;
}


void pgTable::UpdateRows()
{
    pgSet *props = ExecuteSet(wxT("SELECT count(*) AS rows FROM ONLY ") + GetQuotedFullIdentifier());
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

        long setId;
        wxString clusterName;
        long repStat=GetReplicationStatus(browser, &clusterName, &setId);

        wxString clusterInfo;
        clusterInfo.Printf(_("Cluster \"%s\", set %ld"), clusterName.c_str(), setId);

        wxString repString;
        switch (repStat)
        {
            case REPLICATIONSTATUS_SUBSCRIBED:
                repString = _("Published");
                break;
            case REPLICATIONSTATUS_REPLICATED:
                repString = _("Replicated");
                break;
            case REPLICATIONSTATUS_MULTIPLY_PUBLISHED:
                repString = _("Replicated");
                clusterName = _("Multiple clusters");
                break;
            default:
                break;
        }
        if (!repString.IsEmpty())
            properties->AppendItem(repString, clusterInfo);

        properties->AppendItem(_("Inherits tables"), GetHasSubclass());
        properties->AppendItem(_("Inherited tables count"), GetInheritedTableCount());
        if (GetInheritedTableCount())
            properties->AppendItem(_("Inherited tables"), GetInheritedTables());
        properties->AppendItem(_("Has OIDs?"), GetHasOids());
        properties->AppendItem(_("System table?"), GetSystemObject());
        properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

    }
    if (form && GetVacuumHint() && !hintShown)
    {
        ShowHint(form, false);
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
        if (!estimatedRows || (estimatedRows == 1000 && rows.GetValue() != 1000))
            canHint = (rows >= 20);
        else
        {
            double rowsDbl=(wxLongLong_t)rows.GetValue();
            double quot=rowsDbl *10. / estimatedRows;
            canHint = ((quot > 12 || quot < 8) && (rowsDbl < estimatedRows-20. || rowsDbl > estimatedRows+20.));
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

        rc=frmHint::ShowHint(form, hints, GetFullIdentifier(), force);
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
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        table = (pgTable*)tableFactory.CreateObjects(coll, 0, wxT("\n   AND rel.oid=") + GetOidStr());

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
	       wxT(" WHERE schemaname = ") + qtDbString(GetSchema()->GetName())
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
            +  wxT(", CASE WHEN cl.reltoastrelid = 0 THEN ") + qtDbString(_("none")) + wxT(" ELSE pg_size_pretty(pg_relation_size(cl.reltoastrelid)+ COALESCE((SELECT SUM(pg_relation_size(indexrelid)) FROM pg_index WHERE indrelid=cl.reltoastrelid)::int8, 0)) END AS ") + qtIdent(_("Toast Table Size"))
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
    if (collection->GetConnection()->BackendMinimumVersion(8, 0))
    {
        tables= collection->GetDatabase()->ExecuteSet(
            wxT("SELECT rel.oid, relname, spcname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
                    wxT("relhassubclass, reltuples, description, conname, conkey,\n")
            wxT("       EXISTS(select 1 FROM pg_trigger\n")
            wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
            wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
            wxT("                     WHERE tgrelid=rel.oid) AS isrepl\n")
            wxT("  FROM pg_class rel\n")
            wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=rel.reltablespace\n")
            wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0)\n")
            wxT("  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n")
            wxT(" WHERE relkind IN ('r','s','t') AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
            + restriction + 
            wxT(" ORDER BY relname"));
    }
    else
    {
        tables= collection->GetDatabase()->ExecuteSet(
            wxT("SELECT rel.oid, relname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
                    wxT("relhassubclass, reltuples, description, conname, conkey,\n")
            wxT("       EXISTS(select 1 FROM pg_trigger\n")
            wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
            wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
            wxT("                     WHERE tgrelid=rel.oid) AS isrepl\n")
            wxT("  FROM pg_class rel\n")
            wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0)\n")
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
            table->iSetIsReplicated(tables->GetBool(wxT("isrepl")));
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
#include "images/table-repl.xpm"
#include "images/table-repl-sm.xpm"
#include "images/table-sm.xpm"
#include "images/tables.xpm"

pgTableFactory::pgTableFactory() 
: pgSchemaObjFactory(__("Table"), __("New Table..."), __("Create a new Table."), table_xpm, table_sm_xpm)
{
    metaType = PGM_TABLE;
    if (WantSmallIcon())
        replicatedIconId = addIcon(table_repl_sm_xpm);
    else
        replicatedIconId = addIcon(table_repl_xpm);
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
