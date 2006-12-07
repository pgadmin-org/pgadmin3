//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slCluster.cpp PostgreSQL Slony-I Cluster
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "pgObject.h"
#include "slony/slCluster.h"
#include "slony/slNode.h"
#include "slony/slSet.h"
#include "frm/frmMain.h"

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
    localNode=0;
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
        setFactory.AppendMenu(menu);
        nodeFactory.AppendMenu(menu);
    }
    return menu;
}


bool slCluster::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
        wxT("SELECT ") + GetSchemaPrefix() + wxT("uninstallnode();\n")
        wxT("DROP SCHEMA ") + qtIdent(wxT("_") + GetName()) + wxT(" CASCADE;"));
}


wxString slCluster::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = _("-- Use the installation wizard\n-- to generate the Slony-I replication cluster.\n");
    }
    return sql;
}


slNode *slCluster::GetLocalNode(ctlTree *browser)
{
    pgCollection *nodes=browser->FindCollection(nodeFactory, GetId());
    if (nodes)
    {
        slNode *node;

        treeObjectIterator ni(browser, nodes);
        while ((node=(slNode*)ni.GetNextObject()) != 0)
        {
            if (node->GetSlId() == GetLocalNodeID())
                return node;
        }
    }
    return 0;
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
                    obj = form->GetBrowser()->GetObject(servers);
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
                pgServer *server = (pgServer*)form->GetBrowser()->GetObject(serverItem);
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


bool slCluster::ClusterMinimumVersion(int major, int minor)
{
    long ma=StrToLong(clusterVersion);
    long mi=StrToLong(clusterVersion.AfterFirst('.'));
    return ma > major || (ma == major && mi >= minor);
}


long slCluster::GetSlonPid()
{
    long slonPid=StrToLong(GetConnection()->ExecuteScalar(
        wxT("SELECT listenerpid FROM pg_listener WHERE relname = ")
            + qtDbString(wxT("_") + GetName() + wxT("_Event"))));
    return slonPid;
}


void slCluster::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        browser->RemoveDummyChild(this);
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
                    wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%host=") + GetServer()->GetName() + wxT("%")) +
                    wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%dbname=") + GetDatabase()->GetName() + wxT("%"));
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

        browser->AppendCollection(this, nodeFactory);
        browser->AppendCollection(this, setFactory);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for cluster ") + GetName());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("Local node ID"), GetLocalNodeID());
        properties->AppendItem(_("Local node"), GetLocalNodeName());
        
        if (GetAdminNodeID() == -1L)
            properties->AppendItem(_("Admin node"), _("<none>"));
        else
        {
            properties->AppendItem(_("Admin node ID"), GetAdminNodeID());
            properties->AppendItem(_("Admin node"), GetAdminNodeName());
        }

        long slonPid=GetSlonPid();
        if (slonPid)
            properties->AppendItem(wxT("Slon PID"), slonPid);
        else
            properties->AppendItem(wxT("Slon PID"), _("not running"));

        properties->AppendItem(_("Version"), GetClusterVersion());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *slCluster::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *cluster=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        cluster = slClusterFactory.CreateObjects(coll, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));

    return cluster;
}



pgObject *pgaSlClusterFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
    slCluster *cluster=0;

    pgSet *clusters = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT nsp.oid, substr(nspname, 2) as clustername, nspname, pg_get_userbyid(nspowner) AS namespaceowner, description\n")
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
            cluster->iSetOid(clusters->GetOid(wxT("oid")));
            cluster->iSetDatabase(coll->GetDatabase());
            cluster->iSetSchemaPrefix(qtIdent(clusters->GetVal(wxT("nspname"))) + wxT("."));
            cluster->iSetOwner(clusters->GetVal(wxT("namespaceowner")));
            cluster->iSetComment(clusters->GetVal(wxT("description")));

            if (browser)
            {
                browser->AppendObject(coll, cluster);
				clusters->MoveNext();
            }
            else
                break;
        }

		delete clusters;
    }
    return cluster;
}


    
pgObject *slCluster::ReadObjects(pgCollection *coll, ctlTree *browser)
{
    // Get the clusters
    return slClusterFactory.CreateObjects(coll, browser, wxEmptyString);
}



#include "images/slcluster.xpm"
#include "images/slclusters.xpm"

pgaSlClusterFactory::pgaSlClusterFactory() 
: pgDatabaseObjFactory(__("Slony-I Cluster"), __("New Slony-I Cluster..."), __("Create new Slony-I Replication Cluster"), slcluster_xpm)
{
//    metaType = SLM_CLUSTER;
}


slObject::slObject(slCluster *cl, pgaFactory &factory, const wxString &newName)
: pgDatabaseObject(factory, newName)
{
    cluster = cl;
    iSetDatabase(cl->GetDatabase());
}

slObjCollection::slObjCollection(pgaFactory *factory, slCluster *_cluster)
: pgDatabaseObjCollection(factory, _cluster->GetDatabase())
{
    cluster = _cluster;
}

pgCollection *slObjFactory::CreateCollection(pgObject *obj)
{
    return new slObjCollection(GetCollectionFactory(), (slCluster*)obj);
}


pgaSlClusterFactory slClusterFactory;
static pgaCollectionFactory cf(&slClusterFactory, __("Replication"), slclusters_xpm);
