//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddDBReverseEnginering.cpp - Reverse engineering database functions for database designer.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/regex.h>

// App headers
#include "schema/pgSchema.h"
#include "schema/pgDatatype.h"
#include "dd/ddmodel/ddDBReverseEngineering.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/figures/ddRelationshipItem.h"
#include "dd/dditems/figures/ddRelationshipTerminal.h"
#include "images/namespaces.pngc"
#include "images/namespace-sm.pngc"
#include "images/gqbOrderAddAll.pngc"
#include "images/gqbOrderRemoveAll.pngc"
#include "images/gqbOrderRemove.pngc"
#include "images/gqbOrderAdd.pngc"


BEGIN_EVENT_TABLE(ddDBReverseEngineering, wxWizard)
	EVT_WIZARD_FINISHED(wxID_ANY, ddDBReverseEngineering::OnFinishPressed)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SelSchemaPage, wxWizardPage)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, SelSchemaPage::OnWizardPageChanging)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SelTablesPage, wxWizardPage)
	EVT_BUTTON(DDREMOVE, SelTablesPage::OnButtonRemove)
	EVT_BUTTON(DDREMOVEALL, SelTablesPage::OnButtonRemoveAll)
	EVT_BUTTON(DDADD, SelTablesPage::OnButtonAdd)
	EVT_BUTTON(DDADDALL, SelTablesPage::OnButtonAddAll)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, SelTablesPage::OnWizardPageChanging)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ReportPage, wxWizardPage)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, ReportPage::OnWizardPageChanging)
END_EVENT_TABLE()

//
//
//
// -----  Stub & reverse engineering classes Implementation
//
//
//

wxArrayString ddImportDBUtils::getTablesNames(pgConn *connection, wxString schemaName)
{

	wxArrayString out;
	wxString query;

	OID schemaOID = ddImportDBUtils::getSchemaOID(connection, schemaName);

	// Get the child objects.
	query = wxT("SELECT oid, relname, relkind\n")
	        wxT("  FROM pg_class\n")
	        wxT(" WHERE relkind IN ('r') AND relnamespace = ") + NumToStr(schemaOID) + wxT(";");

	pgSet *tables = connection->ExecuteSet(query);

	if (tables)
	{
		while (!tables->Eof())
		{
			wxString tmpname = tables->GetVal(wxT("relname"));
			wxString relkind = tables->GetVal(wxT("relkind"));

			if (relkind == wxT("r")) // Table
			{
				out.Add(tables->GetVal(wxT("relname")));
			}

			tables->MoveNext();
		}

		delete tables;
	}

	return out;
}

OID ddImportDBUtils::getSchemaOID(pgConn *connection, wxString schemaName)
{
	// Search Schemas and insert it
	wxString restr =  wxT(" WHERE ");

	restr += wxT("NOT ");
	restr += wxT("((nspname = 'pg_catalog' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pg_class' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'pgagent' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pga_job' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'information_schema' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'tables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname LIKE '_%' AND EXISTS (SELECT 1 FROM pg_proc WHERE proname='slonyversion' AND pronamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'dbo' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'systables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'sys' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'all_tables' AND relnamespace = nsp.oid LIMIT 1)))\n");

	if (connection->EdbMinimumVersion(8, 2))
	{
		restr += wxT("  AND nsp.nspparent = 0\n");
		// Do not show dbms_job_procedure in schemas
		if (!settings->GetShowSystemObjects())
			restr += wxT("AND NOT (nspname = 'dbms_job_procedure' AND EXISTS(SELECT 1 FROM pg_proc WHERE pronamespace = nsp.oid and proname = 'run_job' LIMIT 1))\n");
	}

	wxString sql;

	if (connection->BackendMinimumVersion(8, 1))
	{
		sql = wxT("SELECT CASE WHEN nspname LIKE E'pg\\\\_temp\\\\_%' THEN 1\n")
		      wxT("            WHEN (nspname LIKE E'pg\\\\_%') THEN 0\n");
	}
	else
	{
		sql = wxT("SELECT CASE WHEN nspname LIKE 'pg\\\\_temp\\\\_%' THEN 1\n")
		      wxT("            WHEN (nspname LIKE 'pg\\\\_%') THEN 0\n");
	}
	sql += wxT("            ELSE 3 END AS nsptyp, nspname, nsp.oid\n")
	       wxT("  FROM pg_namespace nsp\n")
	       + restr + wxT(" AND nspname = '") + schemaName + wxT("' ") +
	       wxT(" ORDER BY 1, nspname");

	pgSet *schemas = connection->ExecuteSet(sql);

	int times = 0;
	OID schemaOID = -1;
	if (schemas)
	{
		while (!schemas->Eof())
		{
			wxString name = schemas->GetVal(wxT("nspname"));
			long nsptyp = schemas->GetLong(wxT("nsptyp"));

			wxStringTokenizer tokens(settings->GetSystemSchemas(), wxT(","));
			while (tokens.HasMoreTokens())
			{
				wxRegEx regex(tokens.GetNextToken());
				if (regex.Matches(name))
				{
					nsptyp = SCHEMATYP_USERSYS;
					break;
				}
			}

			if (nsptyp <= SCHEMATYP_USERSYS && !settings->GetShowSystemObjects())
			{
				schemas->MoveNext();
				continue;
			}

			schemaOID = schemas->GetOid(wxT("oid"));
			times++;
			schemas->MoveNext();
		}

		delete schemas;
	}

	if(times > 1 || schemaOID == -1)
	{
		wxMessageBox(_("Schema not found"), _("getting table OID"),  wxICON_ERROR | wxOK);
		return -1;
	}
	return schemaOID;
}

OID ddImportDBUtils::getTableOID(pgConn *connection, wxString schemaName, wxString tableName)
{

	OID schemaOID = ddImportDBUtils::getSchemaOID(connection, schemaName);
	wxString query;
	OID tableOID = -1;

	// Get the child objects.
	query = wxT("SELECT oid, relname, relkind\n")
	        wxT("  FROM pg_class\n")
	        wxT(" WHERE relkind IN ('r') AND relnamespace = ") + NumToStr(schemaOID) +
	        wxT(" AND relname = '") + tableName + wxT("';");

	pgSet *tables = connection->ExecuteSet(query);
	int times;
	if (tables)
	{
		times = 0;
		while (!tables->Eof())
		{
			wxString relkind = tables->GetVal(wxT("relkind"));
			if (relkind == wxT("r")) // Table
			{
				tableOID = tables->GetOid(wxT("oid"));
				times++;
			}

			tables->MoveNext();
		}

		delete tables;
	}

	if(times > 1 || tableOID == -1)
	{
		wxMessageBox(_("Table not found"), _("getting table OID"),  wxICON_ERROR | wxOK);
		return -1;
	}
	return tableOID;
}

// Don't support inherited tables right now, or tables where a column is part of more than one Unique Key.
ddStubTable *ddImportDBUtils::getTable(pgConn *connection, wxString tableName, OID tableOid)
{
	wxString sql;
	int currentcol;
	ddStubTable *table = NULL;
	ddStubColumn *column = NULL;


	// grab inherited tables  [if found don't allow table import because this feature isn't supported right now]
	sql = wxT("SELECT inhparent::regclass AS inhrelname,\n")
	      wxT("  (SELECT count(*) FROM pg_attribute WHERE attrelid=inhparent AND attnum>0) AS colscount\n")
	      wxT("  FROM pg_inherits\n")
	      wxT("  WHERE inhrelid =  ") + NumToStr(tableOid) + wxT("::oid\n")
	      wxT("  ORDER BY inhseqno");
	pgSet *inhtables = connection->ExecuteSet(sql);

	if(inhtables && inhtables->Eof())
	{
		wxString systemRestriction;
		systemRestriction = wxT("\n   AND att.attnum > 0");

		sql =
		    wxT("SELECT att.*, def.*, pg_catalog.pg_get_expr(def.adbin, def.adrelid) AS defval, CASE WHEN att.attndims > 0 THEN 1 ELSE 0 END AS isarray, format_type(ty.oid,NULL) AS typname, format_type(ty.oid,att.atttypmod) AS displaytypname, tn.nspname as typnspname, et.typname as elemtypname,\n")
		    wxT("  ty.typstorage AS defaultstorage, cl.relname, na.nspname, att.attstattarget, description, cs.relname AS sername, ns.nspname AS serschema,\n")
		    wxT("  (SELECT count(1) FROM pg_type t2 WHERE t2.typname=ty.typname) > 1 AS isdup, indkey");

		if (connection->BackendMinimumVersion(9, 1))
			sql += wxT(",\n  coll.collname, nspc.nspname as collnspname");
		if (connection->BackendMinimumVersion(8, 5))
			sql += wxT(",\n  attoptions");
		if (connection->BackendMinimumVersion(7, 4))
			sql +=
			    wxT(",\n")
			    wxT("  EXISTS(SELECT 1 FROM  pg_constraint WHERE conrelid=att.attrelid AND contype='f'")
			    wxT(" AND att.attnum=ANY(conkey)) As isfk");
		if (connection->BackendMinimumVersion(9, 1))
		{
			sql += wxT(",\n(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=att.attrelid AND sl1.objsubid=att.attnum) AS labels");
			sql += wxT(",\n(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=att.attrelid AND sl2.objsubid=att.attnum) AS providers");
		}

		sql += wxT("\n")
		       wxT("  FROM pg_attribute att\n")
		       wxT("  JOIN pg_type ty ON ty.oid=atttypid\n")
		       wxT("  JOIN pg_namespace tn ON tn.oid=ty.typnamespace\n")
		       wxT("  JOIN pg_class cl ON cl.oid=att.attrelid\n")
		       wxT("  JOIN pg_namespace na ON na.oid=cl.relnamespace\n")
		       wxT("  LEFT OUTER JOIN pg_type et ON et.oid=ty.typelem\n")
		       wxT("  LEFT OUTER JOIN pg_attrdef def ON adrelid=att.attrelid AND adnum=att.attnum\n")
		       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=att.attrelid AND des.objsubid=att.attnum\n")
		       wxT("  LEFT OUTER JOIN (pg_depend JOIN pg_class cs ON classid='pg_class'::regclass AND objid=cs.oid AND cs.relkind='S') ON refobjid=att.attrelid AND refobjsubid=att.attnum\n")
		       wxT("  LEFT OUTER JOIN pg_namespace ns ON ns.oid=cs.relnamespace\n")
		       wxT("  LEFT OUTER JOIN pg_index pi ON pi.indrelid=att.attrelid AND indisprimary\n");
		if (connection->BackendMinimumVersion(9, 1))
			sql += wxT("  LEFT OUTER JOIN pg_collation coll ON att.attcollation=coll.oid\n")
			       wxT("  LEFT OUTER JOIN pg_namespace nspc ON coll.collnamespace=nspc.oid\n");
		sql += wxT(" WHERE att.attrelid = ") + NumToStr(tableOid)
		       + systemRestriction + wxT("\n")
		       wxT("   AND att.attisdropped IS FALSE\n")
		       wxT(" ORDER BY att.attnum");

		table = new ddStubTable(tableName, tableOid);

		pgSet *columns = connection->ExecuteSet(sql);
		if (columns)
		{
			currentcol = 0;
			while (!columns->Eof())
			{
				currentcol++;
				column = new ddStubColumn(columns->GetVal(wxT("attname")), tableOid);
				column->pgColNumber = columns->GetLong(wxT("attnum"));
				wxString pkCols = columns->GetVal(wxT("indkey"));
				bool isPK = false;
				wxStringTokenizer indkey(pkCols);
				while (indkey.HasMoreTokens())
				{
					wxString str = indkey.GetNextToken();
					if (StrToLong(str) == column->pgColNumber)
					{
						isPK = true;
						break;
					}
				}
				column->isPrimaryKey = isPK;

				long typmod = columns->GetLong(wxT("atttypmod"));
				pgDatatype *dt = new pgDatatype(columns->GetVal(wxT("typnspname")), columns->GetVal(wxT("typname")),
				                                columns->GetBool(wxT("isdup")),
				                                columns->GetLong(wxT("attndims")), typmod);

				column->typeColumn = dt;
				column->isNotNull = columns->GetBool(wxT("attnotnull"));
				wxString colName = column->columnName;
				table->cols[colName] = column;
				columns->MoveNext();
			}

			delete columns;
		}
		setUniqueConstraints(connection, table);
		setPkName(connection, table);
		if(inhtables)
		{
			delete inhtables;
			inhtables = NULL;
		}
		return table;
	}

	return table;
}

//true on everything fine, false when some error is found
bool ddImportDBUtils::setUniqueConstraints(pgConn *connection, ddStubTable *table)
{
	bool out = true;
	//temporary fix, this should be adapted to pgadmin way of working
	// check for extended ruleutils with pretty-print option
	wxString prettyOption;
	wxString exprname = connection->ExecuteScalar(wxT("SELECT proname FROM pg_proc WHERE proname='pg_get_viewdef' AND proargtypes[1]=16"));
	if (!exprname.IsEmpty())
		prettyOption = wxT(", true");

	wxString query;

	wxString proname, projoin;
	if (connection->BackendMinimumVersion(7, 4))
	{
		proname = wxT("indnatts, ");
		if (connection->BackendMinimumVersion(7, 5))
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
	        wxT("       ") + proname + wxT("tab.relname as tabname, indclass, con.oid AS conoid, CASE contype WHEN 'p' THEN desp.description WHEN 'u' THEN desp.description WHEN 'x' THEN desp.description ELSE des.description END AS description,\n")
	        wxT("       pg_get_expr(indpred, indrelid") + prettyOption + wxT(") as indconstraint, contype, condeferrable, condeferred, amname\n");
	if (connection->BackendMinimumVersion(8, 2))
		query += wxT(", substring(array_to_string(cls.reloptions, ',') from 'fillfactor=([0-9]*)') AS fillfactor \n");
	query += wxT("  FROM pg_index idx\n")
	         wxT("  JOIN pg_class cls ON cls.oid=indexrelid\n")
	         wxT("  JOIN pg_class tab ON tab.oid=indrelid\n")
	         + projoin +
	         wxT("  JOIN pg_namespace n ON n.oid=tab.relnamespace\n")
	         wxT("  JOIN pg_am am ON am.oid=cls.relam\n")
	         wxT("  LEFT JOIN pg_depend dep ON (dep.classid = cls.tableoid AND dep.objid = cls.oid AND dep.refobjsubid = '0' AND dep.refclassid=(SELECT oid FROM pg_class WHERE relname='pg_constraint') AND dep.deptype='i')\n")
	         wxT("  LEFT OUTER JOIN pg_constraint con ON (con.tableoid = dep.refclassid AND con.oid = dep.refobjid)\n")
	         wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=cls.oid\n")
	         wxT("  LEFT OUTER JOIN pg_description desp ON (desp.objoid=con.oid AND desp.objsubid = 0)\n")
	         wxT(" WHERE indrelid = ") + NumToStr(table->OIDTable) + wxT("::oid")
	         + wxT(" AND contype='u' ")
	         + wxT("\n")
	         wxT(" ORDER BY cls.relname");
	pgSet *indexes = connection->ExecuteSet(query);
	int ukIndex = -1;
	wxArrayString UniqueKeysNames;

	if (indexes)
	{
		while (!indexes->Eof())
		{
			ukIndex++;
			wxString uniqueKeys = indexes->GetVal(wxT("indkey"));

			UniqueKeysNames.Add(indexes->GetVal(wxT("idxname")));

			wxStringTokenizer indkey(uniqueKeys);
			while (indkey.HasMoreTokens())
			{
				//Get column number in unique key
				wxString str = indkey.GetNextToken();
				//look at all columns [change for hashmap in a future if that option is more optimized]
				stubColsHashMap::iterator it;
				ddStubColumn *item;
				for (it = table->cols.begin(); it != table->cols.end(); ++it)
				{
					wxString key = it->first;
					item = it->second;
					//If column belong to unique constraint mark it
					if (StrToLong(str) == item->pgColNumber)
					{
						item->uniqueKeyIndex = ukIndex;
					}
				}
			}
			indexes->MoveNext();
		}
		table->UniqueKeysNames = UniqueKeysNames;
		delete indexes;
	}
	return out;  //false in a future when detect a column in more than one unique key because this is not supported by database designer right now
}

//true on everything fine, false when some error is found
bool ddImportDBUtils::setPkName(pgConn *connection, ddStubTable *table)
{
	bool out = true;
	wxString pkName = wxEmptyString;

	//temporary fix, this should be adapted to pgadmin way of working
	// check for extended ruleutils with pretty-print option
	wxString prettyOption;
	wxString exprname = connection->ExecuteScalar(wxT("SELECT proname FROM pg_proc WHERE proname='pg_get_viewdef' AND proargtypes[1]=16"));
	if (!exprname.IsEmpty())
		prettyOption = wxT(", true");

	wxString query;

	wxString proname, projoin;
	if (connection->BackendMinimumVersion(7, 4))
	{
		proname = wxT("indnatts, ");
		if (connection->BackendMinimumVersion(7, 5))
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
	        wxT("       ") + proname + wxT("tab.relname as tabname, indclass, con.oid AS conoid, CASE contype WHEN 'p' THEN desp.description WHEN 'u' THEN desp.description WHEN 'x' THEN desp.description ELSE des.description END AS description,\n")
	        wxT("       pg_get_expr(indpred, indrelid") + prettyOption + wxT(") as indconstraint, contype, condeferrable, condeferred, amname\n");
	if (connection->BackendMinimumVersion(8, 2))
		query += wxT(", substring(array_to_string(cls.reloptions, ',') from 'fillfactor=([0-9]*)') AS fillfactor \n");
	query += wxT("  FROM pg_index idx\n")
	         wxT("  JOIN pg_class cls ON cls.oid=indexrelid\n")
	         wxT("  JOIN pg_class tab ON tab.oid=indrelid\n")
	         + projoin +
	         wxT("  JOIN pg_namespace n ON n.oid=tab.relnamespace\n")
	         wxT("  JOIN pg_am am ON am.oid=cls.relam\n")
	         wxT("  LEFT JOIN pg_depend dep ON (dep.classid = cls.tableoid AND dep.objid = cls.oid AND dep.refobjsubid = '0' AND dep.refclassid=(SELECT oid FROM pg_class WHERE relname='pg_constraint') AND dep.deptype='i')\n")
	         wxT("  LEFT OUTER JOIN pg_constraint con ON (con.tableoid = dep.refclassid AND con.oid = dep.refobjid)\n")
	         wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=cls.oid\n")
	         wxT("  LEFT OUTER JOIN pg_description desp ON (desp.objoid=con.oid AND desp.objsubid = 0)\n")
	         wxT(" WHERE indrelid = ") + NumToStr(table->OIDTable) + wxT("::oid")
	         + wxT(" AND contype='p' ")
	         + wxT("\n")
	         wxT(" ORDER BY cls.relname");
	pgSet *indexes = connection->ExecuteSet(query);
	if (indexes)
	{
		while (!indexes->Eof())
		{
			pkName = indexes->GetVal(wxT("idxname"));
			indexes->MoveNext();
		}
		delete indexes;
	}

	table->PrimaryKeyName = pkName;
	return out;  //false in a future when detect a column in more than one unique key because this is not supported by database designer right now
}

void ddImportDBUtils::getAllRelationships(pgConn *connection, stubTablesHashMap &tables, ddDatabaseDesign *design)
{
	wxString sql;
	ddRelationshipFigure *relation = NULL;
	ddTableFigure *sourceTabFigure = NULL;
	ddTableFigure *destTabFigure = NULL;
	//Add Tables to the Model
	stubTablesHashMap::iterator mainIt;
	ddStubTable *destStubTable = NULL;
	for (mainIt = tables.begin(); mainIt != tables.end(); ++mainIt)
	{
		wxString key = mainIt->first;
		destStubTable = mainIt->second;

		sql = wxT("SELECT ct.oid, conname, condeferrable, condeferred, confupdtype, confdeltype, confmatchtype, ")
		      wxT("conkey, confkey, confrelid, nl.nspname as fknsp, cl.relname as fktab, ")
		      wxT("nr.nspname as refnsp, cr.relname as reftab, description");
		if (connection->BackendMinimumVersion(9, 1))
			sql += wxT(", convalidated");
		sql += wxT("\n  FROM pg_constraint ct\n")
		       wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
		       wxT("  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n")
		       wxT("  JOIN pg_class cr ON cr.oid=confrelid\n")
		       wxT("  JOIN pg_namespace nr ON nr.oid=cr.relnamespace\n")
		       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=ct.oid\n")
		       wxT(" WHERE contype='f' AND conrelid = ") + NumToStr(destStubTable->OIDTable) + wxT("::oid")
		       //+ restriction +
		       + wxT("\n")
		       wxT(" ORDER BY conname");

		pgSet *foreignKeys = connection->ExecuteSet(sql);

		if (foreignKeys && foreignKeys->NumRows() > 0)
		{
			while (!foreignKeys->Eof())
			{
				wxString sourceSchema, destSchema;
				sourceSchema = foreignKeys->GetVal(wxT("refnsp"));
				destSchema = foreignKeys->GetVal(wxT("fknsp"));

				//  Source Table  ----------------------<| Destination Table

				if(sourceSchema.IsSameAs(destSchema, false))
				{
					wxString sourceTableName = foreignKeys->GetVal(wxT("reftab"));
					wxString destTableName = foreignKeys->GetVal(wxT("fktab"));

					destTabFigure = design->getTable(destTableName);
					sourceTabFigure = design->getTable(sourceTableName);

					//Only if both tables were imported at same time
					if(destTabFigure != NULL && sourceTabFigure != NULL)
					{

						int ukindex = -1; //Only Supporting foreign keys from PK right now when importing model
						wxString RelationshipName = foreignKeys->GetVal(wxT("conname"));

						wxString onUpd = foreignKeys->GetVal(wxT("confupdtype"));
						actionKind onUpdate = 	onUpd.IsSameAs('a') ? FK_ACTION_NO :
						                        onUpd.IsSameAs('r') ? FK_RESTRICT :
						                        onUpd.IsSameAs('c') ? FK_CASCADE :
						                        onUpd.IsSameAs('d') ? FK_SETDEFAULT :
						                        onUpd.IsSameAs('n') ? FK_SETNULL : FK_ACTION_NO;


						wxString onDel = foreignKeys->GetVal(wxT("confdeltype"));
						actionKind onDelete = 	onUpd.IsSameAs('a') ? FK_ACTION_NO :
						                        onUpd.IsSameAs('r') ? FK_RESTRICT :
						                        onUpd.IsSameAs('c') ? FK_CASCADE :
						                        onUpd.IsSameAs('d') ? FK_SETDEFAULT :
						                        onUpd.IsSameAs('n') ? FK_SETNULL : FK_ACTION_NO;

						wxString match = foreignKeys->GetVal(wxT("confmatchtype"));
						bool matchSimple = 	match.IsSameAs('f') ? false :
						                    match.IsSameAs('u') ? true : false;


						//------ Preparing metada to allow discovery of some relationship attributes
						//Source table columns
						wxString fkColsSourceTable = foreignKeys->GetVal(wxT("confkey"));
						//remove {} of string
						fkColsSourceTable.Remove(0, 1);
						fkColsSourceTable.RemoveLast();
						wxString fkColsDestTable = foreignKeys->GetVal(wxT("conkey"));
						//remove {} of string
						fkColsDestTable.Remove(0, 1);
						fkColsDestTable.RemoveLast();

						wxSortedArrayInt sourceFkCols(sortFunc);
						wxSortedArrayInt destFkCols(sortFunc);
						wxSortedArrayInt sourcePKs(sortFunc);
						wxSortedArrayInt destPKs(sortFunc);

						//Split columns from sourceFk
						wxStringTokenizer confkey(fkColsSourceTable);
						while (confkey.HasMoreTokens())
						{
							wxString str = confkey.GetNextToken();
							sourceFkCols.Add(StrToLong(str));
						}

						//Split columns from destFk
						wxStringTokenizer conkey(fkColsDestTable);
						while (conkey.HasMoreTokens())
						{
							wxString str = conkey.GetNextToken();
							destFkCols.Add(StrToLong(str));
						}

						//Get Stub of source table
						ddStubTable *sourceStubTable = tables[sourceTableName];

						//Get PK columns of source
						stubColsHashMap::iterator it;
						ddStubColumn *column;
						for (it = sourceStubTable->cols.begin(); it != sourceStubTable->cols.end(); ++it)
						{
							wxString key = it->first;
							column = it->second;
							if(column->isPrimaryKey)
								sourcePKs.Add(column->pgColNumber);
						}

						//Get PK columns of dest
						for (it = destStubTable->cols.begin(); it != destStubTable->cols.end(); ++it)
						{
							wxString key = it->first;
							column = it->second;
							if(column->isPrimaryKey)
								destPKs.Add(column->pgColNumber);
						}

						//  Source Table  ----------------------<| Destination Table
						//Default assumption is the source of this fk is a Primary Key.
						bool fkFromPk = true;

						//first check: number of columns used as fk at Source is the same of the pk at Source
						if(sourceFkCols.Count() == sourcePKs.Count())
						{
							int i;
							//Because postgres columns numbers are stored in an ordered array,
							//their index should be the same at all positions
							int srcFkCount = sourceFkCols.Count();
							for(i = 0; i < srcFkCount; i++)
							{
								if( sourceFkCols[i] != sourcePKs[i] )
								{
									fkFromPk = false;
									break;
								}
							}
						}
						else
						{
							fkFromPk = true;
						}

						//------ Finding fk from uk or pk?
						int ukIndex = -1;
						//if fkFromPk = false then is fkfromUK?, check that
						//all source fk columns should belong to one Uk at source table.
						if( fkFromPk == false )
						{
							bool error = false;
							int baseColNumber = sourceFkCols[sourceFkCols.Count() - 1];
							int baseUkIdxSourceCol = sourceStubTable->getColumnByNumber(baseColNumber)->uniqueKeyIndex;
							int nextColNumber, nextUkIdxSourceCol;
							int countSrcFkCols = sourceFkCols.Count() - 2;
							while(countSrcFkCols >= 0)
							{
								nextColNumber = sourceFkCols[countSrcFkCols];
								nextUkIdxSourceCol = sourceStubTable->getColumnByNumber(nextColNumber)->uniqueKeyIndex;
								countSrcFkCols--;
								if(baseUkIdxSourceCol != nextUkIdxSourceCol)
								{
									error = true;
									wxMessageBox(_("Error detecting kind of foreign key source: from Pk or from Uk"), _("Error importing relationship"),  wxICON_ERROR | wxOK);
									delete foreignKeys;
									return;
								}
							}
							if(!error)
							{
								ukIndex = baseUkIdxSourceCol;
							}
						}

						//Last check of consistency
						if(fkFromPk == false && ukIndex < 0)
						{
							wxMessageBox(_("Error detecting kind of foreign key source: from Pk or from Uk"), _("Error importing relationship"),  wxICON_ERROR | wxOK);
							delete foreignKeys;
							return;
						}


						//------ identifying relationship or not  -----|-<|?
						//Default assumption is relationship is identifying
						bool identifying = true;

						//first check: number of columns used as fk at Source is the same of the pk at Source
						if(destFkCols.Count() == destPKs.Count())
						{
							int i;
							//Because postgres columns numbers are stored in an ordered array,
							//their index should be the same at all positions
							int destFkCount = destFkCols.Count();
							for(i = 0; i < destFkCount; i++)
							{
								if( destFkCols[i] != destPKs[i] )
								{
									identifying = false;
									break;
								}
							}
						}
						else
						{
							identifying = false;
						}

						//------ 1:1 or 1:M  ?  as a fact 1:1 have a fk,uk at destination table.
						// A foreign key have an one to many relationship when there is an UK for same column(s)
						// inside the foreign key. Assumption, a column on belong to one Uk (no more than one).
						bool oneToMany = true;
						int baseColNumber = destFkCols[destFkCols.Count() - 1];
						int baseUkIdxDestCol = destStubTable->getColumnByNumber(baseColNumber)->uniqueKeyIndex;
						if(baseUkIdxDestCol != -1)
						{
							oneToMany = false;
							int nextUkIdxDestCol, nextColNumber;
							int countDestFkCols = destFkCols.Count() - 2;
							while(countDestFkCols >= 0)
							{
								nextColNumber = destFkCols[countDestFkCols];
								nextUkIdxDestCol = destStubTable->getColumnByNumber(nextColNumber)->uniqueKeyIndex;
								countDestFkCols--;
								//if a dest fk column is not in the same Uk index of first one
								if(nextUkIdxDestCol != baseUkIdxDestCol)
								{
									oneToMany = true;
									break;
								}
							}
						}

						//Step two check all column of fk are inside a unique key (all and not more)
						if(oneToMany == false)  //assumption is 1:1 relationship until now
						{
							int numberColsInUk = 0, nextUkIdxDestCol, nextColNumber;
							ddStubColumn *item;
							for (it = destStubTable->cols.begin(); it != destStubTable->cols.end(); ++it)
							{
								wxString key = it->first;
								item = it->second;
								//at each column with same uk index that base comparison column, count it
								nextColNumber = item->pgColNumber;
								nextUkIdxDestCol = destStubTable->getColumnByNumber(nextColNumber)->uniqueKeyIndex;
								if( nextUkIdxDestCol == baseUkIdxDestCol)
								{
									numberColsInUk++;
								}
							}

							//number of columns in uk used by relationship is bigger or lesser than number of columns
							//in destination table used by relationship as fk dest(dest fk columnn), then is not 1:1
							if(numberColsInUk != destFkCols.Count())
								oneToMany = true;
						}

						//Optional or Mandatory consistency
						bool mandatoryRelationship;

						int countDestFkCols = destFkCols.Count() - 1;
						bool isNotNull;
						int nnCols = 0, nullCols = 0, nextColNumber;
						while(countDestFkCols >= 0)
						{
							nextColNumber = destFkCols[countDestFkCols];
							isNotNull = destStubTable->getColumnByNumber(nextColNumber)->isNotNull;
							countDestFkCols--;
							if(isNotNull)
								nnCols++;
							else
								nullCols++;
						}

						if(nnCols == 0 && nullCols > 0)
						{
							mandatoryRelationship = false;
						}
						else if(nnCols > 0 && nullCols == 0)
						{
							mandatoryRelationship = true;
						}
						else
						{
							wxMessageBox(_("Error detecting kind of foreign key: null or not null"), _("Error importing relationship"),  wxICON_ERROR | wxOK);
							delete foreignKeys;
							return;
						}

						relation = new ddRelationshipFigure();
						relation->setStartTerminal(new ddRelationshipTerminal(relation, false));
						relation->setEndTerminal(new ddRelationshipTerminal(relation, true));
						relation->clearPoints(0);
						relation->initRelationValues(sourceTabFigure, destTabFigure, ukIndex, RelationshipName, onUpdate, onDelete, matchSimple, identifying, oneToMany, mandatoryRelationship, fkFromPk);
						relation->updateConnection(0);
						design->addTableToModel(relation);

						//Add items to relationship
						wxString srcColName, destColName;
						ddColumnFigure *sourceCol = NULL, *destinationCol = NULL;
						bool autoGenFk = false;
						wxString initialColName;
						ddRelationshipItem *item = NULL;
						int i, srcFkCount = sourceFkCols.Count();
						for(i = 0; i < srcFkCount ; i++)
						{
							srcColName  =  sourceStubTable->getColumnByNumber(sourceFkCols[i])->columnName;
							destColName =  destStubTable->getColumnByNumber(destFkCols[i])->columnName;
							sourceCol = sourceTabFigure->getColByName(srcColName);
							destinationCol = destTabFigure->getColByName(destColName);
							initialColName = srcColName;
							item = new ddRelationshipItem();
							item->initRelationshipItemValues(relation, destTabFigure, autoGenFk, destinationCol, sourceCol, initialColName);
							relation->getItemsHashMap()[item->original->getColumnName()] = item;
						}
					}
				}
				foreignKeys->MoveNext();
			}
			delete foreignKeys;
		}
	}
}

bool ddImportDBUtils::existsFk(pgConn *connection, OID destTableOid, wxString schemaName, wxString fkName, wxString sourceTableName)
{
	wxString sql;
	OID sourceOID = getTableOID(connection, schemaName, sourceTableName);
	if(sourceOID == -1 )
	{
		return false;
	}

	sql = wxT("SELECT ct.oid, conname, condeferrable, condeferred, confupdtype, confdeltype, confmatchtype, ")
	      wxT("conkey, confkey, confrelid, nl.nspname as fknsp, cl.relname as fktab, ")
	      wxT("nr.nspname as refnsp, cr.relname as reftab, description");
	if (connection->BackendMinimumVersion(9, 1))
		sql += wxT(", convalidated");
	sql += wxT("\n  FROM pg_constraint ct\n")
	       wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
	       wxT("  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n")
	       wxT("  JOIN pg_class cr ON cr.oid=confrelid\n")
	       wxT("  JOIN pg_namespace nr ON nr.oid=cr.relnamespace\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=ct.oid\n")
	       wxT(" WHERE contype='f' AND conrelid = ") + NumToStr(destTableOid) + wxT("::oid")
	       wxT(" AND conname ='") + fkName + wxT("' ")
	       wxT(" AND confrelid = ") + NumToStr(sourceOID) + wxT("::oid")
	       wxT("\n")
	       wxT(" ORDER BY conname");

	pgSet *foreignKeys = connection->ExecuteSet(sql);

	//relation don't exists then
	if(foreignKeys->NumRows() == 0)
	{
		return false;
	}
	delete foreignKeys;

	return true;
}

int ddImportDBUtils::getPgColumnNum(pgConn *connection, wxString schemaName, wxString tableName, wxString columnName)
{
	int out = -1;
	wxString sql;
	OID tableOid = getTableOID(connection, schemaName, tableName);
	wxString systemRestriction;
	systemRestriction = wxT("\n   AND att.attnum > 0");

	sql =  wxT("SELECT att.attrelid,att.attname, att.attnum ")
	       wxT("\n")
	       wxT("  FROM pg_attribute att\n")
	       wxT("  JOIN pg_type ty ON ty.oid=atttypid\n")
	       wxT("  JOIN pg_namespace tn ON tn.oid=ty.typnamespace\n")
	       wxT("  JOIN pg_class cl ON cl.oid=att.attrelid\n")
	       wxT("  JOIN pg_namespace na ON na.oid=cl.relnamespace\n")
	       wxT("  LEFT OUTER JOIN pg_type et ON et.oid=ty.typelem\n")
	       wxT("  LEFT OUTER JOIN pg_attrdef def ON adrelid=att.attrelid AND adnum=att.attnum\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=att.attrelid AND des.objsubid=att.attnum\n")
	       wxT("  LEFT OUTER JOIN (pg_depend JOIN pg_class cs ON classid='pg_class'::regclass AND objid=cs.oid AND cs.relkind='S') ON refobjid=att.attrelid AND refobjsubid=att.attnum\n")
	       wxT("  LEFT OUTER JOIN pg_namespace ns ON ns.oid=cs.relnamespace\n")
	       wxT("  LEFT OUTER JOIN pg_index pi ON pi.indrelid=att.attrelid AND indisprimary\n");
	if (connection->BackendMinimumVersion(9, 1))
		sql += wxT("  LEFT OUTER JOIN pg_collation coll ON att.attcollation=coll.oid\n")
		       wxT("  LEFT OUTER JOIN pg_namespace nspc ON coll.collnamespace=nspc.oid\n");
	sql += wxT(" WHERE att.attrelid = ") + NumToStr(tableOid)
	       + systemRestriction + wxT("\n")
	       wxT("   AND att.attisdropped IS FALSE\n")
	       wxT(" ORDER BY att.attnum");

	pgSet *columns = connection->ExecuteSet(sql);
	if (columns)
	{
		while (!columns->Eof())
		{
			wxString colName = columns->GetVal(wxT("attname"));
			if(colName.IsSameAs(columnName, false))
			{
				out = columns->GetLong(wxT("attnum"));
				break;
			}
			columns->MoveNext();
		}
		delete columns;
	}
	return out;
}

wxArrayString ddImportDBUtils::getFkAtDbNotInModel(pgConn *connection, OID destTableOid, wxString schemaName, wxArrayString existingFkList, ddDatabaseDesign *design)
{
	wxArrayString out;
	wxString sql;

	sql = wxT("SELECT ct.oid, conname, condeferrable, condeferred, confupdtype, confdeltype, confmatchtype, ")
	      wxT("conkey, confkey, confrelid, nl.nspname as fknsp, cl.relname as fktab, ")
	      wxT("nr.nspname as refnsp, cr.relname as reftab, description");
	if (connection->BackendMinimumVersion(9, 1))
		sql += wxT(", convalidated");
	sql += wxT("\n  FROM pg_constraint ct\n")
	       wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
	       wxT("  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n")
	       wxT("  JOIN pg_class cr ON cr.oid=confrelid\n")
	       wxT("  JOIN pg_namespace nr ON nr.oid=cr.relnamespace\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=ct.oid\n")
	       wxT(" WHERE contype='f' AND conrelid = ") + NumToStr(destTableOid) + wxT("::oid");
	//Add Fk names in model
	int i, max = existingFkList.Count();
	if(max > 0)
	{
		sql += wxT(" AND conname NOT IN( ");
		for(i = 0; i < max - 1; i++)
		{
			sql += wxT("'") + existingFkList[i] + wxT("',");
		}
		if(max >= 1)
		{
			sql += wxT("'") + existingFkList[max - 1] + wxT("'");
		}
		sql +=  wxT(" ) ");
	}

	//Add valid tables sources names to search (others outside a model shouldn't be modified)
	sql += wxT("\n ORDER BY conname");

	pgSet *foreignKeys = connection->ExecuteSet(sql);

	if (foreignKeys && foreignKeys->NumRows() > 0)
	{
		while (!foreignKeys->Eof())
		{
			//Create a list will all relationships in db but not in model [but only for tables IN MODEL because tables not in model SHOULDN'T BE modified]
			wxString sourceTableName = foreignKeys->GetVal(wxT("reftab"));
			if(design->getTable(sourceTableName))
			{
				wxString RelationshipName = foreignKeys->GetVal(wxT("conname"));
				out.Add(RelationshipName);
			}
			foreignKeys->MoveNext();
		}
		delete foreignKeys;
	}
	//return a list with Fks to delete from db because don't exists at model.
	return out;
}

//Assumption Fk exists, and this should be checked before with existsFk function
bool ddImportDBUtils::isModelSameDbFk(pgConn *connection, OID destTableOid, wxString schemaName, wxString fkName, wxString sourceTableName, wxString destTableName, ddStubTable *destStubTable, ddRelationshipFigure *relation)
{
	bool equalFk = true;

	wxString sql;
	OID sourceOID = getTableOID(connection, schemaName, sourceTableName);
	sql = wxT("SELECT ct.oid, conname, condeferrable, condeferred, confupdtype, confdeltype, confmatchtype, ")
	      wxT("conkey, confkey, confrelid, nl.nspname as fknsp, cl.relname as fktab, ")
	      wxT("nr.nspname as refnsp, cr.relname as reftab, description");
	if (connection->BackendMinimumVersion(9, 1))
		sql += wxT(", convalidated");
	sql += wxT("\n  FROM pg_constraint ct\n")
	       wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
	       wxT("  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n")
	       wxT("  JOIN pg_class cr ON cr.oid=confrelid\n")
	       wxT("  JOIN pg_namespace nr ON nr.oid=cr.relnamespace\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=ct.oid\n")
	       wxT(" WHERE contype='f' AND conrelid = ") + NumToStr(destTableOid) + wxT("::oid")
	       wxT(" AND conname ='") + fkName + wxT("' ")
	       wxT(" AND confrelid = ") + NumToStr(sourceOID) + wxT("::oid")
	       wxT("\n")
	       wxT(" ORDER BY conname");

	pgSet *foreignKeys = connection->ExecuteSet(sql);


	//First Step create array with columns from pgCol numbers from destTable (MODEL) in relationship;
	//Second Step create array with columns from pgCol numbers from srcTable (MODEL) in relationship;
	wxSortedArrayInt destPgs(sortFunc);
	wxSortedArrayInt srcPgs(sortFunc);
	columnsHashMap::iterator it;
	ddRelationshipItem *item;
	for (it = relation->getItemsHashMap().begin(); it != relation->getItemsHashMap().end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		int pgColDest = getPgColumnNum(connection, schemaName, destTableName, item->fkColumn->getColumnName());
		destPgs.Add(pgColDest);
		int pgColSrc =  getPgColumnNum(connection, schemaName, sourceTableName, item->original->getColumnName());
		srcPgs.Add(pgColSrc);
	}

	//Extracting relationship metadata information from DB relationship
	int ukindex = -1; //Only Supporting foreign keys from PK right now when importing model
	wxString RelationshipName;
	actionKind onUpdate;
	actionKind onDelete;
	bool matchSimple;
	bool identifying = true; //Default assumption is relationship is identifying
	bool oneToMany = true;
	bool mandatoryRelationship;
	bool fkFromPk = true;	//Default assumption is the source of this fk is a Primary Key.
	wxSortedArrayInt sourceFkCols(sortFunc);
	wxSortedArrayInt destFkCols(sortFunc);


	if (foreignKeys && foreignKeys->NumRows() == 1)
	{
		while (!foreignKeys->Eof())
		{

			//------ Preparing metada to allow discovery of some relationship attributes
			RelationshipName = foreignKeys->GetVal(wxT("conname"));

			wxString onUpd = foreignKeys->GetVal(wxT("confupdtype"));
			onUpdate = 	onUpd.IsSameAs('a') ? FK_ACTION_NO :
			            onUpd.IsSameAs('r') ? FK_RESTRICT :
			            onUpd.IsSameAs('c') ? FK_CASCADE :
			            onUpd.IsSameAs('d') ? FK_SETDEFAULT :
			            onUpd.IsSameAs('n') ? FK_SETNULL : FK_ACTION_NO;


			wxString onDel = foreignKeys->GetVal(wxT("confdeltype"));
			onDelete = 	onUpd.IsSameAs('a') ? FK_ACTION_NO :
			            onUpd.IsSameAs('r') ? FK_RESTRICT :
			            onUpd.IsSameAs('c') ? FK_CASCADE :
			            onUpd.IsSameAs('d') ? FK_SETDEFAULT :
			            onUpd.IsSameAs('n') ? FK_SETNULL : FK_ACTION_NO;

			wxString match = foreignKeys->GetVal(wxT("confmatchtype"));
			matchSimple = 	match.IsSameAs('f') ? false :
			                match.IsSameAs('u') ? true : false;


			//------ Preparing metada to allow discovery of some relationship attributes
			//Source table columns
			wxString fkColsSourceTable = foreignKeys->GetVal(wxT("confkey"));
			wxSortedArrayInt sourcePKs(sortFunc);
			wxSortedArrayInt destPKs(sortFunc);
			//remove {} of string
			fkColsSourceTable.Remove(0, 1);
			fkColsSourceTable.RemoveLast();
			wxString fkColsDestTable = foreignKeys->GetVal(wxT("conkey"));
			//remove {} of string
			fkColsDestTable.Remove(0, 1);
			fkColsDestTable.RemoveLast();

			//Separe columns from sourceFk
			wxStringTokenizer confkey(fkColsSourceTable);
			while (confkey.HasMoreTokens())
			{
				wxString str = confkey.GetNextToken();
				sourceFkCols.Add(StrToLong(str));
			}

			//Separe columns from destFk
			wxStringTokenizer conkey(fkColsDestTable);
			while (conkey.HasMoreTokens())
			{
				wxString str = conkey.GetNextToken();
				destFkCols.Add(StrToLong(str));
			}

			//Get Stub of source table
			ddStubTable *sourceStubTable =	ddImportDBUtils::getTable(connection, sourceTableName, sourceOID);

			//Get PK columns of source
			stubColsHashMap::iterator it;
			ddStubColumn *column;
			for (it = sourceStubTable->cols.begin(); it != sourceStubTable->cols.end(); ++it)
			{
				wxString key = it->first;
				column = it->second;
				if(column->isPrimaryKey)
					sourcePKs.Add(column->pgColNumber);
			}

			//Get PK columns of dest
			for (it = destStubTable->cols.begin(); it != destStubTable->cols.end(); ++it)
			{
				wxString key = it->first;
				column = it->second;
				if(column->isPrimaryKey)
					destPKs.Add(column->pgColNumber);
			}

			//  Source Table  ----------------------<| Destination Table

			//first check: number of columns used as fk at Source is the same of the pk at Source
			if(sourceFkCols.Count() == sourcePKs.Count())
			{
				int i;
				//Because postgres columns numbers are stored in an ordered array,
				//their index should be the same at all positions
				int srcFkCount = sourceFkCols.Count();
				for(i = 0; i < srcFkCount; i++)
				{
					if( sourceFkCols[i] != sourcePKs[i] )
					{
						fkFromPk = false;
						break;
					}
				}
			}
			else
			{
				fkFromPk = true;
			}

			//------ Finding fk from uk or pk?
			int ukIndex = -1;
			//if fkFromPk = false then is fkfromUK?, check that
			//all source fk columns should belong to one Uk at source table.
			if( fkFromPk == false )
			{
				bool error = false;
				int baseColNumber = sourceFkCols[sourceFkCols.Count() - 1];
				int baseUkIdxSourceCol = sourceStubTable->getColumnByNumber(baseColNumber)->uniqueKeyIndex;
				int nextColNumber, nextUkIdxSourceCol;
				int countSrcFkCols = sourceFkCols.Count() - 2;
				while(countSrcFkCols >= 0)
				{
					nextColNumber = sourceFkCols[countSrcFkCols];
					nextUkIdxSourceCol = sourceStubTable->getColumnByNumber(nextColNumber)->uniqueKeyIndex;
					countSrcFkCols--;
					if(baseUkIdxSourceCol != nextUkIdxSourceCol)
					{
						error = true;
						wxMessageBox(_("Error detecting kind of foreign key source: from Pk or from Uk"), _("Error importing relationship"),  wxICON_ERROR | wxOK);
						delete foreignKeys;
						return false;
					}
				}
				if(!error)
				{
					ukIndex = baseUkIdxSourceCol;
				}
			}

			//Last check of consistency
			if(fkFromPk == false && ukIndex < 0)
			{
				wxMessageBox(_("Error detecting kind of foreign key source: from Pk or from Uk"), _("Error importing relationship"),  wxICON_ERROR | wxOK);
				delete foreignKeys;
				return false;
			}


			//------ identifying relationship or not  -----|-<|?
			//first check: number of columns used as fk at Source is the same of the pk at Source
			if(destFkCols.Count() == destPKs.Count())
			{
				int i;
				//Because postgres columns numbers are stored in an ordered array,
				//their index should be the same at all positions
				int destFkCount = destFkCols.Count();
				for(i = 0; i < destFkCount; i++)
				{
					if( destFkCols[i] != destPKs[i] )
					{
						identifying = false;
						break;
					}
				}
			}
			else
			{
				identifying = false;
			}

			//------ 1:1 or 1:M  ?  as a fact 1:1 have a fk,uk at destination table.
			// A foreign key have an one to many relationship when there is an UK for same column(s)
			// inside the foreign key. Assumption, a column on belong to one Uk (no more than one).
			int baseColNumber = destFkCols[destFkCols.Count() - 1];
			int baseUkIdxDestCol = destStubTable->getColumnByNumber(baseColNumber)->uniqueKeyIndex;
			if(baseUkIdxDestCol != -1)
			{
				oneToMany = false;
				int nextUkIdxDestCol, nextColNumber;
				int countDestFkCols = destFkCols.Count() - 2;
				while(countDestFkCols >= 0)
				{
					nextColNumber = destFkCols[countDestFkCols];
					nextUkIdxDestCol = destStubTable->getColumnByNumber(nextColNumber)->uniqueKeyIndex;
					countDestFkCols--;
					//if a dest fk column is not in the same Uk index of first one
					if(nextUkIdxDestCol != baseUkIdxDestCol)
					{
						oneToMany = true;
						break;
					}
				}
			}

			//Step two check all column of fk are inside a unique key (all and not more)
			if(oneToMany == false)  //assumption is 1:1 relationship until now
			{
				int numberColsInUk = 0, nextUkIdxDestCol, nextColNumber;
				ddStubColumn *item;
				for (it = destStubTable->cols.begin(); it != destStubTable->cols.end(); ++it)
				{
					wxString key = it->first;
					item = it->second;
					//at each column with same uk index that base comparison column, count it
					nextColNumber = item->pgColNumber;
					nextUkIdxDestCol = destStubTable->getColumnByNumber(nextColNumber)->uniqueKeyIndex;
					if( nextUkIdxDestCol == baseUkIdxDestCol)
					{
						numberColsInUk++;
					}
				}

				//number of columns in uk used by relationship is bigger or lesser than number of columns
				//in destination table used by relationship as fk dest(dest fk columnn), then is not 1:1
				if(numberColsInUk != destFkCols.Count())
					oneToMany = true;
			}

			//Optional or Mandatory consistency
			int countDestFkCols = destFkCols.Count() - 1;
			bool isNotNull;
			int nnCols = 0, nullCols = 0, nextColNumber;
			while(countDestFkCols >= 0)
			{
				nextColNumber = destFkCols[countDestFkCols];
				isNotNull = destStubTable->getColumnByNumber(nextColNumber)->isNotNull;
				countDestFkCols--;
				if(isNotNull)
					nnCols++;
				else
					nullCols++;
			}

			if(nnCols == 0 && nullCols > 0)
			{
				mandatoryRelationship = false;
			}
			else if(nnCols > 0 && nullCols == 0)
			{
				mandatoryRelationship = true;
			}
			else
			{
				wxMessageBox(_("Error detecting kind of foreign key: null or not null"), _("Error importing relationship"),  wxICON_ERROR | wxOK);
				delete foreignKeys;
				return false;
			}
			delete sourceStubTable;
			foreignKeys->MoveNext();
		}
		//After collection db info compare with model info

		//Is safe to check if this UKindex  is the same Ukindex of relationship because both aren't equal.
		//Before should by unified by same uk index like in comparing uk at table figure class
		//Todo in a future

		//relation is fromPk in model and db?
		if(fkFromPk && !relation->isForeignKeyFromPk())
			equalFk = false;

		//OnUpdateAction is the same kind?
		if(onUpdate != relation->getOnUpdateAction())
			equalFk = false;

		//OnDeleteAction  is the same kind?
		if(onDelete != relation->getOnDeleteAction())
			equalFk = false;

		//Are same match kind
		if(matchSimple != relation->getMatchSimple())
			equalFk = false;

		//are both identifying?
		if(identifying != relation->getIdentifying())
			equalFk = false;

		//are both 1:1 or 1:M
		if(oneToMany != relation->getOneToMany())
			equalFk = false;

		//Mandatory value is the same?
		if(mandatoryRelationship != relation->getMandatory())
			equalFk = false;

		//Columns at both arrays: created from model and created from db are supposed to have same number of item
		//if not fk has changed.

		//Number of source fk columns at DB is the same number of source fk columns at model
		if(sourceFkCols.Count() != srcPgs.Count())
			equalFk = false;

		//Number of destination fk columns at DB is the same number of destination fk columns at model
		if(destFkCols.Count() != destPgs.Count())
			equalFk = false;

		//Now because arrays are sorted they numbers should match exactly (same pg column number)
		int i, max = sourceFkCols.Count();
		for(i = 0; i < max; i++)
		{
			if(sourceFkCols[i] != srcPgs[i])
				equalFk = false;
		}

		max = destFkCols.Count();
		for(i = 0; i < max; i++)
		{
			if(destFkCols[i] != destPgs[i])
				equalFk = false;
		}

		return equalFk;
	}
	else
	{
		wxMessageBox(_("Error fk is repeated"), _("Error comparing relationships"),  wxICON_ERROR | wxOK);
	}
	delete foreignKeys;

	return equalFk;
}

ddTableFigure *ddImportDBUtils::getTableFigure(ddStubTable *table)
{
	wxString name = table->tableName;
	ddTableFigure *tableFigure = new ddTableFigure(name, -1, -1);
	if(tableFigure != NULL)
	{
		//Default Values
		int colsRowsSize = 0;
		int colsWindow = 0;
		int idxsRowsSize = 0;
		int idxsWindow = 0;
		int maxColIndex = 2;
		int minIdxIndex = 2;
		int maxIdxIndex = 2;
		int beginDrawCols = 2;
		int beginDrawIdxs = 2;

		tableFigure->InitTableValues(table->UniqueKeysNames, table->PrimaryKeyName, beginDrawCols, beginDrawIdxs, maxColIndex, minIdxIndex, maxIdxIndex, colsRowsSize, colsWindow, idxsRowsSize, idxsWindow);

		//Add Columns to Table
		stubColsHashMap::iterator it;
		ddStubColumn *item;
		for (it = table->cols.begin(); it != table->cols.end(); ++it)
		{
			wxString key = it->first;
			item = it->second;

			ddColumnOptionType option = item->isNotNull ? notnull : null;
			bool generateFkName = false; //Not automatic names will be used by default at user imported tables.

			wxString dataType = item->typeColumn->Name();

			//temporary conversion fix to datatype of designer should be improved in a future
			int s = -1, p = -1;
			bool useScale = true, needps = false;
			s = item->typeColumn->Length();
			p = item->typeColumn->Precision();

			if(dataType.IsSameAs(wxT("character varying"), false))
			{
				needps = true;
				dataType = wxT("varchar(n)");
			}

			else if(dataType.IsSameAs(wxT("numeric"), false))
			{
				needps = true;
				useScale = false;
				dataType = wxT("numeric(p,s)");
			}
			else if(dataType.IsSameAs(wxT("interval"), false))
			{
				needps = true;
				dataType = wxT("interval(n)");
			}
			else if(dataType.IsSameAs(wxT("bit"), false))
			{
				needps = true;
				dataType = wxT("bit(n)");
			}
			else if(dataType.IsSameAs(wxT("char"), false))
			{
				needps = true;
				dataType = wxT("char(n)");
			}
			else if(dataType.IsSameAs(wxT("varbit"), false))
			{
				needps = true;
				dataType = wxT("varbit(n)");
			}
			else if(dataType.IsSameAs(wxT("character"), false))
			{
				needps = true;
				dataType = wxT("char(n)");
			}

			int precision = -1;
			int scale = -1;
			wxString colName = item->columnName;

			int ukindex = -1;  //By default no ukindex is set

			ddColumnFigure *columnFigure = new ddColumnFigure(colName, tableFigure, option, generateFkName, item->isPrimaryKey, dataType, precision, scale, ukindex, NULL, NULL);
			//a conversion problem I called precision to length of types, pgadmin called scale.
			//this should be fixed, at the same time the datatype subsystem of the database designer will be redesigned.

			/*
			Disable right now, it can be useful at the future when db designer will be improved again
			columnFigure->setPgAttNumCol(item->pgColNumber);
			*/
			if(needps)
			{
				if(useScale)
				{
					columnFigure->setPrecision(s);
					columnFigure->setScale(-1);
				}
				else
				{
					columnFigure->setPrecision(p);
					columnFigure->setScale(s);
				}
			}

			if(item->isUniqueKey())
			{
				columnFigure->setUniqueConstraintIndex(item->uniqueKeyIndex);
			}

			tableFigure->addColumn(-1, columnFigure);
			columnFigure->setRightIconForColumn();
		}
		return tableFigure;
	}
	else
	{
		return NULL;
	}
}

ddStubTable::ddStubTable()
{
	tableName = wxEmptyString;
}

ddStubTable::ddStubTable(wxString name, OID tableOID)
{
	tableName = name;
	OIDTable = tableOID;
}

ddStubTable::~ddStubTable()
{
}

ddStubColumn *ddStubTable::getColumnByNumber(int pgColNumber)
{
	stubColsHashMap::iterator it;
	ddStubColumn *item, *out = NULL;
	for (it = cols.begin(); it != cols.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		//If found pgColNumber at table columns
		if (item->pgColNumber == pgColNumber)
		{
			out = item;
			break;
		}
	}
	return out;
}

ddStubColumn::ddStubColumn(wxString name, OID oidSource, bool notNull, bool pk, pgDatatype *type, int ukIndex)
{
	columnName = name;
	OIDTable = oidSource;
	isNotNull = notNull;
	isPrimaryKey = pk;
	typeColumn = type;
	uniqueKeyIndex = ukIndex;
}

ddStubColumn::ddStubColumn(const ddStubColumn &copy)
{
	columnName = copy.columnName;
	OIDTable = copy.OIDTable;
	isNotNull = copy.isNotNull;
	isPrimaryKey = copy.isPrimaryKey;
	typeColumn = copy.typeColumn;
	uniqueKeyIndex = copy.uniqueKeyIndex;
}

ddStubColumn::ddStubColumn(wxString name, OID oidSource)
{
	columnName = name;
	OIDTable = oidSource;
	uniqueKeyIndex = -1;
	OIDTable = -1;
	typeColumn = NULL;
}

ddStubColumn::ddStubColumn()
{
	uniqueKeyIndex = -1;
	OIDTable = -1;
	typeColumn = NULL;
}

ddStubColumn::~ddStubColumn()
{
	if(typeColumn)
		delete typeColumn;
}

bool ddStubColumn::isUniqueKey()
{
	return uniqueKeyIndex > -1;
};

//
//
//
// -----  ddDBReverseEngineering Implementation
//
//
//

ddDBReverseEngineering::ddDBReverseEngineering(wxFrame *frame, ddDatabaseDesign *design, pgConn *connection, bool useSizer)
	: wxWizard(frame, wxID_ANY, wxT("Import tables from schema wizard"),
	           wxBitmap(*namespaces_png_bmp), wxDefaultPosition,
	           wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	conn = connection;
	OIDSelectedSchema = 0;
	figuresDesign = design;

	// a wizard page may be either an object of predefined class
	initialPage = new wxWizardPageSimple(this);

	frontText = new wxStaticText(initialPage, wxID_ANY,
	                             wxT("Import database tables to model tables wizard.\n")
	                             wxT("\n")
	                             wxT("The next pages will allow you to import database tables inside a database model.")
	                             wxT("\n\n")
	                             wxT("\nSome restrictions apply:\n\n")
	                             wxT("1. Columns that belong to more than one unique constraint aren't supported.\n\n")
	                             wxT("2. Relationships are imported only if both tables (source and destination) are imported.\n\n")
	                             wxT("3. User defined datatypes aren't supported.\n\n")
	                             wxT("4. No indexes, views, sequences and others objects different from tables/relationships can be imported.\n\n")
	                             wxT("5. Tables with same name cannot be imported.\n\n")
	                             wxT("6. Inherited tables cannot be imported.\n\n")
	                             , wxPoint(5, 5)
	                            );

	page2 = new SelSchemaPage(this, initialPage);
	initialPage->SetNext(page2);
	page3 = new SelTablesPage(this, page2);
	page2->SetNext(page3);
	page4 = new ReportPage(this, page3);
	page3->SetNext(page4);
	page4->SetNext(NULL);

	if ( useSizer )
	{
		// allow the wizard to size itself around the pages
		GetPageAreaSizer()->Add(initialPage);
	}
}

// Destructor
ddDBReverseEngineering::~ddDBReverseEngineering()
{
	if(frontText)
		delete frontText;
}



wxArrayString ddDBReverseEngineering::getTables()
{

	wxArrayString out;
	wxString query;

	tablesOIDHM.clear();


	// Get the child objects.
	query = wxT("SELECT oid, relname, relkind\n")
	        wxT("  FROM pg_class\n")
	        wxT(" WHERE relkind IN ('r') AND relnamespace = ") + NumToStr(OIDSelectedSchema) + wxT(";");

	pgSet *tables = conn->ExecuteSet(query);

	if (tables)
	{
		while (!tables->Eof())
		{
			wxString tmpname = tables->GetVal(wxT("relname"));
			wxString relkind = tables->GetVal(wxT("relkind"));

			if (relkind == wxT("r")) // Table
			{
				out.Add(tables->GetVal(wxT("relname")));
				tablesOIDHM[tables->GetVal(wxT("relname"))] = tables->GetOid(wxT("oid"));
			}

			tables->MoveNext();
		}

		delete tables;
	}

	return out;
}

void ddDBReverseEngineering::OnFinishPressed(wxWizardEvent &event)
{
	//Add Tables to the Model
	stubTablesHashMap::iterator it;
	ddStubTable *item;
	for (it = stubsHM.begin(); it != stubsHM.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		figuresDesign->addTableToModel(ddImportDBUtils::getTableFigure(item));
	}
	//Add All relationships to the Model
	ddImportDBUtils::getAllRelationships(getConnection(), stubsHM, getDesign());
}


//
//
//
// -----  SelSchemaPage Implementation
//
//
//

SelSchemaPage::SelSchemaPage(wxWizard *parent, wxWizardPage *prev)
	: wxWizardPage(parent)
{
	wparent = (ddDBReverseEngineering *) parent;
	m_prev = prev;
	m_next = NULL;

	// A top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL );
	this->SetSizer(topSizer);

	//Add a message
	message = new wxStaticText(this, wxID_STATIC, _("Please, select a schema to use at import tables process:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	topSizer->Add(message);
	topSizer->AddSpacer(10);

	//Get Schemas info
	if(wparent && wparent->getConnection())
		refreshSchemas(wparent->getConnection());

	//Add a listbox with schemas
	m_allSchemas = new wxListBox(this, DDALLSCHEMAS, wxDefaultPosition, wxDefaultSize, schemasNames, wxLB_SORT | wxLB_ALWAYS_SB | wxLB_SINGLE);
	topSizer->Add(m_allSchemas, 1, wxEXPAND);
}

SelSchemaPage::~SelSchemaPage()
{
	if(m_allSchemas)
		delete m_allSchemas;
	if(message)
		delete message;
}

void SelSchemaPage::OnWizardPageChanging(wxWizardEvent &event)
{
	if(event.GetDirection() && m_allSchemas->GetSelection() == wxNOT_FOUND)
	{
		wxMessageBox(_("Please, select a Schema to continue to next step."), _("Select a Schema..."), wxICON_WARNING | wxOK, this);
		event.Veto();
	}
	else if(event.GetDirection())
	{
		wparent->OIDSelectedSchema = schemasHM[schemasNames[m_allSchemas->GetSelection()]];
		wparent->schemaName = schemasNames[m_allSchemas->GetSelection()];
		wparent->page3->RefreshTablesList();
	}
}

void SelSchemaPage::refreshSchemas(pgConn *connection)
{

	schemasHM.clear();
	schemasNames.Clear();
	// Search Schemas and insert it
	wxString restr =  wxT(" WHERE ");

	restr += wxT("NOT ");
	restr += wxT("((nspname = 'pg_catalog' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pg_class' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'pgagent' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pga_job' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'information_schema' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'tables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname LIKE '_%' AND EXISTS (SELECT 1 FROM pg_proc WHERE proname='slonyversion' AND pronamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'dbo' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'systables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'sys' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'all_tables' AND relnamespace = nsp.oid LIMIT 1)))\n");

	if (connection->EdbMinimumVersion(8, 2))
	{
		restr += wxT("  AND nsp.nspparent = 0\n");
		// Do not show dbms_job_procedure in schemas
		if (!settings->GetShowSystemObjects())
			restr += wxT("AND NOT (nspname = 'dbms_job_procedure' AND EXISTS(SELECT 1 FROM pg_proc WHERE pronamespace = nsp.oid and proname = 'run_job' LIMIT 1))\n");
	}

	wxString sql;

	if (connection->BackendMinimumVersion(8, 1))
	{
		sql = wxT("SELECT CASE WHEN nspname LIKE E'pg\\\\_temp\\\\_%' THEN 1\n")
		      wxT("            WHEN (nspname LIKE E'pg\\\\_%') THEN 0\n");
	}
	else
	{
		sql = wxT("SELECT CASE WHEN nspname LIKE 'pg\\\\_temp\\\\_%' THEN 1\n")
		      wxT("            WHEN (nspname LIKE 'pg\\\\_%') THEN 0\n");
	}
	sql += wxT("            ELSE 3 END AS nsptyp, nspname, nsp.oid\n")
	       wxT("  FROM pg_namespace nsp\n")
	       + restr +
	       wxT(" ORDER BY 1, nspname");

	pgSet *schemas = connection->ExecuteSet(sql);

	if (schemas)
	{
		while (!schemas->Eof())
		{
			wxString name = schemas->GetVal(wxT("nspname"));
			long nsptyp = schemas->GetLong(wxT("nsptyp"));

			wxStringTokenizer tokens(settings->GetSystemSchemas(), wxT(","));
			while (tokens.HasMoreTokens())
			{
				wxRegEx regex(tokens.GetNextToken());
				if (regex.Matches(name))
				{
					nsptyp = SCHEMATYP_USERSYS;
					break;
				}
			}

			if (nsptyp <= SCHEMATYP_USERSYS && !settings->GetShowSystemObjects())
			{
				schemas->MoveNext();
				continue;
			}

			//Build Schema Tree item
			//this->AppendItem(rootNode, name , DD_IMG_FIG_SCHEMA, DD_IMG_FIG_SCHEMA, NULL);
			schemasNames.Add(name);
			schemasHM[name] = schemas->GetOid(wxT("oid"));
			schemas->MoveNext();
		}

		delete schemas;
	}

}

//
//
//
// -----  SelTablesPage Implementation
//
//
//

SelTablesPage::SelTablesPage(wxWizard *parent, wxWizardPage *prev)
	: wxWizardPage(parent)
{
	wparent = (ddDBReverseEngineering *) parent;
	m_prev = prev;
	m_next = NULL;

	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(2, 3, 0, 0);
	this->SetSizer(mainSizer);

	leftText = new wxStaticText(this, wxID_STATIC, _("Table(s) from selected schema"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	mainSizer->Add(leftText);
	centerText = new wxStaticText(this, wxID_STATIC, wxT(" "), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	mainSizer->Add(centerText);

	rightText = new wxStaticText(this, wxID_STATIC, _("Tables(s) to be imported"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	mainSizer->Add(rightText, wxALIGN_LEFT);

	//left listbox with all tables from selected schema
	m_allTables = new wxListBox( this, DDALLTABS, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED | wxLB_ALWAYS_SB | wxLB_SORT);
	mainSizer->AddGrowableRow(1);
	mainSizer->AddGrowableCol(0);
	mainSizer->Add(m_allTables , 1, wxEXPAND);

	addBitmap = *gqbOrderAdd_png_bmp;
	addAllBitmap = *gqbOrderAddAll_png_bmp;
	removeBitmap = *gqbOrderRemove_png_bmp;
	removeAllBitmap = *gqbOrderRemoveAll_png_bmp;

	buttonAdd = new wxBitmapButton( this, DDADD,  addBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add Column") );
	buttonAdd->SetToolTip(_("Add the selected table(s)"));
	buttonAddAll = new wxBitmapButton( this, DDADDALL,  addAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add All Columns") );
	buttonAddAll->SetToolTip(_("Add all tables"));
	buttonRemove = new wxBitmapButton( this, DDREMOVE,  removeBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove Column") );
	buttonRemove->SetToolTip(_("Remove the selected table(s)"));
	buttonRemoveAll = new wxBitmapButton( this, DDREMOVEALL,  removeAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove All Columns") );
	buttonRemoveAll->SetToolTip(_("Remove all tables"));

	wxBoxSizer *buttonsSizer = new wxBoxSizer( wxVERTICAL );

	buttonsSizer->Add(
	    this->buttonAdd,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	buttonsSizer->Add(
	    this->buttonAddAll,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	buttonsSizer->Add(
	    this->buttonRemove,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	buttonsSizer->Add(
	    this->buttonRemoveAll,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	mainSizer->Add(
	    buttonsSizer,
	    0,                                        // make vertically unstretchable
	    wxALIGN_CENTER );                         // no border and centre horizontally

	//right listbox with selected tables from schema to be imported.
	m_selTables = new wxListBox( this, DDSELTABS, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED | wxLB_ALWAYS_SB | wxLB_SORT);
	mainSizer->AddGrowableCol(2);
	mainSizer->Add(m_selTables , 1, wxEXPAND);
	mainSizer->Fit(this);
}

SelTablesPage::~SelTablesPage()
{
	if(rightText)
		delete rightText;
	if(centerText)
		delete centerText;
	if(leftText)
		delete leftText;
	if(m_allTables)
		delete m_allTables;
	if(m_selTables)
		delete m_selTables;
	if(buttonAdd)
		delete buttonAdd;
	if(buttonAddAll)
		delete buttonAddAll;
	if(buttonRemove)
		delete buttonRemove;
	if(buttonRemoveAll)
		delete buttonRemoveAll;
}

void SelTablesPage::RefreshTablesList()
{
	m_allTables->Set(wparent->getTables(), (void **)NULL);
}

void SelTablesPage::OnButtonAdd(wxCommandEvent &)
{
	wxArrayInt positions;
	if(m_allTables->GetSelections(positions) > 0)
	{
		int i;
		int size = positions.Count();
		for(i = 0; i < size; i++)
		{
			m_selTables->Append(m_allTables->GetString(positions[i]));
			m_allTables->Deselect(positions[i]);
		}

		for(i = (size - 1); i >= 0 ; i--)
		{
			m_allTables->Delete(positions[i]);
		}

		if(m_allTables->GetCount() > 0)
			m_allTables->Select(0);
	}
}

void SelTablesPage::OnButtonAddAll(wxCommandEvent &)
{
	int itemsCount = m_allTables->GetCount();
	if( itemsCount > 0)
	{
		do
		{
			m_allTables->Deselect(0);
			m_selTables->Append(m_allTables->GetString(0));
			m_allTables->Delete(0);
			itemsCount--;
		}
		while(itemsCount > 0);
	}
}

void SelTablesPage::OnButtonRemove(wxCommandEvent &)
{
	wxArrayInt positions;
	if(m_selTables->GetSelections(positions) > 0)
	{
		int i;
		int size = positions.Count();  //warning about conversion should be ignored
		for(i = 0; i < size; i++)
		{
			m_allTables->Append(m_selTables->GetString(positions[i]));
			m_selTables->Deselect(positions[i]);
		}

		for(i = (size - 1); i >= 0 ; i--)
		{
			m_selTables->Delete(positions[i]);
		}

		if(m_selTables->GetCount() > 0)
			m_selTables->Select(0);
	}
}

void SelTablesPage::OnButtonRemoveAll(wxCommandEvent &)
{
	int itemsCount = m_selTables->GetCount();
	if( itemsCount > 0)
	{
		do
		{
			m_selTables->Deselect(0);
			m_allTables->Append(m_selTables->GetString(0));
			m_selTables->Delete(0);
			itemsCount--;
		}
		while(itemsCount > 0);
	}
}

void SelTablesPage::OnWizardPageChanging(wxWizardEvent &event)
{
	if(event.GetDirection() && m_selTables->GetCount() <= 0)
	{
		wxMessageBox(_("Please, select at least a table to continue to next step."), _("Select some Tables..."), wxICON_WARNING | wxOK, this);
		event.Veto();
	}
	else if(event.GetDirection())
	{

		int itemsCount = m_selTables->GetCount();
		if( itemsCount > 0)
		{
			int item = 0;
			do
			{
				ddStubTable *table = ddImportDBUtils::getTable(wparent->getConnection(), m_selTables->GetString(item), wparent->tablesOIDHM[m_selTables->GetString(item)]);
				if(table == NULL)
				{
					ReportPage *tmp = (ReportPage *) m_next;
					tmp->results->AppendText(_("Error when preparing to import table: ") + m_selTables->GetString(item) + _(", this table have inherited columns and this feature is not supported at this moment.\n\n"));
				}
				else if(wparent->getDesign()->getTable(m_selTables->GetString(item)) != NULL)
				{
					ReportPage *tmp = (ReportPage *) m_next;
					tmp->results->AppendText(_("Error when preparing to import table: ") + m_selTables->GetString(item) + _(", this table already exists in the model and updating table at a model is not supported at this moment.\n\n"));
				}
				else if(table->tableName.Length() > 0)
				{
					if(m_next)
					{
						ReportPage *tmp = (ReportPage *) m_next;
						tmp->results->AppendText(_("Prepared to import table: ") + table->tableName + _("\n"));
						wparent->stubsHM[table->tableName] = table;
					}
				}
				else
				{
					if(m_next)
					{
						ReportPage *tmp = (ReportPage *) m_next;
						tmp->results->AppendText(_("Error when preparing to import table: ") + m_selTables->GetString(item) + _("\n"));
					}
				}
				item++;
			}
			while(item < itemsCount);
		}
	}
	else if(!event.GetDirection())
	{
		//Reset tables after a warning
		int answer = wxMessageBox(_("Returning to previous dialog will remove all selected tables, do you like to continue?"), _("Confirm"), wxYES_NO | wxCANCEL, this);
		if (answer == wxYES)
		{
			m_selTables->Clear();
			m_allTables->Clear();
			wparent->tablesOIDHM.clear();
		}
		else
			event.Veto();
	}
}

//
//
//
// -----  ReportPage Implementation
//
//
//

ReportPage::ReportPage(wxWizard *parent, wxWizardPage *prev)
	: wxWizardPage(parent)
{
	wparent = (ddDBReverseEngineering *) parent;
	m_prev = prev;
	m_next = NULL;

	// A top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL );
	this->SetSizer(topSizer);

	//Add a message
	results = new wxTextCtrl(this, wxID_ANY , wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_LEFT
	                        );
	topSizer->Add(results, 1, wxEXPAND);
	topSizer->Fit(this);
}

ReportPage::~ReportPage()
{
	if(results)
		delete results;
}

void ReportPage::OnWizardPageChanging(wxWizardEvent &event)
{
	if(!event.GetDirection())
	{
		int answer = wxMessageBox(_("Returning to previous dialog will delete imported tables before adding it to the model?"), _("Confirm"), wxYES_NO | wxCANCEL, this);
		if (answer == wxYES)
		{
			results->Clear();
			wparent->stubsHM.clear();
		}
		else
			event.Veto();
	}
}

