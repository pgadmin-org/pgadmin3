//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

class pgServers: public pgObject
{
public:
    pgServers() : pgObject(PG_SERVERS, wxString("Servers")) {}
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0) {};
};


class pgServer : public pgObject
{
public:
    pgServer(const wxString& newServer = wxString(""), const wxString& newDatabase = wxString(""), const wxString& newUsername = wxString(""), int newPort = 5432);
    ~pgServer();
    int GetType() const { return PG_SERVER; }
    wxString GetTypeName() const { return wxString("Server"); }
    int Connect(wxFrame *form, bool lockFields = FALSE);
    wxString GetIdentifier() const;
    wxString GetVersionString();
    float GetVersionNumber();
    double GetLastSystemOID();
    wxString GetDatabase() const { return database; }
    wxString GetUsername() const { return username; }
    wxString GetPassword() const { return password; }
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
    bool SetPassword(const wxString& newVal);
    void iSetPort(int newVal) { port = newVal; }
    int ExecuteVoid(const wxString& sql) { return conn->ExecuteVoid(sql); }
    wxString ExecuteScalar(const wxString& sql) { return conn->ExecuteScalar(sql); }
    pgSet *ExecuteSet(const wxString& sql) { 
		return conn->ExecuteSet(sql); }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    
private:
    pgConn *conn;
    bool connected;
    wxString database, username, password, ver, error;
    wxString lastDatabase, lastSchema, description;
    int port;
    double lastSystemOID;
    float versionNum;
    wxFrame *parentWin;
};

#endif