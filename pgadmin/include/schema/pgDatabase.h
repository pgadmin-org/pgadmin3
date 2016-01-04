//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgDatabase.h - PostgreSQL Database
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGDATABASE_H
#define PGDATABASE_H

#include "pgServer.h"

class pgDatabaseFactory : public pgServerObjFactory
{
public:
	pgDatabaseFactory();
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

extern pgDatabaseFactory databaseFactory;


// Class declarations
class pgDatabase : public pgServerObject
{
public:
	pgDatabase(const wxString &newName = wxT(""));
	~pgDatabase();
	wxString GetTranslatedMessage(int kindOfMessage) const;
	int GetIconId();

	static wxString GetDefaultPrivileges(const wxChar &cType, wxString strDefPrivs, const wxString &strSchema);

	pgDatabase *GetDatabase() const
	{
		return (pgDatabase *)this;
	}
	bool BackendMinimumVersion(int major, int minor)
	{
		return connection()->BackendMinimumVersion(major, minor);
	}
	bool BackendMinimumVersion(int major, int minor, int patch)
	{
		return connection()->BackendMinimumVersion(major, minor, patch);
	}

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowHint(frmMain *form, bool force);
	void ShowStatistics(frmMain *form, ctlListView *statistics);

	pgSet *ExecuteSet(const wxString &sql);
	wxString ExecuteScalar(const wxString &sql);
	bool ExecuteVoid(const wxString &sql, bool reportError = true);
	void UpdateDefaultSchema();

	pgConn *CreateConn(const wxString &applicationname)
	{
		return server->CreateConn(GetName(), GetOid(), applicationname);
	}

	wxString GetPrettyOption() const
	{
		return prettyOption;
	}

	bool GetCreatePrivilege() const
	{
		return createPrivilege;
	}
	void iSetCreatePrivilege(const bool b)
	{
		createPrivilege = b;
	}

	// Default Privileges on ALL
	wxString GetDefPrivsOnTables()
	{
		return m_defPrivsOnTables;
	}
	wxString GetDefPrivsOnSequences()
	{
		return m_defPrivsOnSeqs;
	}
	wxString GetDefPrivsOnFunctions()
	{
		return m_defPrivsOnFuncs;
	}
	wxString GetDefPrivsOnTypes()
	{
		return m_defPrivsOnTypes;
	}

	wxString GetPath() const
	{
		return path;
	};
	void iSetPath(const wxString &newVal)
	{
		path = newVal;
	}
	wxString GetTablespace() const
	{
		return tablespace;
	};
	void iSetTablespace(const wxString &newVal)
	{
		tablespace = newVal;
	}
	OID GetTablespaceOid() const
	{
		return tablespaceOid;
	};
	void iSetTablespaceOid(const OID newVal)
	{
		tablespaceOid = newVal;
	}
	wxString GetDefaultTablespace() const
	{
		return defaultTablespace;
	};
	void iSetDefaultTablespace(const wxString &newVal)
	{
		defaultTablespace = newVal;
	}
	wxString GetEncoding() const
	{
		return encoding;
	}
	void iSetEncoding(const wxString &newVal)
	{
		encoding = newVal;
	}
	wxString GetSchemaRestriction()
	{
		return schemaRestriction;
	}
	void iSetSchemaRestriction(const wxString &s)
	{
		schemaRestriction = s;
	}
	wxString GetCollate() const
	{
		return collate;
	}
	void iSetCollate( const wxString &newVal)
	{
		collate = newVal;
	}
	wxString GetCType() const
	{
		return ctype;
	}
	void iSetCType( const wxString &newVal)
	{
		ctype = newVal;
	}
	long GetConnectionLimit()
	{
		return connectionLimit;
	}
	void iSetConnectionLimit(long newVal)
	{
		connectionLimit = newVal;
	}

	wxArrayString &GetVariables()
	{
		return variables;
	}
	bool GetAllowConnections() const
	{
		return allowConnections;
	}
	void iSetAllowConnections(bool newVal)
	{
		allowConnections = newVal;
	}
	wxString GetSearchPath() const
	{
		return searchPath;
	}
	wxString GetSchemaPrefix(const wxString &schemaname) const;
	wxString GetQuotedSchemaPrefix(const wxString &schemaname) const;
	wxString GetDefaultSchema()
	{
		return defaultSchema;
	};
	bool GetConnected()
	{
		return connected;
	}
	bool GetSystemObject() const;
	long GetMissingFKs() const
	{
		return missingFKs;
	}
	wxArrayString GetSlonyClusters(ctlTree *browser);



	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	bool CanCreate();
	bool CanMaintenance()
	{
		return connected;
	}
	bool CanBackup()
	{
		return connected;
	}
	bool CanRestore()
	{
		return connected;
	}
	bool GetCanHint();
	bool RequireDropConfirm()
	{
		return true;
	}
	pgConn *connection();
	int Connect();
	void Disconnect();
	void CheckAlive();
	void AppendSchemaChange(const wxString &sql);
	wxString GetSchemaChanges()
	{
		return schemaChanges;
	}
	void ClearSchemaChanges()
	{
		schemaChanges = wxEmptyString;
	}

	wxMenu *GetNewMenu();
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}

	bool CanDebugPlpgsql();
	bool CanDebugEdbspl();

private:
	pgConn *conn;
	bool connected;
	bool useServerConnection;
	wxString searchPath, path, tablespace, defaultTablespace, encoding, collate, ctype;
	wxString prettyOption, defaultSchema;
	bool allowConnections, createPrivilege;
	long missingFKs;
	long connectionLimit;
	wxArrayString variables;

	wxString schemaChanges;
	wxString schemaRestriction;
	wxString m_defPrivsOnTables, m_defPrivsOnSeqs, m_defPrivsOnFuncs, m_defPrivsOnTypes;

	int canDebugPlpgsql, canDebugEdbspl;

	OID tablespaceOid;
};

class pgDatabaseCollection : public pgServerObjCollection
{
public:
	pgDatabaseCollection(pgaFactory *factory, pgServer *sv);
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowStatistics(frmMain *form, ctlListView *statistics);
};


////////////////////////////////////////////////

class pgDatabaseObjFactory : public pgServerObjFactory
{
public:
	pgDatabaseObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
		: pgServerObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};

class disconnectDatabaseFactory : public contextActionFactory
{
public:
	disconnectDatabaseFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


// Object that lives in a database
class pgDatabaseObject : public pgObject
{
public:
	pgDatabaseObject(pgaFactory &factory, const wxString &newName = wxEmptyString) : pgObject(factory, newName) {}
	pgDatabaseObject(int newType, const wxString &newName) : pgObject(newType, newName) {}

	void iSetDatabase(pgDatabase *newDatabase)
	{
		database = newDatabase;
	}
	pgDatabase *GetDatabase() const
	{
		return database;
	}
	pgServer *GetServer() const;

	void DisplayStatistics(ctlListView *statistics, const wxString &query);

	// compiles a prefix from the schema name with '.', if necessary
	wxString GetSchemaPrefix(const wxString &schemaname) const;
	wxString GetQuotedSchemaPrefix(const wxString &schemaname) const;

	bool CanDrop();
	bool CanEdit()
	{
		return true;
	}
	bool CanCreate();

protected:
	pgDatabase *database;
};


// collection of pgDatabaseObject
class pgDatabaseObjCollection : public pgCollection
{
public:
	pgDatabaseObjCollection(pgaFactory *factory, pgDatabase *db);
	bool CanCreate();
};



#endif
