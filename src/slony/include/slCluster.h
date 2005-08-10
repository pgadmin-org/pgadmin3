//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slCluster.h PostgreSQL Slony-I Cluster
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLCLUSTER_H
#define SLCLUSTER_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;
class frmMain;
class RemoteConn;

WX_DECLARE_OBJARRAY(RemoteConn, RemoteConnArray);


class pgaSlClusterFactory : public pgaFactory
{
public:
    pgaSlClusterFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, wxTreeCtrl *browser, const wxString &restr=wxEmptyString);
};
extern pgaSlClusterFactory slClusterFactory;


class slCluster : public pgDatabaseObject
{
public:
    slCluster(const wxString& newName = wxT(""));
    ~slCluster();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *coll, wxTreeCtrl *browser);


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

    bool RequireDropConfirm() { return true; }
    bool WantDummyChild() { return true; }

    pgConn *GetNodeConn(frmMain *form, long nodeId, bool create=true);

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    wxMenu *GetNewMenu();
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString schemaPrefix;
    wxString localNodeName, adminNodeName;
    wxString clusterVersion;
    long localNodeID, adminNodeID;

    RemoteConnArray remoteConns;
};

#endif

