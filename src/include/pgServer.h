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
    int GetType();
    wxString GetTypeName() const;
    int Connect(bool lockFields = FALSE);
    wxString GetIdentifier() const;
    wxString GetVersionString();
    float GetVersionNumber();
    double GetLastSystemOID();
    wxString GetDatabase() const;
    wxString GetUsername() const;
    wxString GetPassword() const;
    wxString GetLastError() const;
    int GetPort();
    bool GetConnected();
    void iSetDatabase(const wxString& newVal);
    void iSetUsername(const wxString& newVal);
    void iSetPassword(const wxString& newVal);
    bool SetPassword(const wxString& newVal);
    void iSetPort(int newVal);
    int ExecuteVoid(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql) const;
    pgSet ExecuteSet(const wxString& sql);

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