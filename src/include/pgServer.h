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
class pgServer : public pgObject
{
public:
    pgServer(const wxString& newServer = wxString(""), const wxString& newDatabase = wxString(""), const wxString& newUsername = wxString(""), int newPort = 5432);
    ~pgServer();
    int GetType() const { return PG_SERVER; }
    wxString GetTypeName() const { return wxString("Server"); }
    int Connect(bool lockFields = FALSE);
    wxString GetIdentifier() const;
    wxString GetVersionString();
    float GetVersionNumber();
    double GetLastSystemOID();
    wxString GetDatabase() const { return database; }
    wxString GetUsername() const { return username; }
    wxString GetPassword() const { return password; }
    wxString GetLastError() const;
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

private:
    pgConn *conn;
    bool connected;
    wxString database, username, password, ver, error;
    int port;
    double lastSystemOID;
    float versionNum;
    wxFrame *parentWin;
};

#endif