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
    referencedBy->ClearAll();
    referencedBy->AddColumn(_("Type"), 60);
    referencedBy->AddColumn(_("Database"), 80);
    referencedBy->AddColumn(_("Name"), 300);

    OID stdOid;
    wxString tsoid=GetOidStr();

    form->StartMsg(_(" Retrieving tablespace usage"));
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

    pgCollection *databases;

    wxCookieType cookie;
    wxTreeItemId item=form->GetBrowser()->GetFirstChild(GetServer()->GetId(), cookie);
    while (item)
    {
        databases = (pgCollection*)form->GetBrowser()->GetItemData(item);
        if (databases && databases->GetType() == PG_DATABASES)
            break;
        else
            databases=0;

        item=form->GetBrowser()->GetNextChild(GetServer()->GetId(), cookie);
    }
    
    size_t i;
    for (i=0 ; i < dblist.GetCount() ; i++)
    {
        wxString dbname=dblist.Item(i);
        pgConn *conn=0;
        pgConn *tmpConn=0;

        if (GetServer()->GetDatabaseName() == dbname)
            conn = GetServer()->GetConnection();
        else
        {
            item=form->GetBrowser()->GetFirstChild(databases->GetId(), cookie);
            while (item)
            {
                pgDatabase *db=(pgDatabase*)form->GetBrowser()->GetItemData(item);
                if (db->GetType() == PG_DATABASE && db->GetName() == dbname)
                {
                    if (db->GetConnected())
                        conn = db->GetConnection();
                    break;
                }
                item=form->GetBrowser()->GetNextChild(databases->GetId(), cookie);
            }
        }
        if (conn && conn->GetStatus() != PGCONN_OK)
            conn=0;

        if (!conn)
        {
		    tmpConn = new pgConn(GetServer()->GetName(), dbname, GetServer()->GetUsername(), 
                              GetServer()->GetPassword(), GetServer()->GetPort(), GetServer()->GetSSL());
            if (tmpConn->GetStatus() == PGCONN_OK)
                conn=tmpConn;
        }

        if (conn)
        {
            set=conn->ExecuteSet(
                wxT("SELECT cl.relkind, COALESCE(cin.nspname, cln.nspname) as nspname, COALESCE(ci.relname, cl.relname) as relname, ci.relname as indname\n")
                wxT("  FROM pg_class cl\n")
                wxT("  JOIN pg_namespace cln ON cl.relnamespace=cln.oid\n")
                wxT("  LEFT OUTER JOIN pg_index ind ON ind.indexrelid=cl.oid\n")
                wxT("  LEFT OUTER JOIN pg_class ci ON ind.indrelid=ci.oid\n")
                wxT("  JOIN pg_namespace cin ON ci.relnamespace=cin.oid\n")
                wxT(" WHERE cl.reltablespace IN (") + tsoid + wxT(")\n")
                wxT("UNION ALL\n")
                wxT("SELECT 'n', null, nspname, null\n")
                wxT("  FROM pg_namespace WHERE nsptablespace IN (") + tsoid + wxT(")\n")
                wxT(" ORDER BY 1,2,3"));
            
            if (set)
            {
                while (!set->Eof())
                {
                    int id=0;

                    wxString relname = qtIdent(set->GetVal(wxT("nspname")));
                    if (!relname.IsEmpty())
                        relname += wxT(".");
                    relname += qtIdent(set->GetVal(wxT("relname")));

                    switch (set->GetVal(wxT("relkind")).c_str()[0])
                    {
                        case 'r':   id=PG_TABLE;        break;
                        case 's':   id=PG_SEQUENCE;     break;
                        case 'n':   id=PG_SCHEMA;       break;
                        case 'i':   id=PG_INDEX;        
                                    relname = qtIdent(set->GetVal(wxT("indname"))) + wxT(" ON ") + relname;
                                    break;
                    }

                    if (id)
                    {
                        wxString typname = typesList[id].typName;
                        int icon = typesList[id].typeIcon;
                        referencedBy->AppendItem(icon, typname, dbname, relname);
                    }

                    set->MoveNext();
                }
                delete set;
            }
        }

        if (tmpConn)
            delete tmpConn;
    }
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
