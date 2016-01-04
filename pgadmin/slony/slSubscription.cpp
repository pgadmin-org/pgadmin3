//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slSubscription.cpp PostgreSQL Slony-I subscription
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "slony/slSubscription.h"
#include "slony/slTable.h"
#include "slony/slSequence.h"
#include "frm/frmMain.h"



slSubscription::slSubscription(slSet *s, const wxString &newName)
	: slSetObject(s, subscriptionFactory, newName)
{
}

int slSubscription::GetIconId()
{
	if (GetReceiverId() == GetCluster()->GetLocalNodeID())
		return subscriptionFactory.GetIconId();
	else
		return subscriptionFactory.GetExportedIconId();
}


bool slSubscription::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetDatabase()->ExecuteVoid(
	           wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	           + wxT("unsubscribeset(") + NumToStr(GetSet()->GetSlId())
	           + wxT(", ") + NumToStr(GetReceiverId())
	           + wxT(");"));

}


wxString slSubscription::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony subscription");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony subscription");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony subscription \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony subscription \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop Slony subscription cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop Slony subscription?");
			break;
		case PROPERTIESREPORT:
			message = _("Slony subscription properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Slony subscription properties");
			break;
		case DDLREPORT:
			message = _("Slony subscription DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Slony subscription DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Slony subscription dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Slony subscription dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Slony subscription dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Slony subscription dependents");
			break;
	}

	return message;
}

bool slSubscription::CanCreate()
{
	return GetSet()->GetOriginId() != GetReceiverId() && slSetObject::CanCreate();
}


bool slSubscription::CanDrop()
{
	return GetReceiverId() == GetCluster()->GetLocalNodeID();
}


wxString slSubscription::GetSql(ctlTree *browser)
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


void slSubscription::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);
		// Log

		if (WantDummyChild())
		{
			wxTreeItemId id = browser->GetItemParent(browser->GetItemParent(GetId()));
			if (id)
			{
				slSet *set = (slSet *)browser->GetObject(id);
				if (set && set->IsCreatedBy(setFactory))
				{
					wxLogInfo(wxT("Adding child object to subscription %s"), GetIdentifier().c_str());

					browser->AppendCollection(this, slSequenceFactory);
					browser->AppendCollection(this, slTableFactory);
				}
			}
		}
	}


	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Provider ID"), GetProviderId());
		properties->AppendItem(_("Provider Name"), GetProviderNode());
		properties->AppendItem(_("Receiver ID"), GetReceiverId());
		properties->AppendItem(_("Receiver Name"), GetReceiverNode());

		properties->AppendYesNoItem(_("Active"), GetActive());
		properties->AppendYesNoItem(_("May forward"), GetForward());
		if (GetForward())
			properties->AppendYesNoItem(_("Is forwarded"), GetIsSubscribed());
	}
}



pgObject *slSubscription::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *subscription = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		subscription = subscriptionFactory.CreateObjects(coll, 0, wxT(" WHERE sub_set=") + NumToStr(GetSet()->GetSlId())
		               + wxT(" AND sub_receiver = ") + NumToStr(GetReceiverId()) + wxT("\n"));
	return subscription;
}



pgObject *slSubscriptionFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restr)
{
	slSetObjCollection *collection = (slSetObjCollection *)coll;
	slSubscription *subscription = 0;
	wxString restriction;
	if (restr.IsEmpty())
		restriction = wxT(" WHERE sub_set = ") + NumToStr(collection->GetSlId());
	else
		restriction = restr;

	wxString prefix = collection->GetCluster()->GetSchemaPrefix();
	pgSet *subscriptions = collection->GetDatabase()->ExecuteSet(
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
			subscription = new slSubscription(collection->GetSet(), subscriptions->GetVal(wxT("receiver_name")));
			subscription->iSetActive(subscriptions->GetBool(wxT("sub_active")));
			subscription->iSetForward(subscriptions->GetBool(wxT("sub_forward")));
			subscription->iSetReceiverId(subscriptions->GetLong(wxT("sub_receiver")));
			subscription->iSetProviderId(subscriptions->GetLong(wxT("sub_provider")));
			subscription->iSetReceiverNode(subscriptions->GetVal(wxT("receiver_name")));
			subscription->iSetProviderNode(subscriptions->GetVal(wxT("provider_name")));
			subscription->iSetIsSubscribed(subscriptions->GetBool(wxT("is_subscribed")));

			if (browser)
			{
				browser->AppendObject(coll, subscription);
				subscriptions->MoveNext();
			}
			else
				break;
		}

		delete subscriptions;
	}
	return subscription;
}


wxString slSubscriptionCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony subscriptions");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony subscriptions");
			break;
		case OBJECTSLISTREPORT:
			message = _("Slony subscriptions list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////

#include "images/slsubscription.pngc"
#include "images/slsubscriptions.pngc"

slSubscriptionFactory::slSubscriptionFactory()
	: slSetObjFactory(__("Subscription"), __("New Subscription"), __("Create a new Subscription."), slsubscription_png_img)
{
	metaType = SLM_SUBSCRIPTION;
}


pgCollection *slSubscriptionFactory::CreateCollection(pgObject *obj)
{
	return new slSubscriptionCollection(GetCollectionFactory(), (slSet *)obj);
}


slSubscriptionFactory subscriptionFactory;
static pgaCollectionFactory cf(&subscriptionFactory, __("Subscriptions"), slsubscriptions_png_img);
