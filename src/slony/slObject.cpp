//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slObject.cpp PostgreSQL Slony-I Objects
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "slObject.h"
#include "slCluster.h"
#include "slNode.h"
#include "slPath.h"
#include "slListen.h"
#include "slSet.h"
#include "slSequence.h"
#include "slTable.h"
#include "slSubscription.h"

slCollection::slCollection(int newType, slCluster *_cluster)
: pgCollection(newType, _cluster->GetDatabase())
{
    cluster = _cluster;
}



int slCollection::GetIconId()
{
    switch (GetType())
    {
        case SL_NODES:              return SLICON_NODE;
        case SL_PATHS:              return SLICON_PATH;
        case SL_LISTENS:            return SLICON_LISTEN;
        case SL_SETS:               return SLICON_SET;
        default:    return 0;
    }
}


bool slCollection::CanCreate()
{
    switch (GetType())
    {
//        case SL_CLUSTERS:
        case SL_PATHS:
        case SL_LISTENS:
        case SL_SETS:
        case SL_NODES:
            return true;
        
        default:
            return false;
    }
}


void slCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    switch (GetType())
    {
        case SL_NODES:
            slNode::ShowStatistics(this, statistics);
            break;
    }
}


void slCollection::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), false) == 0)
    {
        switch (GetType())
        {
            case SL_NODES:
                slNode::ReadObjects(this, browser);
                break;
            case SL_SETS:
                slSet::ReadObjects(this, browser);
                break;
            default:
                return;
        }
    }

    UpdateChildCount(browser);
    if (properties)
        ShowList(browser, properties);
}



slSetCollection::slSetCollection(int newType, slSet *_set, slSubscription *sub)
: slCollection(newType, _set->GetCluster())
{
    set = _set;
    subscription = sub;
    iSetSlId(set->GetSlId());
}


int slSetCollection::GetIconId()
{
    switch (GetType())
    {
        case SL_SUBSCRIPTIONS:      return SLICON_SUBSCRIPTION;
        case SL_SEQUENCES:          return -1;
        case SL_TABLES:             return -1;
        default:    return 0;
    }
}


bool slSetCollection::CanCreate()
{
    if (set->GetSubscriptionCount())
        return false;

    switch (GetType())
    {
        case SL_SUBSCRIPTIONS:
            return set->GetOriginId() != GetCluster()->GetLocalNodeID();

        case SL_TABLES:
        case SL_SEQUENCES:
            return set->GetOriginId() == GetCluster()->GetLocalNodeID();

        case SL_NODES:
        default:
            return false;
    }
}

void slSetCollection::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), false) == 0)
    {
        switch (GetType())
        {
            case SL_SEQUENCES:
                slSequence::ReadObjects(this, browser);
                break;
            case SL_TABLES:
                slTable::ReadObjects(this, browser);
                break;
            case SL_SUBSCRIPTIONS:
                slSubscription::ReadObjects(this, browser);
                break;
            default:
                return;
        }
    }

    UpdateChildCount(browser);
    if (properties)
        ShowList(browser, properties);
}


slNodeCollection::slNodeCollection(int newType, slNode *n)
: slCollection(newType, n->GetCluster())
{
    node=n;
    iSetSlId(n->GetSlId());
}


void slNodeCollection::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), false) == 0)
    {
        switch (GetType())
        {
            case SL_PATHS:
                slPath::ReadObjects(this, browser);
                break;
            case SL_LISTENS:
                slListen::ReadObjects(this, browser);
                break;
            default:
                return;
        }
    }

    UpdateChildCount(browser);
    if (properties)
        ShowList(browser, properties);
}



////////////////////////////////////////////////////////7

slObject::slObject(slCluster *cl, int newType, const wxString &newName)
: pgDatabaseObject(newType, newName)
{
    cluster = cl;
    iSetDatabase(cl->GetDatabase());
}


slNodeObject::slNodeObject(slNode *n, int newType, const wxString &newName)
: slObject(n->GetCluster(), newType, newName)
{
    node = n;
}


slSetObject::slSetObject(slSet *s, int newType, const wxString &newName)
: slObject(s->GetCluster(), newType, newName)
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


