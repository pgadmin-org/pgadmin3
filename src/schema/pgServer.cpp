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
#include "misc.h"
#include "frmConnect.h"
#include "pgServer.h"
#include "pgObject.h"


pgServer::pgServer(const wxString& newName, const wxString& newDatabase, const wxString& newUsername, int newPort)
: pgObject(PG_SERVER, newName)
{  
    wxLogInfo(wxT("Creating a pgServer object"));

    database = newDatabase;
    username = newUsername;
    port = newPort;

    connected = FALSE;
    versionNum = 0.0;
    lastSystemOID = 0;
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

int pgServer::Connect(bool lockFields) {

    wxLogInfo(wxT("Getting connection details..."));
    frmConnect *winConnect = new frmConnect(this, this->GetName(), database, username, port);
    if (!winConnect) {
        wxLogError(wxT("Couldn't create a connection dialogue!"));
        return PGCONN_BAD;
    }

    if (lockFields) winConnect->LockFields();

    if (winConnect->ShowModal() != 0) {
        delete winConnect;
        return PGCONN_ABORTED;
    }

    wxLogInfo(wxT("Attempting to create a connection object..."));
    StartMsg(wxT("Connecting to database"));
    conn = new pgConn(this->GetName(), database, username, password, port);
    if (!conn) {
        wxLogError(wxT("Couldn't create a connection object!"));
        return PGCONN_BAD;
    }

    delete winConnect;
    EndMsg();
    int status = conn->GetStatus();
    if (status == PGCONN_OK) {

        // Check the server version
        if (conn->GetVersionNumber() >= SERVER_MIN_VERSION) {
            connected = TRUE;
        } else {
            error.Printf(wxT("The PostgreSQL server must be at least version %1.1f!"), SERVER_MIN_VERSION);
            connected = FALSE;
            return PGCONN_BAD;
        }

    } else {
        connected = FALSE;
    }

    return status;
}

wxString pgServer::GetIdentifier() const
{
    wxString id;
    id.Printf(wxT("%s:%d"), GetName().c_str(), port);
    return wxString(id);
}

wxString pgServer::GetVersionString()
{
    if (connected) {
      if (ver.IsEmpty()) {
          ver = wxString(conn->GetVersionString());
      }
      return ver;
    } else {
        return wxString("");
    }
}

float pgServer::GetVersionNumber()
{
    if (connected) {
      if (versionNum == 0) {
          versionNum = conn->GetVersionNumber();
      }
      return versionNum;
    } else {
        return 0.0;
    }
}

double pgServer::GetLastSystemOID()
{
    if (connected) {
      if (lastSystemOID == 0) {
          lastSystemOID = conn->GetLastSystemOID();
      }
      return lastSystemOID;
    } else {
        return 0;
    }
}

wxString pgServer::GetDatabase() const
{
    return database;
}
void pgServer::iSetDatabase(const wxString& newVal)
{
    database = newVal;
}

wxString pgServer::GetUsername() const
{
    return username;
}
void pgServer::iSetUsername(const wxString& newVal)
{
    username = newVal;
}

wxString pgServer::GetPassword() const
{
    return password;
}
void pgServer::iSetPassword(const wxString& newVal)
{
    password = newVal;
}
bool pgServer::SetPassword(const wxString& newVal)
{
    wxString sql;
    sql.Printf(wxT("ALTER USER %s WITH PASSWORD %s;"), qtIdent(username).c_str(), qtString(newVal).c_str());
    int x = conn->ExecuteVoid(sql);
    if (x == PGCONN_COMMAND_OK) {
        password = newVal;
        return TRUE;
    } else {
        return FALSE;
    }
}

int pgServer::GetPort()
{
    return port;
}
void pgServer::iSetPort(int newVal)
{
    port = newVal;
}

wxString pgServer::GetLastError() const
{
    wxString msg;
    if (error != wxT("")) {
        if (conn->GetLastError() != wxT("")) {
            msg.Printf(wxT("%s\n%s"), error.c_str(), conn->GetLastError().c_str());
        } else {
            msg.Printf(wxT("%s"), error.c_str());
        }
    } else {
        msg.Printf(wxT("%s"), conn->GetLastError().c_str());
    }
    return msg;
}

bool pgServer::GetConnected()
{
    return connected;
}

int pgServer::ExecuteVoid(const wxString& sql)
{
    return conn->ExecuteVoid(sql);
}

wxString pgServer::ExecuteScalar(const wxString& sql) const
{
    return conn->ExecuteScalar(sql);
}

pgSet pgServer::ExecuteSet(const wxString& sql)
{
    return *conn->ExecuteSet(sql);
}