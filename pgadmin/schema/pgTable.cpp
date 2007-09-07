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


#include "utils/misc.h"
#include "frm/frmHint.h"
#include "frm/frmMain.h"
#include "frm/frmMaintenance.h"
#include "schema/pgTable.h"
#include "schema/pgColumn.h"
#include "schema/pgIndexConstraint.h"
#include "schema/pgForeignKey.h"
#include "schema/pgCheck.h"
#include "utils/sysSettings.h"
#include "utils/pgfeatures.h"
#include "schema/pgRule.h"
#include "schema/pgTrigger.h"
#include "schema/pgConstraints.h"


// App headers

pgTable::pgTable(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, tableFactory, newName)
{
    inheritedTableCount=0;
    rowsCounted = false;
    showExtendedStatistics = false;
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
    wxString sql = wxT("DROP TABLE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}


void pgTable::AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory)
{
    wxString tmp;

    pgCollection *collection=browser->FindCollection(factory, GetId());
    if (collection)
    {
		tmp += wxT("\n");
        collection->ShowTreeDetail(browser);

        treeObjectIterator idxIt(browser, collection);
        pgObject *obj;
        while ((obj = idxIt.GetNextObject()) != 0)
        {
            obj->ShowTreeDetail(browser);

            tmp += obj->GetSql(browser) + wxT("\n");
        }
    }

    if (!tmp.IsEmpty() && tmp != wxT("\n"))
        sql += tmp;
}


wxString pgTable::GetSql(ctlTree *browser)
{
	wxString colDetails, conDetails;
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
                            sql += wxT(" -- ") + firstLineOnly(prevComment);

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
					sql += wxT(" -- ") + firstLineOnly(prevComment);

                sql += wxT("\n  CONSTRAINT ") + data->GetQuotedIdentifier() 
                    + wxT(" ") + data->GetTypeName().Upper() 
                    + wxT(" ") ;

                prevComment = data->GetComment();
                if (!data->GetComment().IsEmpty())
                    conDetails += wxT("COMMENT ON CONSTRAINT ") + data->GetQuotedIdentifier() + 
                                  wxT(" ON ") + GetQuotedFullIdentifier() + 
                                  wxT(" IS ") + qtDbString(data->GetComment()) + wxT(";\n");
            
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
			sql += wxT(" -- ") + firstLineOnly(prevComment);

        sql += wxT("\n)");
        if (GetInheritedTableCount())
        {
            sql += wxT("\nINHERITS (") + GetQuotedInheritedTables() + wxT(")");
        }

        if (GetConnection()->BackendMinimumVersion(8, 2))
        {
            sql += wxT("\nWITH (");
            if (GetFillFactor().Length() > 0)
                sql += wxT("FILLFACTOR=") + GetFillFactor() + wxT(", ");
            if (GetHasOids())
                sql +=  wxT("OIDS=TRUE");
            else
                sql +=  wxT("OIDS=FALSE");
            sql += wxT(")");
        }
        else
        {
            if (GetHasOids())
                sql +=  wxT("\nWITH OIDS");
            else
                sql +=  wxT("\nWITHOUT OIDS");
        }

        if (tablespace != GetDatabase()->GetDefaultTablespace())
            sql += wxT("\nTABLESPACE ") + qtIdent(tablespace);

        sql += wxT(";\n")
            + GetOwnerSql(7, 3);

        if (GetConnection()->BackendMinimumVersion(8, 2))
            sql += GetGrant(wxT("arwdxt"));
        else
            sql += GetGrant(wxT("arwdRxt"));

        sql += GetCommentSql();

		// Column/constraint comments
        if (!colDetails.IsEmpty())
		    sql += colDetails + wxT("\n");

        if (!conDetails.IsEmpty())
            sql += conDetails + wxT("\n");

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


bool pgTable::EnableTriggers(const bool b)
{
	wxString sql = wxT("ALTER TABLE ") + GetQuotedFullIdentifier() + wxT(" ");
    
	if (!b)
        sql += wxT("DISABLE");
	else
        sql += wxT("ENABLE");
        
	sql += wxT(" TRIGGER ALL");

    return GetDatabase()->ExecuteVoid(sql);
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
        if (GetConnection()->BackendMinimumVersion(8, 0))
            properties->AppendItem(_("Tablespace"), tablespace);
        properties->AppendItem(_("ACL"), GetAcl());
        if (GetPrimaryKey().IsNull())
            properties->AppendItem(_("Primary key"), _("<no primary key>"));
        else
            properties->AppendItem(_("Primary key"), GetPrimaryKey());

        properties->AppendItem(_("Rows (estimated)"), GetEstimatedRows());

        if (GetConnection()->BackendMinimumVersion(8, 2))
            properties->AppendItem(_("Fill factor"), GetFillFactor());

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


bool pgTable::HasPgstattuple()
{
    return GetConnection()->HasFeature(FEATURE_PGSTATTUPLE);
}

void pgTable::iSetTriggersEnabled(ctlTree *browser, bool enable)
{
    pgCollection *triggers=browser->FindCollection(triggerFactory, GetId());
    if (triggers)
    {
        triggers->ShowTreeDetail(browser);
        treeObjectIterator trgIt(browser, triggers);

        pgTrigger *trigger;
        while ((trigger = (pgTrigger *)trgIt.GetNextObject()) != 0)
        {
			trigger->iSetEnabled(enable);
		}
	}
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

    if (showExtendedStatistics)
    {
        sql += wxT("\n")
               wxT(", tuple_count AS ") + qtIdent(_("Tuple Count")) + wxT(",\n")
               wxT("  pg_size_pretty(tuple_len) AS ") + qtIdent(_("Tuple Length")) + wxT(",\n")
               wxT("  tuple_percent AS ") + qtIdent(_("Tuple Percent")) + wxT(",\n")
               wxT("  dead_tuple_count AS ") + qtIdent(_("Dead Tuple Count")) + wxT(",\n")
               wxT("  pg_size_pretty(dead_tuple_len) AS ") + qtIdent(_("Dead Tuple Length")) + wxT(",\n")
               wxT("  dead_tuple_percent AS ") + qtIdent(_("Dead Tuple Percent")) + wxT(",\n")
               wxT("  pg_size_pretty(free_space) AS ") + qtIdent(_("Free Space")) + wxT(",\n")
               wxT("  free_percent AS ") + qtIdent(_("Free Percent")) + wxT("\n")
               wxT("  FROM pgstattuple('") + GetQuotedFullIdentifier() + wxT("'), pg_stat_all_tables stat");
    }
    else
    {
        sql += wxT("\n")
               wxT("  FROM pg_stat_all_tables stat");
    }
    sql +=  wxT("\n")
        wxT("  JOIN pg_statio_all_tables statio ON stat.relid = statio.relid\n")
        wxT("  JOIN pg_class cl ON cl.oid=stat.relid\n")
        wxT(" WHERE stat.relid = ") + GetOidStr();


    DisplayStatistics(statistics, sql);
}


pgObject *pgTableFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    wxString query;
    pgTable *table=0;

    pgSet *tables;
    if (collection->GetConnection()->BackendMinimumVersion(8, 0))
    {
        query= wxT("SELECT rel.oid, relname, rel.reltablespace AS spcoid, spcname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
                    wxT("relhassubclass, reltuples, description, conname, conkey,\n")
            wxT("       EXISTS(select 1 FROM pg_trigger\n")
            wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
            wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
            wxT("                     WHERE tgrelid=rel.oid) AS isrepl\n");
        if (collection->GetConnection()->BackendMinimumVersion(8, 2))
            query += wxT(", substring(array_to_string(reloptions, ',') from 'fillfactor=([0-9]*)') AS fillfactor \n");
        query += wxT("  FROM pg_class rel\n")
            wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=rel.reltablespace\n")
            wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0)\n")
            wxT("  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n")
            wxT(" WHERE relkind IN ('r','s','t') AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
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
            wxT(" WHERE relkind IN ('r','s','t') AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
            + restriction + 
            wxT(" ORDER BY relname");
    }
    tables= collection->GetDatabase()->ExecuteSet(query);
    if (tables)
    {
        while (!tables->Eof())
        {
            table = new pgTable(collection->GetSchema(), tables->GetVal(wxT("relname")));

            table->iSetOid(tables->GetOid(wxT("oid")));
            table->iSetOwner(tables->GetVal(wxT("relowner")));
            table->iSetAcl(tables->GetVal(wxT("relacl")));
            if (collection->GetConnection()->BackendMinimumVersion(8, 0))
            {
				if (tables->GetOid(wxT("spcoid")) == 0)
					table->iSetTablespaceOid(collection->GetDatabase()->GetTablespaceOid());
				else
					table->iSetTablespaceOid(tables->GetOid(wxT("spcoid")));

                if (tables->GetVal(wxT("spcname")) == wxEmptyString)
                    table->iSetTablespace(collection->GetDatabase()->GetTablespace());
                else
                    table->iSetTablespace(tables->GetVal(wxT("spcname")));
            }
            table->iSetComment(tables->GetVal(wxT("description")));
            table->iSetHasOids(tables->GetBool(wxT("relhasoids")));
            table->iSetEstimatedRows(tables->GetDouble(wxT("reltuples")));
            if (collection->GetConnection()->BackendMinimumVersion(8, 2)) {
                table->iSetFillFactor(tables->GetVal(wxT("fillfactor")));
            }
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

bool pgTableObjCollection::CanCreate()
{
    if (GetTable()->GetMetaType() == PGM_VIEW)
        return false;

    return GetSchema()->GetCreatePrivilege();
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


countRowsFactory::countRowsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Count"), _("Count rows in the selected object."));
}


wxWindow *countRowsFactory::StartDialog(frmMain *form, pgObject *obj)
{
    form->StartMsg(_("Counting rows"));

    ((pgTable*)obj)->UpdateRows();
    
    wxTreeItemId item=form->GetBrowser()->GetSelection();
    if (obj == form->GetBrowser()->GetObject(item))
        obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());

    form->EndMsg();

    return 0;
}


bool countRowsFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->IsCreatedBy(tableFactory);
}


executePgstattupleFactory::executePgstattupleFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Extended statistics"), _("Get extended statistics via pgstattuple for the selected object."), wxITEM_CHECK);
}


wxWindow *executePgstattupleFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (!((pgTable*)obj)->GetShowExtendedStatistics())
	{
		((pgTable*)obj)->iSetShowExtendedStatistics(true);
		wxTreeItemId item=form->GetBrowser()->GetSelection();
		if (obj == form->GetBrowser()->GetObject(item))
			form->SelectStatisticsTab();
	}
	else
		((pgTable*)obj)->iSetShowExtendedStatistics(false);

	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), form->GetToolBar());

    return 0;
}


bool executePgstattupleFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->IsCreatedBy(tableFactory) && ((pgTable*)obj)->HasPgstattuple();
}

bool executePgstattupleFactory::CheckChecked(pgObject *obj)
{
    return obj && ((pgTable*)obj)->GetShowExtendedStatistics();
}

disableAllTriggersFactory::disableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Disable triggers"), _("Disable all triggers on the selected table."));
}


wxWindow *disableAllTriggersFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to disable all triggers on this table?"), _("Disable triggers"), wxYES_NO) == wxNO)
		return 0;
	
	if (!((pgTable*)obj)->EnableTriggers(false))
		return 0;

	((pgTable *)obj)->iSetTriggersEnabled(form->GetBrowser(), false);
	
    return 0;
}


bool disableAllTriggersFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->IsCreatedBy(tableFactory) && obj->CanEdit()
               && (obj->GetOwner() == obj->GetConnection()->GetUser() || obj->GetServer()->GetSuperUser())
               && ((pgTable*)obj)->GetConnection()->BackendMinimumVersion(8, 1);
}

enableAllTriggersFactory::enableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Enable triggers"), _("Enable all triggers on the selected table."));
}


wxWindow *enableAllTriggersFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to enable all triggers on this table?"), _("Enable triggers"), wxYES_NO) == wxNO)
		return 0;
	
	if (!((pgTable*)obj)->EnableTriggers(true))
		return 0;

	((pgTable *)obj)->iSetTriggersEnabled(form->GetBrowser(), true);

    return 0;
}


bool enableAllTriggersFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->IsCreatedBy(tableFactory) && obj->CanEdit()
               && (obj->GetOwner() == obj->GetConnection()->GetUser() || obj->GetServer()->GetSuperUser())
               && ((pgTable*)obj)->GetConnection()->BackendMinimumVersion(8, 1);
}

