//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

	name = typname;

    if (name.StartsWith(wxT("_")))
    {
        if (!numdims)
           numdims=1;
        name=name.Mid(1);
    }
	if (name.Right(2) == wxT("[]"))
	{
		if (!numdims)
			numdims=1;
		name=name.Left(typname.Len()-2);
	}

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
            len=(typmod-4L) >> 16L;
            prec=(typmod-4) & 0xffff;
            length += NumToStr(len);
            if (prec)
                length += wxT(",") + NumToStr(prec);
        }
        else if (name == wxT("time") || name == wxT("timetz")
              || name == wxT("time without time zone") || name == wxT("time with time zone")
              || name == wxT("timestamp") || name == wxT("timestamptz")
              || name == wxT("timestamp without time zone") || name == wxT("timestamp with time zone")
              || name == wxT("bit"))
        {
            prec=0;
            len=typmod;
            length += NumToStr(len);
        }
        else if (name == wxT("interval"))
        {
            prec=0;
            len=(typmod & 0xffff);
            length += NumToStr(len);
        }
        else
        {
            prec=0;
            len=typmod-4L;
            length += NumToStr(len);
        }
        length += wxT(")");
    }
    else
        len=prec=0;
}

wxString pgDatatype::FullName() const 
{
    if (name == wxT("time with time zone"))
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
    wxString str=GetSchemaPrefix(db);
    if (!str.IsEmpty())
        return qtIdent(str.Left(str.Length()-1)) + wxT(".");
    return str;
}


long pgDatatype::GetTypmod(const wxString &name, const wxString &len, const wxString &prec)
{
    if (len.IsEmpty())
        return -1;
    if (name == wxT("numeric"))
    {
        return (((long)StrToLong(len) << 16) + StrToLong(prec)) +4;
    }
    else if (name == wxT("time") || name == wxT("timetz")
          || name == wxT("time without time zone") || name == wxT("time with time zone")
          || name == wxT("timestamp") || name == wxT("timestamptz")
          || name == wxT("timestamp without time zone") || name == wxT("timestamp with time zone")
          || name == wxT("interval")  || name == wxT("bit"))
    {
        return StrToLong(len);
    }
    else
    {
        return StrToLong(len)+4;
    }
}


DatatypeReader::DatatypeReader(pgDatabase *db, const wxString &condition)
{
    init(db, condition);
}


DatatypeReader::DatatypeReader(pgDatabase *db, bool withDomains)
{
    wxString condition=wxT("typisdefined AND typtype ");
    if (withDomains)
        condition += wxT("IN ('b', 'c', 'd')");
    else
        condition += wxT("IN ('b', 'c')");

    condition += wxT("AND typname NOT IN (SELECT relname FROM pg_class WHERE relnamespace = typnamespace AND relkind != 'c') ");

    if (!settings->GetShowSystemObjects())
        condition += wxT(" AND nsp.nspname NOT LIKE 'information_schema'");
    init(db, condition);
}

void DatatypeReader::init(pgDatabase *db, const wxString &condition)
{
    database=db;
    set=db->GetConnection()->ExecuteSet(
        wxT("SELECT format_type(t.oid,NULL) AS typname, CASE WHEN typelem > 0 THEN typelem ELSE t.oid END as elemoid, typlen, typtype, t.oid, nspname,\n")
        wxT("       (SELECT COUNT(1) FROM pg_type t2 WHERE t2.typname = t.typname) > 1 AS isdup\n")
        wxT("  FROM pg_type t\n")
        wxT("  JOIN pg_namespace nsp ON typnamespace=nsp.oid\n")
        wxT(" WHERE (NOT (typname = 'unknown' AND nspname = 'pg_catalog')) AND ") + condition + wxT("\n")
        wxT(" ORDER BY CASE WHEN typtype='d' THEN 0 ELSE 1 END, (t.typelem>0)::bool, 1"));
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
