//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgDatatype.cpp - PostgreSQL Datatypes
//
//////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDatatype.h"
#include "pgDatabase.h"
#include "pgDefs.h"


pgDatatype::pgDatatype(const wxString &typname, long numdims, long typmod)
{
    if (typname.StartsWith(wxT("_")))
    {
        if (!numdims)
           numdims=1;
        name=typname.Mid(1);
    }
    else
        name=typname;

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
              || name == wxT("timestamp") || name == wxT("timestamptz")
              || name == wxT("interval")  || name == wxT("bit"))
        {
            prec=0;
            len=typmod;
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


long pgDatatype::GetTypmod(const wxString &name, const wxString &len, const wxString &prec)
{
    if (len.IsEmpty())
        return -1;
    if (name == wxT("numeric"))
    {
        return (((long)StrToLong(len) << 16) + StrToLong(prec)) +4;
    }
    else if (name == wxT("time") || name == wxT("timetz")
          || name == wxT("timestamp") || name == wxT("timestamptz")
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
        condition += wxT("IN ('b', 'd')");
    else
        condition += wxT("= 'b'");
    init(db, condition);
}

void DatatypeReader::init(pgDatabase *db, const wxString &condition)
{
    database=db;
    set=db->GetConnection()->ExecuteSet(
        wxT("SELECT typname, CASE WHEN typelem > 0 THEN typelem ELSE t.oid END as elemoid, typlen, typtype, t.oid, nspname\n")
        wxT("  FROM pg_type t\n")
        wxT("  JOIN pg_namespace nsp ON typnamespace=nsp.oid\n")
        wxT(" WHERE ") + condition + wxT("\n")
        wxT(" ORDER BY CASE WHEN typtype='d' THEN 0 ELSE 1 END, (t.typelem>0)::bool, t.typname"));
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
    return pgDatatype(set->GetVal(wxT("typname")));
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
    return database->GetSchemaPrefix(set->GetVal(wxT("nspname")));
}


wxString DatatypeReader::GetQuotedSchemaPrefix() const
{
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
