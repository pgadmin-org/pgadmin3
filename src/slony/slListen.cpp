//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slListen.cpp PostgreSQL Slony-I listen
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "slListen.h"
#include "slObject.h"
#include "slNode.h"
#include "slCluster.h"
#include "frmMain.h"


slListen::slListen(slNode *n, const wxString& newName)
: slNodeObject(n, SL_LISTEN, newName)
{
    wxLogInfo(wxT("Creating a slListen object"));
}

slListen::~slListen()
{
    wxLogInfo(wxT("Destroying a slListen object"));
}


bool slListen::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("droplisten(") + NumToStr(GetOriginId())
            + wxT(", ") + NumToStr(GetSlId())
            + wxT(", ") + NumToStr(GetNode()->GetSlId())
            + wxT(");\n"));
}


wxString slListen::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Node  will listen to ") + GetName()
                + wxT(" for replication data from ") + GetOriginName() + wxT(".\n\n")

              wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
                    + wxT("storelisten(") + NumToStr(GetOriginId())
                    + wxT(", ") + NumToStr(GetSlId())
                    + wxT(", ") + NumToStr(GetNode()->GetSlId())
                    + wxT(");\n");
    }
    return sql;
}


void slListen::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for listen ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Provider"), GetName());
        properties->AppendItem(_("Provider ID"), GetSlId());
        properties->AppendItem(_("Origin"), GetOriginName());
        properties->AppendItem(_("OriginID"), GetOriginId());
    }
}



pgObject *slListen::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *listen=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        slNodeCollection *coll=(slNodeCollection*)browser->GetItemData(parentItem);
        if (coll->GetType() == SL_LISTENS)
            listen = ReadObjects(coll, 0, 
                wxT(" WHERE li_origin =") + NumToStr(GetOriginId()) +
                wxT("   AND li_provider = ") + NumToStr(GetSlId()) +
                wxT("   AND li_receiver = ") + NumToStr(GetNode()->GetSlId()) +
                wxT("\n"));
    }
    return listen;
}



pgObject *slListen::ReadObjects(slNodeCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slListen *listen=0;

    pgSet *listens = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT li_origin, li_provider, li_receiver, no.no_comment as origin_name, np.no_comment as provider_name\n")
        wxT("  FROM ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_listen\n")
        wxT("  JOIN ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_node no ON no.no_id=li_origin\n")
        wxT("  JOIN ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_node np ON np.no_id=li_provider\n")
         + restriction +
        wxT(" ORDER BY li_origin, li_provider"));

    if (listens)
    {
        while (!listens->Eof())
        {
            listen = new slListen(coll->GetNode(), listens->GetVal(wxT("provider_name")).BeforeFirst('\n'));
            listen->iSetSlId(listens->GetLong(wxT("li_provider")));
            listen->iSetOriginId(listens->GetLong(wxT("li_origin")));
            listen->iSetOriginName(listens->GetVal(wxT("origin_name")).BeforeFirst('\n'));

            if (browser)
            {
                coll->AppendBrowserItem(browser, listen);
				listens->MoveNext();
            }
            else
                break;
        }

		delete listens;
    }
    return listen;
}


    
pgObject *slListen::ReadObjects(slNodeCollection *coll, wxTreeCtrl *browser)
{
    // Get the listens
    wxString restriction = wxT(" WHERE li_receiver = ") + NumToStr(coll->GetSlId());
    return ReadObjects(coll, browser, restriction);
}

