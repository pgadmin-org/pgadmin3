//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slCluster.cpp PostgreSQL Slony-I Cluster
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "slCluster.h"
#include "slObject.h"
#include "frmMain.h"

#include <wx/arrimpl.cpp>


class RemoteConn
{
public:
    RemoteConn(long id, const wxString str, pgConn *c) { connInfo=str; nodeID=id; conn=c; }
    ~RemoteConn() { if (conn) delete conn; }

    wxString connInfo;
    pgConn *conn;
    long nodeID;
};


WX_DEFINE_OBJARRAY(RemoteConnArray);

slCluster::slCluster(const wxString& newName)
: pgDatabaseObject(SL_CLUSTER, newName)
{
    wxLogInfo(wxT("Creating a slCluster object"));
}

slCluster::~slCluster()
{
    wxLogInfo(wxT("Destroying a slCluster object"));
}


wxMenu *slCluster::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();

//    if (GetCreatePrivilege())
    {
        AppendMenu(menu, SL_SET);
        AppendMenu(menu, SL_NODE);
    }
    return menu;
}


bool slCluster::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(
        wxT("SELECT ") + GetSchemaPrefix() + wxT("uninstallnode();\n")
        wxT("DROP SCHEMA ") + qtIdent(wxT("_") + GetName()) + wxT(" CASCADE;"));
}


wxString slCluster::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = _("-- Use the installation wizard to generate the Slony-I replication cluster.\n");
    }
    return sql;
}


pgConn *slCluster::GetNodeConn(frmMain *form, long nodeId, bool create)
{
    size_t i;

    if (nodeId == GetLocalNodeID())
        return GetDatabase()->GetConnection();

    for (i=0 ; i < remoteConns.GetCount() ; i++)
    {
        if (remoteConns[i].nodeID == nodeId)
            return remoteConns[i].conn;
    }

    if (create && GetHasConnInfo())
    {
        wxString connstr=GetDatabase()->ExecuteScalar(
            wxT("SELECT no_conninfo FROM ") + GetSchemaPrefix() + wxT("sl_node\n")
            wxT(" WHERE no_id = ") + NumToStr(nodeId));
        if (!connstr.IsEmpty())
        {
            // check for server registration
            wxTreeItemId servers = GetId();
            pgObject *obj=this;
            while (obj && obj->GetType() != PG_SERVERS)
            {
                servers = form->GetBrowser()->GetItemParent(servers);
                if (servers)            
                    obj = (pgObject*)form->GetBrowser()->GetItemData(servers);
            }
            
            wxCookieType cookie;

            wxStringTokenizer strtok(connstr, wxT(" "), wxTOKEN_RET_EMPTY);

            wxString lastToken;
            wxArrayString tokens;
            wxString str;

            while (strtok.HasMoreTokens())
            {
                str = strtok.GetNextToken();
                if (str.Find('=') < 0)
                {
                    if (!lastToken.IsEmpty())
                        lastToken += wxT(" ");
                    lastToken += str;
                }
                else 
                {
                    if (!lastToken.IsEmpty())
                        tokens.Add(lastToken);
                    lastToken = str;
                }
            }
            if (!lastToken.IsEmpty())
                tokens.Add(lastToken);

            size_t i;

            wxString host, dbname;
            int port=5432;
            for (i=0 ; i < tokens.GetCount() ; i++)
            {
                str=tokens[i].BeforeFirst('=');
                if (str == wxT("host"))
                    host = tokens[i].AfterFirst('=');
                else if (str == wxT("dbname"))
                    dbname = tokens[i].AfterFirst('=');
                else if (str == wxT("port"))
                    port = StrToLong(tokens[i].AfterFirst('='));
            }

            if (host.IsEmpty() || dbname.IsEmpty())
                return NULL;

            wxTreeItemId serverItem=form->GetBrowser()->GetFirstChild(servers, cookie);        
            while (serverItem)
            {
                pgServer *server = (pgServer*)form->GetBrowser()->GetItemData(serverItem);
                if (server && server->GetType() == PG_SERVER)
                {
                    if (server->GetName() == host && server->GetPort() == port)
                    {
                        if (!server->GetConnected())
                        {
                            server->Connect(form, server->GetNeedPwd());
                            if (!server->GetConnected())
                            {
                                wxLogError(server->GetLastError());
                                return NULL;
                            }
                        }
                        pgConn *conn = server->CreateConn(dbname);

                        if (conn)
                            remoteConns.Add(new RemoteConn(nodeId, connstr, conn));

                        return conn;
                    }
                }
                serverItem = form->GetBrowser()->GetNextChild(servers, cookie);
            }

        }
    }
    return NULL;
}


void slCluster::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child object to cluster ") + GetIdentifier());

        pgSet *set=GetDatabase()->ExecuteSet(
            wxT("SELECT last_value, no_comment\n")
            wxT("  FROM ") + GetSchemaPrefix() + wxT("sl_local_node_id\n")
            wxT("  JOIN ") + GetSchemaPrefix() + wxT("sl_node ON no_id = last_value"));
        if (set)
        {
            iSetLocalNodeID(set->GetLong(wxT("last_value")));
            iSetLocalNodeName(set->GetVal(wxT("no_comment")));
            delete set;
        }

        pgCollection *collection;

        // Nodes
        collection = new slCollection(SL_NODES, this);
        AppendBrowserItem(browser, collection);

        // Sets
        collection = new slCollection(SL_SETS, this);
        AppendBrowserItem(browser, collection);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for cluster ") + GetName());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("Local node ID"), GetLocalNodeID());
        properties->AppendItem(_("Local node"), GetLocalNodeName());
        properties->AppendItem(_("Version"), GetClusterVersion());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *slCluster::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *cluster=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgCollection *coll=(pgCollection*)browser->GetItemData(parentItem);
        if (coll->GetType() == PG_DATABASES)
            cluster = ReadObjects(coll, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));
    }
    return cluster;
}



pgObject *slCluster::ReadObjects(pgCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slCluster *cluster=0;

    pgSet *clusters = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT substr(nspname, 2) as clustername, nspname, pg_get_userbyid(nspowner) AS namespaceowner, description, attname\n")
        wxT("  FROM pg_namespace nsp\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
        wxT("  JOIN pg_proc pro ON pronamespace=nsp.oid AND proname = 'slonyversion'\n")
        wxT("  JOIN pg_class cl ON relnamespace=nsp.oid AND relname = 'sl_node'\n")
        wxT("  LEFT JOIN pg_attribute att ON attrelid = cl.oid AND attname = 'no_conninfo'\n")
         + restriction +
        wxT(" ORDER BY nspname"));

    if (clusters)
    {
        while (!clusters->Eof())
        {
            cluster = new slCluster(clusters->GetVal(wxT("clustername")));
            cluster->iSetDatabase(coll->GetDatabase());
            cluster->iSetSchemaPrefix(qtIdent(clusters->GetVal(wxT("nspname"))) + wxT("."));
            cluster->iSetOwner(clusters->GetVal(wxT("namespaceowner")));
            cluster->iSetComment(clusters->GetVal(wxT("description")));
            cluster->iSetHasConnInfo(clusters->GetVal(wxT("attname")) == wxT("no_conninfo"));

            if (browser)
            {
                coll->AppendBrowserItem(browser, cluster);
				clusters->MoveNext();
            }
            else
                break;
        }

		delete clusters;
    }
    return cluster;
}


    
pgObject *slCluster::ReadObjects(pgCollection *coll, wxTreeCtrl *browser)
{
    // Get the clusters
    return ReadObjects(coll, browser, wxEmptyString);
}

