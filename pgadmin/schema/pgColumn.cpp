//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgColumn.cpp - Column class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"
#include "schema/pgDatatype.h"
#include "schema/pgColumn.h"


pgColumn::pgColumn(pgTable *newTable, const wxString &newName)
	: pgTableObject(newTable, columnFactory, newName)
{
	isFK = false;
	isPK = false;
	isReferenced = -1;
}

pgColumn::~pgColumn()
{
}


wxString pgColumn::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on column");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing column");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for column");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop column \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop column \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop column cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop column?");
			break;
		case PROPERTIESREPORT:
			message = _("Column properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Column properties");
			break;
		case DDLREPORT:
			message = _("Column DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Column DDL");
			break;
		case STATISTICSREPORT:
			message = _("Column statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Column statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Column dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Column dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Column dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Column dependents");
			break;
	}

	return message;
}


bool pgColumn::IsReferenced()
{
	if (isReferenced < 0)
	{
		isReferenced = (int)StrToLong(GetConnection()->ExecuteScalar(
		                                  wxT("SELECT COUNT(1) FROM pg_depend dep\n")
		                                  wxT("  JOIN pg_class cl ON dep.classid=cl.oid AND relname='pg_rewrite'\n")
		                                  wxT(" WHERE refobjid=") + GetTableOidStr()
		                                  + wxT(" AND refobjsubid=") + NumToStr(GetColNumber())));
	}

	return (isReferenced != 0);
}


bool pgColumn::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("ALTER TABLE ") + GetQuotedFullTable();
	sql += wxT(" DROP COLUMN ") + GetQuotedIdentifier();

	return GetDatabase()->ExecuteVoid(sql);
}


void pgColumn::ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &where)
{
	pgObject::ShowDependencies(form, Dependencies,
	                           wxT("\n WHERE dep.objid=") + table->GetOidStr() +
	                           wxT(" AND dep.objsubid=") + NumToStr(colNumber));
}


void pgColumn::ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where)
{
	pgObject::ShowDependents(form, referencedBy,
	                         wxT("\n WHERE dep.refobjid=") + table->GetOidStr() +
	                         wxT(" AND dep.refobjsubid=") + NumToStr(colNumber));
}

wxString pgColumn::GetSql(ctlTree *browser)
{
	if (sql.IsNull() && !GetSystemObject())
	{
		if (GetTable()->GetMetaType() == PGM_VIEW)
		{
			sql = wxT("-- Column: ") + GetQuotedIdentifier() + wxT("\n\n");

			if (!GetDefault().IsEmpty())
				sql += wxT("ALTER TABLE ") + GetQuotedFullTable()
				       + wxT(" ALTER COLUMN ") + GetQuotedIdentifier()
				       + wxT(" SET DEFAULT ") + GetDefault() + wxT(";\n");

			sql += GetCommentSql();
		}
		else if (GetTable()->GetMetaType() == PGM_CATALOGOBJECT || GetTable()->GetMetaType() == GP_EXTTABLE)
		{
			sql = wxT("-- Column: ") + GetQuotedIdentifier() + wxT("\n\n");
		}
		else
		{
			if (GetInheritedCount())
				sql = wxT("-- Column inherited; cannot be changed");
			else
			{
				sql = wxT("-- Column: ") + GetQuotedIdentifier() + wxT("\n\n")
				      + wxT("-- ALTER TABLE ") + GetQuotedFullTable()
				      + wxT(" DROP COLUMN ") + GetQuotedIdentifier() + wxT(";")

				      + wxT("\n\nALTER TABLE ") + GetQuotedFullTable()
				      + wxT(" ADD COLUMN ") + GetQuotedIdentifier() + wxT(" ")
				      + GetQuotedTypename();
				if (!GetCollation().IsEmpty() && GetCollation() != wxT("pg_catalog.\"default\""))
					sql += wxT(" COLLATE ") + GetCollation();
				sql += wxT(";\n");

				sql += GetStorageSql();

				if (GetNotNull())
					sql += wxT("ALTER TABLE ") + GetQuotedFullTable()
					       + wxT(" ALTER COLUMN ") + GetQuotedIdentifier()
					       + wxT(" SET NOT NULL;\n");
				if (!GetDefault().IsEmpty())
					sql += wxT("ALTER TABLE ") + GetQuotedFullTable()
					       + wxT(" ALTER COLUMN ") + GetQuotedIdentifier()
					       + wxT(" SET DEFAULT ") + GetDefault() + wxT(";\n");
				sql += GetAttstattargetSql();

				sql += GetVariablesSql();

				sql += GetCommentSql();

				if (GetDatabase()->BackendMinimumVersion(8, 4))
					sql += GetPrivileges();

				if (GetConnection()->BackendMinimumVersion(9, 1))
					sql += GetSeqLabelsSql();
			}
		}
	}

	return sql;
}

wxString pgColumn::GetCommentSql()
{
	wxString commentSql;

	if (!GetComment().IsEmpty())
		commentSql = wxT("COMMENT ON COLUMN ") + GetQuotedFullTable() + wxT(".") + GetQuotedIdentifier()
		             +  wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");

	return commentSql;
}

wxString pgColumn::GetStorageSql()
{
	wxString storageSql;

	if (GetStorage() != GetDefaultStorage())
		storageSql = wxT("ALTER TABLE ") + GetQuotedFullTable()
		             + wxT(" ALTER COLUMN ") + GetQuotedIdentifier()
		             + wxT(" SET STORAGE ") + GetStorage() + wxT(";\n");

	return storageSql;
}

wxString pgColumn::GetAttstattargetSql()
{
	wxString attstattargetSql;

	if (GetAttstattarget() >= 0)
		attstattargetSql = wxT("ALTER TABLE ") + GetQuotedFullTable()
		                   + wxT(" ALTER COLUMN ") + GetQuotedIdentifier()
		                   + wxT(" SET STATISTICS ") + NumToStr(GetAttstattarget()) + wxT(";\n");

	return attstattargetSql;
}

wxString pgColumn::GetVariablesSql()
{
	wxString variablesSql;

	size_t i;
	for (i = 0 ; i < variables.GetCount() ; i++)
		variablesSql += wxT("ALTER TABLE ") + GetQuotedFullTable()
		                + wxT(" ALTER COLUMN ") + GetQuotedIdentifier()
		                + wxT(" SET (") + variables.Item(i) + wxT(");\n");

	return variablesSql;
}

wxString pgColumn::GetPrivileges()
{
	wxString privileges;
	wxString strAcl = GetAcl();
	if (!strAcl.IsEmpty())
	{
		wxArrayString aclArray;
		strAcl = strAcl.Mid(1, strAcl.Length() - 2);
		getArrayFromCommaSeparatedList(strAcl, aclArray);
		wxString role;
		for (unsigned int index = 0; index < aclArray.Count(); index++)
		{
			wxString strCurrAcl = aclArray[index];
			/*
			* In rare case, we can have ',' (comma) in the user name.
			* But, we need to handle them also
			*/
			if (strCurrAcl.Find(wxChar('=')) == wxNOT_FOUND)
			{
				// Check it is start of the ACL
				if (strCurrAcl[0U] == (wxChar)'"')
					role = strCurrAcl + wxT(",");
				continue;
			}
			else
				strCurrAcl = role + strCurrAcl;

			if (strCurrAcl[0U] == (wxChar)'"')
				strCurrAcl = strCurrAcl.Mid(1, strCurrAcl.Length() - 1);
			role = strCurrAcl.BeforeLast('=');
			wxString value = strCurrAcl.Mid(role.Length() + 1).BeforeLast('/');

			if (role.Left(6).IsSameAs(wxT("group "), false))
			{
				role = wxT("group ") + qtIdent(qtStrip(role.Mid(6)));
			}
			else if (role.IsEmpty())
			{
				role = wxT("public");
			}
			else
				role = qtIdent(qtStrip(role));

			privileges += pgObject::GetPrivileges(wxT("awrx"), value, GetQuotedFullTable(), role, GetQuotedIdentifier());
			role.Clear();
		}
	}
	return privileges;
}
wxString pgColumn::GetDefinition()
{
	wxString sql = wxEmptyString;
	wxString seqDefault1, seqDefault2;

	if (table->GetOfTypeOid() == 0)
		sql += GetQuotedTypename();

	if (!GetCollation().IsEmpty() && GetCollation() != wxT("pg_catalog.\"default\""))
		sql += wxT(" COLLATE ") + GetCollation();

	wxString full_tabname = wxEmptyString;
	if (GetDatabase()->BackendMinimumVersion(8, 1))
	{
		wxString schpref;

		schpref = schema->GetQuotedPrefix();
		full_tabname = GetTableName() + wxT("_") + GetName() + wxT("_seq");

		// If the generated sequence name is longer than 64 characters, then
		//   - If both table & column name exceed 29 chars, truncate both to 29
		//   - If one of table or column name exceeds 29 chars, truncate it to
		//     29 chars plus however much less than 29 chars the other name is.
		if (full_tabname.Length() > 64)
		{
			int tlen = GetTableName().Length();
			int clen = GetName().Length();

			if (tlen > 29 && clen > 29)
				full_tabname = GetTableName().Left(29) + wxT("_") + GetName().Left(29) + wxT("_seq");
			else if (tlen > 29)
				full_tabname = GetTableName().Left(29 + (29 - clen)) + wxT("_") + GetName() + wxT("_seq");
			else if (clen > 29)
				full_tabname = GetTableName() + wxT("_") + GetName().Left(29 + (29 - tlen)) + wxT("_seq");
		}

		full_tabname = qtIdent(full_tabname);

		if (schpref != wxEmptyString)
			full_tabname = schpref + full_tabname;

		seqDefault1 = wxT("nextval('") + full_tabname + wxT("'::regclass)");
		seqDefault2 = seqDefault1;
	}
	else
	{
		seqDefault1 = wxT("nextval('")
		              + schema->GetName() + wxT(".") + GetTableName()
		              + wxT("_") + GetName() + wxT("_seq'::text)");
		seqDefault2 = wxT("nextval('\"")
		              + schema->GetName() + wxT(".") + GetTableName()
		              + wxT("_") + GetName() + wxT("_seq\"'::text)");
	}

	if ((sql == wxT("integer") || sql == wxT("bigint") || sql == wxT("smallint") ||
	        sql == wxT("pg_catalog.integer") || sql == wxT("pg_catalog.bigint") || sql == wxT("pg_catalog.smallint"))
	        && (GetDefault() == seqDefault1 || GetDefault() == seqDefault2))
	{
		if (GetDatabase()->BackendMinimumVersion(8, 1))
		{
			pgSet *set = ExecuteSet(
			                 wxT("SELECT classid\n")
			                 wxT("  FROM pg_depend\n")
			                 wxT(" WHERE refobjid=") + table->GetOidStr() +
			                 wxT(" AND refobjsubid = ") + NumToStr(GetColNumber()) +
			                 wxT(" AND objid = '") + full_tabname + wxT("'::regclass") +
			                 wxT(" AND deptype='a'"));

			if (set && set->NumRows())
			{
				if (sql.Right(6) == wxT("bigint"))
					sql = wxT("bigserial");
				else if (sql.Right(8) == wxT("smallint"))
					sql = wxT("smallserial");
				else
					sql = wxT("serial");
			}

			if (GetNotNull())
				sql += wxT(" NOT NULL");

			if (!set || !(set->NumRows()))
				AppendIfFilled(sql, wxT(" DEFAULT "), GetDefault());

			if (set)
				delete set;
		}
		else
		{
			if (sql.Right(6) == wxT("bigint"))
				sql = wxT("bigserial");
			else if (sql.Right(8) == wxT("smallint"))
				sql = wxT("smallserial");
			else
				sql = wxT("serial");

			if (GetNotNull())
				sql += wxT(" NOT NULL");
		}
	}
	else
	{
		if (GetNotNull())
			sql += wxT(" NOT NULL");
		AppendIfFilled(sql, wxT(" DEFAULT "), GetDefault());
	}
	return sql;
}


void pgColumn::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		wxStringTokenizer indkey(pkCols);
		while (indkey.HasMoreTokens())
		{
			wxString str = indkey.GetNextToken();
			if (StrToLong(str) == GetColNumber())
			{
				isPK = true;
				break;
			}
		}

		if (!GetDatabase()->BackendMinimumVersion(7, 4))
		{
			// 7.3 misses the ANY(array) comparison
			pgSet *set = ExecuteSet(
			                 wxT("SELECT conkey\n")
			                 wxT("  FROM pg_constraint ct\n")
			                 wxT("  JOIN pg_class cl on cl.oid=confrelid\n")
			                 wxT(" WHERE contype='f' AND conrelid = ") + GetTableOidStr() + wxT("\n")
			                 wxT(" ORDER BY conname"));
			if (set)
			{
				wxString str;
				while (!isFK && !set->Eof())
				{
					wxStringTokenizer conkey(set->GetVal(0));

					while (conkey.HasMoreTokens())
					{
						str = conkey.GetNextToken();
						if (StrToLong(str.Mid(1)) == GetColNumber())
						{
							isFK = true;
							break;
						}
					}

					set->MoveNext();
				}
				delete set;
			}
		}
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("Position"), GetColNumber());
		properties->AppendItem(_("Data type"), GetVarTypename());
		if (GetDatabase()->BackendMinimumVersion(9, 1))
		{
			properties->AppendItem(_("Collation"), GetCollation());
		}
		if (GetTable()->GetMetaType() != PGM_CATALOGOBJECT)
		{
			properties->AppendItem(_("Default"), GetDefault());
			if (GetTable()->GetMetaType() != PGM_VIEW && GetTable()->GetMetaType() != GP_EXTTABLE)
			{
				properties->AppendItem(_("Sequence"), database->GetSchemaPrefix(GetSerialSchema()) + GetSerialSequence());

				properties->AppendYesNoItem(_("Not NULL?"), GetNotNull());
				properties->AppendYesNoItem(_("Primary key?"), GetIsPK());
				properties->AppendYesNoItem(_("Foreign key?"), GetIsFK());
				properties->AppendItem(_("Storage"), GetStorage());
				if (GetInheritedCount() != 0)
				{
					properties->AppendItem(_("Inherited"),
					                       wxT("Yes (from table ") + GetInheritedTableName() + wxT(")"));
				}
				else
				{
					properties->AppendYesNoItem(_("Inherited"), false);
				}
				properties->AppendItem(_("Statistics"), GetAttstattarget());
				size_t i;
				for (i = 0 ; i < variables.GetCount() ; i++)
				{
					wxString item = variables.Item(i);
					properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
				}
				properties->AppendYesNoItem(_("System column?"), GetSystemObject());
			}
		}
		if (GetDatabase()->BackendMinimumVersion(8, 4))
		{
			properties->AppendItem(_("ACL"), GetAcl());
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
}


void pgColumn::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	DisplayStatistics(statistics,
	                  wxT("SELECT null_frac AS ") + qtIdent(_("Null Fraction")) +
	                  wxT(", avg_width AS ") + qtIdent(_("Average Width")) +
	                  wxT(", n_distinct AS ") + qtIdent(_("Distinct Values")) +
	                  wxT(", most_common_vals AS ") + qtIdent(_("Most Common Values")) +
	                  wxT(", most_common_freqs AS ") + qtIdent(_("Most Common Frequencies")) +
	                  wxT(", histogram_bounds AS ") + qtIdent(_("Histogram Bounds")) +
	                  wxT(", correlation AS ") + qtIdent(_("Correlation")) + wxT("\n")
	                  wxT("  FROM pg_stats\n")
	                  wxT(" WHERE schemaname = ") + qtDbString(schema->GetName()) + wxT("\n")
	                  wxT("   AND tablename = ") + qtDbString(GetTableName()) + wxT("\n")
	                  wxT("   AND attname = ") + qtDbString(GetName()));
}


pgObject *pgColumn::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *column = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		column = columnFactory.CreateObjects(coll, 0, wxT("\n   AND att.attnum=") + NumToStr(GetColNumber()));

	return column;
}



pgObject *pgColumnFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	pgTableObjCollection *collection = (pgTableObjCollection *)coll;
	pgColumn *column = 0;
	pgDatabase *database = collection->GetDatabase();
	inheritHashMap inhMap;

	// grab inherited tables with attibute names
	pgSet *inhtables = database->ExecuteSet(
	                       wxT("SELECT\n")
	                       wxT("    array_to_string(array_agg(inhrelname), ', ') inhrelname,\n")
	                       wxT("    attrname\n")
	                       wxT("FROM\n")
	                       wxT("    (SELECT\n")
	                       wxT("        inhparent::regclass AS inhrelname,\n")
	                       wxT("        a.attname AS attrname\n")
	                       wxT("    FROM\n")
	                       wxT("        pg_inherits i\n")
	                       wxT("        LEFT JOIN pg_attribute a ON\n")
	                       wxT("            (attrelid = inhparent AND attnum > 0)\n")
	                       wxT("    WHERE inhrelid = ") + collection->GetOidStr() + wxT("::oid\n")
	                       wxT("    ORDER BY inhseqno) a\n")
	                       wxT("GROUP BY attrname"));

	if (inhtables)
	{
		while (!inhtables->Eof())
		{
			wxString attrName = inhtables->GetVal(wxT("attrname"));
			wxString inhrelName = inhtables->GetVal(wxT("inhrelname"));
			inhMap[attrName] = inhrelName;
			inhtables->MoveNext();
		}

		delete inhtables;
	}

	wxString systemRestriction;
	if (!settings->GetShowSystemObjects())
		systemRestriction = wxT("\n   AND att.attnum > 0");

	wxString sql =
	    wxT("SELECT att.*, def.*, pg_catalog.pg_get_expr(def.adbin, def.adrelid) AS defval, CASE WHEN att.attndims > 0 THEN 1 ELSE 0 END AS isarray, format_type(ty.oid,NULL) AS typname, format_type(ty.oid,att.atttypmod) AS displaytypname, tn.nspname as typnspname, et.typname as elemtypname,\n")
	    wxT("  ty.typstorage AS defaultstorage, cl.relname, na.nspname, att.attstattarget, description, cs.relname AS sername, ns.nspname AS serschema,\n")
	    wxT("  (SELECT count(1) FROM pg_type t2 WHERE t2.typname=ty.typname) > 1 AS isdup, indkey");

	if (database->BackendMinimumVersion(9, 1))
		sql += wxT(",\n  coll.collname, nspc.nspname as collnspname");
	if (database->BackendMinimumVersion(8, 5))
		sql += wxT(",\n  attoptions");
	if (database->BackendMinimumVersion(7, 4))
		sql +=
		    wxT(",\n")
		    wxT("  EXISTS(SELECT 1 FROM  pg_constraint WHERE conrelid=att.attrelid AND contype='f'")
		    wxT(" AND att.attnum=ANY(conkey)) As isfk");
	if (database->BackendMinimumVersion(9, 1))
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
	       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=att.attrelid AND des.objsubid=att.attnum AND des.classoid='pg_class'::regclass)\n")
	       wxT("  LEFT OUTER JOIN (pg_depend JOIN pg_class cs ON classid='pg_class'::regclass AND objid=cs.oid AND cs.relkind='S') ON refobjid=att.attrelid AND refobjsubid=att.attnum\n")
	       wxT("  LEFT OUTER JOIN pg_namespace ns ON ns.oid=cs.relnamespace\n")
	       wxT("  LEFT OUTER JOIN pg_index pi ON pi.indrelid=att.attrelid AND indisprimary\n");
	if (database->BackendMinimumVersion(9, 1))
		sql += wxT("  LEFT OUTER JOIN pg_collation coll ON att.attcollation=coll.oid\n")
		       wxT("  LEFT OUTER JOIN pg_namespace nspc ON coll.collnamespace=nspc.oid\n");
	sql += wxT(" WHERE att.attrelid = ") + collection->GetOidStr()
	       + restriction + systemRestriction + wxT("\n")
	       wxT("   AND att.attisdropped IS FALSE\n")
	       wxT(" ORDER BY att.attnum");

	pgSet *columns = database->ExecuteSet(sql);
	if (columns)
	{
		while (!columns->Eof())
		{
			wxString attrName = columns->GetVal(wxT("attname"));
			column = new pgColumn(collection->GetTable(), attrName);

			column->iSetAttTypId(columns->GetOid(wxT("atttypid")));
			column->iSetColNumber(columns->GetLong(wxT("attnum")));
			column->iSetIsArray(columns->GetBool(wxT("isarray")));
			column->iSetComment(columns->GetVal(wxT("description")));
			column->iSetSerialSequence(columns->GetVal(wxT("sername")));
			column->iSetSerialSchema(columns->GetVal(wxT("serschema")));
			column->iSetPkCols(columns->GetVal(wxT("indkey")));
			if (database->BackendMinimumVersion(7, 4))
				column->iSetIsFK(columns->GetBool(wxT("isfk")));

			if (columns->GetBool(wxT("atthasdef")))
				column->iSetDefault(columns->GetVal(wxT("defval")));
			column->iSetStatistics(columns->GetLong(wxT("attstattarget")));

			wxString storage = columns->GetVal(wxT("attstorage"));
			column->iSetStorage(
			    storage == wxT("p") ? wxT("PLAIN") :
			    storage == wxT("e") ? wxT("EXTERNAL") :
			    storage == wxT("m") ? wxT("MAIN") :
			    storage == wxT("x") ? wxT("EXTENDED") : wxT("Unknown"));
			wxString defaultStorage = columns->GetVal(wxT("defaultstorage"));
			column->iSetDefaultStorage(
			    defaultStorage == wxT("p") ? wxT("PLAIN") :
			    defaultStorage == wxT("e") ? wxT("EXTERNAL") :
			    defaultStorage == wxT("m") ? wxT("MAIN") :
			    defaultStorage == wxT("x") ? wxT("EXTENDED") : wxT("Unknown"));

			column->iSetTyplen(columns->GetLong(wxT("attlen")));

			long typmod = columns->GetLong(wxT("atttypmod"));
			pgDatatype dt(columns->GetVal(wxT("typnspname")), columns->GetVal(wxT("typname")),
			              columns->GetBool(wxT("isdup")),
			              columns->GetLong(wxT("attndims")), typmod);


			column->iSetTypmod(typmod);
			column->iSetLength(dt.Length());
			column->iSetPrecision(dt.Precision());
			column->iSetRawTypename(dt.Name());

			column->iSetVarTypename(dt.FullName());
			column->iSetQuotedTypename(columns->GetVal(wxT("displaytypname")));

			column->iSetNotNull(columns->GetBool(wxT("attnotnull")));
			column->iSetQuotedFullTable(database->GetQuotedSchemaPrefix(columns->GetVal(wxT("nspname")))
			                            + qtIdent(columns->GetVal(wxT("relname"))));
			column->iSetTableName(columns->GetVal(wxT("relname")));
			column->iSetInheritedCount(columns->GetLong(wxT("attinhcount")));

			// Check whether the attribute is inherited
			inheritHashMap::iterator it = inhMap.find(attrName);
			if (it != inhMap.end())
				column->iSetInheritedTableName(it->second);

			column->iSetIsLocal(columns->GetBool(wxT("attislocal")));
			column->iSetAttstattarget(columns->GetLong(wxT("attstattarget")));
			if (database->BackendMinimumVersion(8, 5))
			{
				wxString str = columns->GetVal(wxT("attoptions"));
				if (!str.IsEmpty())
					FillArray(column->GetVariables(), str.Mid(1, str.Length() - 2));
			}
			if (database->BackendMinimumVersion(8, 4))
				column->iSetAcl(columns->GetVal(wxT("attacl")));
			if (database->BackendMinimumVersion(9, 1))
			{
				wxString coll = wxEmptyString;
				if (!columns->GetVal(wxT("collname")).IsEmpty())
					coll = qtIdent(columns->GetVal(wxT("collnspname"))) + wxT(".") + qtIdent(columns->GetVal(wxT("collname")));
				column->iSetCollation(coll);
			}

			if (database->BackendMinimumVersion(9, 1))
			{
				column->iSetProviders(columns->GetVal(wxT("providers")));
				column->iSetLabels(columns->GetVal(wxT("labels")));
			}

			if (browser)
			{
				browser->AppendObject(collection, column);
				columns->MoveNext();
			}
			else
				break;
		}

		delete columns;
	}

	inhMap.clear();
	return column;
}

/////////////////////////////

pgColumnCollection::pgColumnCollection(pgaFactory *factory, pgTable *tbl)
	: pgTableObjCollection(factory, tbl)
{
}


wxString pgColumnCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on columns");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing columns");
			break;
		case OBJECTSLISTREPORT:
			message = _("Columns list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/column.pngc"
#include "images/column-sm.pngc"
#include "images/columns.pngc"

pgColumnFactory::pgColumnFactory()
	: pgTableObjFactory(__("Column"), __("New Column..."), __("Create a new Column."), column_png_img, column_sm_png_img)
{
	metaType = PGM_COLUMN;
}


pgCollection *pgColumnFactory::CreateCollection(pgObject *obj)
{
	return new pgColumnCollection(GetCollectionFactory(), (pgTable *)obj);
}

pgColumnFactory columnFactory;
static pgaCollectionFactory cf(&columnFactory, __("Columns"), columns_png_img);
