//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgServer.h - PostgreSQL Server
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSERVER_H
#define PGSERVER_H

#include "db/pgConn.h"
#include "pgCollection.h"


class frmMain;


class pgServerFactory : public pgaFactory
{
public:
	pgServerFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);

	int GetClosedIconId()
	{
		return WantSmallIcon() ? smallClosedId : closedId;
	}

protected:
	int closedId, smallClosedId;
};
extern pgServerFactory serverFactory;


class pgServer : public pgObject
{
public:
	pgServer(const wxString &newServer = wxT(""), const wxString &newDescription = wxT(""), const wxString &newDatabase = wxT(""), const wxString &newUsername = wxT(""), int newPort = 5432, bool storePwd = false, bool restore = true, int sslMode = 0, const wxString &colour = wxT("#FFFFFF"), const wxString &group = wxEmptyString);
	~pgServer();
	int GetIconId();

	wxString GetTypeName() const
	{
		return wxT("Server");
	}
	int Connect(frmMain *form, bool askPassword = true, const wxString &pwd = wxEmptyString, bool forceStorePassword = false);
	bool Disconnect(frmMain *form);
	void StorePassword();
	bool GetPasswordIsStored();
	void InvalidatePassword()
	{
		passwordValid = false;
	}

	bool StartService();
	bool StopService();
	bool GetServerRunning();
	bool GetServerControllable();
	bool ReloadConfiguration();

	pgServer *GetServer() const;

	wxString GetIdentifier() const;
	wxString GetVersionString();
	wxString GetVersionNumber();
	OID GetLastSystemOID();
	wxString GetDatabaseName() const
	{
		return database;
	}
	wxString GetUsername() const
	{
		return username;
	}
	wxString GetPassword() const
	{
		return (password == wxEmptyString ? conn->GetPassword() : password);
	}
	bool GetStorePwd() const
	{
		return storePwd;
	}
	bool GetRestore() const
	{
		return restore;
	}
	wxString GetLastError() const;

	bool GetDiscovered() const
	{
		return discovered;
	}
	void iSetDiscovered(const bool b)
	{
		discovered = b;
	}
	wxString GetServiceID() const
	{
		return serviceId;
	}
	void iSetServiceID(const wxString &s);
	wxString GetDiscoveryID() const
	{
		return discoveryId;
	}
	void iSetDiscoveryID(const wxString &s)
	{
		discoveryId = s;
	}

	bool GetCreatePrivilege() const
	{
		return createPrivilege;
	}
	void iSetCreatePrivilege(const bool b)
	{
		createPrivilege = b;
	}
	bool GetSuperUser() const
	{
		return superUser;
	}
	void iSetSuperUser(const bool b)
	{
		superUser = b;
	}
	bool GetCreateRole() const
	{
		return createRole;
	}
	void iSetCreateRole(const bool b)
	{
		createRole = b;
	}

	bool GetInRecovery() const
	{
		return inRecovery;
	}
	void iSetInRecovery(const bool b)
	{
		inRecovery = b;
	}
	wxDateTime GetConfLoadedSince()
	{
		return confLoadedSince;
	}
	void iSetConfLoadedSince(const wxDateTime &d)
	{
		confLoadedSince = d;
	}
	wxString GetReceiveLoc() const
	{
		return receiveLoc;
	}
	void iSetReceiveLoc(const wxString &s)
	{
		receiveLoc = s;
	}
	wxString GetReplayLoc() const
	{
		return replayLoc;
	}
	void iSetReplayLoc(const wxString &s)
	{
		replayLoc = s;
	}

	pgConn *CreateConn(wxString dbName = wxEmptyString, OID oid = 0, wxString applicationname = wxEmptyString);

	wxString GetLastDatabase() const
	{
		return lastDatabase;
	}
	void iSetLastDatabase(const wxString &s)
	{
		lastDatabase = s;
	}
	wxString GetLastSchema() const
	{
		return lastSchema;
	}
	void iSetLastSchema(const wxString &s)
	{
		lastSchema = s;
	}
	wxString GetDescription() const
	{
		return description;
	}
	void iSetDescription(const wxString &s)
	{
		description = s;
	}

	wxString GetDbRestriction() const
	{
		return dbRestriction;
	}
	void iSetDbRestriction(const wxString &s)
	{
		dbRestriction = s;
	}

	long GetServerIndex() const
	{
		return serverIndex;
	}
	void iSetServerIndex(long l)
	{
		serverIndex = l;
	}
	wxString GetFullName();
	wxString GetFullIdentifier();
	int GetPort() const
	{
		return port;
	}
	int GetSSL() const
	{
		return ssl;
	}
	bool GetConnected() const
	{
		return connected;
	}
	void iSetDatabase(const wxString &newVal)
	{
		database = newVal;
	}
	void iSetPort(int newval)
	{
		port = newval;
	}
	void iSetSSL(int newval)
	{
		ssl = newval;
	}
	void iSetUsername(const wxString &newVal)
	{
		username = newVal;
	}
	void iSetPassword(const wxString &newVal)
	{
		password = newVal;
	}
	void iSetStorePwd(const bool b)
	{
		storePwd = b;
	}
	void iSetRestore(const bool b)
	{
		restore = b;
	}
	bool SetPassword(const wxString &newVal);
	wxDateTime GetUpSince()
	{
		return upSince;
	}
	void iSetUpSince(const wxDateTime &d)
	{
		upSince = d;
	}
	void iSetColour(const wxString &s)
	{
		colour = s;
	}
	wxString GetColour()
	{
		return colour;
	}

	void iSetGroup(const wxString &s)
	{
		group = s;
	}
	wxString GetGroup()
	{
		return group;
	}

	bool HasPrivilege(const wxString &objTyp, const wxString &objName, const wxString &priv)
	{
		return conn->HasPrivilege(objTyp, objName, priv);
	}
	bool ExecuteVoid(const wxString &sql)
	{
		return conn->ExecuteVoid(sql);
	}
	wxString ExecuteScalar(const wxString &sql)
	{
		return conn->ExecuteScalar(sql);
	}
	pgSet *ExecuteSet(const wxString &sql)
	{
		return conn->ExecuteSet(sql);
	}
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowHint(frmMain *form, bool force);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/managing-databases");
	}
	wxMenu *GetNewMenu();

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
	{
		return true;
	}
	bool GetCanHint();
	bool CanEdit()
	{
		return true;
	}
	bool CanDrop()
	{
		return true;
	}
	bool CanBackupGlobals()
	{
		return true;
	}
	bool HasStats()
	{
		return true;
	}
	bool HasDepends()
	{
		return false;
	}
	bool HasReferences()
	{
		return false;
	}

	pgConn *connection()
	{
		return conn;
	}

	void ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &where = wxEmptyString);
	void ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where = wxEmptyString);

private:
	wxString passwordFilename();

	pgConn *conn;
	long serverIndex;
	bool connected, passwordValid, autovacuumRunning;
	wxString database, username, password, ver, error;
	wxString lastDatabase, lastSchema, description, serviceId, discoveryId;
	wxDateTime upSince;
	int port, ssl;
	bool storePwd, restore, discovered, createPrivilege, superUser, createRole;
	OID lastSystemOID;
	OID dbOid;
	wxString versionNum;
	wxString dbRestriction;
	wxString colour;
	wxString group;

	bool inRecovery;
	wxString receiveLoc, replayLoc;
	wxDateTime confLoadedSince;

#ifdef WIN32
	SC_HANDLE scmHandle;
	SC_HANDLE serviceHandle;
	wxArrayString GetDependentServices(SC_HANDLE handle);
#endif
};


// collection of pgServer
class pgServerCollection : public pgCollection
{
public:
	pgServerCollection(pgaFactory *factory);
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0) {};
};


///////////////////////////////////////////////


class pgServerObjFactory : public pgaFactory
{
public:
	pgServerObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, const char **img, const char **imgSm = 0)
		: pgaFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};


// Object that lives under a server
class pgServerObject : public pgObject
{
public:
	pgServerObject(pgaFactory &factory, const wxString &newName = wxEmptyString) : pgObject(factory, newName) {}
	pgServerObject(int newType, const wxString &newName) : pgObject(newType, newName) {}

	void iSetServer(pgServer *s)
	{
		server = s;
	}
	pgServer *GetServer() const
	{
		return server;
	}

	void FillOwned(ctlTree *browser, ctlListView *referencedBy, const wxArrayString &dblist, const wxString &query);

	bool CanCreate();
	bool CanDrop();
	bool CanEdit()
	{
		return true;
	}

protected:
	pgServer *server;
};


// collection of pgServerObject
class pgServerObjCollection : public pgCollection
{
public:
	pgServerObjCollection(pgaFactory *factory, pgServer *server);
	bool CanCreate();
};

class addServerFactory : public actionFactory
{
public:
	addServerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class startServiceFactory : public contextActionFactory
{
public:
	startServiceFactory (menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class stopServiceFactory : public contextActionFactory
{
public:
	stopServiceFactory (menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class connectServerFactory : public contextActionFactory
{
public:
	connectServerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class disconnectServerFactory : public contextActionFactory
{
public:
	disconnectServerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class reloadconfServiceFactory : public contextActionFactory
{
public:
	reloadconfServiceFactory (menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
