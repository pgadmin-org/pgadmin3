//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSubscription.cpp PostgreSQL Slony-I subscription
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "slSubscription.h"
#include "slObject.h"
#include "slCluster.h"
#include "slSet.h"
#include "frmMain.h"


slSubscription::slSubscription(slSet *s, const wxString& newName)
: slSetObject(s, SL_SUBSCRIPTION, newName)
{
    wxLogInfo(wxT("Creating a slSubscription object"));
}

slSubscription::~slSubscription()
{
    wxLogInfo(wxT("Destroying a slSubscription object"));
}


bool slSubscription::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(
        wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("unsubscribeset(") + NumToStr(GetSet()->GetSlId())
            + wxT(", ") + NumToStr(GetReceiverId())
            + wxT(");"));

}


bool slSubscription::CanCreate()
{
    return GetSet()->GetOriginId() != GetReceiverId() && slSetObject::CanCreate();
}


wxString slSubscription::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        if (GetReceiverId() != GetCluster()->GetLocalNodeID())
            sql = wxT("-- Subscription must be maintained on receiver node.\n");
        else
            sql = wxT("-- subscribe replication set\n\n")
                  wxT(" SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("subscribeset(")
                + NumToStr(GetSet()->GetSlId()) + wxT(", ")
                + NumToStr(GetProviderId()) + wxT(", ")
                + NumToStr(GetReceiverId()) + wxT(", ")
                + BoolToStr(GetForward()) + wxT(");");
    }
    return sql;
}


bool slSubscription::WantDummyChild()
{
    return GetSet()->GetOriginId() != GetCluster()->GetLocalNodeID();
}


void slSubscription::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log

        if (WantDummyChild())
        {
            wxTreeItemId id=browser->GetItemParent(browser->GetItemParent(GetId()));
            if (id)
            {
                slSet *set=(slSet*)browser->GetItemData(id);
                if (set && set->GetType() == SL_SET)
                {
                    wxLogInfo(wxT("Adding child object to subscription ") + GetIdentifier());

                    slSetCollection *collection;
                    collection = new slSetCollection(SL_SEQUENCES, set, this);
                    AppendBrowserItem(browser, collection);

                    collection = new slSetCollection(SL_TABLES, set, this);
                    AppendBrowserItem(browser, collection);
                }
            }
        }
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for subscription ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Provider ID"), GetProviderId());
        properties->AppendItem(_("Provider Name"), GetProviderNode());
        properties->AppendItem(_("Receiver ID"), GetReceiverId());
        properties->AppendItem(_("Receiver Name"), GetReceiverNode());

        properties->AppendItem(_("Active"), GetActive());
        properties->AppendItem(_("May forward"), GetForward());
        if (GetForward())
            properties->AppendItem(_("Is forwarded"), GetIsSubscribed());
    }
}



pgObject *slSubscription::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *subscription=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        slSetCollection *coll=(slSetCollection*)browser->GetItemData(parentItem);
        if (coll->GetType() == SL_SUBSCRIPTIONS)
            subscription = ReadObjects(coll, 0, wxT(" WHERE sub_set=") + NumToStr(GetSet()->GetSlId()) 
                            + wxT(" AND sub_receiver = ") + NumToStr(GetReceiverId()) + wxT("\n"));
    }
    return subscription;
}



pgObject *slSubscription::ReadObjects(slSetCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slSubscription *subscription=0;

    wxString prefix=coll->GetCluster()->GetSchemaPrefix();
    pgSet *subscriptions = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT sub_set, sub_provider, sub_receiver, sub_forward, sub_active,\n")
              wxT(" re.no_comment as receiver_name, pr.no_comment as provider_name,\n")
              wxT(" EXISTS (SELECT 1 FROM ") + prefix + wxT("sl_subscribe s2 WHERE s2.sub_provider = s1.sub_receiver AND s1.sub_set=s2.sub_set) AS is_subscribed\n")
        wxT("  FROM ") + prefix + wxT("sl_subscribe s1\n")
        wxT("  JOIN ") + prefix + wxT("sl_set ON set_id = sub_set\n")
        wxT("  JOIN ") + prefix + wxT("sl_node pr ON pr.no_id = sub_provider\n")
        wxT("  JOIN ") + prefix + wxT("sl_node re ON re.no_id = sub_receiver\n")
         + restriction +
        wxT(" ORDER BY sub_provider, sub_receiver"));

    if (subscriptions)
    {
        while (!subscriptions->Eof())
        {
            subscription = new slSubscription(coll->GetSet(), subscriptions->GetVal(wxT("receiver_name")));
            subscription->iSetActive(subscriptions->GetBool(wxT("sub_active")));
            subscription->iSetForward(subscriptions->GetBool(wxT("sub_forward")));
            subscription->iSetReceiverId(subscriptions->GetLong(wxT("sub_receiver")));
            subscription->iSetProviderId(subscriptions->GetLong(wxT("sub_provider")));
            subscription->iSetReceiverNode(subscriptions->GetVal(wxT("receiver_name")));
            subscription->iSetProviderNode(subscriptions->GetVal(wxT("provider_name")));
            subscription->iSetIsSubscribed(subscriptions->GetBool(wxT("is_subscribed")));

            if (browser)
            {
                coll->AppendBrowserItem(browser, subscription);
				subscriptions->MoveNext();
            }
            else
                break;
        }

		delete subscriptions;
    }
    return subscription;
}


    
pgObject *slSubscription::ReadObjects(slSetCollection *coll, wxTreeCtrl *browser)
{
    // Get the subscriptions
    wxString restriction = wxT(" WHERE sub_set = ") + NumToStr(coll->GetSet()->GetSlId());
    return ReadObjects(coll, browser, restriction);
}

