//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#include "pgConn.h"

// Class declarations
class pgDatabase : public pgServerObject
{
public:
    pgDatabase(const wxString& newName = wxT(""));
    ~pgDatabase();

    int GetIcon() { return PGICON_DATABASE; }
    pgDatabase *GetDatabase() const { return (pgDatabase*)this; }
    bool BackendMinimumVersion(int major, int minor) { return connection()->BackendMinimumVersion(major, minor); }

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));
    static void ShowStatistics(pgCollection *collection, ctlListView *statistics);
    
    pgSet *ExecuteSet(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql);
    bool ExecuteVoid(const wxString& sql);
    void UpdateDefaultSchema();

    pgConn *CreateConn() { return server->CreateConn(GetName()); }

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
    
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
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
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

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

#endif
