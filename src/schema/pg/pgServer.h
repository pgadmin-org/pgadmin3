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

// Class declarations
class pgServer : public wxTreeItemData
{
public:
    pgServer(wxFrame *parent);
    ~pgServer();
    pgConn *cnMaster;
    int Connect();
    wxString GetIdentifier();
    wxString GetServer();
    void SetServer(wxString& szNewVal);
    wxString GetDatabase();
    void SetDatabase(wxString& szNewVal);
    wxString GetUsername();
    void SetUsername(wxString& szNewVal);
    wxString GetPassword();
    void SetPassword(wxString& szNewVal);
    long GetPort();
    void SetPort(long lNewVal);

private:
    wxString szServer, szDatabase, szUsername, szPassword;
    long lPort;
    wxFrame *winParent;
};

#endif
