//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include "schema/gpPartition.h"


// App headers

pgTable::pgTable(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, tableFactory, newName)
{
	Init();
}

pgTable::pgTable(pgSchema *newSchema, pgaFactory &newFactory, const wxString &newName)
	: pgSchemaObject(newSchema, newFactory, newName)
{
	Init();
}

pgTable::~pgTable()
{
}


void pgTable::Init()
{
	rows = 0;
	estimatedRows = 0.0;

	hasToastTable = false;
	autovacuum_enabled = 0;
	toast_autovacuum_enabled = 0;

	isPartitioned = false;
	hasOids = false;
	unlogged = false;
	hasSubclass = false;
	rowsCounted = false;
	isReplicated = false;
	showExtendedStatistics = false;
	distributionIsRandom = false;

	inheritedTableCount = 0;
	triggerCount = 0;

	tablespaceOid = 0;
	ofTypeOid = 0;
}


wxString pgTable::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on table");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing table");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop table \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop table \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop table cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop table?");
			break;
		case PROPERTIESREPORT:
			message = _("Table properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Table properties");
			break;
		case DDLREPORT:
			message = _("Table DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Table DDL");
			break;
		case DATADICTIONNARYREPORT:
			message = _("Table Data dictionary report");
			message += wxT(" - ") + GetName();
			break;
		case STATISTICSREPORT:
			message = _("Table statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Table statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Table dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Table dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Table dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Table dependents");
			break;
		case BACKUPTITLE:
			message = wxString::Format(_("Backup table \"%s\""),
			                           GetFullIdentifier().c_str());
			break;
		case RESTORETITLE:
			message = wxString::Format(_("Restore table \"%s\""),
			                           GetFullIdentifier().c_str());
			break;
	}

	return message;
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
	wxMenu *menu = pgObject::GetNewMenu();
	if (schema->GetCreatePrivilege())
	{
		columnFactory.AppendMenu(menu);
		if (GetPrimaryKey().IsEmpty())      // Will not notice if pk has been added after last refresh
			primaryKeyFactory.AppendMenu(menu);
		foreignKeyFactory.AppendMenu(menu);
		excludeFactory.AppendMenu(menu);
		uniqueFactory.AppendMenu(menu);
		checkFactory.AppendMenu(menu);
		indexFactory.AppendMenu(menu);
		ruleFactory.AppendMenu(menu);
		triggerFactory.AppendMenu(menu);

		/*
		 * TEMPORARY:  Disable adding new partitions until that code is working right.
		 *
		if (GetConnection() != 0 && GetConnection()->GetIsGreenplum() && GetIsPartitioned())
		    partitionFactory.AppendMenu(menu);
		 */
	}
	return menu;
}


int pgTable::GetReplicationStatus(ctlTree *browser, wxString *clusterName, long *setId)
{
	wxArrayString clusters = GetDatabase()->GetSlonyClusters(browser);

	bool isSubscribed = false;

	size_t i;
	for (i = 0 ; i < clusters.GetCount() ; i++)
	{
		wxString nsp = qtIdent(wxT("_") + clusters.Item(i));

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

			long provider = sets.GetLong(wxT("sub_provider"));
			if (provider)
			{
				if (provider != sets.GetLong(wxT("localnode")))
					return REPLICATIONSTATUS_REPLICATED;

				isSubscribed = true;

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


bool pgTable::Truncate(bool cascaded)
{
	wxString sql = wxT("TRUNCATE TABLE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


bool pgTable::ResetStats()
{
	wxString sql = wxT("SELECT pg_stat_reset_single_table_counters(")
	               + NumToStr(this->GetOid())
	               + wxT(")");
	return GetDatabase()->ExecuteVoid(sql);
}


void pgTable::AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory)
{
	wxString tmp;

	pgCollection *collection = browser->FindCollection(factory, GetId());
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

void pgTable::AppendStuffNoSql(wxString &sql, ctlTree *browser, pgaFactory &factory)
{
	pgCollection *collection = browser->FindCollection(factory, GetId());
	if (collection)
	{
		collection->ShowTreeDetail(browser);

		treeObjectIterator idxIt(browser, collection);
		pgObject *obj;
		while ((obj = idxIt.GetNextObject()) != 0)
		{
			obj->ShowTreeDetail(browser);
		}
	}
}



wxString pgTable::GetSql(ctlTree *browser)
{
	wxString colDetails, conDetails;
	wxString prevComment;
	wxString cols_sql = wxEmptyString;

	wxString columnPrivileges;

	if (sql.IsNull())
	{
		// make sure all kids are appended
		ShowTreeDetail(browser);
		sql = wxT("-- Table: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP TABLE ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE ");
		if (GetUnlogged())
			sql +=  wxT("UNLOGGED ");
		sql += wxT("TABLE ") + GetQuotedFullIdentifier();

		// of type (9.0 material)
		if (ofTypeOid > 0)
			sql += wxT("\nOF ") + qtIdent(ofType);

		// Get a count of the constraints.
		int consCount = 0;
		pgCollection *constraints = browser->FindCollection(primaryKeyFactory, GetId());
		if (constraints)
			consCount = browser->GetChildrenCount(constraints->GetId());

		// Get the columns
		pgCollection *columns = browser->FindCollection(columnFactory, GetId());
		if (columns)
		{
			columns->ShowTreeDetail(browser);
			treeObjectIterator colIt1(browser, columns);
			treeObjectIterator colIt2(browser, columns);


			int lastRealCol = 0;
			int currentCol = 0;
			pgColumn *column;

			// Iterate the columns to find the last 'real' one
			while ((column = (pgColumn *)colIt1.GetNextObject()) != 0)
			{
				currentCol++;

				if (column->GetInheritedCount() == 0)
					lastRealCol = currentCol;
			}

			// Now build the actual column list
			int colCount = 0;
			while ((column = (pgColumn *)colIt2.GetNextObject()) != 0)
			{
				column->ShowTreeDetail(browser);
				if (column->GetColNumber() > 0)
				{
					if (colCount)
					{
						// Only add a comma if this isn't the last 'real' column, or if there are constraints
						if (colCount != lastRealCol || consCount)
							cols_sql += wxT(",");
						if (!prevComment.IsEmpty())
							cols_sql += wxT(" -- ") + firstLineOnly(prevComment);

						cols_sql += wxT("\n");
					}

					if (column->GetInheritedCount() > 0)
					{
						if (!column->GetIsLocal())
						{
							cols_sql += wxString::Format(wxT("-- %s "), _("Inherited"))
							            + wxT("from table ") +  column->GetInheritedTableName() + wxT(":");
						}
					}

					if (ofTypeOid > 0)
					{
						if (column->GetDefinition().Length() == 0)
						{
							cols_sql += wxString::Format(wxT("-- %s "), _("Inherited"))
							            + wxT("from type ") +  ofType + wxT(": ")
							            + column->GetQuotedIdentifier();
						}
						else
						{
							cols_sql += wxT("  ") + column->GetQuotedIdentifier() + wxT(" WITH OPTIONS ")
							            + column->GetDefinition();
						}
					}
					else
					{
						cols_sql += wxT("  ") + column->GetQuotedIdentifier() + wxT(" ")
						            + column->GetDefinition();
					}

					prevComment = column->GetComment();

					// Whilst we are looping round the columns, grab their comments as well.
					colDetails += column->GetCommentSql();
					if (colDetails.Length() > 0)
						if (colDetails.Last() != '\n')
							colDetails += wxT("\n");
					colDetails += column->GetStorageSql();
					if (colDetails.Length() > 0)
						if (colDetails.Last() != '\n')
							colDetails += wxT("\n");
					colDetails += column->GetAttstattargetSql();
					if (colDetails.Length() > 0)
						if (colDetails.Last() != '\n')
							colDetails += wxT("\n");
					colDetails += column->GetVariablesSql();
					if (colDetails.Length() > 0)
						if (colDetails.Last() != '\n')
							colDetails += wxT("\n");

					colCount++;
					columnPrivileges += column->GetPrivileges();
				}
			}
		}

		// Now iterate the constraints
		if (constraints)
		{
			constraints->ShowTreeDetail(browser);
			treeObjectIterator consIt(browser, constraints);

			pgObject *data;

			while ((data = consIt.GetNextObject()) != 0)
			{
				data->ShowTreeDetail(browser);

				cols_sql += wxT(",");

				if (!prevComment.IsEmpty())
					cols_sql += wxT(" -- ") + firstLineOnly(prevComment);

				cols_sql += wxT("\n  CONSTRAINT ") + data->GetQuotedIdentifier()
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
					case PGM_EXCLUDE:
						cols_sql += ((pgIndexConstraint *)data)->GetDefinition();
						break;
					case PGM_FOREIGNKEY:
						cols_sql += ((pgForeignKey *)data)->GetDefinition();
						break;
					case PGM_CHECK:
						cols_sql += wxT("(") + ((pgCheck *)data)->GetDefinition() + wxT(")");
						if (GetDatabase()->BackendMinimumVersion(9, 2) && ((pgCheck *)data)->GetNoInherit())
							cols_sql += wxT(" NO INHERIT");
						if (GetDatabase()->BackendMinimumVersion(9, 2) && !((pgCheck *)data)->GetValid())
							cols_sql += wxT(" NOT VALID");
						break;
				}
			}
		}
		if (!prevComment.IsEmpty())
			cols_sql += wxT(" -- ") + firstLineOnly(prevComment);

		sql += wxT("\n(\n") + cols_sql + wxT("\n)");

		if (GetInheritedTableCount())
		{
			sql += wxT("\nINHERITS (") + GetQuotedInheritedTables() + wxT(")");
		}

		if (GetConnection()->BackendMinimumVersion(8, 2))
		{
			sql += wxT("\nWITH (");
			if (GetFillFactor().Length() > 0)
				sql += wxT("\n  FILLFACTOR=") + GetFillFactor() + wxT(", ");
			if (GetAppendOnly().Length() > 0)
				sql += wxT("APPENDONLY=") + GetAppendOnly() + wxT(", ");
			if (GetCompressLevel().Length() > 0)
				sql += wxT("COMPRESSLEVEL=") + GetCompressLevel() + wxT(", ");
			if (GetOrientation().Length() > 0)
				sql += wxT("ORIENTATION=") + GetOrientation() + wxT(", ");
			if (GetCompressType().Length() > 0)
				sql += wxT("COMPRESSTYPE=") + GetCompressType() + wxT(", ");
			if (GetBlocksize().Length() > 0)
				sql += wxT("BLOCKSIZE=") + GetBlocksize() + wxT(", ");
			if (GetChecksum().Length() > 0)
				sql += wxT("CHECKSUM=") + GetChecksum() + wxT(", ");
			if (GetHasOids())
				sql +=  wxT("\n  OIDS=TRUE");
			else
				sql +=  wxT("\n  OIDS=FALSE");
			if(GetConnection()->BackendMinimumVersion(8, 4))
			{
				if (GetCustomAutoVacuumEnabled())
				{
					if (GetAutoVacuumEnabled() == 1)
						sql += wxT(",\n  autovacuum_enabled=true");
					else if (GetCustomAutoVacuumEnabled() == 0)
						sql += wxT(",\n  autovacuum_enabled=false");
					if (!GetAutoVacuumVacuumThreshold().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_vacuum_threshold=") + GetAutoVacuumVacuumThreshold();
					}
					if (!GetAutoVacuumVacuumScaleFactor().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_vacuum_scale_factor=") + GetAutoVacuumVacuumScaleFactor();
					}
					if (!GetAutoVacuumAnalyzeThreshold().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_analyze_threshold=") + GetAutoVacuumAnalyzeThreshold();
					}
					if (!GetAutoVacuumAnalyzeScaleFactor().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_analyze_scale_factor=") + GetAutoVacuumAnalyzeScaleFactor();
					}
					if (!GetAutoVacuumVacuumCostDelay().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_vacuum_cost_delay=") + GetAutoVacuumVacuumCostDelay();
					}
					if (!GetAutoVacuumVacuumCostLimit().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_vacuum_cost_limit=") + GetAutoVacuumVacuumCostLimit();
					}
					if (!GetAutoVacuumFreezeMinAge().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_freeze_min_age=") + GetAutoVacuumFreezeMinAge();
					}
					if (!GetAutoVacuumFreezeMaxAge().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_freeze_max_age=") + GetAutoVacuumFreezeMaxAge();
					}
					if (!GetAutoVacuumFreezeTableAge().IsEmpty())
					{
						sql += wxT(",\n  autovacuum_freeze_table_age=") + GetAutoVacuumFreezeTableAge();
					}
				}
				if (GetHasToastTable() && GetToastCustomAutoVacuumEnabled())
				{
					if (GetToastAutoVacuumEnabled() == 1)
						sql += wxT(",\n  toast.autovacuum_enabled=true");
					else if (GetToastAutoVacuumEnabled() == 0)
						sql += wxT(",\n  toast.autovacuum_enabled=false");
					if (!GetToastAutoVacuumVacuumThreshold().IsEmpty())
					{
						sql += wxT(",\n  toast.autovacuum_vacuum_threshold=") + GetToastAutoVacuumVacuumThreshold();
					}
					if (!GetToastAutoVacuumVacuumScaleFactor().IsEmpty())
					{
						sql += wxT(",\n  toast.autovacuum_vacuum_scale_factor=") + GetToastAutoVacuumVacuumScaleFactor();
					}
					if (!GetToastAutoVacuumVacuumCostDelay().IsEmpty())
					{
						sql += wxT(",\n  toast.autovacuum_vacuum_cost_delay=") + GetToastAutoVacuumVacuumCostDelay();
					}
					if (!GetToastAutoVacuumVacuumCostLimit().IsEmpty())
					{
						sql += wxT(",\n  toast.autovacuum_vacuum_cost_limit=") + GetToastAutoVacuumVacuumCostLimit();
					}
					if (!GetToastAutoVacuumFreezeMinAge().IsEmpty())
					{
						sql += wxT(",\n  toast.autovacuum_freeze_min_age=") + GetToastAutoVacuumFreezeMinAge();
					}
					if (!GetToastAutoVacuumFreezeMaxAge().IsEmpty())
					{
						sql += wxT(",\n  toast.autovacuum_freeze_max_age=") + GetToastAutoVacuumFreezeMaxAge();
					}
					if (!GetToastAutoVacuumFreezeTableAge().IsEmpty())
					{
						sql += wxT(",\n  toast.autovacuum_freeze_table_age=") + GetToastAutoVacuumFreezeTableAge();
					}
				}
			}
			sql += wxT("\n)");
		}
		else
		{
			if (GetHasOids())
				sql +=  wxT("\nWITH OIDS");
			else
				sql +=  wxT("\nWITHOUT OIDS");
		}

		if (GetConnection()->BackendMinimumVersion(8, 0) && tablespace != GetDatabase()->GetDefaultTablespace())
			sql += wxT("\nTABLESPACE ") + qtIdent(tablespace);

		if (GetConnection()->GetIsGreenplum())
		{
			// Add Greenplum DISTRIBUTED BY
			if (distributionIsRandom)
			{
				sql += wxT("\nDISTRIBUTED RANDOMLY");
			}
			else if (GetDistributionColNumbers().Length() == 0)
			{
				// catalog table or other non-distributed table
			}
			else
			{
				// convert list of columns numbers to column names
				wxStringTokenizer collist(GetDistributionColNumbers(), wxT(","));
				wxString cn;
				wxString distributionColumns;
				while (collist.HasMoreTokens())
				{
					cn = collist.GetNextToken();
					pgSet *set = ExecuteSet(
					                 wxT("SELECT attname\n")
					                 wxT("  FROM pg_attribute\n")
					                 wxT(" WHERE attrelid=") + GetOidStr() + wxT(" AND attnum IN (") + cn + wxT(")"));
					if (set)
					{
						if (!distributionColumns.IsNull())
						{
							distributionColumns += wxT(", ");
						}
						distributionColumns += qtIdent(set->GetVal(0));
						delete set;
					}
				}

				sql += wxT("\nDISTRIBUTED BY (");
				sql += distributionColumns;

				sql += wxT(")");
			}

			if (GetIsPartitioned())
				if (GetConnection()->BackendMinimumVersion(8, 2, 9) && GetConnection()->GetIsGreenplum())
					if (GetPartitionDef().Length() == 0)
					{
						wxString query = wxT("SELECT pg_get_partition_def(");
						query += GetOidStr();
						query += wxT(", true) ");
						wxString partition_def = GetDatabase()->ExecuteScalar(query);
						iSetPartitionDef(partition_def);
						// pg_get_partition_def() doesn't work on partitions
						if (GetPartitionDef().Length() == 0)
							iSetPartitionDef(wxT("-- This partition has subpartitions"));
					}
			if (partitionDef.Length() > 0)
				sql += wxT("\n") + partitionDef + wxT("\n");


		}


		sql += wxT(";\n")
		       + GetOwnerSql(7, 3);

		if (GetConnection()->BackendMinimumVersion(8, 4))
			sql += GetGrant(wxT("arwdDxt"));
		else if (GetConnection()->BackendMinimumVersion(8, 2))
			sql += GetGrant(wxT("arwdxt"));
		else
			sql += GetGrant(wxT("arwdRxt"));

		sql += GetCommentSql();

		if (GetConnection()->BackendMinimumVersion(9, 1))
			sql += GetSeqLabelsSql();

		// Column/constraint comments
		if (!colDetails.IsEmpty())
			sql += colDetails + wxT("\n");

		if (!conDetails.IsEmpty())
			sql += conDetails + wxT("\n");

		if (!columnPrivileges.IsEmpty())
		{
			sql += columnPrivileges + wxT("\n");
		}

		AppendStuff(sql, browser, indexFactory);
		AppendStuff(sql, browser, ruleFactory);
		AppendStuff(sql, browser, triggerFactory);

		/*
		 * Disable adding partitions until that code works.
		 *
		if (partitionDef.Length() > 0)
		{
		    AppendStuffNoSql(sql, browser, partitionFactory);
		}
		 */
	}
	return sql;
}

wxString pgTable::GetCoveringIndex(ctlTree *browser, const wxString &collist)
{
	// delivers the name of the index which covers the named columns
	wxCookieType cookie;

	wxTreeItemId collItem = browser->GetFirstChild(GetId(), cookie);
	while (collItem)
	{
		pgObject *data = browser->GetObject(collItem);
		if (data && data->IsCollection() && (data->GetMetaType() == PGM_CONSTRAINT || data->GetMetaType() == PGM_INDEX))
		{
			wxCookieType cookie2;
			wxTreeItemId item = browser->GetFirstChild(collItem, cookie2);
			while (item)
			{
				pgIndex *index = (pgIndex *)browser->GetObject(item);
				if (index && (index->GetMetaType() == PGM_INDEX || index->GetMetaType() == PGM_PRIMARYKEY
				              || index->GetMetaType() == PGM_UNIQUE || index->GetMetaType() == PGM_EXCLUDE))
				{
					index->ShowTreeDetail(browser);
					if (collist == index->GetColumns() ||
					        collist + wxT(",") == index->GetColumns().Left(collist.Length() + 1))
						return index->GetName();
				}
				item = browser->GetNextChild(collItem, cookie2);
			}
		}
		collItem = browser->GetNextChild(GetId(), cookie);
	}

	return wxEmptyString;
}


wxString pgTable::GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM)
{
	wxString sql;
	wxString line;

	int colcount = 0;
	pgCollection *columns = browser->FindCollection(columnFactory, GetId());
	if (columns)
	{
		columns->ShowTreeDetail(browser);
		treeObjectIterator colIt(browser, columns);

		pgColumn *column;
		while ((column = (pgColumn *)colIt.GetNextObject()) != 0)
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
	pgCollection *columns = browser->FindCollection(columnFactory, GetId());
	return columns;
}

pgCollection *pgTable::GetConstraintCollection(ctlTree *browser)
{
	pgCollection *constraints = browser->FindCollection(constraintFactory, GetId());
	return constraints;
}

wxString pgTable::GetSelectSql(ctlTree *browser)
{
	wxString qms;
	wxString sql =
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

wxString pgTable::GetDeleteSql(ctlTree *browser)
{
	wxString sql =
	    wxT("DELETE FROM ") + GetQuotedFullIdentifier() + wxT("\n")
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
	pgSet *props = ExecuteSet(
	                   wxT("SELECT c.oid, c.relname , nspname\n")
	                   wxT("  FROM pg_inherits i\n")
	                   wxT("  JOIN pg_class c ON c.oid = i.inhparent\n")
	                   wxT("  JOIN pg_namespace n ON n.oid=c.relnamespace\n")
	                   wxT(" WHERE i.inhrelid = ") + GetOidStr() + wxT("\n")
	                   wxT(" ORDER BY inhseqno"));
	if (props)
	{
		inheritedTableCount = 0;
		inheritedTables = wxT("");
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
			inheritedTablesOidList.Add(props->GetVal(wxT("oid")));
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
		expandedKids = true;

		browser->RemoveDummyChild(this);

		// Log
		wxLogInfo(wxT("Adding child object to table %s"), GetIdentifier().c_str());

		browser->AppendCollection(this, columnFactory);
		browser->AppendCollection(this, constraintFactory);
		browser->AppendCollection(this, indexFactory);
		browser->AppendCollection(this, ruleFactory);
		browser->AppendCollection(this, triggerFactory);

		if (GetConnection() != 0 && GetConnection()->GetIsGreenplum() && GetIsPartitioned())
			browser->AppendCollection(this, partitionFactory);


		// convert list of columns numbers to column names
		wxStringTokenizer collist(GetPrimaryKeyColNumbers(), wxT(","));
		wxString cn;

		while (collist.HasMoreTokens())
		{
			cn = collist.GetNextToken();
			pgSet *set = ExecuteSet(
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
		if (GetConnection() != 0 && GetConnection()->GetIsGreenplum())
		{
			gpPartition *p = dynamic_cast<gpPartition *>(this);
			if (p != 0)
				properties->AppendItem(_("Partition Name"), p->GetPartitionName());
		}
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		if (GetConnection()->BackendMinimumVersion(8, 0))
			properties->AppendItem(_("Tablespace"), tablespace);
		properties->AppendItem(_("ACL"), GetAcl());
		if (GetConnection()->BackendMinimumVersion(9, 0))
			properties->AppendItem(_("Of type"), ofType);
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
		long repStat = GetReplicationStatus(browser, &clusterName, &setId);

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

		properties->AppendYesNoItem(_("Inherits tables"), GetHasSubclass());
		properties->AppendItem(_("Inherited tables count"), GetInheritedTableCount());
		if (GetInheritedTableCount())
			properties->AppendItem(_("Inherited tables"), GetInheritedTables());
		if (GetConnection()->BackendMinimumVersion(9, 1))
			properties->AppendYesNoItem(_("Unlogged?"), GetUnlogged());
		properties->AppendYesNoItem(_("Has OIDs?"), GetHasOids());
		properties->AppendYesNoItem(_("System table?"), GetSystemObject());

		/* Custom AutoVacuum Settings */
		if (GetConnection()->BackendMinimumVersion(8, 4) && GetCustomAutoVacuumEnabled())
		{
			if (GetAutoVacuumEnabled() != 2)
			{
				properties->AppendItem(_("Table auto-vacuum enabled?"), GetAutoVacuumEnabled() == 1 ? _("Yes") : _("No"));
			}
			if (!GetAutoVacuumVacuumThreshold().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum VACUUM base threshold"), GetAutoVacuumVacuumThreshold());
			if (!GetAutoVacuumVacuumScaleFactor().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum VACUUM scale factor"), GetAutoVacuumVacuumScaleFactor());
			if (!GetAutoVacuumAnalyzeThreshold().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum ANALYZE base threshold"), GetAutoVacuumAnalyzeThreshold());
			if (!GetAutoVacuumAnalyzeScaleFactor().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum ANALYZE scale factor"), GetAutoVacuumAnalyzeScaleFactor());
			if (!GetAutoVacuumVacuumCostDelay().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum VACUUM cost delay"), GetAutoVacuumVacuumCostDelay());
			if (!GetAutoVacuumVacuumCostLimit().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum VACUUM cost limit"), GetAutoVacuumVacuumCostLimit());
			if (!GetAutoVacuumFreezeMinAge().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum FREEZE minimum age"), GetAutoVacuumFreezeMinAge());
			if (!GetAutoVacuumFreezeMaxAge().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum FREEZE maximum age"), GetAutoVacuumFreezeMaxAge());
			if (!GetAutoVacuumFreezeTableAge().IsEmpty())
				properties->AppendItem(_("Table auto-vacuum FREEZE table age"), GetAutoVacuumFreezeTableAge());
		}

		/* Custom TOAST-TABLE AutoVacuum Settings */
		if (GetConnection()->BackendMinimumVersion(8, 4) &&
		        GetHasToastTable() &&
		        GetToastCustomAutoVacuumEnabled())
		{
			if (GetToastAutoVacuumEnabled() != 2)
			{
				properties->AppendItem(_("Toast auto-vacuum enabled?"), GetToastAutoVacuumEnabled() == 1 ? _("Yes") : _("No"));
			}
			if (!GetToastAutoVacuumVacuumThreshold().IsEmpty())
				properties->AppendItem(_("Toast auto-vacuum VACUUM base threshold"), GetToastAutoVacuumVacuumThreshold());
			if (!GetToastAutoVacuumVacuumScaleFactor().IsEmpty())
				properties->AppendItem(_("Toast auto-vacuum VACUUM scale factor"), GetToastAutoVacuumVacuumScaleFactor());
			if (!GetToastAutoVacuumVacuumCostDelay().IsEmpty())
				properties->AppendItem(_("Toast auto-vacuum VACUUM cost delay"), GetToastAutoVacuumVacuumCostDelay());
			if (!GetToastAutoVacuumVacuumCostLimit().IsEmpty())
				properties->AppendItem(_("Toast auto-vacuum VACUUM cost limit"), GetToastAutoVacuumVacuumCostLimit());
			if (!GetToastAutoVacuumFreezeMinAge().IsEmpty())
				properties->AppendItem(_("Toast auto-vacuum FREEZE minimum age"), GetToastAutoVacuumFreezeMinAge());
			if (!GetToastAutoVacuumFreezeMaxAge().IsEmpty())
				properties->AppendItem(_("Toast auto-vacuum FREEZE maximum age"), GetToastAutoVacuumFreezeMaxAge());
			if (!GetToastAutoVacuumFreezeTableAge().IsEmpty())
				properties->AppendItem(_("Toast auto-vacuum FREEZE table age"), GetToastAutoVacuumFreezeTableAge());
		}
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

		if (!GetLabels().IsEmpty())
		{
			wxArrayString seclabels = GetProviderLabelArray();
			if (seclabels.GetCount() > 0)
			{
				for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
				{
					properties->AppendItem(seclabels.Item(index), seclabels.Item(index + 1));
				}
			}
		}
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
	bool canHint = false;

	if (rowsCounted)
	{
		if (!estimatedRows || (estimatedRows == 1000 && rows.GetValue() != 1000))
			canHint = (rows >= 20);
		else
		{
			double rowsDbl = (wxLongLong_t)rows.GetValue();
			double quot = rowsDbl * 10. / estimatedRows;
			canHint = ((quot > 12 || quot < 8) && (rowsDbl < estimatedRows - 20. || rowsDbl > estimatedRows + 20.));
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

		rc = frmHint::ShowHint(form, hints, GetFullIdentifier(), force);
	}
	else
		rc = frmHint::ShowHint(form, HINT_VACUUM, GetFullIdentifier(), force);

	if (rc == HINT_RC_FIX)
	{
		frmMaintenance *frm = new frmMaintenance(form, this);
		frm->Go();
	}
}




pgObject *pgTable::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgTable *table = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		table = (pgTable *)tableFactory.CreateObjects(coll, 0, wxT("\n   AND rel.oid=") + GetOidStr());

	return table;
}


bool pgTable::HasPgstattuple()
{
	return GetConnection()->HasFeature(FEATURE_PGSTATTUPLE);
}

void pgTable::iSetTriggersEnabled(ctlTree *browser, bool enable)
{
	pgCollection *triggers = browser->FindCollection(triggerFactory, GetId());
	if (triggers)
	{
		triggers->ShowTreeDetail(browser);
		treeObjectIterator trgIt(browser, triggers);

		pgTrigger *trigger;
		while ((trigger = (pgTrigger *)trgIt.GetNextObject()) != 0)
		{
			trigger->SetEnabled(browser, enable);
		}
	}
}

///////////////////////////////////////////////////////////


pgTableCollection::pgTableCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgTableCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on tables");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing tables");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for tables");
			break;
		case STATISTICSREPORT:
			message = _("Tables statistics report");
			break;
		case OBJSTATISTICS:
			message = _("Tables statistics");
			break;
		case OBJECTSLISTREPORT:
			message = _("Tables list report");
			break;
	}

	return message;
}


void pgTableCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxLogInfo(wxT("Displaying statistics for tables on %s"), GetSchema()->GetIdentifier().c_str());

	bool hasSize = GetConnection()->HasFeature(FEATURE_SIZE);

	// Add the statistics view columns
	statistics->ClearAll();
	statistics->AddColumn(_("Table Name"));
	statistics->AddColumn(_("Tuples inserted"));
	statistics->AddColumn(_("Tuples updated"));
	statistics->AddColumn(_("Tuples deleted"));
	if (GetConnection()->BackendMinimumVersion(8, 3))
	{
		statistics->AddColumn(_("Tuples HOT updated"));
		statistics->AddColumn(_("Live tuples"));
		statistics->AddColumn(_("Dead tuples"));
	}
	if (GetConnection()->BackendMinimumVersion(8, 2))
	{
		statistics->AddColumn(_("Last vacuum"));
		statistics->AddColumn(_("Last autovacuum"));
		statistics->AddColumn(_("Last analyze"));
		statistics->AddColumn(_("Last autoanalyze"));
	}
	if (GetConnection()->BackendMinimumVersion(9, 1))
	{
		statistics->AddColumn(_("Vacuum counter"));
		statistics->AddColumn(_("Autovacuum counter"));
		statistics->AddColumn(_("Analyze counter"));
		statistics->AddColumn(_("Autoanalyze counter"));
	}
	if (hasSize)
		statistics->AddColumn(_("Size"), 50);

	wxString sql = wxT("SELECT st.relname, n_tup_ins, n_tup_upd, n_tup_del");
	if (GetConnection()->BackendMinimumVersion(8, 3))
		sql += wxT(", n_tup_hot_upd, n_live_tup, n_dead_tup");
	if (GetConnection()->BackendMinimumVersion(8, 2))
		sql += wxT(", last_vacuum, last_autovacuum, last_analyze, last_autoanalyze");
	if (GetConnection()->BackendMinimumVersion(9, 1))
		sql += wxT(", vacuum_count, autovacuum_count, analyze_count, autoanalyze_count");
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
		long pos = 0;
		int i;
		while (!stats->Eof())
		{
			i = 4;
			statistics->InsertItem(pos, stats->GetVal(wxT("relname")), PGICON_STATISTICS);
			statistics->SetItem(pos, 1, stats->GetVal(wxT("n_tup_ins")));
			statistics->SetItem(pos, 2, stats->GetVal(wxT("n_tup_upd")));
			statistics->SetItem(pos, 3, stats->GetVal(wxT("n_tup_del")));
			if (GetConnection()->BackendMinimumVersion(8, 3))
			{
				statistics->SetItem(pos, i++, stats->GetVal(wxT("n_tup_hot_upd")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("n_live_tup")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("n_dead_tup")));
			}
			if (GetConnection()->BackendMinimumVersion(8, 2))
			{
				statistics->SetItem(pos, i++, stats->GetVal(wxT("last_vacuum")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("last_autovacuum")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("last_analyze")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("last_autoanalyze")));
			}
			if (GetConnection()->BackendMinimumVersion(9, 1))
			{
				statistics->SetItem(pos, i++, stats->GetVal(wxT("vacuum_count")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("autovacuum_count")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("analyze_count")));
				statistics->SetItem(pos, i++, stats->GetVal(wxT("autoanalyze_count")));
			}
			if (hasSize)
				statistics->SetItem(pos, i, stats->GetVal(wxT("size")));
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
	    wxT(", idx_tup_fetch AS ") + qtIdent(_("Index Tuples Fetched")) +
	    wxT(", n_tup_ins AS ") + qtIdent(_("Tuples Inserted")) +
	    wxT(", n_tup_upd AS ") + qtIdent(_("Tuples Updated")) +
	    wxT(", n_tup_del AS ") + qtIdent(_("Tuples Deleted"));

	if (GetConnection()->BackendMinimumVersion(8, 3))
	{
		sql +=
		    wxT(", n_tup_hot_upd AS ") + qtIdent(_("Tuples HOT Updated")) +
		    wxT(", n_live_tup AS ") + qtIdent(_("Live Tuples")) +
		    wxT(", n_dead_tup AS ") + qtIdent(_("Dead Tuples"));
	}

	sql +=   wxT(", heap_blks_read AS ") + qtIdent(_("Heap Blocks Read")) +
	         wxT(", heap_blks_hit AS ") + qtIdent(_("Heap Blocks Hit")) +
	         wxT(", idx_blks_read AS ") + qtIdent(_("Index Blocks Read")) +
	         wxT(", idx_blks_hit AS ") + qtIdent(_("Index Blocks Hit")) +
	         wxT(", toast_blks_read AS ") + qtIdent(_("Toast Blocks Read")) +
	         wxT(", toast_blks_hit AS ") + qtIdent(_("Toast Blocks Hit")) +
	         wxT(", tidx_blks_read AS ") + qtIdent(_("Toast Index Blocks Read")) +
	         wxT(", tidx_blks_hit AS ") + qtIdent(_("Toast Index Blocks Hit"));

	if (GetConnection()->BackendMinimumVersion(8, 2))
	{
		sql +=
		    wxT(", last_vacuum AS ") + qtIdent(_("Last Vacuum")) +
		    wxT(", last_autovacuum AS ") + qtIdent(_("Last Autovacuum")) +
		    wxT(", last_analyze AS ") + qtIdent(_("Last Analyze")) +
		    wxT(", last_autoanalyze AS ") + qtIdent(_("Last Autoanalyze"));
	}

	if (GetConnection()->BackendMinimumVersion(9, 1))
	{
		sql +=
		    wxT(", vacuum_count AS ") + qtIdent(_("Vacuum counter")) +
		    wxT(", autovacuum_count AS ") + qtIdent(_("Autovacuum counter")) +
		    wxT(", analyze_count AS ") + qtIdent(_("Analyze counter")) +
		    wxT(", autoanalyze_count AS ") + qtIdent(_("Autoanalyze counter"));
	}

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
	pgTable *table = 0;

	// Greenplum returns reltuples and relpages as tuples per segmentDB and pages per segmentDB,
	// so we need to multiply them by the number of segmentDBs to get reasonable values.
	long gp_segments = 1;
	if (collection->GetConnection()->GetIsGreenplum())
	{
		query = wxT("SELECT count(*) AS gp_segments from pg_catalog.gp_configuration where definedprimary = 't' and content >= 0");
		gp_segments = StrToLong(collection->GetDatabase()->ExecuteScalar(query));
		if (gp_segments <= 1)
			gp_segments = 1;
	}

	pgSet *tables;
	if (collection->GetConnection()->BackendMinimumVersion(8, 0))
	{
		query = wxT("SELECT rel.oid, rel.relname, rel.reltablespace AS spcoid, spc.spcname, pg_get_userbyid(rel.relowner) AS relowner, rel.relacl, rel.relhasoids, ")
		        wxT("rel.relhassubclass, rel.reltuples, des.description, con.conname, con.conkey,\n")
		        wxT("       EXISTS(select 1 FROM pg_trigger\n")
		        wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
		        wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
		        wxT("                     WHERE tgrelid=rel.oid) AS isrepl,\n");

		if (collection->GetConnection()->BackendMinimumVersion(9, 0))
		{
			query += wxT("       (select count(*) FROM pg_trigger\n")
			         wxT("                     WHERE tgrelid=rel.oid AND tgisinternal = FALSE) AS triggercount\n");
		}
		else
		{
			query += wxT("       (select count(*) FROM pg_trigger\n")
			         wxT("                     WHERE tgrelid=rel.oid AND tgisconstraint = FALSE) AS triggercount\n");
		}

		if (collection->GetConnection()->BackendMinimumVersion(9, 1))
			query += wxT(", rel.relpersistence \n");
		if (collection->GetConnection()->BackendMinimumVersion(8, 2))
			query += wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'fillfactor=([0-9]*)') AS fillfactor \n");
		if (collection->GetConnection()->GetIsGreenplum())
		{
			query += wxT(", gpd.localoid, gpd.attrnums \n");
			query += wxT(", substring(array_to_string(rel.reloptions, ',') from 'appendonly=([a-z]*)') AS appendonly \n");
			query += wxT(", substring(array_to_string(rel.reloptions, ',') from 'compresslevel=([0-9]*)') AS compresslevel \n");
			query += wxT(", substring(array_to_string(rel.reloptions, ',') from 'orientation=([a-z]*)') AS orientation \n");
			query += wxT(", substring(array_to_string(rel.reloptions, ',') from 'compresstype=([a-z0-9]*)') AS compresstype \n");
			query += wxT(", substring(array_to_string(rel.reloptions, ',') from 'blocksize=([0-9]*)') AS blocksize \n");
			query += wxT(", substring(array_to_string(rel.reloptions, ',') from 'checksum=([a-z]*)') AS checksum \n");
			if (collection->GetConnection()->GetIsGreenplum() && collection->GetConnection()->BackendMinimumVersion(8, 2, 9))
				query += wxT(", rel.oid in (select parrelid from pg_partition) as ispartitioned\n");
		}
		else if (collection->GetConnection()->BackendMinimumVersion(8, 4))
		{
			query += wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_enabled=([a-z|0-9]*)') AS autovacuum_enabled \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_vacuum_threshold=([0-9]*)') AS autovacuum_vacuum_threshold \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_vacuum_scale_factor=([0-9]*[.][0-9]*)') AS autovacuum_vacuum_scale_factor \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_analyze_threshold=([0-9]*)') AS autovacuum_analyze_threshold \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_analyze_scale_factor=([0-9]*[.][0-9]*)') AS autovacuum_analyze_scale_factor \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_vacuum_cost_delay=([0-9]*)') AS autovacuum_vacuum_cost_delay \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_vacuum_cost_limit=([0-9]*)') AS autovacuum_vacuum_cost_limit \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_freeze_min_age=([0-9]*)') AS autovacuum_freeze_min_age \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_freeze_max_age=([0-9]*)') AS autovacuum_freeze_max_age \n")
			         wxT(", substring(array_to_string(rel.reloptions, ',') FROM 'autovacuum_freeze_table_age=([0-9]*)') AS autovacuum_freeze_table_age \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_enabled=([a-z|0-9]*)') AS toast_autovacuum_enabled \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_threshold=([0-9]*)') AS toast_autovacuum_vacuum_threshold \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_scale_factor=([0-9]*[.][0-9]*)') AS toast_autovacuum_vacuum_scale_factor \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_analyze_threshold=([0-9]*)') AS toast_autovacuum_analyze_threshold \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_analyze_scale_factor=([0-9]*[.][0-9]*)') AS toast_autovacuum_analyze_scale_factor \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_cost_delay=([0-9]*)') AS toast_autovacuum_vacuum_cost_delay \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_cost_limit=([0-9]*)') AS toast_autovacuum_vacuum_cost_limit \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_freeze_min_age=([0-9]*)') AS toast_autovacuum_freeze_min_age \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_freeze_max_age=([0-9]*)') AS toast_autovacuum_freeze_max_age \n")
			         wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_freeze_table_age=([0-9]*)') AS toast_autovacuum_freeze_table_age \n")
			         wxT(", rel.reloptions AS reloptions, tst.reloptions AS toast_reloptions \n")
			         wxT(", (CASE WHEN rel.reltoastrelid = 0 THEN false ELSE true END) AS hastoasttable\n");
		}
		if (collection->GetConnection()->BackendMinimumVersion(9, 0))
			query += wxT(", rel.reloftype, typ.typname\n");
		if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
		{
			query += wxT(",\n(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=rel.oid AND sl1.objsubid=0) AS labels");
			query += wxT(",\n(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=rel.oid AND sl2.objsubid=0) AS providers");
		}

		query += wxT("  FROM pg_class rel\n")
		         wxT("  LEFT OUTER JOIN pg_tablespace spc on spc.oid=rel.reltablespace\n")
		         wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0 AND des.classoid='pg_class'::regclass)\n")
		         wxT("  LEFT OUTER JOIN pg_constraint con ON con.conrelid=rel.oid AND con.contype='p'\n");

		// Add the toast table for vacuum parameters.
		if (collection->GetConnection()->BackendMinimumVersion(8, 4))
			query += wxT("  LEFT OUTER JOIN pg_class tst ON tst.oid = rel.reltoastrelid\n");

		if (collection->GetConnection()->GetIsGreenplum())
			query += wxT("  LEFT OUTER JOIN gp_distribution_policy gpd ON gpd.localoid=rel.oid\n");

		if (collection->GetConnection()->BackendMinimumVersion(9, 0))
			query += wxT("LEFT JOIN pg_type typ ON rel.reloftype=typ.oid\n");

		query += wxT(" WHERE rel.relkind IN ('r','s','t') AND rel.relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n");

		// Greenplum: Eliminate (sub)partitions from the display, only show the parent partitioned table
		// and eliminate external tables
		if (collection->GetConnection()->GetIsGreenplum() && collection->GetConnection()->BackendMinimumVersion(8, 2, 9))
			query += wxT("AND rel.relstorage <> 'x' AND rel.oid NOT IN (SELECT parchildrelid from pg_partition_rule)");

		query += restriction +
		         wxT(" ORDER BY rel.relname");
	}
	else
	{
		query = wxT("SELECT rel.oid, rel.relname, pg_get_userbyid(rel.relowner) AS relowner, rel.relacl, rel.relhasoids, ")
		        wxT("rel.relhassubclass, rel.reltuples, des.description, con.conname, con.conkey,\n")
		        wxT("       (select count(*) FROM pg_trigger\n")
		        wxT("                     WHERE tgrelid=rel.oid AND tgisconstraint = FALSE) AS triggercount,\n")
		        wxT("       EXISTS(select 1 FROM pg_trigger\n")
		        wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
		        wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
		        wxT("                     WHERE tgrelid=rel.oid) AS isrepl\n")
		        wxT("  FROM pg_class rel\n")
		        wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0 AND des.classoid='pg_class'::regclass)\n")
		        wxT("  LEFT OUTER JOIN pg_constraint con ON con.conrelid=rel.oid AND con.contype='p'\n")
		        wxT(" WHERE rel.relkind IN ('r','s','t') AND rel.relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
		        + restriction +
		        wxT(" ORDER BY rel.relname");
	}
	tables = collection->GetDatabase()->ExecuteSet(query);
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
			if (collection->GetConnection()->BackendMinimumVersion(9, 0))
			{
				table->iSetOfTypeOid(tables->GetOid(wxT("reloftype")));
				table->iSetOfType(tables->GetVal(wxT("typname")));
			}
			else
			{
				table->iSetOfTypeOid(0);
				table->iSetOfType(wxT(""));
			}
			table->iSetComment(tables->GetVal(wxT("description")));
			if (collection->GetConnection()->BackendMinimumVersion(9, 1))
				table->iSetUnlogged(tables->GetVal(wxT("relpersistence")) == wxT("u"));
			else
				table->iSetUnlogged(false);
			table->iSetHasOids(tables->GetBool(wxT("relhasoids")));
			table->iSetEstimatedRows(tables->GetDouble(wxT("reltuples")) * gp_segments);
			if (collection->GetConnection()->BackendMinimumVersion(8, 2))
			{
				table->iSetFillFactor(tables->GetVal(wxT("fillfactor")));
			}
			if (collection->GetConnection()->BackendMinimumVersion(8, 4))
			{
				table->iSetRelOptions(tables->GetVal(wxT("reloptions")));
				if (table->GetCustomAutoVacuumEnabled())
				{
					if (tables->GetVal(wxT("autovacuum_enabled")).IsEmpty())
						table->iSetAutoVacuumEnabled(2);
					else if (tables->GetBool(wxT("autovacuum_enabled")))
						table->iSetAutoVacuumEnabled(1);
					else
						table->iSetAutoVacuumEnabled(0);
					table->iSetAutoVacuumVacuumThreshold(tables->GetVal(wxT("autovacuum_vacuum_threshold")));
					table->iSetAutoVacuumVacuumScaleFactor(tables->GetVal(wxT("autovacuum_vacuum_scale_factor")));
					table->iSetAutoVacuumAnalyzeThreshold(tables->GetVal(wxT("autovacuum_analyze_threshold")));
					table->iSetAutoVacuumAnalyzeScaleFactor(tables->GetVal(wxT("autovacuum_analyze_scale_factor")));
					table->iSetAutoVacuumVacuumCostDelay(tables->GetVal(wxT("autovacuum_vacuum_cost_delay")));
					table->iSetAutoVacuumVacuumCostLimit(tables->GetVal(wxT("autovacuum_vacuum_cost_limit")));
					table->iSetAutoVacuumFreezeMinAge(tables->GetVal(wxT("autovacuum_freeze_min_age")));
					table->iSetAutoVacuumFreezeMaxAge(tables->GetVal(wxT("autovacuum_freeze_max_age")));
					table->iSetAutoVacuumFreezeTableAge(tables->GetVal(wxT("autovacuum_freeze_table_age")));
				}
				table->iSetHasToastTable(tables->GetBool(wxT("hastoasttable")));
				if (table->GetHasToastTable())
				{
					table->iSetToastRelOptions(tables->GetVal(wxT("toast_reloptions")));

					if (table->GetToastCustomAutoVacuumEnabled())
					{
						if (tables->GetVal(wxT("toast_autovacuum_enabled")).IsEmpty())
							table->iSetToastAutoVacuumEnabled(2);
						else if (tables->GetBool(wxT("toast_autovacuum_enabled")))
							table->iSetToastAutoVacuumEnabled(1);
						else
							table->iSetToastAutoVacuumEnabled(0);

						table->iSetToastAutoVacuumVacuumThreshold(tables->GetVal(wxT("toast_autovacuum_vacuum_threshold")));
						table->iSetToastAutoVacuumVacuumScaleFactor(tables->GetVal(wxT("toast_autovacuum_vacuum_scale_factor")));
						table->iSetToastAutoVacuumVacuumCostDelay(tables->GetVal(wxT("toast_autovacuum_vacuum_cost_delay")));
						table->iSetToastAutoVacuumVacuumCostLimit(tables->GetVal(wxT("toast_autovacuum_vacuum_cost_limit")));
						table->iSetToastAutoVacuumFreezeMinAge(tables->GetVal(wxT("toast_autovacuum_freeze_min_age")));
						table->iSetToastAutoVacuumFreezeMaxAge(tables->GetVal(wxT("toast_autovacuum_freeze_max_age")));
						table->iSetToastAutoVacuumFreezeTableAge(tables->GetVal(wxT("toast_autovacuum_freeze_table_age")));
					}
				}
			}
			table->iSetHasSubclass(tables->GetBool(wxT("relhassubclass")));
			table->iSetPrimaryKeyName(tables->GetVal(wxT("conname")));
			table->iSetIsReplicated(tables->GetBool(wxT("isrepl")));
			table->iSetTriggerCount(tables->GetLong(wxT("triggercount")));
			wxString cn = tables->GetVal(wxT("conkey"));
			cn = cn.Mid(1, cn.Length() - 2);
			table->iSetPrimaryKeyColNumbers(cn);

			if (collection->GetConnection()->GetIsGreenplum())
			{
				Oid lo = tables->GetOid(wxT("localoid"));
				wxString db = tables->GetVal(wxT("attrnums"));
				db = db.Mid(1, db.Length() - 2);
				table->iSetDistributionColNumbers(db);
				if (lo > 0 && db.Length() == 0)
					table->iSetDistributionIsRandom();
				table->iSetAppendOnly(tables->GetVal(wxT("appendonly")));
				table->iSetCompressLevel(tables->GetVal(wxT("compresslevel")));
				table->iSetOrientation(tables->GetVal(wxT("orientation")));
				table->iSetCompressType(tables->GetVal(wxT("compresstype")));
				table->iSetBlocksize(tables->GetVal(wxT("blocksize")));
				table->iSetChecksum(tables->GetVal(wxT("checksum")));

				table->iSetPartitionDef(wxT(""));
				table->iSetIsPartitioned(false);

				if (collection->GetConnection()->BackendMinimumVersion(8, 2, 9))
				{
					table->iSetIsPartitioned(tables->GetBool(wxT("ispartitioned")));
				}

			}

			if (collection->GetConnection()->BackendMinimumVersion(9, 1))
			{
				table->iSetProviders(tables->GetVal(wxT("providers")));
				table->iSetLabels(tables->GetVal(wxT("labels")));
			}

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
	// We don't create sub-objects of Views or External tables
	if (GetTable()->GetMetaType() == PGM_VIEW || GetTable()->GetMetaType() == GP_EXTTABLE)
		return false;

	return GetSchema()->GetCreatePrivilege();
}


#include "images/table.pngc"
#include "images/table-repl.pngc"
#include "images/table-repl-sm.pngc"
#include "images/table-sm.pngc"
#include "images/tables.pngc"

pgTableFactory::pgTableFactory()
	: pgSchemaObjFactory(__("Table"), __("New Table..."), __("Create a new Table."), table_png_img, table_sm_png_img)
{
	metaType = PGM_TABLE;
	if (WantSmallIcon())
		replicatedIconId = addIcon(table_repl_sm_png_img);
	else
		replicatedIconId = addIcon(table_repl_png_img);
}

pgCollection *pgTableFactory::CreateCollection(pgObject *obj)
{
	return new pgTableCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgTableFactory tableFactory;
static pgaCollectionFactory cf(&tableFactory, __("Tables"), tables_png_img);


pgCollection *pgTableObjFactory::CreateCollection(pgObject *obj)
{
	return new pgTableObjCollection(GetCollectionFactory(), (pgTable *)obj);
}


countRowsFactory::countRowsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Count"), _("Count rows in the selected object."));
}


wxWindow *countRowsFactory::StartDialog(frmMain *form, pgObject *obj)
{
	form->StartMsg(_("Counting rows"));

	((pgTable *)obj)->UpdateRows();

	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());

	form->EndMsg();

	return 0;
}


bool countRowsFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory);
}


executePgstattupleFactory::executePgstattupleFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Extended table statistics"), _("Get extended statistics via pgstattuple for the selected object."), wxITEM_CHECK);
}


wxWindow *executePgstattupleFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (!((pgTable *)obj)->GetShowExtendedStatistics())
	{
		((pgTable *)obj)->iSetShowExtendedStatistics(true);
		wxTreeItemId item = form->GetBrowser()->GetSelection();
		if (obj == form->GetBrowser()->GetObject(item))
			form->SelectStatisticsTab();
	}
	else
		((pgTable *)obj)->iSetShowExtendedStatistics(false);

	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), (ctlMenuToolbar *)form->GetToolBar());

	return 0;
}


bool executePgstattupleFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory) && ((pgTable *)obj)->HasPgstattuple();
}

bool executePgstattupleFactory::CheckChecked(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory) && ((pgTable *)obj)->GetShowExtendedStatistics();
}

disableAllTriggersFactory::disableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Disable triggers"), _("Disable all triggers on the selected table."));
}


wxWindow *disableAllTriggersFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to disable all triggers on this table?"), _("Disable triggers"), wxYES_NO) != wxYES)
		return 0;

	if (!((pgTable *)obj)->EnableTriggers(false))
		return 0;

	((pgTable *)obj)->iSetTriggersEnabled(form->GetBrowser(), false);

	return 0;
}


bool disableAllTriggersFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory) && obj->CanEdit()
	       && (obj->GetOwner() == obj->GetConnection()->GetUser() || obj->GetServer()->GetSuperUser())
	       && ((pgTable *)obj)->GetConnection()->BackendMinimumVersion(8, 1)
	       && ((pgTable *)obj)->GetTriggerCount() > 0;
}

enableAllTriggersFactory::enableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Enable triggers"), _("Enable all triggers on the selected table."));
}


wxWindow *enableAllTriggersFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to enable all triggers on this table?"), _("Enable triggers"), wxYES_NO) != wxYES)
		return 0;

	if (!((pgTable *)obj)->EnableTriggers(true))
		return 0;

	((pgTable *)obj)->iSetTriggersEnabled(form->GetBrowser(), true);

	return 0;
}


bool enableAllTriggersFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory) && obj->CanEdit()
	       && (obj->GetOwner() == obj->GetConnection()->GetUser() || obj->GetServer()->GetSuperUser())
	       && ((pgTable *)obj)->GetConnection()->BackendMinimumVersion(8, 1)
	       && ((pgTable *)obj)->GetTriggerCount() > 0;
}

truncateFactory::truncateFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Truncate"),  _("Truncate the selected table."));
}


wxWindow *truncateFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to truncate this table?\n\nWARNING: This action will delete ALL data in the table!"), _("Truncate table"), wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT) != wxYES)
		return 0;

	((pgTable *)obj)->Truncate(false);
	((pgTable *)obj)->UpdateRows();
	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());

	return 0;
}


bool truncateFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory);
}


truncateCascadedFactory::truncateCascadedFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Truncate Cascaded"), _("Truncate the selected table and all referencing tables."));
}


wxWindow *truncateCascadedFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to truncate this table and all tables that have foreign key references to this table?\n\nWARNING: This action will delete ALL data in the tables!"), _("Truncate table cascaded"), wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT) != wxYES)
		return 0;

	((pgTable *)obj)->Truncate(true);
	((pgTable *)obj)->UpdateRows();
	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());

	return 0;
}


bool truncateCascadedFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory) && ((pgTable *)obj)->GetConnection()->BackendMinimumVersion(8, 2);
}


resetTableStatsFactory::resetTableStatsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Reset table statistics"),  _("Reset statistics of the selected table."));
}


wxWindow *resetTableStatsFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to reset statistics of this table?"), _("Reset statistics"), wxYES_NO) != wxYES)
		return 0;

	((pgTable *)obj)->ResetStats();
	((pgTable *)obj)->ShowStatistics(form, form->GetStatistics());

	return 0;
}


bool resetTableStatsFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory) && ((pgTable *)obj)->GetConnection()->BackendMinimumVersion(9, 0);
}
