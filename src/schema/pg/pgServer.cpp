//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgServer.cpp - PostgreSQL Server
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>

// App headers
#include "../../pgAdmin3.h"
#include "../../ui/forms/frmConnect.h"
#include "pgServer.h"


pgServer::pgServer(wxFrame *parent)
: wxTreeItemData()
{  
    wxLogDebug(wxT("Creating a pgServer object"));
    winParent = parent;
}

pgServer::~pgServer()
{
    wxLogDebug(wxT("Destroying a pgServer object"));
}

int pgServer::Connect() {

    wxLogInfo(wxT("Getting connection details..."));
    frmConnect *winConnect = new frmConnect(winParent);
    winConnect->Show(TRUE);

    if (winConnect->GetCancelled()) {
        delete winConnect;
        return PGCONN_ABORTED;
    }

    szServer = winConnect->GetServer();
    szDatabase = winConnect->GetDatabase();
    szUsername = winConnect->GetUsername();
    szPassword = winConnect->GetPassword();
    lPort = winConnect->GetPort();

    wxLogDebug(wxT("Attempting to create a connection object..."));
    cnMaster = new pgConn(szServer, szDatabase, szUsername, szPassword, lPort);

    delete winConnect;
    return cnMaster->GetStatus();
}

wxString pgServer::GetIdentifier()
{
    wxString szID;
    szID.Printf(wxT("%s:%d"), szServer, lPort);
    return wxString(szID);
}

wxString pgServer::GetServer()
{
    return szServer;
}
void pgServer::SetServer(wxString& szNewVal)
{
    szServer = szNewVal;
}

wxString pgServer::GetDatabase()
{
    return szDatabase;
}
void pgServer::SetDatabase(wxString& szNewVal)
{
    szDatabase = szNewVal;
}

wxString pgServer::GetUsername()
{
    return szUsername;
}
void pgServer::SetUsername(wxString& szNewVal)
{
    szUsername = szNewVal;
}

wxString pgServer::GetPassword()
{
    return szPassword;
}
void pgServer::SetPassword(wxString& szNewVal)
{
    szPassword = szNewVal;
}

long pgServer::GetPort()
{
    return lPort;
}
void pgServer::SetPort(long lNewVal)
{
    lPort = lNewVal;
}
