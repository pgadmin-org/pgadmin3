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
: pgDatabaseObject(slClusterFactory, newName)
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


bool slCluster::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
        wxT("SELECT ") + GetSchemaPrefix() + wxT("uninstallnode();\n")
        wxT("DROP SCHEMA ") + qtIdent(wxT("_") + GetName()) + wxT(" CASCADE;"));
}


wxString slCluster::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = _("-- Use the installation wizard\n-- to generate the Slony-I replication cluster.\n");
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

    if (create && adminNodeID >= 0)
    {
        wxString connstr=GetDatabase()->ExecuteScalar(
            wxT("SELECT pa_conninfo FROM ") + GetSchemaPrefix() + wxT("sl_path\n")
            wxT(" WHERE pa_server = ") + NumToStr(nodeId) +
            wxT("   AND pa_client = ") + NumToStr(adminNodeID));

        if (!connstr.IsEmpty())
        {
            // check for server registration
            wxTreeItemId servers = GetId();
            pgObject *obj=this;
            while (obj && obj != form->GetServerCollection())
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
                if (server && server->IsCreatedBy(serverFactory))
                {
                    if (server->GetName() == host && server->GetPort() == port)
                    {
                        if (!server->GetConnected())
                        {
                            server->Connect(form, server->GetStorePwd());
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
        pgSet *set;

        set=GetDatabase()->ExecuteSet(
            wxT("SELECT no_id, no_comment, ") + GetSchemaPrefix() + wxT("slonyversion() AS version\n")
            wxT("  FROM ") + GetSchemaPrefix() + wxT("sl_local_node_id\n")
            wxT("  JOIN ") + GetSchemaPrefix() + wxT("sl_node ON no_id = last_value"));
        if (set)
        {
            iSetClusterVersion(set->GetVal(wxT("version")));
            iSetLocalNodeID(set->GetLong(wxT("no_id")));
            iSetLocalNodeName(set->GetVal(wxT("no_comment")).BeforeFirst('\n'));
            delete set;
        }

        adminNodeID = settings->Read(wxT("Replication/") + GetName() + wxT("/AdminNode"), -1L);

        wxString sql =  wxT("SELECT no_id, no_comment\n")
                        wxT("  FROM ") + GetSchemaPrefix() + wxT("sl_node\n");

        if (adminNodeID == -1L)
        {
            sql +=  wxT("  JOIN ") + GetSchemaPrefix() + wxT("sl_path ON no_id = pa_client\n")
                    wxT(" WHERE pa_server = ") + NumToStr(localNodeID) + 
                    wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%host=") + GetServer()->GetName() + wxT("%")) +
                    wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%dbname=") + GetDatabase()->GetName() + wxT("%"));
        }
        else
            sql += wxT(" WHERE no_id = ") + NumToStr(adminNodeID); 


        set = GetDatabase()->ExecuteSet(sql);
        if (set)
        {
            if (!set->Eof())
            {
                adminNodeID = set->GetLong(wxT("no_id"));
                adminNodeName = set->GetVal(wxT("no_comment"));
                settings->Write(wxT("Replication/") + GetName() + wxT("/AdminNode"), adminNodeID);
            }
            delete set;
        }

        wxLogInfo(wxT("Adding child object to cluster ") + GetIdentifier());
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
        properties->AppendItem(_("Admin node ID"), GetAdminNodeID());
        properties->AppendItem(_("Admin node"), GetAdminNodeName());
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
        if (coll->IsCollection())
            cluster = slClusterFactory.CreateObjects(coll, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));
    }
    return cluster;
}



pgObject *pgaSlClusterFactory::CreateObjects(pgCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slCluster *cluster=0;

    pgSet *clusters = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT substr(nspname, 2) as clustername, nspname, pg_get_userbyid(nspowner) AS namespaceowner, description\n")
        wxT("  FROM pg_namespace nsp\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
        wxT("  JOIN pg_proc pro ON pronamespace=nsp.oid AND proname = 'slonyversion'\n")
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
    return slClusterFactory.CreateObjects(coll, browser, wxEmptyString);
}



#include "images/slcluster.xpm"

pgaSlClusterFactory::pgaSlClusterFactory() 
: pgaFactory(__("Slony-I Cluster"), __("New Slony-I Cluster"), __("Create new Slony-I Replication Cluster"), slcluster_xpm)
{
}

pgaSlClusterFactory slClusterFactory;
static pgaCollectionFactory cf(&slClusterFactory, __("Replication"));
