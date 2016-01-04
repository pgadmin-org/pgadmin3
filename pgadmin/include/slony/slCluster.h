//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slCluster.h PostgreSQL Slony-I Cluster
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLCLUSTER_H
#define SLCLUSTER_H

#include "schema/pgDatabase.h"

class frmMain;
class RemoteConn;
class slNode;


WX_DECLARE_OBJARRAY(RemoteConn, RemoteConnArray);


class pgaSlClusterFactory : public pgDatabaseObjFactory
{
public:
	pgaSlClusterFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgaSlClusterFactory slClusterFactory;


class slCluster : public pgDatabaseObject
{
public:
	slCluster(const wxString &newName = wxT(""));

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	static pgObject *ReadObjects(pgCollection *coll, ctlTree *browser);

	wxString GetTranslatedMessage(int kindOfMessage) const;

	bool ClusterMinimumVersion(int major, int minor);

	void iSetSchemaPrefix(const wxString &s)
	{
		schemaPrefix = s;
	}
	wxString GetSchemaPrefix() const
	{
		return schemaPrefix;
	}
	void iSetLocalNodeID(long l)
	{
		localNodeID = l;
	}
	long GetLocalNodeID()
	{
		return localNodeID;
	}
	void iSetLocalNodeName(const wxString &s)
	{
		localNodeName = s;
	}
	wxString GetLocalNodeName() const
	{
		return localNodeName;
	}
	void iSetAdminNodeID(long l)
	{
		adminNodeID = l;
	}
	long GetAdminNodeID()
	{
		return adminNodeID;
	}
	void iSetAdminNodeName(const wxString &s)
	{
		adminNodeName = s;
	}
	wxString GetAdminNodeName() const
	{
		return adminNodeName;
	}
	void iSetClusterVersion(const wxString &s)
	{
		clusterVersion = s;
	}
	wxString GetClusterVersion() const
	{
		return clusterVersion;
	}
	long GetSlonPid();

	slNode *GetLocalNode(ctlTree *browser);

	bool RequireDropConfirm()
	{
		return true;
	}
	bool WantDummyChild()
	{
		return true;
	}

	pgConn *GetNodeConn(frmMain *form, long nodeId, bool create = true);

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


class slClusterCollection : public pgDatabaseObjCollection
{
public:
	slClusterCollection(pgaFactory *factory, pgDatabase *db);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


// Slony-I object
class slObject : public pgDatabaseObject
{
public:
	slObject(slCluster *_slCluster, pgaFactory &factory, const wxString &newName = wxT(""));
	slCluster *GetCluster()
	{
		return cluster;
	}

	void iSetSlId(long i)
	{
		slId = i;
	}
	long GetSlId() const
	{
		return slId;
	}

private:
	slCluster *cluster;
	long slId;
};


// Collection of Slony-I objects
class slObjCollection : public pgDatabaseObjCollection
{
public:
	slObjCollection(pgaFactory *factory, slCluster *_cluster);

	slCluster *GetCluster()
	{
		return cluster;
	}
	long GetSlId() const
	{
		return slId;
	}
	void iSetSlId(long l)
	{
		slId = l;
	}

private:
	slCluster *cluster;
	long slId;
};

class slObjFactory : public pgDatabaseObjFactory
{
public:
	slObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0) : pgDatabaseObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};

class clusterActionFactory : public contextActionFactory
{
public:
	clusterActionFactory(menuFactoryList *list) : contextActionFactory(list) {}
	bool CheckEnable(pgObject *obj);
};


class slonyRestartFactory : public clusterActionFactory
{
public:
	slonyRestartFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class slonyUpgradeFactory : public clusterActionFactory
{
public:
	slonyUpgradeFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class slonyFailoverFactory : public clusterActionFactory
{
public:
	slonyFailoverFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


#endif


