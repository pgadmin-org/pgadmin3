//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgDatatype.cpp - PostgreSQL Datatypes
//
//////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgDatatype.h"
#include "schema/pgDatabase.h"
#include "utils/pgDefs.h"


pgDatatype::pgDatatype(const wxString &nsp, const wxString &typname, bool isDup, long numdims, long typmod)
{
	needSchema = isDup;
	schema = nsp;

	// Above 7.4, format_type also sends the schema name if it's not included
	// in the search_path, so we need to skip it in the typname
	if (typname.Contains(schema + wxT("\".")))
		name = typname.Mid(schema.Len() + 3); // "+2" because of the two double quotes
	else if (typname.Contains(schema + wxT(".")))
		name = typname.Mid(schema.Len() + 1);
	else
		name = typname;

	if (name.StartsWith(wxT("_")))
	{
		if (!numdims)
			numdims = 1;
		name = name.Mid(1);
	}
	if (name.Right(2) == wxT("[]"))
	{
		if (!numdims)
			numdims = 1;
		name = name.Left(name.Len() - 2);
	}

	if (name.StartsWith(wxT("\"")) && name.EndsWith(wxT("\"")))
		name = name.Mid(1, name.Len() - 2);

	if (numdims > 0)
	{
		while (numdims--)
			array += wxT("[]");
	}

	if (typmod != -1)
	{
		length = wxT("(");
		if (name == wxT("numeric"))
		{
			len = (typmod - 4L) >> 16L;
			prec = (typmod - 4) & 0xffff;
			length += NumToStr(len);
			if (prec)
				length += wxT(",") + NumToStr(prec);
		}
		else if (name == wxT("time") || name == wxT("timetz")
		         || name == wxT("time without time zone") || name == wxT("time with time zone")
		         || name == wxT("timestamp") || name == wxT("timestamptz")
		         || name == wxT("timestamp without time zone") || name == wxT("timestamp with time zone")
		         || name == wxT("bit") || name == wxT("bit varying") || name == wxT("varbit"))
		{
			prec = 0;
			len = typmod;
			length += NumToStr(len);
		}
		else if (name == wxT("interval"))
		{
			prec = 0;
			len = (typmod & 0xffff);
			length += NumToStr(len);
		}
		else if (name == wxT("date"))
		{
			len = prec = 0;
			length = wxT(""); /* Clear Length */
		}
		else
		{
			prec = 0;
			len = typmod - 4L;
			length += NumToStr(len);
		}

		if (length.Length() > 0)
			length += wxT(")");
	}
	else
		len = prec = 0;
}

// Return the full name of the type, with dimension and array qualifiers
wxString pgDatatype::FullName() const
{
	if (name == wxT("char") && schema == wxT("pg_catalog"))
		return wxT("\"char\"") + array;
	else if (name == wxT("time with time zone"))
		return wxT("time") + length + wxT(" with time zone") + array;
	else if (name == wxT("time without time zone"))
		return wxT("time") + length + wxT(" without time zone") + array;
	else if (name == wxT("timestamp with time zone"))
		return wxT("timestamp") + length + wxT(" with time zone") + array;
	else if (name == wxT("timestamp without time zone"))
		return wxT("timestamp") + length + wxT(" without time zone") + array;
	else
		return name + length + array;
}

// Return the quoted full name of the type, with dimension and array qualifiers
wxString pgDatatype::QuotedFullName() const
{
	if (name == wxT("char") && schema == wxT("pg_catalog"))
		return wxT("\"char\"") + array;
	else if (name == wxT("time with time zone"))
		return wxT("time") + length + wxT(" with time zone") + array;
	else if (name == wxT("time without time zone"))
		return wxT("time") + length + wxT(" without time zone") + array;
	else if (name == wxT("timestamp with time zone"))
		return wxT("timestamp") + length + wxT(" with time zone") + array;
	else if (name == wxT("timestamp without time zone"))
		return wxT("timestamp") + length + wxT(" without time zone") + array;
	else
		return qtTypeIdent(name) + length + array;
}

wxString pgDatatype::GetSchemaPrefix(pgDatabase *db) const
{
	if (schema.IsEmpty() || (!db && schema == wxT("pg_catalog")))
		return wxEmptyString;

	if (needSchema)
		return schema + wxT(".");

	return db->GetSchemaPrefix(schema);
}


wxString pgDatatype::GetQuotedSchemaPrefix(pgDatabase *db) const
{
	wxString str = GetSchemaPrefix(db);
	if (!str.IsEmpty())
		return qtIdent(str.Left(str.Length() - 1)) + wxT(".");
	return str;
}


long pgDatatype::GetTypmod(const wxString &name, const wxString &len, const wxString &prec)
{
	if (len.IsEmpty())
		return -1;
	if (name == wxT("numeric"))
	{
		return (((long)StrToLong(len) << 16) + StrToLong(prec)) + 4;
	}
	else if (name == wxT("time") || name == wxT("timetz")
	         || name == wxT("time without time zone") || name == wxT("time with time zone")
	         || name == wxT("timestamp") || name == wxT("timestamptz")
	         || name == wxT("timestamp without time zone") || name == wxT("timestamp with time zone")
	         || name == wxT("interval")  || name == wxT("bit") || name == wxT("bit varying") || name == wxT("varbit"))
	{
		return StrToLong(len);
	}
	else
	{
		return StrToLong(len) + 4;
	}
}


DatatypeReader::DatatypeReader(pgDatabase *db, const wxString &condition, bool addSerials)
{
	init(db, condition, addSerials);
}


DatatypeReader::DatatypeReader(pgDatabase *db, bool withDomains, bool addSerials)
{
	wxString condition = wxT("typisdefined AND typtype ");
	// We don't get pseudotypes here
	if (withDomains)
		condition += wxT("IN ('b', 'c', 'd', 'e', 'r')");
	else
		condition += wxT("IN ('b', 'c', 'e', 'r')");

	condition += wxT("AND NOT EXISTS (select 1 from pg_class where relnamespace=typnamespace and relname = typname and relkind != 'c') AND (typname not like '_%' OR NOT EXISTS (select 1 from pg_class where relnamespace=typnamespace and relname = substring(typname from 2)::name and relkind != 'c')) ");

	if (!settings->GetShowSystemObjects())
		condition += wxT(" AND nsp.nspname != 'information_schema'");
	init(db, condition, addSerials);
}

void DatatypeReader::init(pgDatabase *db, const wxString &condition, bool addSerials)
{
	database = db;
	wxString sql = wxT("SELECT * FROM (SELECT format_type(t.oid,NULL) AS typname, CASE WHEN typelem > 0 THEN typelem ELSE t.oid END as elemoid, typlen, typtype, t.oid, nspname,\n")
	               wxT("       (SELECT COUNT(1) FROM pg_type t2 WHERE t2.typname = t.typname) > 1 AS isdup\n")
	               wxT("  FROM pg_type t\n")
	               wxT("  JOIN pg_namespace nsp ON typnamespace=nsp.oid\n")
	               wxT(" WHERE (NOT (typname = 'unknown' AND nspname = 'pg_catalog')) AND ") + condition + wxT("\n");

	if (addSerials)
	{
		if (db->GetConnection()->BackendMinimumVersion(9, 2))
		{
			sql += wxT(" UNION SELECT 'smallserial', 0, 2, 'b', 0, 'pg_catalog', false\n");
		}
		sql += wxT(" UNION SELECT 'bigserial', 0, 8, 'b', 0, 'pg_catalog', false\n");
		sql += wxT(" UNION SELECT 'serial', 0, 4, 'b', 0, 'pg_catalog', false\n");
	}

	sql += wxT("  ) AS dummy ORDER BY nspname <> 'pg_catalog', nspname <> 'public', nspname, 1");

	set = db->GetConnection()->ExecuteSet(sql);
}


bool DatatypeReader::IsDomain() const
{
	return set->GetVal(wxT("typtype")) == 'd';
}


bool DatatypeReader::IsVarlen() const
{
	return set->GetLong(wxT("typlen")) == -1;
}


bool DatatypeReader::MaySpecifyLength() const
{
	if (IsDomain())
		return false;

	switch ((long)set->GetOid(wxT("elemoid")))
	{
		case PGOID_TYPE_BIT:
		case PGOID_TYPE_CHAR:
		case PGOID_TYPE_VARCHAR:
		case PGOID_TYPE_NUMERIC:
			return true;
		default:
			return false;
	}
}


bool DatatypeReader::MaySpecifyPrecision() const
{
	if (IsDomain())
		return false;

	switch ((long)set->GetOid(wxT("elemoid")))
	{
		case PGOID_TYPE_NUMERIC:
			return true;
		default:
			return false;
	}
}


pgDatatype DatatypeReader::GetDatatype() const
{
	return pgDatatype(set->GetVal(wxT("nspname")), set->GetVal(wxT("typname")), set->GetBool(wxT("isdup")));
}


wxString DatatypeReader::GetTypename() const
{
	return set->GetVal(wxT("typname"));
}


wxString DatatypeReader::GetSchema() const
{
	return set->GetVal(wxT("nspname"));
}


wxString DatatypeReader::GetSchemaPrefix() const
{
	if (set->GetBool(wxT("isdup")))
		return set->GetVal(wxT("nspname")) + wxT(".");
	else
		return database->GetSchemaPrefix(set->GetVal(wxT("nspname")));
}


wxString DatatypeReader::GetQuotedSchemaPrefix() const
{
	if (set->GetBool(wxT("isdup")))
		return qtIdent(set->GetVal(wxT("nspname"))) + wxT(".");
	else
		return database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname")));
}


wxString DatatypeReader::GetOidStr() const
{
	return set->GetVal(wxT("oid"));
}


OID DatatypeReader::GetOid() const
{
	return set->GetOid(wxT("oid"));
}
