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
#include <wx/treectrl.h>

// App headers
#include "../../pgAdmin3.h"
#include "../../db/pg/pgConn.h"
#include "pgObject.h"

// Class declarations
class pgServer : public pgObject
{
public:
    pgServer(const wxString& szNewServer = wxString(""), const wxString& szNewDatabase = wxString(""), const wxString& szNewUsername = wxString(""), int iNewPort = 5432);
    ~pgServer();
    int GetType();
    wxString GetTypeName() const;
    int Connect(bool bLockFields = FALSE);
    wxString GetIdentifier() const;
    wxString GetServerVersion();
    wxString GetLastSystemOID();
    wxString GetServer() const;
    wxString GetDatabase() const;
    wxString GetUsername() const;
    wxString GetPassword() const;
    wxString GetLastError() const;
    int GetPort();
    bool GetConnected();
    void iSetServer(const wxString& szNewVal);
    void iSetDatabase(const wxString& szNewVal);
    void iSetUsername(const wxString& szNewVal);
    void iSetPassword(const wxString& szNewVal);
    void iSetPort(int iNewVal);
    int ExecuteVoid(const wxString& szSQL);
    wxString ExecuteScalar(const wxString& szSQL) const;
    pgSet ExecuteSet(const wxString& szSQL);

private:
    pgConn *cnMaster;
    bool bConnected;
    wxString szServer, szDatabase, szUsername, szPassword, szVer, szLastSystemOID;
    int iPort;
    wxFrame *winParent;
};

#endif