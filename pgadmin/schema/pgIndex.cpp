//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgIndex.cpp - Index class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "utils/misc.h"
#include "utils/pgfeatures.h"
#include "schema/pgIndex.h"
#include "schema/pgConstraints.h"
#include "schema/pgIndexConstraint.h"


pgIndexBase::pgIndexBase(pgTable *newTable, pgaFactory &factory, const wxString &newName)
	: pgTableObject(newTable, factory, newName)
{
	showExtendedStatistics = false;
}

bool pgIndexBase::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP INDEX ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgIndexBase::GetCreate()
{
	wxString str;
// no functional indexes so far

	str = wxT("CREATE ");
	if (GetIsUnique())
		str += wxT("UNIQUE ");
	str += wxT("INDEX ");
	str += qtIdent(GetName())
	       + wxT("\n  ON ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
	       + wxT("\n  USING ") + GetIndexType()
	       + wxT("\n  (");
	if (GetProcName().IsNull())
		str += GetQuotedColumns();
	else
	{
		str += GetQuotedSchemaPrefix(GetProcNamespace()) + qtIdent(GetProcName()) + wxT("(") + GetQuotedColumns() + wxT(")");
		if (!this->GetOperatorClasses().IsNull())
			str += wxT(" ") + GetOperatorClasses();
	}

	str += wxT(")");

	if (GetConnection()->BackendMinimumVersion(8, 2) && GetFillFactor().Length() > 0)
		str += wxT("\n  WITH (FILLFACTOR=") + GetFillFactor() + wxT(")");

	if (GetConnection()->BackendMinimumVersion(8, 0) && tablespace != GetDatabase()->GetDefaultTablespace())
		str += wxT("\nTABLESPACE ") + qtIdent(tablespace);

	AppendIfFilled(str, wxT("\n  WHERE "), GetConstraint());

	str += wxT(";\n");

	if (GetConnection()->BackendMinimumVersion(7, 5))
		if (GetIsClustered())
			str += wxT("ALTER TABLE ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
			       +  wxT(" CLUSTER ON ") + qtIdent(GetName())
			       + wxT(";\n");

	return str;
}


wxString pgIndexBase::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Index: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP INDEX ") + GetQuotedFullIdentifier() + wxT(";\n\n")
		      + GetCreate()
		      + GetCommentSql();
	}
	return sql;
}



void pgIndexBase::ReadColumnDetails()
{
	if (!expandedKids)
	{
		expandedKids = true;

		// Allocate memory to store column def
		if (columnCount > 0) columnList.Alloc(columnCount);

		if (GetConnection()->BackendMinimumVersion(7, 4))
		{
			long i;

			for (i = 1 ; i <= columnCount ; i++)
			{
				if (i > 1)
				{
					columns += wxT(", ");
					quotedColumns += wxT(", ");
				}

				wxString options, coldef;
				if (GetConnection()->BackendMinimumVersion(8, 3))
					options = wxT("  i.indoption[") + NumToStr((long)(i - 1)) + wxT("] AS options,\n");

				pgSet *res = ExecuteSet(
				                 wxT("SELECT\n") + options +
				                 wxT("  CASE WHEN (o.opcdefault = FALSE) THEN\n")
				                 wxT("    pg_get_indexdef(i.indexrelid, ") + NumToStr(i) + GetDatabase()->GetPrettyOption() + wxT(") || ' ' || o.opcname\n") +
				                 wxT("  ELSE\n") +
				                 wxT("    pg_get_indexdef(i.indexrelid, ") + NumToStr(i) + GetDatabase()->GetPrettyOption() + wxT(")\n") +
				                 wxT("  END AS coldef\n") +
				                 wxT("FROM pg_index i\n") +
				                 wxT("JOIN pg_attribute a ON (a.attrelid = i.indexrelid AND attnum = ") + NumToStr(i) + wxT(")\n") +
				                 wxT("LEFT OUTER JOIN pg_opclass o ON (o.oid = i.indclass[") + NumToStr((long)(i - 1)) + wxT("])\n") +
				                 wxT("WHERE i.indexrelid = ") + GetOidStr());

				if (res->NumRows() > 0)
				{
					coldef = res->GetVal(wxT("coldef"));

					// Get the column options
					if (GetConnection()->BackendMinimumVersion(8, 3))
					{
						long opt = res->GetLong(wxT("options"));

						if (opt && (opt & 0x0001)) // Descending...
						{
							coldef += wxT(" DESC");
							// NULLS FIRST is the default for descending
							if (!(opt && (opt & 0x0002)))
								coldef += wxT(" NULLS LAST");
						}
						else // Ascending...
						{
							if ((opt && (opt & 0x0002)))
								coldef += wxT(" NULLS FIRST");
						}
					}
				}

				columns += coldef;
				quotedColumns += coldef;
				columnList.Add(coldef);
			}
		}
		else
		{
			// its a 7.3 db

			// We cannot use SELECT IN (colNumbers) here because we couldn't be sure
			// about the read order
			wxStringTokenizer collist(GetColumnNumbers());
			wxStringTokenizer args(procArgTypeList);
			wxString cn, ct;
			columnCount = 0;

			while (collist.HasMoreTokens())
			{
				cn = collist.GetNextToken();
				ct = args.GetNextToken();

				pgSet *colSet = ExecuteSet(
				                    wxT("SELECT attname as conattname\n")
				                    wxT("  FROM pg_attribute\n")
				                    wxT(" WHERE attrelid=") + GetTableOidStr() + wxT(" AND attnum=") + cn);
				if (colSet)
				{
					if (columnCount)
					{
						columns += wxT(", ");
						quotedColumns += wxT(", ");
					}
					wxString colName = colSet->GetVal(0);
					columns += colName;
					columnList.Add(colName);
					quotedColumns += qtIdent(colName);

					if (!ct.IsNull())
					{
						pgSet *typeSet = ExecuteSet(wxT(
						                                "SELECT typname FROM pg_type where oid=") + ct);
						if (typeSet)
						{
							if (columnCount)
							{
								procArgs += wxT(", ");
								typedColumns += wxT(", ");
								quotedTypedColumns += wxT(", ");
							}
							wxString colType = typeSet->GetVal(0);
							procArgs += colType;
							typedColumns += colName + wxT("::") + colType;
							quotedTypedColumns += qtIdent(colName) + wxT("::") + colType;
							delete typeSet;
						}
					}
					delete colSet;
				}
				columnCount++;
			}
		}
		wxStringTokenizer ops(operatorClassList);
		wxString op;
		while (ops.HasMoreTokens())
		{
			op = ops.GetNextToken();
			pgSet *set = ExecuteSet(wxT(
			                            "SELECT opcname FROM pg_opclass WHERE oid=") + op);
			if (set)
			{
				if (!operatorClasses.IsNull())
					operatorClasses += wxT(", ");
				operatorClasses += set->GetVal(0);
				delete set;
			}
		}
	}
}


void pgIndexBase::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	ReadColumnDetails();
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		if (GetConnection()->BackendMinimumVersion(8, 0))
			properties->AppendItem(_("Tablespace"), tablespace);
		if (!GetProcName().IsNull())
			properties->AppendItem(_("Procedure "), GetSchemaPrefix(GetProcNamespace()) + GetProcName() + wxT("(") + GetTypedColumns() + wxT(")"));
		else
			properties->AppendItem(_("Columns"), GetColumns());

		properties->AppendItem(_("Operator classes"), GetOperatorClasses());
		properties->AppendItem(_("Unique?"), GetIsUnique());
		properties->AppendItem(_("Primary?"), GetIsPrimary());
		properties->AppendItem(_("Clustered?"), GetIsClustered());
		properties->AppendItem(_("Access method"), GetIndexType());
		properties->AppendItem(_("Constraint"), GetConstraint());
		properties->AppendItem(_("System index?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(8, 2))
			properties->AppendItem(_("Fill factor"), GetFillFactor());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}


void pgIndexBase::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxString sql =
	    wxT("SELECT idx_scan AS ") + qtIdent(_("Index Scans")) +
	    wxT(", idx_tup_read AS ") + qtIdent(_("Index Tuples Read")) +
	    wxT(", idx_tup_fetch AS ") + qtIdent(_("Index Tuples Fetched")) +
	    wxT(", idx_blks_read AS ") + qtIdent(_("Index Blocks Read")) +
	    wxT(", idx_blks_hit AS ") + qtIdent(_("Index Blocks Hit"));

	if (GetConnection()->HasFeature(FEATURE_SIZE))
		sql += wxT(", pg_size_pretty(pg_relation_size(") + GetOidStr() + wxT(")) AS ") + qtIdent(_("Index Size"));

	if (showExtendedStatistics)
	{
		sql += wxT(", version AS ") + qtIdent(_("Version")) + wxT(",\n")
		       wxT("  tree_level AS ") + qtIdent(_("Tree Level")) + wxT(",\n")
		       wxT("  pg_size_pretty(index_size) AS ") + qtIdent(_("Index Size")) + wxT(",\n")
		       wxT("  root_block_no AS ") + qtIdent(_("Root Block No")) + wxT(",\n")
		       wxT("  internal_pages AS ") + qtIdent(_("Internal Pages")) + wxT(",\n")
		       wxT("  leaf_pages AS ") + qtIdent(_("Leaf Pages")) + wxT(",\n")
		       wxT("  empty_pages AS ") + qtIdent(_("Empty Pages")) + wxT(",\n")
		       wxT("  deleted_pages AS ") + qtIdent(_("Deleted Pages")) + wxT(",\n")
		       wxT("  avg_leaf_density AS ") + qtIdent(_("Average Leaf Density")) + wxT(",\n")
		       wxT("  leaf_fragmentation AS ") + qtIdent(_("Leaf Fragmentation")) + wxT("\n")
		       wxT("  FROM pgstatindex('") + GetQuotedFullIdentifier() + wxT("'), pg_stat_all_indexes stat");
	}
	else
	{
		sql += wxT("\n")
		       wxT("  FROM pg_stat_all_indexes stat");
	}
	sql +=  wxT("\n")
	        wxT("  JOIN pg_statio_all_indexes statio ON stat.indexrelid = statio.indexrelid\n")
	        wxT("  JOIN pg_class cl ON cl.oid=stat.indexrelid\n")
	        wxT(" WHERE stat.indexrelid = ") + GetOidStr();

	DisplayStatistics(statistics, sql);
}


pgObject *pgIndexBase::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *index = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		index = indexFactory.CreateObjects(coll, 0, wxT("\n   AND cls.oid=") + GetOidStr());

	return index;
}


bool pgIndexBase::HasPgstatindex()
{
	return GetConnection()->HasFeature(FEATURE_PGSTATINDEX);
}


executePgstatindexFactory::executePgstatindexFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Extended index statistics"), _("Get extended statistics via pgstatindex for the selected object."), wxITEM_CHECK);
}


wxWindow *executePgstatindexFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (!((pgIndexBase *)obj)->GetShowExtendedStatistics())
	{
		((pgIndexBase *)obj)->iSetShowExtendedStatistics(true);
		wxTreeItemId item = form->GetBrowser()->GetSelection();
		if (obj == form->GetBrowser()->GetObject(item))
			form->SelectStatisticsTab();
	}
	else
		((pgIndexBase *)obj)->iSetShowExtendedStatistics(false);

	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), (ctlMenuToolbar *)form->GetToolBar());

	return 0;
}


bool executePgstatindexFactory::CheckEnable(pgObject *obj)
{
	return obj &&
	       (obj->IsCreatedBy(indexFactory) || obj->IsCreatedBy(primaryKeyFactory) || obj->IsCreatedBy(uniqueFactory)) &&
	       ((pgIndexBase *)obj)->HasPgstatindex();
}

bool executePgstatindexFactory::CheckChecked(pgObject *obj)
{
	if (!obj)
		return false;

	if (obj->GetMetaType() == PGM_INDEX || obj->GetMetaType() == PGM_PRIMARYKEY || obj->GetMetaType() == PGM_UNIQUE)
		return ((pgIndexBase *)obj)->GetShowExtendedStatistics();

	return false;
}


pgIndex::pgIndex(pgTable *newTable, const wxString &newName)
	: pgIndexBase(newTable, indexFactory, newName)
{
}


pgObject *pgIndexBaseFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	pgTableObjCollection *collection = (pgTableObjCollection *)coll;
	pgIndexBase *index = 0;
	wxString query;

	wxString proname, projoin;
	if (collection->GetConnection()->BackendMinimumVersion(7, 4))
	{
		proname = wxT("indnatts, ");
		if (collection->GetConnection()->BackendMinimumVersion(7, 5))
		{
			proname += wxT("cls.reltablespace AS spcoid, spcname, ");
			projoin = wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=cls.reltablespace\n");
		}
	}
	else
	{
		proname = wxT("proname, pn.nspname as pronspname, proargtypes, ");
		projoin =   wxT("  LEFT OUTER JOIN pg_proc pr ON pr.oid=indproc\n")
		            wxT("  LEFT OUTER JOIN pg_namespace pn ON pn.oid=pr.pronamespace\n");
	}
	query = wxT("SELECT DISTINCT ON(cls.relname) cls.oid, cls.relname as idxname, indrelid, indkey, indisclustered, indisunique, indisprimary, n.nspname,\n")
	        wxT("       ") + proname + wxT("tab.relname as tabname, indclass, con.oid AS conoid, CASE contype WHEN 'p' THEN desp.description WHEN 'u' THEN desp.description ELSE des.description END AS description,\n")
	        wxT("       pg_get_expr(indpred, indrelid") + collection->GetDatabase()->GetPrettyOption() + wxT(") as indconstraint, contype, condeferrable, condeferred, amname\n");
	if (collection->GetConnection()->BackendMinimumVersion(8, 2))
		query += wxT(", substring(array_to_string(cls.reloptions, ',') from 'fillfactor=([0-9]*)') AS fillfactor \n");
	query += wxT("  FROM pg_index idx\n")
	         wxT("  JOIN pg_class cls ON cls.oid=indexrelid\n")
	         wxT("  JOIN pg_class tab ON tab.oid=indrelid\n")
	         + projoin +
	         wxT("  JOIN pg_namespace n ON n.oid=tab.relnamespace\n")
	         wxT("  JOIN pg_am am ON am.oid=cls.relam\n")
	         wxT("  LEFT JOIN pg_depend dep ON (dep.classid = cls.tableoid AND dep.objid = cls.oid AND dep.refobjsubid = '0')\n")
	         wxT("  LEFT OUTER JOIN pg_constraint con ON (con.tableoid = dep.refclassid AND con.oid = dep.refobjid)\n")
	         wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=cls.oid\n")
	         wxT("  LEFT OUTER JOIN pg_description desp ON (desp.objoid=con.oid AND desp.objsubid = 0)\n")
	         wxT(" WHERE indrelid = ") + collection->GetOidStr()
	         + restriction + wxT("\n")
	         wxT(" ORDER BY cls.relname");
	pgSet *indexes = collection->GetDatabase()->ExecuteSet(query);

	if (indexes)
	{
		while (!indexes->Eof())
		{
			switch (indexes->GetVal(wxT("contype"))[0U])
			{
				case 0:
					index = new pgIndex(collection->GetTable(), indexes->GetVal(wxT("idxname")));
					break;
				case 'p':
					index = new pgPrimaryKey(collection->GetTable(), indexes->GetVal(wxT("idxname")));
					((pgPrimaryKey *)index)->iSetConstraintOid(indexes->GetOid(wxT("conoid")));
					break;
				case 'u':
					index = new pgUnique(collection->GetTable(), indexes->GetVal(wxT("idxname")));
					((pgUnique *)index)->iSetConstraintOid(indexes->GetOid(wxT("conoid")));
					break;
				default:
					index = 0;
					break;
			}

			index->iSetOid(indexes->GetOid(wxT("oid")));
			index->iSetIsClustered(indexes->GetBool(wxT("indisclustered")));
			index->iSetIsUnique(indexes->GetBool(wxT("indisunique")));
			index->iSetIsPrimary(indexes->GetBool(wxT("indisprimary")));
			index->iSetColumnNumbers(indexes->GetVal(wxT("indkey")));
			index->iSetIdxSchema(indexes->GetVal(wxT("nspname")));
			index->iSetComment(indexes->GetVal(wxT("description")));
			index->iSetIdxTable(indexes->GetVal(wxT("tabname")));
			index->iSetRelTableOid(indexes->GetOid(wxT("indrelid")));
			if (collection->GetConnection()->BackendMinimumVersion(7, 4))
			{
				index->iSetColumnCount(indexes->GetLong(wxT("indnatts")));
				if (collection->GetConnection()->BackendMinimumVersion(8, 0))
				{
					if (indexes->GetOid(wxT("spcoid")) == 0)
						index->iSetTablespaceOid(collection->GetDatabase()->GetTablespaceOid());
					else
						index->iSetTablespaceOid(indexes->GetOid(wxT("spcoid")));

					if (indexes->GetVal(wxT("spcname")) == wxEmptyString)
						index->iSetTablespace(collection->GetDatabase()->GetTablespace());
					else
						index->iSetTablespace(indexes->GetVal(wxT("spcname")));
				}

			}
			else
			{
				index->iSetColumnCount(0L);
				index->iSetProcNamespace(indexes->GetVal(wxT("pronspname")));
				index->iSetProcName(indexes->GetVal(wxT("proname")));
				index->iSetProcArgTypeList(indexes->GetVal(wxT("proargtypes")));
			}
			index->iSetOperatorClassList(indexes->GetVal(wxT("indclass")));
			index->iSetDeferrable(indexes->GetBool(wxT("condeferrable")));
			index->iSetDeferred(indexes->GetBool(wxT("condeferred")));
			index->iSetConstraint(indexes->GetVal(wxT("indconstraint")));
			index->iSetIndexType(indexes->GetVal(wxT("amname")));
			if (collection->GetConnection()->BackendMinimumVersion(8, 2))
				index->iSetFillFactor(indexes->GetVal(wxT("fillfactor")));

			if (browser)
			{
				browser->AppendObject(collection, index);
				indexes->MoveNext();
			}
			else
				break;
		}

		delete indexes;
	}
	return index;
}


pgCollection *pgIndexBaseFactory::CreateCollection(pgObject *obj)
{
	return new pgIndexBaseCollection(GetCollectionFactory(), (pgTable *)obj);
}



pgObject *pgIndexFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	return pgIndexBaseFactory::CreateObjects(collection, browser, restriction + wxT("\n   AND conname IS NULL"));
}


/////////////////////////////

#include "images/index.xpm"
#include "images/indexes.xpm"

pgIndexFactory::pgIndexFactory()
	: pgIndexBaseFactory(__("Index"), __("New Index..."), __("Create a new Index."), index_xpm)
{
	metaType = PGM_INDEX;
}


pgIndexFactory indexFactory;
static pgaCollectionFactory cf(&indexFactory, __("Indexes"), indexes_xpm);


pgIndexBaseCollection::pgIndexBaseCollection(pgaFactory *factory, pgTable *tbl)
	: pgTableObjCollection(factory, tbl)
{
}


void pgIndexBaseCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxLogInfo(wxT("Displaying statistics for indexes on ") + GetTable()->GetName());

	bool hasSize = GetConnection()->HasFeature(FEATURE_SIZE);

	// Add the statistics view columns
	statistics->ClearAll();
	statistics->AddColumn(_("Index Name"));
	statistics->AddColumn(_("Index Scans"));
	statistics->AddColumn(_("Index Tuples Read"));
	statistics->AddColumn(_("Index Tuples Fetched"));
	if (hasSize)
		statistics->AddColumn(_("Size"));

	wxString sql = wxT("SELECT indexrelname, ")
	               wxT("idx_scan, idx_tup_read, idx_tup_fetch");

	if (hasSize)
		sql += wxT(", pg_size_pretty(pg_relation_size(indexrelid)) AS ") + qtIdent(wxT("size"));

	sql += wxT("\n")
	       wxT("  FROM pg_stat_all_indexes stat\n")
	       wxT("  JOIN pg_class cls ON cls.oid=indexrelid\n")
	       wxT("  LEFT JOIN pg_depend dep ON (dep.classid = cls.tableoid AND dep.objid = cls.oid AND dep.refobjsubid = '0')\n")
	       wxT("  LEFT OUTER JOIN pg_constraint con ON (con.tableoid = dep.refclassid AND con.oid = dep.refobjid)\n")
	       wxT("  WHERE schemaname = ") + qtDbString(GetTable()->GetSchema()->GetName())
	       + wxT(" AND stat.relname = ") + qtDbString(GetTable()->GetName())
	       + wxT(" AND con.contype IS NULL")
	       + wxT("\n ORDER BY indexrelname");

	pgSet *stats = GetDatabase()->ExecuteSet(sql);

	if (stats)
	{
		long pos = 0;
		while (!stats->Eof())
		{
			statistics->InsertItem(pos, stats->GetVal(wxT("indexrelname")), PGICON_STATISTICS);
			statistics->SetItem(pos, 1, stats->GetVal(wxT("idx_scan")));
			statistics->SetItem(pos, 2, stats->GetVal(wxT("idx_tup_read")));
			statistics->SetItem(pos, 3, stats->GetVal(wxT("idx_tup_fetch")));
			if (hasSize)
				statistics->SetItem(pos, 4, stats->GetVal(wxT("size")));
			stats->MoveNext();
			pos++;
		}

		delete stats;
	}
}
