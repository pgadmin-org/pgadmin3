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
    pgServer *GetServer();
    void SetServer(pgServer *newServer);
    wxString GetPath() const;
    void iSetPath(const wxString& newVal);
    wxString GetEncoding() const;
    void iSetEncoding(const wxString& newVal);
    wxString GetVariables() const;
    void iSetVariables(const wxString& newVal);
    bool GetAllowConnections();
    void iSetAllowConnections(bool newVal);
    bool GetConnected();
    bool GetSystemObject();
    wxString GetSql() const;

    int Connect();

private:
    pgServer *server;
    pgConn *database;
    wxString path, encoding, variables;
    bool allowConnections, connected;
};

#endif
