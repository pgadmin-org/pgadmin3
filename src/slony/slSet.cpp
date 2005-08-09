//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSet.cpp PostgreSQL Slony-I Set
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "slSet.h"
#include "slObject.h"
#include "slCluster.h"
#include "frmMain.h"


slSet::slSet(slCluster *cl, const wxString& newName)
: slObject(cl, SL_SET, newName)
{
    wxLogInfo(wxT("Creating a slSet object"));
}

slSet::~slSet()
{
    wxLogInfo(wxT("Destroying a slSet object"));
}


int slSet::GetIcon()
{
    if (GetOriginId() == GetCluster()->GetLocalNodeID())
        return SLICON_SET;
    else
        return SLICON_SET2;
}


wxMenu *slSet::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();

    if (GetOriginId() == GetCluster()->GetLocalNodeID())
    {
        AppendMenu(menu, SL_SEQUENCE);
        AppendMenu(menu, SL_TABLE);
    }
    else
    {
        AppendMenu(menu, SL_SUBSCRIPTION);
    }

    return menu;
}


bool slSet::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("dropset(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slSet::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Create replication set ") + GetName() + wxT(".\n\n")
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("storeset(") 
                    + NumToStr(GetSlId()) + wxT(", ")
                    + qtString(GetComment()) + wxT(");\n");
    }
    return sql;
}

void slSet::ShowDependsOn(frmMain *form, ctlListView *list, const wxString &wh)
{
    list->ClearAll();
    list->AddColumn(_("Type"), 60);
    list->AddColumn(_("Name"), 100);
    list->AddColumn(_("Comment"), 200);

    wxString where;
    if (wh.IsEmpty())
        where = wxT(" WHERE set_id=") + NumToStr(GetSlId());
    else
        where = wh;

    pgSet *set=GetDatabase()->ExecuteSet(
        wxT("SELECT relkind, nspname, relname, id, comment\n")
        wxT("  FROM pg_class cl\n")
        wxT("  JOIN pg_namespace nsp ON nsp.oid=cl.relnamespace\n")
        wxT("  JOIN (\n")
        wxT("       SELECT tab_id AS id, tab_reloid AS oid, tab_altered AS altered, tab_comment AS comment\n")
        wxT("         FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_table t\n")
        wxT("         JOIN ") + GetCluster()->GetSchemaPrefix() + wxT("sl_set s ON tab_set=set_id ") + where +wxT("\n")
        wxT("       UNION\n")
        wxT("       SELECT seq_id, seq_reloid, NULL, seq_comment\n")
        wxT("         FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_sequence t\n")
        wxT("         JOIN ") + GetCluster()->GetSchemaPrefix() + wxT("sl_set s ON seq_set=set_id ") + where + wxT("\n")
        wxT("       ) AS set ON set.oid=cl.oid"));

    if (set)
    {
        while (!set->Eof())
        {
            wxString name = set->GetVal(wxT("nspname")) + wxT(".") + set->GetVal(wxT("relname"));
            wxString typestr = set->GetVal(wxT("relkind"));
            wxString comment = set->GetVal(wxT("comment"));
            wxString typname;
            int icon=-1;
            switch (typestr.c_str()[0])
            {
                case 'S':   typname = _("Sequence");   icon = -1; break;
                case 'r':   typname = _("Table");      icon = -1;    break;
            }

            list->AppendItem(icon, typname, name, comment);

            set->MoveNext();
        }
        delete set;
    }
}


bool slSet::CanDrop()
{
    return !GetSubscriptionCount() && GetOriginId() == GetCluster()->GetLocalNodeID();
}


void slSet::ShowReferencedBy(frmMain *form, ctlListView *referencedBy, const wxString &wh)
{
}


void slSet::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child object to set ") + GetIdentifier());

        slCollection *collection;

        if (GetOriginId() == GetCluster()->GetLocalNodeID())
        {
            collection = new slSetCollection(SL_SEQUENCES, this);
            AppendBrowserItem(browser, collection);

            collection = new slSetCollection(SL_TABLES, this);
            AppendBrowserItem(browser, collection);
        }

        collection = new slSetCollection(SL_SUBSCRIPTIONS, this);
        AppendBrowserItem(browser, collection);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for set ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("ID"), GetSlId());
        properties->AppendItem(_("Origin ID"), GetOriginId());
        properties->AppendItem(_("Origin Node"), GetOriginNode());
        properties->AppendItem(_("Subscriptions"), GetSubscriptionCount());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *slSet::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *set=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        slCollection *coll=(slCollection*)browser->GetItemData(parentItem);
        if (coll->GetType() == SL_SETS)
            set = ReadObjects(coll, 0, wxT(" WHERE set_id=") + NumToStr(GetSlId()) + wxT("\n"));
    }
    return set;
}



pgObject *slSet::ReadObjects(slCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slSet *set=0;
    wxString prefix=coll->GetCluster()->GetSchemaPrefix();

    pgSet *sets = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT set_id, set_origin, no_comment, set_comment,\n")
        wxT("       (SELECT COUNT(1) FROM ") + prefix+ wxT("sl_subscribe where sub_set=set_id) AS subcount\n")
        wxT("  FROM ") + prefix + wxT("sl_set\n")
        wxT("  JOIN ") + prefix + wxT("sl_node ON set_origin=no_id\n")
         + restriction +
        wxT(" ORDER BY set_id"));

    if (sets)
    {
        while (!sets->Eof())
        {
            set = new slSet(coll->GetCluster(), sets->GetVal(wxT("set_comment")).BeforeFirst('\n'));
            set->iSetSlId(sets->GetLong(wxT("set_id")));
            set->iSetOriginId(sets->GetLong(wxT("set_origin")));
            set->iSetOriginNode(sets->GetVal(wxT("no_comment")));
            set->iSetSubscriptionCount(sets->GetLong(wxT("subcount")));
            set->iSetComment(sets->GetVal(wxT("set_comment")));

            if (browser)
            {
                coll->AppendBrowserItem(browser, set);
				sets->MoveNext();
            }
            else
                break;
        }

		delete sets;
    }
    return set;
}


    
pgObject *slSet::ReadObjects(slCollection *coll, wxTreeCtrl *browser)
{
    // Get the sets
    return ReadObjects(coll, browser, wxEmptyString);
}




