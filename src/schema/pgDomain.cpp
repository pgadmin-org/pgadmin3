//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgDomain.cpp - Domain class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgDomain.h"
#include "pgCollection.h"


pgDomain::pgDomain(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_DOMAIN, newName)
{
}

pgDomain::~pgDomain()
{
}

bool pgDomain::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP DOMAIN ") + GetQuotedFullIdentifier());
}

wxString pgDomain::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Domain: ") + GetQuotedFullIdentifier() + wxT("\n")
            + wxT("CREATE DOMAIN ") + GetQuotedFullIdentifier() 
            + wxT("\n  AS ") + GetBasetype();
        if (typlen == -1 && typmod > 0)
        {
            sql += wxT("(") + NumToStr(length);
            if (precision >= 0)
                sql += wxT(", ") + NumToStr(precision);
            sql += wxT(")");
        }
        if (dimensions)
        {
            sql += wxT("[");
            for (int i=0 ; i < dimensions ; i++)
            {
                if (i)
                    sql += GetDelimiter() + wxT(" ");
            }
            sql += wxT("]");
        }
        AppendIfFilled(sql, wxT("\n  DEFAULT "), GetDefault());
        // CONSTRAINT Name Dont know where it's stored, may be omitted anyway
        if (notNull)
            sql += wxT("\n  NOT NULL");


        sql += wxT(";\n")
            + GetCommentSql();
    }

    return sql;
}



void pgDomain::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids = true;
        // append type here
    }
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("Base Type"), GetBasetype());
        if (GetLength() == -2)
            InsertListItem(properties, pos++, wxT("Length"), wxT("null-terminated"));
        else
        {
            if (GetDimensions() || GetLength() < 0)
                InsertListItem(properties, pos++, wxT("Length"), wxT("variable"));
            else
                InsertListItem(properties, pos++, wxT("Length"), GetLength());
        }
        if (GetPrecision() >= 0)
            InsertListItem(properties, pos++, wxT("Precision"), GetPrecision());
        // dimensions, delimiter missing
        if (GetDimensions())
            InsertListItem(properties, pos++, wxT("Dimensions"), GetDimensions());
        InsertListItem(properties, pos++, wxT("Default"), GetDefault());
        InsertListItem(properties, pos++, wxT("Not Null?"), GetNotNull());
        InsertListItem(properties, pos++, wxT("System Domain?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



pgObject *pgDomain::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *domain=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_DOMAINS)
            domain = ReadObjects((pgCollection*)obj, 0, wxT("   AND d.oid=") + GetOidStr() + wxT("\n"));
    }
    return domain;
}



pgObject *pgDomain::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgDomain *domain=0;
    pgSet *domains= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT d.oid, d.typname as domname, d.typbasetype, b.typname as basetype, pg_get_userbyid(d.typowner) as domainowner, \n"
        "       d.typlen, d.typtypmod, d.typnotnull, d.typdefault, d.typndims, d.typdelim,\n"
        "       CASE WHEN d.oid=1700 OR d.typbasetype=1700 THEN 1 ELSE 0 END AS isnumeric, description\n"
        "  FROM pg_type d\n"
        "  JOIN pg_type b ON b.oid = CASE WHEN d.typndims>0 then d.typelem ELSE d.typbasetype END\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=d.oid\n"
        " WHERE d.typtype = 'd' AND d.typnamespace = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n"
        + restriction +
        " ORDER BY d.typname"));

    if (domains)
    {
        while (!domains->Eof())
        {
            domain = new pgDomain(collection->GetSchema(), domains->GetVal(wxT("domname")));

            domain->iSetOid(domains->GetOid(wxT("oid")));
            domain->iSetOwner(domains->GetVal(wxT("domainowner")));
            domain->iSetBasetype(domains->GetVal(wxT("basetype")));
            domain->iSetBasetypeOid(domains->GetBool(wxT("typbasetype")));
            domain->iSetComment(domains->GetVal(wxT("description")));
            long typlen=domains->GetLong(wxT("typlen"));
            long typmod=domains->GetLong(wxT("typtypmod"));
            bool isnum=domains->GetBool(wxT("isnumeric"));
            long length=typlen, precision=-1;
            if (typlen == -1 && typmod > 0)
            {
                    if (isnum)
                    {
                        length=(typmod-4) >> 16;
                        precision=(typmod-4) & 0xffff;
                    }
                    else
                        length = typmod-4;
            }
            domain->iSetTyplen(typlen);
            domain->iSetTypmod(typmod);
            domain->iSetLength(length);
            domain->iSetPrecision(precision);
            domain->iSetDefault(domains->GetVal(wxT("typdefault")));
            domain->iSetNotNull(domains->GetBool(wxT("typnotnull")));
            domain->iSetDimensions(domains->GetLong(wxT("typndims")));
            domain->iSetDelimiter(domains->GetVal(wxT("typdelim")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, domain);
    			domains->MoveNext();
            }
            else
                break;
        }

		delete domains;
    }
    return domain;
}
