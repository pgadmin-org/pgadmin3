//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgDatatype.cpp - PostgreSQL Datatypes
//
//////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDatatype.h"
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


DatatypeReader::DatatypeReader(pgConn *conn, bool withDomains)
{
    wxString condition;
    if (withDomains)
        condition=wxT("IN ('b', 'd')");
    else
        condition=wxT("= 'b'");

    set=conn->ExecuteSet(wxT(
        "SELECT t.typname, CASE WHEN typelem > 0 THEN typelem ELSE t.oid END as elemoid, t.typlen, t.typtype, t.oid, nspname\n"
        "  FROM pg_type t\n"
        "  JOIN pg_namespace nsp ON t.typnamespace=nsp.oid\n"
        " WHERE t.typisdefined AND t.typtype ") + condition + wxT("\n"
        " ORDER BY t.typtype DESC, (t.typelem>0)::bool, t.typname"));
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
    wxString schema=set->GetVal(wxT("nspname"));
    if (schema == wxT("pg_catalog"))
        schema=wxT("");
    else
        schema += wxT(".");
    return schema;
}


wxString DatatypeReader::GetQuotedSchemaPrefix() const
{
    wxString schema=set->GetVal(wxT("nspname"));
    if (schema == wxT("pg_catalog"))
        schema=wxT("");
    else
        schema = qtIdent(schema) + wxT(".");
    return schema;
}


wxString DatatypeReader::GetOidStr() const
{
    return set->GetVal(wxT("oid"));
}
