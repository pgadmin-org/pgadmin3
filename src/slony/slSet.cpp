//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
#include "slCluster.h"
#include "slSequence.h"
#include "slTable.h"
#include "slSubscription.h"
#include "frmMain.h"


slSet::slSet(slCluster *cl, const wxString& newName)
: slObject(cl, setFactory, newName)
{
    wxLogInfo(wxT("Creating a slSet object"));
}

slSet::~slSet()
{
    wxLogInfo(wxT("Destroying a slSet object"));
}


int slSet::GetIconId()
{
    if (GetOriginId() == GetCluster()->GetLocalNodeID())
        return setFactory.GetExportedIconId();
    else
        return setFactory.GetIconId();
}


wxMenu *slSet::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();

    if (GetOriginId() == GetCluster()->GetLocalNodeID())
    {
        slSequenceFactory.AppendMenu(menu);
        slTableFactory.AppendMenu(menu);

        if (GetCluster()->ClusterMinimumVersion(1,1))
            subscriptionFactory.AppendMenu(menu);
    }
    else
    {
        if (!GetCluster()->ClusterMinimumVersion(1,1))
            subscriptionFactory.AppendMenu(menu);
    }

    return menu;
}


bool slSet::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("dropset(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slSet::GetSql(ctlTree *browser)
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


wxString slSet::GetLockXXID()
{
    return GetConnection()->ExecuteScalar(
                wxT("SELECT set_locked FROM ") 
                + GetCluster()->GetSchemaPrefix() + wxT("sl_set\n")
                wxT(" WHERE set_id=") + NumToStr(GetSlId()));
}


bool slSet::Lock()
{
    return GetConnection()->ExecuteVoid(wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
        + wxT("lockSet(") + NumToStr(GetSlId()) + wxT(");"));
}


bool slSet::Unlock()
{
    return GetConnection()->ExecuteVoid(wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
        + wxT("unlockSet(") + NumToStr(GetSlId()) + wxT(");"));
}


bool slSet::CanDrop()
{
    return !GetSubscriptionCount() && GetOriginId() == GetCluster()->GetLocalNodeID();
}


void slSet::ShowReferencedBy(frmMain *form, ctlListView *referencedBy, const wxString &wh)
{
}


void slSet::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        browser->RemoveDummyChild(this);
        // Log
        wxLogInfo(wxT("Adding child object to set ") + GetIdentifier());

        if (GetOriginId() == GetCluster()->GetLocalNodeID())
        {
            browser->AppendCollection(this, slSequenceFactory);
            browser->AppendCollection(this, slTableFactory);
        }
        browser->AppendCollection(this, subscriptionFactory);
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

        wxString lockXXID = GetLockXXID();

        if (!lockXXID.IsEmpty())
            properties->AppendItem(_("Lock XXID"), lockXXID);

        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *slSet::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *set=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        set = setFactory.CreateObjects(coll, 0, wxT(" WHERE set_id=") + NumToStr(GetSlId()) + wxT("\n"));

    return set;
}



pgObject *slSetFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
    slSet *set=0;
    slObjCollection *collection=(slObjCollection*)coll;
    wxString prefix=collection->GetCluster()->GetSchemaPrefix();

    pgSet *sets = collection->GetDatabase()->ExecuteSet(
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
            set = new slSet(collection->GetCluster(), sets->GetVal(wxT("set_comment")).BeforeFirst('\n'));
            set->iSetSlId(sets->GetLong(wxT("set_id")));
            set->iSetOriginId(sets->GetLong(wxT("set_origin")));
            set->iSetOriginNode(sets->GetVal(wxT("no_comment")));
            set->iSetSubscriptionCount(sets->GetLong(wxT("subcount")));
            set->iSetComment(sets->GetVal(wxT("set_comment")));

            if (browser)
            {
                browser->AppendObject(coll, set);
				sets->MoveNext();
            }
            else
                break;
        }

		delete sets;
    }
    return set;
}

    
//////////////////////////////////////////////////

#include "images/slset.xpm"
#include "images/slset2.xpm"
#include "images/slsets.xpm"

slSetFactory::slSetFactory() 
: slObjFactory(__("Set"), __("New Replication Set"), __("Create a new Replication Set."), slset2_xpm)
{
    exportedIconId = addIcon(slset_xpm);
    metaType = SLM_SET;
}


slSetObject::slSetObject(slSet *s, pgaFactory &factory, const wxString &newName)
: slObject(s->GetCluster(), factory, newName)
{
    set=s;
}

bool slSetObject::CanDrop()
{
    return !set->GetSubscriptionCount() && set->GetOriginId() == GetCluster()->GetLocalNodeID();
}


bool slSetObject::CanCreate()
{
    return !set->GetSubscriptionCount() && set->GetOriginId() == GetCluster()->GetLocalNodeID();
}


slSetObjCollection::slSetObjCollection(pgaFactory *factory, slSet *_set)
: slObjCollection(factory, _set->GetCluster())
{
    set = _set;
    subscription = 0;
    iSetSlId(set->GetSlId());
}


bool slSetObjCollection::CanCreate()
{
    if (set->GetSubscriptionCount())
        return false;

    switch (GetMetaType())
    {
        case SLM_SUBSCRIPTION:
            return set->GetOriginId() != GetCluster()->GetLocalNodeID();

        case SLM_TABLE:
        case SLM_SEQUENCE:
            return set->GetOriginId() == GetCluster()->GetLocalNodeID();
        default:
            return false;
    }
}


pgCollection *slSetObjFactory::CreateCollection(pgObject *obj)
{
    return new slSetObjCollection(GetCollectionFactory(), (slSet*)obj);
}

slSetFactory setFactory;
static pgaCollectionFactory cf(&setFactory, __("Replication Sets"), slsets_xpm);

