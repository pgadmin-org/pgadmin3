//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
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


slListen::slListen(slNode *n, const wxString& newName)
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
        expandedKids=true;

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
    pgObject *listen=0;
    pgCollection *coll=browser->GetParentCollection(item);
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
    slNodeObjCollection *collection=(slNodeObjCollection*)coll;
    slListen *listen=0;
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
            wxString orgName=listens->GetVal(wxT("origin_name")).BeforeFirst('\n');
            wxString provName=listens->GetVal(wxT("provider_name")).BeforeFirst('\n');

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


///////////////////////////////////////////////////

#include "images/sllisten.xpm"
#include "images/sllistens.xpm"

slListenFactory::slListenFactory() 
: slNodeObjFactory(__("Listen"), __("New Listen"), __("Create a new Listen."), sllisten_xpm)
{
    metaType = SLM_LISTEN;
}


slListenFactory listenFactory;
static pgaCollectionFactory cf(&listenFactory, __("listens"), sllistens_xpm);
