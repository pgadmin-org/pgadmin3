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


pgTablespace::pgTablespace(const wxString& newName)
: pgServerObject(PG_TABLESPACE, newName)
{
    wxLogInfo(wxT("Creating a pgTablespace object"));
}

pgTablespace::~pgTablespace()
{
    wxLogInfo(wxT("Destroying a pgTablespace object"));
}


void pgTablespace::ShowReferencedBy(ctlListView *referencedBy, const wxString &where)
{
#if 0
    // unfortunately, only the current database is examined...

    referencedBy->ClearAll();
    referencedBy->AddColumn(_("Type"), 60);
    referencedBy->AddColumn(_("Name"), 100);

    wxString tsoid=GetOidStr();

    if (GetName() == wxT("default"))
        tsoid += wxT(", 0::oid");

    pgSet *set=GetConnection()->ExecuteSet(
        wxT("SELECT nspname, relname, relkind, NULL as dbname\n")
        wxT("  FROM pg_class cl\n")
        wxT("  JOIN pg_namespace nsp ON cl.relnamespace=nsp.oid\n")
        wxT(" WHERE cl.reltablespace IN (") + tsoid + wxT(")\n")
        wxT(" ORDER BY relkind, relname"));
    if (set)
    {
        while (!set->Eof())
        {
            int id=0;

            wxString refname = qtIdent(set->GetVal(wxT("nspname")));
            if (!refname.IsEmpty())
                refname += wxT(".");
            refname += qtIdent(set->GetVal(wxT("relname")));

            switch (set->GetVal(wxT("relkind")).c_str()[0])
            {
                case 'r':   id=PG_TABLE;        break;
                case 'i':   id=PG_INDEX;        break;
                case 's':   id=PG_SEQUENCE;     break;
                case 'd':   id=PG_DATABASE;     break;
            }


            if (id)
            {
                wxString typname = typesList[id].typName;
                int icon = typesList[id].typeIcon;
                referencedBy->AppendItem(icon, typname, refname);
            }
            set->MoveNext();
        }

        delete set;
    }
#endif
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
            sql + wxT("\n\nCREATE TABLESPACE ") + GetQuotedIdentifier()
                + wxT("\n  OWNER ") + qtIdent(GetOwner())
                + wxT("\n  LOCATION ")+ qtString(location)
                + wxT(";\n");

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
        wxT("SELECT ts.oid, spcname, spclocation, usename FROM pg_tablespace ts\n")
        wxT("  JOIN pg_user us ON us.usesysid=ts.spcowner\n")
        + restriction + wxT(" ORDER BY spcname"));

    if (tablespaces)
    {
        while (!tablespaces->Eof())
        {

            tablespace = new pgTablespace(tablespaces->GetVal(wxT("spcname")));
            tablespace->iSetServer(collection->GetServer());
            tablespace->iSetOid(tablespaces->GetOid(wxT("oid")));
            tablespace->iSetOwner(tablespaces->GetVal(wxT("usename")));
            tablespace->iSetLocation(tablespaces->GetVal(wxT("spclocation")));


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
