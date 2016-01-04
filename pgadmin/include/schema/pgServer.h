//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
class pgServer;

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
#define DEFAULT_SSH_PORT  22

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
class CSSHTunnelThread;
#endif

class pgServer : public pgObject
{
public:
	pgServer(const wxString &newServer = wxT(""), const wxString &newHostAddr = wxT(""), const wxString &newDescription = wxT(""),
	         const wxString &newService = wxT(""), const wxString &newDatabase = wxT(""), const wxString &newUsername = wxT(""), int newPort = 5432,
	         bool storePwd = false, const wxString &newRolename = wxT(""), bool restore = true, int sslMode = 0,
	         const wxString &colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX), const wxString &group = wxEmptyString,
	         bool sshTunnel = false, const wxString &newTunnelHost = wxEmptyString, const wxString &newTunnelUserName = wxEmptyString, bool authModePwd = true,
	         const wxString &newTunnelPassword = wxEmptyString, const wxString &newPublicKey = wxEmptyString, const wxString &newIdentityFile = wxEmptyString,
	         const int &sshPort = DEFAULT_SSH_PORT);
	~pgServer();
	int GetIconId();

	wxString GetTypeName() const
	{
		return wxT("Server");
	}
	wxString GetTranslatedMessage(int kindOfMessage) const;
	int Connect(frmMain *form, bool askPassword = true, const wxString &pwd = wxEmptyString, bool forceStorePassword = false, bool askTunnelPassword = false);
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
	bool IsReplayPaused();
	bool PauseReplay();
	bool ResumeReplay();
	bool AddNamedRestorePoint();

	pgServer *GetServer() const;

	wxString GetIdentifier() const;
	wxString GetVersionString();
	wxString GetVersionNumber();
	OID GetLastSystemOID();
	wxString GetHostAddr() const
	{
		return hostaddr;
	}
	wxString GetService() const
	{
		return service;
	}
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
	wxString GetRolename() const
	{
		return rolename;
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
	bool GetReplayPaused() const
	{
		return replayPaused;
	}
	void SetReplayPaused(const bool b)
	{
		replayPaused = b;
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
	wxString GetReplayTimestamp() const
	{
		return replayTimestamp;
	}
	void iSetReplayTimestamp(const wxString &s)
	{
		replayTimestamp = s;
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
	void iSetHostAddr(const wxString &s)
	{
		hostaddr = s;
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
	void iSetService(const wxString &newVal)
	{
		service = newVal;
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
	void iSetRolename(const wxString &newVal)
	{
		rolename = newVal;
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

	wxString GetSSLCert() const
	{
		return sslcert;
	}
	void SetSSLCert(const wxString &s)
	{
		sslcert = s;
	}
	wxString GetSSLKey() const
	{
		return sslkey;
	}
	void SetSSLKey(const wxString &s)
	{
		sslkey = s;
	}
	wxString GetSSLRootCert() const
	{
		return sslrootcert;
	}
	void SetSSLRootCert(const wxString &s)
	{
		sslrootcert = s;
	}
	wxString GetSSLCrl() const
	{
		return sslcrl;
	}
	void SetSSLCrl(const wxString &s)
	{
		sslcrl = s;
	}

	bool GetSSLCompression() const
	{
		return sslcompression;
	}
	void iSetSSLCompression(const bool b)
	{
		sslcompression = b;
	}

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	//SSH Tunnel
	bool GetSSHTunnel() const
	{
		return sshTunnel;
	}
	void iSetSSHTunnel(const bool b)
	{
		sshTunnel = b;
	}
	bool GetAuthModePwd() const
	{
		return authModePwd;
	}
	void iSetAuthModePwd(const bool b)
	{
		authModePwd = b;
	}
	wxString GetLocalListenHost() const
	{
		return local_listenhost;
	}
	void SetLocalListenHost(const wxString &s)
	{
		local_listenhost = s;
	}
	int GetLocalListenPort() const
	{
		return local_listenport;
	}
	void SetLocalListenPort(int newVal)
	{
		local_listenport = newVal;
	}
	wxString GetTunnelHost() const
	{
		return tunnelHost;
	}
	void SetTunnelHost(const wxString &s)
	{
		tunnelHost = s;
	}
	wxString GetTunnelUserName() const
	{
		return tunnelUserName;
	}
	void SetTunnelUserName(const wxString &s)
	{
		tunnelUserName = s;
	}
	wxString GetTunnelPassword() const
	{
		return tunnelPassword;
	}
	void SetTunnelPassword(const wxString &s)
	{
		tunnelPassword = s;
	}
	wxString GetPublicKeyFile() const
	{
		return publicKeyFile;
	}
	void SetPublicKeyFile(const wxString &s)
	{
		publicKeyFile = s;
	}
	wxString GetIdentityFile() const
	{
		return identityFile;
	}
	void SetIdentityFile(const wxString &s)
	{
		identityFile = s;
	}
	int GetTunnelPort() const
	{
		return tunnelPort;
	}
	void iSetTunnelPort(const int newval)
	{
		tunnelPort = newval;
	}
#endif

	void ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &where = wxEmptyString);
	void ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where = wxEmptyString);

private:
	wxString passwordFilename();

	pgConn *conn;
	long serverIndex;
	bool connected, passwordValid, autovacuumRunning;
	wxString service, hostaddr, database, username, password, rolename, ver, error;
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
	wxString sslcert, sslkey, sslrootcert, sslcrl;
	bool sslcompression;
	bool sshTunnel;

	bool inRecovery, replayPaused;
	wxString receiveLoc, replayLoc, replayTimestamp;
	wxDateTime confLoadedSince;

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	bool createSSHTunnel();

	//SSH Tunnel
	CSSHTunnelThread *tunnelObj;
	bool authModePwd;
	int local_listenport;
	int tunnelPort;
	wxString tunnelHost, tunnelUserName, tunnelPassword, publicKeyFile, identityFile, local_listenhost;
#endif

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
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0) {};
};


///////////////////////////////////////////////


class pgServerObjFactory : public pgaFactory
{
public:
	pgServerObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
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

class pausereplayServiceFactory : public contextActionFactory
{
public:
	pausereplayServiceFactory (menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class resumereplayServiceFactory : public contextActionFactory
{
public:
	resumereplayServiceFactory (menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class addnamedrestorepointServiceFactory : public contextActionFactory
{
public:
	addnamedrestorepointServiceFactory (menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
