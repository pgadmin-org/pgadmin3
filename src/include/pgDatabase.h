//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgDatabase.h - PostgreSQL Database
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGDATABASE_H
#define PGDATABASE_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgCollection.h"
#include "pgConn.h"

class pgDatabaseFactory : public pgServerObjFactory
{
public:
    pgDatabaseFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
    virtual pgCollection *CreateCollection(pgObject *obj);

    int GetClosedIconId() { return closedId; }
protected:
    int closedId;
};

extern pgDatabaseFactory databaseFactory;


// Class declarations
class pgDatabase : public pgServerObject
{
public:
    pgDatabase(const wxString& newName = wxT(""));
    ~pgDatabase();
    int GetIconId();

    pgDatabase *GetDatabase() const { return (pgDatabase*)this; }
    bool BackendMinimumVersion(int major, int minor) { return connection()->BackendMinimumVersion(major, minor); }

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    
    pgSet *ExecuteSet(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql);
    bool ExecuteVoid(const wxString& sql);
    void UpdateDefaultSchema();

    pgConn *CreateConn() { return server->CreateConn(GetName(), GetOid()); }

    wxString GetPrettyOption() const { return prettyOption; }

    bool GetCreatePrivilege() const { return createPrivilege; }
    void iSetCreatePrivilege(const bool b) { createPrivilege=b; }
    wxString GetPath() const { return path; };
    void iSetPath(const wxString& newVal) { path = newVal; }
    wxString GetTablespace() const { return tablespace; };
    void iSetTablespace(const wxString& newVal) { tablespace = newVal; }
    wxString GetEncoding() const { return encoding; }
    void iSetEncoding(const wxString& newVal) { encoding = newVal; }
    wxArrayString& GetVariables() { return variables; }
    bool GetAllowConnections() const { return allowConnections; }
    void iSetAllowConnections(bool newVal) { allowConnections = newVal; }
    wxString GetSearchPath() const { return searchPath; }
    wxString GetSchemaPrefix(const wxString &schemaname) const;
    wxString GetQuotedSchemaPrefix(const wxString &schemaname) const;
    bool GetConnected() { return connected; }
    bool GetSystemObject() const;
    long GetMissingFKs() const { return missingFKs; }
    
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    bool CanMaintenance() { return true; }
    bool CanBackup() { return connected; }
    bool CanRestore() { return connected; }
    bool RequireDropConfirm() { return true; }
    pgConn *connection();
    int Connect();
    void Disconnect();
    void CheckAlive();
    void AppendSchemaChange(const wxString &sql);
    wxString GetSchemaChanges() { return schemaChanges; }
    void ClearSchemaChanges() { schemaChanges=wxEmptyString; }

    wxMenu *GetNewMenu();
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    pgConn *conn;
    bool connected;
    bool useServerConnection;
    wxString searchPath, path, tablespace, encoding;
    wxString prettyOption, defaultSchema;
    bool allowConnections, createPrivilege;
    long missingFKs;
    wxArrayString variables;

    wxString schemaChanges;
};

class pgDatabaseCollection : public pgServerObjCollection
{
public:
    pgDatabaseCollection(pgaFactory *factory, pgServer *sv);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
};


class pgDatabaseObjCollection : public pgCollection
{
public:
    pgDatabaseObjCollection(pgaFactory *factory, pgDatabase *db);
    bool CanCreate();
};


class pgDatabaseObjFactory : public pgServerObjFactory
{
public:
    pgDatabaseObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img) : pgServerObjFactory(tn, ns, nls, img) {}
    virtual pgCollection *CreateCollection(pgObject *obj);
};

#endif
