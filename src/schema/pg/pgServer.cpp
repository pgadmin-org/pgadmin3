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


pgServer::pgServer(const wxString& szNewServer, const wxString& szNewDatabase, const wxString& szNewUsername, int iNewPort)
: pgObject()
{  
    wxLogInfo(wxT("Creating a pgServer object"));
    szServer = szNewServer;
    szDatabase = szNewDatabase;
    szUsername = szNewUsername;
    iPort = iNewPort;
}

pgServer::~pgServer()
{
    wxLogInfo(wxT("Destroying a pgServer object"));
}

int pgServer::GetType()
{
    return PG_SERVER;
}

wxString pgServer::GetTypeName() const
{
    return wxString("Server");
}

int pgServer::Connect() {

    wxLogInfo(wxT("Getting connection details..."));
    frmConnect *winConnect = new frmConnect(this, szServer, szDatabase, szUsername, iPort);

    if (winConnect->ShowModal() != 0) {
        delete winConnect;
        return PGCONN_ABORTED;
    }

    wxLogInfo(wxT("Attempting to create a connection object..."));
    StartMsg(wxT("Connecting to database"));
    cnMaster = new pgConn(szServer, szDatabase, szUsername, szPassword, iPort);

    delete winConnect;
    EndMsg();
    return cnMaster->GetStatus();
}

wxString pgServer::GetIdentifier() const
{
    wxString szID;
    szID.Printf(wxT("%s:%d"), szServer.c_str(), iPort);
    return wxString(szID);
}

wxString pgServer::GetServerVersion()
{
    if (szVer.IsEmpty()) {
        szVer = wxString(cnMaster->GetServerVersion());
    }
    return szVer;
}

wxString pgServer::GetServer() const
{
    return szServer;
}
void pgServer::iSetServer(const wxString& szNewVal)
{
    szServer = szNewVal;
}

wxString pgServer::GetDatabase() const
{
    return szDatabase;
}
void pgServer::iSetDatabase(const wxString& szNewVal)
{
    szDatabase = szNewVal;
}

wxString pgServer::GetUsername() const
{
    return szUsername;
}
void pgServer::iSetUsername(const wxString& szNewVal)
{
    szUsername = szNewVal;
}

wxString pgServer::GetPassword() const
{
    return szPassword;
}
void pgServer::iSetPassword(const wxString& szNewVal)
{
    szPassword = szNewVal;
}

int pgServer::GetPort()
{
    return iPort;
}
void pgServer::iSetPort(int iNewVal)
{
    iPort = iNewVal;
}
