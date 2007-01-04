//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slCluster.h PostgreSQL Slony-I Cluster
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLCLUSTER_H
#define SLCLUSTER_H

#include "pgDatabase.h"

class frmMain;
class RemoteConn;
class slNode;


WX_DECLARE_OBJARRAY(RemoteConn, RemoteConnArray);


class pgaSlClusterFactory : public pgDatabaseObjFactory
{
public:
    pgaSlClusterFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgaSlClusterFactory slClusterFactory;


class slCluster : public pgDatabaseObject
{
public:
    slCluster(const wxString& newName = wxT(""));
    ~slCluster();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *coll, ctlTree *browser);

    bool ClusterMinimumVersion(int major, int minor);

    void iSetSchemaPrefix(const wxString &s) { schemaPrefix = s; }
    wxString GetSchemaPrefix() const { return schemaPrefix; }
    void iSetLocalNodeID(long l) { localNodeID = l; }
    long GetLocalNodeID() { return localNodeID; }
    void iSetLocalNodeName(const wxString &s) { localNodeName = s; }
    wxString GetLocalNodeName() const { return localNodeName; }
    void iSetAdminNodeID(long l) { adminNodeID = l; }
    long GetAdminNodeID() { return adminNodeID; }
    void iSetAdminNodeName(const wxString &s) { adminNodeName = s; }
    wxString GetAdminNodeName() const { return adminNodeName; }
    void iSetClusterVersion(const wxString &s) { clusterVersion =s; }
    wxString GetClusterVersion() const { return clusterVersion; }
    long GetSlonPid();

    slNode *GetLocalNode(ctlTree *browser);

    bool RequireDropConfirm() { return true; }
    bool WantDummyChild() { return true; }

    pgConn *GetNodeConn(frmMain *form, long nodeId, bool create=true);

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxMenu *GetNewMenu();
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    wxString schemaPrefix;
    wxString localNodeName, adminNodeName;
    wxString clusterVersion;
    long localNodeID, adminNodeID;
    slNode *localNode;

    RemoteConnArray remoteConns;
};



// Slony-I object
class slObject : public pgDatabaseObject
{
public:
    slObject(slCluster *_slCluster, pgaFactory &factory, const wxString& newName = wxT(""));
    slCluster *GetCluster() { return cluster; }

    void iSetSlId(long i) { slId=i; }
    long GetSlId() const { return slId; }

private:
    slCluster *cluster;
    long slId;
};


// Collection of Slony-I objects
class slObjCollection : public pgDatabaseObjCollection
{
public:
    slObjCollection(pgaFactory *factory, slCluster *_cluster);

    slCluster *GetCluster() { return cluster; }
    long GetSlId() const { return slId; }
    void iSetSlId(long l) { slId = l; }

private:
    slCluster *cluster;
    long slId;
};

class slObjFactory : public pgDatabaseObjFactory
{
public:
    slObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img, char **smImg=0) : pgDatabaseObjFactory(tn, ns, nls, img, smImg) {}
    virtual pgCollection *CreateCollection(pgObject *obj);
};

#endif

