//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#include "pgDomain.h"
#include "pgDatatype.h"


pgDomain::pgDomain(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, domainFactory, newName)
{
}

pgDomain::~pgDomain()
{
}

bool pgDomain::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql=wxT("DROP DOMAIN ") + GetQuotedFullIdentifier();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}

wxString pgDomain::GetSql(ctlTree *browser)
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
        AppendIfFilled(sql, wxT("\n   "), GetCheck());

        sql += wxT(";\n")
            + GetOwnerSql(7, 4)
            + GetCommentSql();
    }

    return sql;
}



void pgDomain::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids = true;
        if (GetConnection()->BackendMinimumVersion(7, 4))
        {
            pgSet *set=ExecuteSet(
                wxT("SELECT conname, pg_get_constraintdef(oid) AS consrc FROM pg_constraint WHERE contypid=") + GetOidStr());
            if (set)
            {
                while (!set->Eof())
                {
                    if (!check.IsEmpty())
                    {
                        check += wxT(" ");
                    }
                    wxString conname=set->GetVal(wxT("conname"));
                    if (!conname.StartsWith(wxT("$")))
                        check += wxT("CONSTRAINT ") + qtIdent(conname) + wxT(" ");
                    check += set->GetVal(wxT("consrc"));

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
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Base type"), GetBasetype());
        if (GetDimensions())
            properties->AppendItem(_("Dimensions"), GetDimensions());
        properties->AppendItem(_("Default"), GetDefault());
        properties->AppendItem(_("Check"), GetCheck());
        properties->AppendItem(_("Not NULL?"), GetNotNull());
        properties->AppendItem(_("System domain?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgDomain::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *domain=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->IsCollection())
            domain = domainFactory.CreateObjects((pgCollection*)obj, 0, wxT("   AND d.oid=") + GetOidStr() + wxT("\n"));
    }
    return domain;
}


////////////////////////////////////////////////////



pgObject *pgDomainFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgDomain *domain=0;

    pgDatabase *db=collection->GetDatabase();

    pgSet *domains= db->ExecuteSet(
        wxT("SELECT d.oid, d.typname as domname, d.typbasetype, b.typname as basetype, pg_get_userbyid(d.typowner) as domainowner, \n")
        wxT("       d.typlen, d.typtypmod, d.typnotnull, d.typdefault, d.typndims, d.typdelim, bn.nspname as basensp,\n")
        wxT("       description, (SELECT COUNT(1) FROM pg_type t2 WHERE t2.typname=d.typname) > 1 AS domisdup,\n")
        wxT("       (SELECT COUNT(1) FROM pg_type t3 WHERE t3.typname=b.typname) > 1 AS baseisdup\n")
        wxT("  FROM pg_type d\n")
        wxT("  JOIN pg_type b ON b.oid = CASE WHEN d.typndims>0 then d.typelem ELSE d.typbasetype END\n")
        wxT("  JOIN pg_namespace bn ON bn.oid=b.typnamespace\n")
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
            domain->iSetBasetypeOid(domains->GetOid(wxT("typbasetype")));
            domain->iSetComment(domains->GetVal(wxT("description")));
            long typmod=domains->GetLong(wxT("typtypmod"));

            pgDatatype dt(domains->GetVal(wxT("basensp")), domains->GetVal(wxT("basetype")), 
                domains->GetBool(wxT("baseisdup")), domains->GetLong(wxT("typndims")), typmod);

            domain->iSetTyplen(domains->GetLong(wxT("typlen")));
            domain->iSetTypmod(typmod);
            domain->iSetLength(dt.Length());
            domain->iSetPrecision(dt.Precision());
            domain->iSetBasetype(dt.GetSchemaPrefix(db) + dt.FullName());
            domain->iSetQuotedBasetype(dt.GetQuotedSchemaPrefix(db) + dt.QuotedFullName());
            domain->iSetDefault(domains->GetVal(wxT("typdefault")));
            domain->iSetNotNull(domains->GetBool(wxT("typnotnull")));
            domain->iSetDimensions(domains->GetLong(wxT("typndims")));
            domain->iSetDelimiter(domains->GetVal(wxT("typdelim")));
            domain->iSetIsDup(domains->GetBool(wxT("domisdup")));

            if (browser)
            {
                browser->AppendObject(collection, domain);
    			domains->MoveNext();
            }
            else
                break;
        }

		delete domains;
    }
    return domain;
}

#include "images/domain.xpm"
#include "images/domain-sm.xpm"
#include "images/domains.xpm"

pgDomainFactory::pgDomainFactory() 
: pgSchemaObjFactory(__("Domain"), __("New Domain"), __("Create a new Domain."), domain_xpm, domain_sm_xpm)
{
}


pgDomainFactory domainFactory;
static pgaCollectionFactory cf(&domainFactory, __("Domains"), domains_xpm);
