//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgServer.h - PostgreSQL Server
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSERVER_H
#define PGSERVER_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
// Class declarations


class pgServer : public pgObject
{
public:
    pgServer(const wxString& newServer = wxT(""), const wxString& newDatabase = wxT(""), const wxString& newUsername = wxT(""), int newPort = 5432, bool trusted=false);
    ~pgServer();
    int GetType() const { return PG_SERVER; }
    wxString GetTypeName() const { return wxT("Server"); }
    int Connect(wxFrame *form, bool lockFields = FALSE);
    wxString GetIdentifier() const;
    wxString GetVersionString();
    float GetVersionNumber();
    OID GetLastSystemOID();
    wxString GetDatabase() const { return database; }
    wxString GetUsername() const { return username; }
    wxString GetPassword() const { return password; }
    bool GetTrusted() const { return trusted; }
    wxString GetLastError() const;

    wxString GetLastDatabase() const { return lastDatabase; }
    void iSetLastDatabase(const wxString& s) { lastDatabase=s; }
    wxString GetLastSchema() const { return lastSchema; }
    void iSetLastSchema(const wxString& s) { lastSchema=s; }
    wxString GetDescription() const { return description; }
    void iSetDescription(const wxString& s) { description=s; }

    wxString GetFullName() const;
    int GetPort() const { return port; }
    bool GetConnected() const { return connected; }
    void iSetDatabase(const wxString& newVal) { database = newVal; }
    void iSetUsername(const wxString& newVal) { username = newVal; }
    void iSetPassword(const wxString& newVal) { password = newVal; }
    void iSetTrusted(const bool b) { trusted=b; }
    bool SetPassword(const wxString& newVal);

    void iSetPort(int newVal) { port = newVal; }
    bool ExecuteVoid(const wxString& sql) { return conn->ExecuteVoid(sql); }
    wxString ExecuteScalar(const wxString& sql) { return conn->ExecuteScalar(sql); }
    pgSet *ExecuteSet(const wxString& sql) { 
		return conn->ExecuteSet(sql); }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    wxString GetHelpPage(bool forCreate) const { return wxT("managing-databases.html"); }
    int GetIcon() { return PGICON_SERVER; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser) { return true; }
    bool CanDrop() { return true; }

    pgConn *connection() { return conn; }
    
private:
    pgConn *conn;
    bool connected;
    wxString database, username, password, ver, error;
    wxString lastDatabase, lastSchema, description;
    int port;
    bool trusted;
    OID lastSystemOID;
    float versionNum;
    wxFrame *parentWin;
};

#endif
