//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
class pgDatabase : public pgObject
{
public:
    pgDatabase(const wxString& newName = wxString(""));
    ~pgDatabase();
    pgServer *GetServer() const { return server; }
    void SetServer(pgServer *newServer) { server = newServer; }
    wxString GetPath() const { return path; };
    void iSetPath(const wxString& newVal) { path = newVal; }
    wxString GetEncoding() const { return encoding; }
    void iSetEncoding(const wxString& newVal) { encoding = newVal; }
    wxString GetVariables() const { return variables; }
    void iSetVariables(const wxString& newVal) { variables = newVal; }
    bool GetAllowConnections() const { return allowConnections; }
    void iSetAllowConnections(bool newVal) { allowConnections = newVal; }
    bool GetConnected() const { return connected; }
    bool GetSystemObject() const;
    wxString GetSql() const;

    int Connect();

private:
    pgServer *server;
    pgConn *database;
    wxString path, encoding, variables;
    bool allowConnections, connected;
};

#endif
