//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "pgDatatype.h"


pgDomain::pgDomain(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_DOMAIN, newName)
{
}

pgDomain::~pgDomain()
{
}

bool pgDomain::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP DOMAIN ") + GetQuotedFullIdentifier() + wxT(";"));
}

wxString pgDomain::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Domain: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP DOMAIN ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE DOMAIN ") + GetQuotedFullIdentifier() 
            + wxT("\n  AS ") + GetQuotedBasetype();
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

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Owner"), GetOwner());
        InsertListItem(properties, pos++, _("Base type"), GetBasetype());
        if (GetDimensions())
            InsertListItem(properties, pos++, _("Dimensions"), GetDimensions());
        InsertListItem(properties, pos++, _("Default"), GetDefault());
        InsertListItem(properties, pos++, _("Not NULL?"), GetNotNull());
        InsertListItem(properties, pos++, _("System domain?"), GetSystemObject());
        InsertListItem(properties, pos++, _("Comment"), GetComment());
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
    pgSet *domains= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT d.oid, d.typname as domname, d.typbasetype, b.typname as basetype, pg_get_userbyid(d.typowner) as domainowner, \n")
        wxT("       d.typlen, d.typtypmod, d.typnotnull, d.typdefault, d.typndims, d.typdelim,\n")
        wxT("       description\n")
        wxT("  FROM pg_type d\n")
        wxT("  JOIN pg_type b ON b.oid = CASE WHEN d.typndims>0 then d.typelem ELSE d.typbasetype END\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=d.oid\n")
        wxT(" WHERE d.typtype = 'd' AND d.typnamespace = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n")
        + restriction +
        wxT(" ORDER BY d.typname"));

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
            long typmod=domains->GetLong(wxT("typtypmod"));

            pgDatatype dt(domains->GetVal(wxT("basetype")), domains->GetLong(wxT("typndims")), typmod);

            domain->iSetTyplen(domains->GetLong(wxT("typlen")));
            domain->iSetTypmod(typmod);
            domain->iSetLength(dt.Length());
            domain->iSetPrecision(dt.Precision());
            domain->iSetBasetype(dt.FullName());
            domain->iSetQuotedBasetype(dt.QuotedFullName());
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
