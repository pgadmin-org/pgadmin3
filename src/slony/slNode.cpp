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
        if (GetCluster()->GetLocalNodeID() == GetSlId())
            sql = wxT("-- Local node; created when initializing cluster.\n");
        else
            sql = wxT("-- Create replication node ") + GetName() + wxT(".\n\n")
                  wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("storenode(") 
                        + NumToStr(GetSlId()) + wxT(", ")
                        + qtString(GetName()) + wxT(");\n");
    }
    return sql;
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
        if (GetCluster()->GetHasConnInfo())
            properties->AppendItem(_("Connect info"), GetConnInfo());

        properties->AppendItem(_("Connected"), conn != NULL);

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
            properties->AppendItem(_("Running PID"), _("no connection to server"));
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
            node = new slNode(coll->GetCluster(), nodes->GetVal(wxT("no_comment")));
            node->iSetSlId(nodes->GetLong(wxT("no_id")));
            node->iSetActive(nodes->GetBool(wxT("no_active")));
            if (coll->GetCluster()->GetHasConnInfo())
                node->iSetConnInfo(nodes->GetVal(wxT("no_conninfo")));

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

