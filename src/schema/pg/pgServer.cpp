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
    wxLogInfo(wxT("Creating a pgServer object"));
    winParent = parent;
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
    frmConnect *winConnect = new frmConnect(winParent);
    winConnect->Show(TRUE);

	wxLogInfo("If you can see this in the log, but no button has been pressed, then the dialogue wasn't modal!");
	
    if (winConnect->GetCancelled()) {
        delete winConnect;
        return PGCONN_ABORTED;
    }

    szServer = winConnect->GetServer();
    szDatabase = winConnect->GetDatabase();
    szUsername = winConnect->GetUsername();
    szPassword = winConnect->GetPassword();
    iPort = winConnect->GetPort();

    wxLogInfo(wxT("Attempting to create a connection object..."));
    cnMaster = new pgConn(szServer, szDatabase, szUsername, szPassword, iPort);

    delete winConnect;
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
void pgServer::SetServer(const wxString& szNewVal)
{
    szServer = szNewVal;
}

wxString pgServer::GetDatabase() const
{
    return szDatabase;
}
void pgServer::SetDatabase(const wxString& szNewVal)
{
    szDatabase = szNewVal;
}

wxString pgServer::GetUsername() const
{
    return szUsername;
}
void pgServer::SetUsername(const wxString& szNewVal)
{
    szUsername = szNewVal;
}

wxString pgServer::GetPassword() const
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
