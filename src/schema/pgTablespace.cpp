//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgTablespace.cpp - PostgreSQL Tablespace
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgTablespace.h"
#include "pgCollection.h"
#include "frmMain.h"


pgTablespace::pgTablespace(const wxString& newName)
: pgServerObject(PG_TABLESPACE, newName)
{
    wxLogInfo(wxT("Creating a pgTablespace object"));
}

pgTablespace::~pgTablespace()
{
    wxLogInfo(wxT("Destroying a pgTablespace object"));
}


void pgTablespace::ShowReferencedBy(frmMain *form, ctlListView *referencedBy, const wxString &where)
{
    form->StartMsg(_(" Retrieving tablespace usage"));

    referencedBy->ClearAll();
    referencedBy->AddColumn(_("Type"), 60);
    referencedBy->AddColumn(_("Database"), 80);
    referencedBy->AddColumn(_("Name"), 300);

    OID stdOid;
    wxString tsoid=GetOidStr();

    if (GetName() == wxT("default"))
    {
        stdOid=0;
        tsoid += wxT(", 0::oid");
    }
    else
        stdOid = GetOid();

    wxArrayString dblist;

    pgSet *set=GetConnection()->ExecuteSet(
        wxT("SELECT datname, datallowconn, dattablespace\n")
        wxT("  FROM pg_database db\n")
        wxT(" ORDER BY datname"));

    if (set)
    {
        while (!set->Eof())
        {
            wxString datname=set->GetVal(wxT("datname"));
            if (set->GetBool(wxT("datallowconn")))
                dblist.Add(datname);
            OID oid=set->GetOid(wxT("dattablespace"));
            if (oid == GetOid() || oid == stdOid)
                referencedBy->AppendItem(PGICON_DATABASE, _("Database"), datname);

            set->MoveNext();
        }
        delete set;
    }

    FillOwned(form->GetBrowser(), referencedBy, dblist, 
        wxT("SELECT cl.relkind, COALESCE(cin.nspname, cln.nspname) as nspname, COALESCE(ci.relname, cl.relname) as relname, cl.relname as indname\n")
        wxT("  FROM pg_class cl\n")
        wxT("  JOIN pg_namespace cln ON cl.relnamespace=cln.oid\n")
        wxT("  LEFT OUTER JOIN pg_index ind ON ind.indexrelid=cl.oid\n")
        wxT("  LEFT OUTER JOIN pg_class ci ON ind.indrelid=ci.oid\n")
        wxT("  LEFT OUTER JOIN pg_namespace cin ON ci.relnamespace=cin.oid\n")
        wxT(" WHERE cl.reltablespace IN (") + tsoid + wxT(")\n")
        wxT("UNION ALL\n")
        wxT("SELECT 'n', null, nspname, null\n")
        wxT("  FROM pg_namespace WHERE nsptablespace IN (") + tsoid + wxT(")\n")
        wxT(" ORDER BY 1,2,3"));

    form->EndMsg();
}


bool pgTablespace::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetConnection()->ExecuteVoid(wxT("DROP TABLESPACE ") + GetQuotedFullIdentifier() + wxT(";"));
}


wxString pgTablespace::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Tablespace: \"") + GetName() + wxT("\"\n\n");
        if (location.IsEmpty())
            sql += wxT("-- System Tablespace\n");
        else
            sql += wxT("\n\nCREATE TABLESPACE ") + GetQuotedIdentifier()
                +  wxT("\n  OWNER ") + qtIdent(GetOwner())
                +  wxT("\n  LOCATION ") + qtString(location)
                +  wxT(";\n");

    }
    return sql;
}


void pgTablespace::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

    }
    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for Tablespace ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Location"), GetLocation());
        properties->AppendItem(_("ACL"), GetAcl());
    }
}



pgObject *pgTablespace::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *tablespace=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_TABLESPACES)
            tablespace = ReadObjects((pgCollection*)obj, 0, wxT("\n WHERE ts.oid=") + GetOidStr());
    }
    return tablespace;
}



pgObject *pgTablespace::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgTablespace *tablespace=0;

    wxString tabname;


    pgSet *tablespaces = collection->GetServer()->ExecuteSet(
        wxT("SELECT ts.oid, spcname, spclocation, pg_get_userbyid(spcowner) as spcuser, spcacl FROM pg_tablespace ts\n")
        + restriction + wxT(" ORDER BY spcname"));

    if (tablespaces)
    {
        while (!tablespaces->Eof())
        {

            tablespace = new pgTablespace(tablespaces->GetVal(wxT("spcname")));
            tablespace->iSetServer(collection->GetServer());
            tablespace->iSetOid(tablespaces->GetOid(wxT("oid")));
            tablespace->iSetOwner(tablespaces->GetVal(wxT("spcuser")));
            tablespace->iSetLocation(tablespaces->GetVal(wxT("spclocation")));
            tablespace->iSetAcl(tablespaces->GetVal(wxT("spcacl")));


            if (browser)
            {
                collection->AppendBrowserItem(browser, tablespace);
				tablespaces->MoveNext();
            }
            else
                break;
        }

		delete tablespaces;
    }
    return tablespace;
}
