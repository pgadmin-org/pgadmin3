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

// App headers
#include "../../pgAdmin3.h"
#include "../../ui/forms/frmConnect.h"
#include "pgServer.h"
#include "pgObject.h"


pgServer::pgServer(wxFrame *parent)
: pgObject()
{  
    wxLogDebug(wxT("Creating a pgServer object"));
    winParent = parent;
}

pgServer::~pgServer()
{
    wxLogDebug(wxT("Destroying a pgServer object"));
}

int pgServer::GetType()
{
    return PG_SERVER;
}

wxString pgServer::GetTypeName()
{
    return wxString("Server");
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
    iPort = winConnect->GetPort();

    wxLogDebug(wxT("Attempting to create a connection object..."));
    cnMaster = new pgConn(szServer, szDatabase, szUsername, szPassword, iPort);

    delete winConnect;
    return cnMaster->GetStatus();
}

wxString pgServer::GetIdentifier()
{
    wxString szID;
    szID.Printf(wxT("%s:%d"), szServer, iPort);
    return wxString(szID);
}

wxString pgServer::GetServerVersion()
{
    if (szVer.IsEmpty()) {
      szVer = cnMaster->GetServerVersion();
    }
    return szVer;
}

wxString pgServer::GetServer()
{
    return szServer;
}
void pgServer::SetServer(const wxString& szNewVal)
{
    szServer = szNewVal;
}

wxString pgServer::GetDatabase()
{
    return szDatabase;
}
void pgServer::SetDatabase(const wxString& szNewVal)
{
    szDatabase = szNewVal;
}

wxString pgServer::GetUsername()
{
    return szUsername;
}
void pgServer::SetUsername(const wxString& szNewVal)
{
    szUsername = szNewVal;
}

wxString pgServer::GetPassword()
{
    return szPassword;
}
void pgServer::SetPassword(const wxString& szNewVal)
{
    szPassword = szNewVal;
}

int pgServer::GetPort()
{
    return iPort;
}
void pgServer::SetPort(int iNewVal)
{
    iPort = iNewVal;
}
