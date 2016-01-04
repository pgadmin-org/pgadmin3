//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slListen.cpp PostgreSQL Slony-I listen
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgObject.h"
#include "slony/slListen.h"
#include "slony/slNode.h"
#include "slony/slCluster.h"
#include "frm/frmMain.h"


slListen::slListen(slNode *n, const wxString &newName)
	: slNodeObject(n, listenFactory, newName)
{
}

bool slListen::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetDatabase()->ExecuteVoid(
	           wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	           + wxT("droplisten(") + NumToStr(GetSlId())
	           + wxT(", ") + NumToStr(GetProviderId())
	           + wxT(", ") + NumToStr(GetNode()->GetSlId())
	           + wxT(");\n"));
}


wxString slListen::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony listen");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony listen");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony listen \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony listen \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop Slony listen cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop Slony listen?");
			break;
		case PROPERTIESREPORT:
			message = _("Slony listen properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Slony listen properties");
			break;
		case DDLREPORT:
			message = _("Slony listen DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Slony listen DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Slony listen dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Slony listen dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Slony listen dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Slony listen dependents");
			break;
	}

	return message;
}

wxString slListen::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Node  will listen to ") + GetProviderName()
		      + wxT(" for replication data from ") + GetOriginName() + wxT(".\n\n")

		      wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
		      + wxT("storelisten(") + NumToStr(GetSlId())
		      + wxT(", ") + NumToStr(GetProviderId())
		      + wxT(", ") + NumToStr(GetNode()->GetSlId())
		      + wxT(");\n");
	}
	return sql;
}


void slListen::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);
	}


	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Origin"), GetOriginName());
		properties->AppendItem(_("Origin ID"), GetSlId());
		properties->AppendItem(_("Provider"), GetProviderName());
		properties->AppendItem(_("Provider ID"), GetProviderId());
	}
}



pgObject *slListen::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *listen = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		listen = listenFactory.CreateObjects(coll, 0,
		                                     wxT(" WHERE li_origin =") + NumToStr(GetSlId()) +
		                                     wxT("   AND li_provider = ") + NumToStr(GetProviderId()) +
		                                     wxT("   AND li_receiver = ") + NumToStr(GetNode()->GetSlId()) +
		                                     wxT("\n"));

	return listen;
}



pgObject *slListenFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restr)
{
	slNodeObjCollection *collection = (slNodeObjCollection *)coll;
	slListen *listen = 0;
	wxString restriction;
	if (restr.IsEmpty())
		restriction = wxT(" WHERE li_receiver = ") + NumToStr(collection->GetSlId());
	else
		restriction = restr;

	pgSet *listens = collection->GetDatabase()->ExecuteSet(
	                     wxT("SELECT li_origin, li_provider, li_receiver, no.no_comment as origin_name, np.no_comment as provider_name\n")
	                     wxT("  FROM ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_listen\n")
	                     wxT("  JOIN ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_node no ON no.no_id=li_origin\n")
	                     wxT("  JOIN ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_node np ON np.no_id=li_provider\n")
	                     + restriction +
	                     wxT(" ORDER BY li_origin, li_provider"));

	if (listens)
	{
		while (!listens->Eof())
		{
			wxString orgName = listens->GetVal(wxT("origin_name")).BeforeFirst('\n');
			wxString provName = listens->GetVal(wxT("provider_name")).BeforeFirst('\n');

			listen = new slListen(collection->GetNode(), orgName + wxT(" (") + provName + wxT(")"));
			listen->iSetSlId(listens->GetLong(wxT("li_origin")));
			listen->iSetProviderId(listens->GetLong(wxT("li_provider")));
			listen->iSetOriginName(orgName);
			listen->iSetProviderName(provName);

			if (browser)
			{
				browser->AppendObject(collection, listen);
				listens->MoveNext();
			}
			else
				break;
		}

		delete listens;
	}
	return listen;
}


wxString slListenCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony listens");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony listens");
			break;
		case OBJECTSLISTREPORT:
			message = _("Slony listens list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////

#include "images/sllisten.pngc"
#include "images/sllistens.pngc"

slListenFactory::slListenFactory()
	: slNodeObjFactory(__("Listen"), __("New Listen"), __("Create a new Listen."), sllisten_png_img)
{
	metaType = SLM_LISTEN;
}


pgCollection *slListenFactory::CreateCollection(pgObject *obj)
{
	return new slListenCollection(GetCollectionFactory(), (slNode *)obj);
}


slListenFactory listenFactory;
static pgaCollectionFactory cf(&listenFactory, __("listens"), sllistens_png_img);
