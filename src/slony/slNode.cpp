//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slNode.cpp PostgreSQL Slony-I node
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "slNode.h"
#include "slObject.h"
#include "slCluster.h"
#include "frmMain.h"


slNode::slNode(slCluster *cl, const wxString& newName)
: slObject(cl, SL_NODE, newName)
{
    wxLogInfo(wxT("Creating a slNode object"));
    pid = -1;
}

slNode::~slNode()
{
    wxLogInfo(wxT("Destroying a slNode object"));
}


wxMenu *slNode::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();

//    if (GetCreatePrivilege())

    return menu;
}


bool slNode::CanDrop()
{
    return (GetSlId() != GetCluster()->GetLocalNodeID());
}


bool slNode::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("dropnode(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slNode::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Create replication node ") + GetName() + wxT(".\n\n")
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("storenode(") 
                    + NumToStr(GetSlId()) + wxT(", ")
                    + qtString(GetComment()) + wxT(");\n");
    }
    return sql;
}


void slNode::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    CreateListColumns(statistics, _("Statistic"), _("Value"));

    if (GetCluster()->GetLocalNodeID() == GetSlId())
    {
        pgSet *stats=GetDatabase()->ExecuteSet(
            wxT("SELECT st_last_event,\n")
            wxT("       MAX(st_last_received_ts - st_last_received_event_ts) AS roundtrip,\n")
            wxT("       SUM(st_lag_num_events) AS sumlagevents, st_last_event - MAX(st_lag_num_events) as oldestlagevent,")
            wxT("       MAX(st_last_event_ts - st_last_received_ts) AS maxeventlag")
            wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_status\n")
            wxT(" WHERE st_origin = ") + NumToStr(GetCluster()->GetLocalNodeID()) + wxT("\n")
            wxT("  GROUP BY st_last_event"));
        if (pgSet)

        if (stats)
        {
            statistics->AppendItem(_("Last event"), stats->GetLong(wxT("st_last_event")));
            statistics->AppendItem(_("Max response time"), stats->GetVal(wxT("roundtrip")));
            long lags=stats->GetLong(wxT("sumlagevents"));
             statistics->AppendItem(_("Acks outstanding"), lags);


            if (lags > 0)
            {
                statistics->AppendItem(_("Oldest outstanding"), stats->GetLong(wxT("oldestlagevent")));
                statistics->AppendItem(_("Outstanding for"), stats->GetVal(wxT("maxeventlag")));
            }

            delete stats;
        }
    }
    else
    {
        pgSet *stats=GetDatabase()->ExecuteSet(
            wxT("SELECT st_last_event, st_last_event_ts, st_last_received, st_last_received_ts,\n")
            wxT("       st_last_received_ts - st_last_received_event_ts AS roundtrip,\n")
            wxT("       st_last_event_ts - st_last_received_ts AS eventlag")
            wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_status\n")
            wxT(" WHERE st_origin = ") + NumToStr(GetCluster()->GetLocalNodeID()) + wxT("\n")
            wxT("   AND st_received = ") + NumToStr(GetSlId()));
        if (pgSet)

        if (stats)
        {
            long evno=stats->GetLong(wxT("st_last_event"));
            long ack = stats->GetLong(wxT("st_last_received"));

            statistics->AppendItem(_("Last event"), evno);
            statistics->AppendItem(_("Last event timestamp"), stats->GetDateTime(wxT("st_last_event_ts")));
            statistics->AppendItem(_("Last acknowledged"), ack);
            statistics->AppendItem(_("Last ack timestamp"), stats->GetDateTime(wxT("st_last_received_ts")));
            statistics->AppendItem(_("Last response time"), stats->GetVal(wxT("roundtrip")));

            if (evno > ack)
            {
                statistics->AppendItem(_("Outstanding acks"), evno - ack);
                statistics->AppendItem(_("No ack for"), stats->GetVal(wxT("eventlag")));
            }

            delete stats;
        }
    }
}


void slNode::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    pgConn *conn = GetCluster()->GetNodeConn(form, GetSlId(), pid<0);

    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child object to node ") + GetIdentifier());

        slNodeCollection *collection;

        collection = new slNodeCollection(SL_PATHS, this);
        AppendBrowserItem(browser, collection);

        collection = new slNodeCollection(SL_LISTENS, this);
        AppendBrowserItem(browser, collection);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for node ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("ID"), GetSlId());
        if (GetCluster()->GetLocalNodeID() == GetSlId())
            properties->AppendItem(_("Local node"), true);
        properties->AppendItem(_("Active"), GetActive());
        properties->AppendItem(_("Connected"), conn != NULL);
        properties->AppendItem(_("Comment"), GetComment());

        if (conn && pid < 0)
            pid=StrToLong(conn->ExecuteScalar(
                wxT("SELECT listenerpid FROM pg_listener WHERE relname=") + qtString(wxT("_") + GetCluster()->GetName() + wxT("_Restart"))));

        if (conn)
        {
            if (GetPid())
                properties->AppendItem(_("Running PID"), GetPid());
            else
                properties->AppendItem(_("Running PID"), _("not running"));
        }
        else
        {
            if (GetSlId() != GetCluster()->GetAdminNodeID())
                properties->AppendItem(_("Running PID"), _("administrative node"));
        }
    }
}



pgObject *slNode::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *node=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        slCollection *coll=(slCollection*)browser->GetItemData(parentItem);
        if (coll->GetType() == SL_NODES)
            node = ReadObjects(coll, 0, wxT(" WHERE no_id=") + NumToStr(GetSlId()) + wxT("\n"));
    }
    return node;
}



pgObject *slNode::ReadObjects(slCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slNode *node=0;

    pgSet *nodes = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT * FROM ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_node\n")
         + restriction +
        wxT(" ORDER BY no_id"));

    if (nodes)
    {
        while (!nodes->Eof())
        {
            node = new slNode(coll->GetCluster(), nodes->GetVal(wxT("no_comment")).BeforeFirst('\n'));
            node->iSetSlId(nodes->GetLong(wxT("no_id")));
            node->iSetActive(nodes->GetBool(wxT("no_active")));
            node->iSetComment(nodes->GetVal(wxT("no_comment")));

            if (browser)
            {
                coll->AppendBrowserItem(browser, node);
				nodes->MoveNext();
            }
            else
                break;
        }

		delete nodes;
    }
    return node;
}


    
pgObject *slNode::ReadObjects(slCollection *coll, wxTreeCtrl *browser)
{
    // Get the nodes
    return ReadObjects(coll, browser, wxEmptyString);
}


void slNode::ShowStatistics(slCollection *collection, ctlListView *statistics)
{
    wxLogInfo(wxT("Displaying statistics for nodes on Cluster ")+ collection->GetCluster()->GetIdentifier());

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->AddColumn(_("Node"), 50);
    statistics->AddColumn(_("Roundtrip"), 50);
    statistics->AddColumn(_("Acks outstanding"), 50);
    statistics->AddColumn(_("Outstanding time"), 50);

   pgSet *stats=collection->GetDatabase()->ExecuteSet(
        wxT("SELECT st_received, st_last_event, st_lag_num_events, st_last_event_ts, st_last_received, st_last_received_ts,\n")
        wxT("       st_last_received_ts - st_last_received_event_ts AS roundtrip,\n")
        wxT("       CASE WHEN st_lag_num_events > 0 THEN st_last_event_ts - st_last_received_ts END AS eventlag")
        wxT("  FROM ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_status\n")
        wxT(" WHERE st_origin = ") + NumToStr(collection->GetCluster()->GetLocalNodeID()));

    if (stats)
    {
        long pos=0;
        while (!stats->Eof())
        {
            statistics->InsertItem(pos, NumToStr(stats->GetLong(wxT("st_received"))), SLICON_NODE);
            statistics->SetItem(pos, 1, stats->GetVal(wxT("roundtrip")));
            statistics->SetItem(pos, 2, NumToStr(stats->GetLong(wxT("st_lag_num_events"))));
            statistics->SetItem(pos, 3, stats->GetVal(wxT("eventlag")));
            stats->MoveNext();
            pos++;
        }

	    delete stats;
    }
}
