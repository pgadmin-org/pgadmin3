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
    pgDatabase(const wxString& szNewName = wxString(""));
    ~pgDatabase();
    pgServer *GetServer();
    void SetServer(pgServer *objNewServer);
    wxString GetPath() const;
    void iSetPath(const wxString& szNewVal);
    wxString GetEncoding() const;
    void iSetEncoding(const wxString& szNewVal);
    wxString GetVariables() const;
    void iSetVariables(const wxString& szNewVal);
    bool GetAllowConnections();
    void iSetAllowConnections(bool bNewVal);
    bool GetConnected();
    bool GetSystemObject();
    wxString GetSql() const;

    int Connect();

private:
    pgServer *objServer;
    pgConn *cnDatabase;
    wxString szPath, szEncoding, szVariables;
    bool bAllowConnections, bConnected;
};

#endif
