//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gpPartition.cpp - Greenplum Table Partition class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"


#include "utils/misc.h"
#include "frm/frmHint.h"
#include "frm/frmMain.h"
#include "frm/frmMaintenance.h"
#include "schema/pgTable.h"
#include "schema/gpPartition.h"
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

gpPartition::gpPartition(pgSchema *newSchema, const wxString &newName)
	: pgTable(newSchema, partitionFactory, newName)
{
}

gpPartition::~gpPartition()
{
}

bool gpPartition::CanCreate()
{
	return false;
}

wxMenu *gpPartition::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();
	if (schema->GetCreatePrivilege())
	{

	}
	return menu;
}
/*
wxString gpPartition::GetCreate()
{
    wxString sql;

   // sql = GetQuotedIdentifier() + wxT(" ")
    //    + GetTypeName().Upper() + GetDefinition();
    sql = wxT("Not implemented yet..sorry");
    return sql;
};
*/

wxString gpPartition::GetSql(ctlTree *browser)
{
	wxString sql;
	sql = wxT("-- ");
	sql += _("Note: This DDL is a representation of how the partition might look as a table.");
	sql += wxT("\n\n");

	sql += pgTable::GetSql(browser);
	return sql;
}

pgObject *gpPartition::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	gpPartition *partition = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		partition = (gpPartition *)partitionFactory.CreateObjects(coll, 0, wxT("\n   AND rel.oid=") + GetOidStr());

	return partition;
}

///////////////////////////////////////////////////////////

gpPartitionCollection::gpPartitionCollection(pgaFactory *factory, gpPartition *_table)
	: pgTableCollection(factory, _table->GetSchema())
{
	iSetOid(_table->GetOid());
}


pgObject *gpPartitionFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	gpPartitionCollection *collection = (gpPartitionCollection *)coll;
	wxString query;
	gpPartition *table = 0;

	// Greenplum returns reltuples and relpages as tuples per segmentDB and pages per segmentDB,
	// so we need to multiply them by the number of segmentDBs to get reasonable values.
	long gp_segments = 1;

	query = wxT("SELECT count(*) AS gp_segments from pg_catalog.gp_configuration where definedprimary = 't' and content >= 0");
	gp_segments = StrToLong(collection->GetDatabase()->ExecuteScalar(query));
	if (gp_segments <= 1)
		gp_segments = 1;


	pgSet *tables;

	query = wxT("SELECT rel.oid, relname, rel.reltablespace AS spcoid, spcname, pg_get_userbyid(relowner) AS relowner, relacl, relhasoids, ")
	        wxT("relhassubclass, reltuples, description, conname, conkey, parname, \n")
	        wxT("       EXISTS(select 1 FROM pg_trigger\n")
	        wxT("                       JOIN pg_proc pt ON pt.oid=tgfoid AND pt.proname='logtrigger'\n")
	        wxT("                       JOIN pg_proc pc ON pc.pronamespace=pt.pronamespace AND pc.proname='slonyversion'\n")
	        wxT("                     WHERE tgrelid=rel.oid) AS isrepl\n");

	query += wxT(", substring(array_to_string(reloptions, ',') from 'fillfactor=([0-9]*)') AS fillfactor \n");
	query += wxT(", gpd.localoid, gpd.attrnums \n");
	query += wxT(", substring(array_to_string(reloptions, ',') from 'appendonly=([a-z]*)') AS appendonly \n");
	query += wxT(", substring(array_to_string(reloptions, ',') from 'compresslevel=([0-9]*)') AS compresslevel \n");
	query += wxT(", substring(array_to_string(reloptions, ',') from 'orientation=([a-z]*)') AS orientation \n");
	query += wxT(", substring(array_to_string(reloptions, ',') from 'compresstype=([a-z0-9]*)') AS compresstype \n");
	query += wxT(", substring(array_to_string(reloptions, ',') from 'blocksize=([0-9]*)') AS blocksize \n");
	query += wxT(", substring(array_to_string(reloptions, ',') from 'checksum=([a-z]*)') AS checksum \n");
	//query += wxT(", rel.oid in (select parrelid from pg_partition) as ispartitioned\n"); // This only works for top-level tables, not intermediate ones
	// This looks for intermediate partitions that have subpartitions
	query += wxT(", rel.oid in (select pr.parchildrelid from pg_partition_rule pr, pg_partition pp where pr.paroid = pp.oid and pp.parlevel < (select max(parlevel) from pg_partition where parrelid = pp.parrelid)) as ispartitioned \n");


	query += wxT("  FROM pg_class rel JOIN pg_partition_rule pr ON(rel.oid = pr.parchildrelid) JOIN pg_partition p ON (pr.paroid = p.oid)\n")
	         wxT("  JOIN pg_inherits i ON (rel.oid = i.inhrelid) \n")
	         wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=rel.reltablespace\n")
	         wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rel.oid AND des.objsubid=0 AND des.classoid='pg_class'::regclass)\n")
	         wxT("  LEFT OUTER JOIN pg_constraint c ON c.conrelid=rel.oid AND c.contype='p'\n");
	query += wxT("  LEFT OUTER JOIN gp_distribution_policy gpd ON gpd.localoid=rel.oid\n");
	query += wxT(" WHERE relkind = 'r' ");
	query += wxT(" AND i.inhparent = ") + collection->GetOidStr() + wxT("\n");

	query += restriction +
	         wxT(" ORDER BY relname");


	tables = collection->GetDatabase()->ExecuteSet(query);
	if (tables)
	{
		while (!tables->Eof())
		{
			table = new gpPartition(collection->GetSchema(), tables->GetVal(wxT("relname")));

			table->iSetOid(tables->GetOid(wxT("oid")));
			table->iSetOwner(tables->GetVal(wxT("relowner")));
			table->iSetAcl(tables->GetVal(wxT("relacl")));

			if (tables->GetOid(wxT("spcoid")) == 0)
				table->iSetTablespaceOid(collection->GetDatabase()->GetTablespaceOid());
			else
				table->iSetTablespaceOid(tables->GetOid(wxT("spcoid")));

			if (tables->GetVal(wxT("spcname")) == wxEmptyString)
				table->iSetTablespace(collection->GetDatabase()->GetTablespace());
			else
				table->iSetTablespace(tables->GetVal(wxT("spcname")));

			table->iSetComment(tables->GetVal(wxT("description")));
			table->iSetHasOids(tables->GetBool(wxT("relhasoids")));
			table->iSetEstimatedRows(tables->GetDouble(wxT("reltuples")) * gp_segments);

			table->iSetFillFactor(tables->GetVal(wxT("fillfactor")));

			table->iSetHasSubclass(tables->GetBool(wxT("relhassubclass")));
			table->iSetPartitionName(tables->GetVal(wxT("parname")));
			table->iSetPrimaryKeyName(tables->GetVal(wxT("conname")));
			table->iSetIsReplicated(tables->GetBool(wxT("isrepl")));
			wxString cn = tables->GetVal(wxT("conkey"));
			cn = cn.Mid(1, cn.Length() - 2);
			table->iSetPrimaryKeyColNumbers(cn);


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
			table->iSetIsPartitioned(tables->GetBool(wxT("ispartitioned")));

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

void gpPartitionFactory::AppendMenu(wxMenu *menu)
{
}

#include "images/table.pngc"
#include "images/table-sm.pngc"
#include "images/tables.pngc"

gpPartitionFactory::gpPartitionFactory()
	: pgTableObjFactory(__("Partition"), __("New Partition..."), __("Create a new Partition."), table_png_img, table_sm_png_img)
{
	metaType = GP_PARTITION;
}

pgCollection *gpPartitionFactory::CreateCollection(pgObject *obj)
{
	return new gpPartitionCollection(GetCollectionFactory(), (gpPartition *)obj );
}

gpPartitionFactory partitionFactory;
static pgaCollectionFactory cf(&partitionFactory, __("Partitions"), tables_png_img);


