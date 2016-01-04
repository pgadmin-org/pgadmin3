//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepSubscription.cpp - PostgreSQL Slony-I Subscription Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "slony/dlgRepSubscription.h"
#include "slony/slCluster.h"
#include "slony/slSubscription.h"
#include "slony/slSet.h"



// pointer to controls
#define txtOrigin       CTRL_TEXT("txtOrigin")
#define stReceiver      CTRL_STATIC("stReceiver")
#define txtReceiver     CTRL_TEXT("txtReceiver")
#define cbProvider      CTRL_COMBOBOX("cbProvider")
#define stProvider      CTRL_TEXT("stProvider")
#define chkForward      CTRL_CHECKBOX("chkForward")



BEGIN_EVENT_TABLE(dlgRepSubscription, dlgProperty)
	EVT_COMBOBOX(XRCID("cbProvider"),   dlgRepSubscription::OnChange)
	EVT_CHECKBOX(XRCID("chkForward"),   dlgRepSubscription::OnChange)
END_EVENT_TABLE();


dlgProperty *slSubscriptionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRepSubscription(this, frame, (slSubscription *)node, (slSet *)parent);
}

dlgRepSubscription::dlgRepSubscription(pgaFactory *f, frmMain *frame, slSubscription *sub, slSet *s)
	: dlgRepProperty(f, frame, s->GetCluster(), wxT("dlgRepSubscription"))
{
	subscription = sub;
	set = s;
}


pgObject *dlgRepSubscription::GetObject()
{
	return subscription;
}


int dlgRepSubscription::Go(bool modal)
{
	txtOrigin->SetValue(NumToStr(set->GetOriginId()));
	if (subscription)
	{
		// edit mode
		chkForward->SetValue(subscription->GetForward());
		txtReceiver->SetValue(IdAndName(subscription->GetReceiverId(), subscription->GetReceiverNode()));
	}
	else
	{
		// create mode
		txtReceiver->SetValue(IdAndName(cluster->GetLocalNodeID(), cluster->GetLocalNodeName()));

		if (cluster->ClusterMinimumVersion(1, 1))
		{
			// This is very ugly: starting with Slony-I 1.1, this must be called on the provider,
			// not on the receiver.
			stProvider->SetLabel(_("Receiver"));
			stReceiver->SetLabel(_("Provider"));
		}
	}

	if (set->GetOriginId() == cluster->GetLocalNodeID() && subscription)
	{
		chkForward->SetValue(subscription->GetForward());
		cbProvider->Append(IdAndName(subscription->GetProviderId(), subscription->GetProviderNode()));
		cbProvider->SetSelection(0);
		cbProvider->Disable();
		chkForward->Disable();
		EnableOK(false);
	}
	else
	{
		pgSet *sets = connection->ExecuteSet(
		                  wxT("SELECT no_id, no_comment\n")
		                  wxT("  FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node\n")
		                  wxT(" WHERE no_active AND no_id <> ") + NumToStr(cluster->GetLocalNodeID()));

		if (sets)
		{
			while (!sets->Eof())
			{
				cbProvider->Append(IdAndName(sets->GetLong(wxT("no_id")), sets->GetVal(wxT("no_comment"))),
				                   (void *)sets->GetLong(wxT("no_id")));

				if (subscription && sets->GetLong(wxT("no_id")) == subscription->GetProviderId())
					cbProvider->SetSelection(cbProvider->GetCount() - 1);
				sets->MoveNext();
			}
			delete sets;
		}
	}
	if (!subscription && cbProvider->GetCount())
	{
		cbProvider->SetSelection(0);
		EnableOK(true);
	}
	return dlgProperty::Go(modal);
}


pgObject *dlgRepSubscription::CreateObject(pgCollection *collection)
{
	pgObject *obj = subscriptionFactory.CreateObjects(collection, 0,
	                wxT(" WHERE set_id = ") + NumToStr(set->GetSlId()) +
	                wxT("   AND sub_receiver = ") + NumToStr(cluster->GetLocalNodeID()));

	return obj;
}


void dlgRepSubscription::CheckChange()
{
	if (subscription)
	{
		int sel = cbProvider->GetCurrentSelection();

		EnableOK(sel >= 0 && (chkForward->GetValue() != subscription->GetForward()
		                      ||   (long)cbProvider->wxItemContainer::GetClientData(sel) != subscription->GetProviderId()));
	}
	else
	{
		bool enable = true;

		EnableOK(enable);
	}
}



wxString dlgRepSubscription::GetSql()
{
	wxString sql;

	sql = wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("subscribeset(")
	      + NumToStr(set->GetSlId()) + wxT(", ");

	if (cluster && cluster->ClusterMinimumVersion(1, 1))
	{
		// Actually, provider and receiver are exchanged here.
		sql += NumToStr(cluster->GetLocalNodeID()) + wxT(", ")
		       + NumToStr((long)cbProvider->wxItemContainer::GetClientData(cbProvider->GetCurrentSelection()));
	}
	else
	{
		sql += NumToStr((long)cbProvider->wxItemContainer::GetClientData(cbProvider->GetCurrentSelection())) + wxT(", ")
		       + NumToStr(cluster->GetLocalNodeID());
	}
	sql += wxT(", ")
	       + BoolToStr(chkForward->GetValue());

	// Omit copy?
	if (cluster && cluster->ClusterMinimumVersion(2, 0) &&
	        cluster->GetClusterVersion() != wxT("2.0.0") &&
	        cluster->GetClusterVersion() != wxT("2.0.1") &&
	        cluster->GetClusterVersion() != wxT("2.0.2"))
		sql += wxT(", false");

	sql += wxT(");");

	return sql;
}
