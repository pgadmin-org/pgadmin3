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
#include "pgAdmin3.h"
#include "frmConnect.h"
#include "pgServer.h"
#include "pgObject.h"


pgServer::pgServer(const wxString& szNewName, const wxString& szNewDatabase, const wxString& szNewUsername, int iNewPort)
: pgObject()
{  
    wxLogInfo(wxT("Creating a pgServer object"));

    vCtor(PG_SERVER, szNewName);
    szDatabase = szNewDatabase;
    szUsername = szNewUsername;
    iPort = iNewPort;

    bConnected = FALSE;
    fVer = 0.0;
    dLastSystemOID = 0;
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

int pgServer::Connect(bool bLockFields) {

    wxLogInfo(wxT("Getting connection details..."));
    frmConnect *winConnect = new frmConnect(this, this->GetName(), szDatabase, szUsername, iPort);
    if (bLockFields) winConnect->LockFields();

    if (winConnect->ShowModal() != 0) {
        delete winConnect;
        return PGCONN_ABORTED;
    }

    wxLogInfo(wxT("Attempting to create a connection object..."));
    StartMsg(wxT("Connecting to database"));
    cnMaster = new pgConn(this->GetName(), szDatabase, szUsername, szPassword, iPort);

    delete winConnect;
    EndMsg();
    int iStatus = cnMaster->GetStatus();
    if (iStatus == PGCONN_OK) {

        // Check the server version
        if (cnMaster->GetVersionNumber() >= SERVER_MIN_VERSION) {
            bConnected = TRUE;
        } else {
            szError.Printf(wxT("The PostgreSQL server must be at least version %1.1f!"), SERVER_MIN_VERSION);
            bConnected = FALSE;
            return PGCONN_BAD;
        }

    } else {
        bConnected = FALSE;
    }

    return iStatus;
}

wxString pgServer::GetIdentifier() const
{
    wxString szID;
    szID.Printf(wxT("%s:%d"), GetName().c_str(), iPort);
    return wxString(szID);
}

wxString pgServer::GetVersionString()
{
    if (bConnected) {
      if (szVer.IsEmpty()) {
          szVer = wxString(cnMaster->GetVersionString());
      }
      return szVer;
    } else {
        return wxString("");
    }
}

float pgServer::GetVersionNumber()
{
    if (bConnected) {
      if (fVer == 0) {
          fVer = cnMaster->GetVersionNumber();
      }
      return fVer;
    } else {
        return 0.0;
    }
}

double pgServer::GetLastSystemOID()
{
    if (bConnected) {
      if (dLastSystemOID == 0) {
          dLastSystemOID = cnMaster->GetLastSystemOID();
      }
      return dLastSystemOID;
    } else {
        return 0;
    }
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

wxString pgServer::GetLastError() const
{
    wxString szMsg;
    if (szError != wxT("")) {
        if (cnMaster->GetLastError() != wxT("")) {
            szMsg.Printf(wxT("%s\n%s"), szError.c_str(), cnMaster->GetLastError().c_str());
        } else {
            szMsg.Printf(wxT("%s"), szError.c_str());
        }
    } else {
        szMsg.Printf(wxT("%s"), cnMaster->GetLastError().c_str());
    }
    return szMsg;
}

bool pgServer::GetConnected()
{
    return bConnected;
}

int pgServer::ExecuteVoid(const wxString& szSQL)
{
    return cnMaster->ExecuteVoid(szSQL);
}

wxString pgServer::ExecuteScalar(const wxString& szSQL) const
{
    return cnMaster->ExecuteScalar(szSQL);
}

pgSet pgServer::ExecuteSet(const wxString& szSQL)
{
    return *cnMaster->ExecuteSet(szSQL);
}