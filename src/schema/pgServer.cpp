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

	// Keith 2003.03.05
	// Because we need to delete it later
	conn = NULL;

}

pgServer::~pgServer()
{
	// Keith 2003.03.05
	// This was not being deleted and was causing memory leaksd
	if (conn)
		delete conn;

    wxLogInfo(wxT("Destroying a pgServer object"));
}

int pgServer::Connect(bool lockFields) 
{
    wxLogInfo(wxT("Getting connection details..."));

	// Keith 2003.03.05
	// It's simpler to use a reference for modal dialogs
    frmConnect winConnect(this, this->GetName(), database, username, port);

    if (lockFields) 
		winConnect.LockFields();

	switch (winConnect.ShowModal()) {
		case wxID_OK:
			break;
		case wxID_CANCEL:
	        return PGCONN_ABORTED;
		default:
	        wxLogError(wxT("Couldn't create a connection dialogue!"));
		    return PGCONN_BAD;
	}

    wxLogInfo(wxT("Attempting to create a connection object..."));
	StartMsg(wxT("Connecting to database"));
    conn = new pgConn(this->GetName(), database, username, password, port);   
	if (!conn) {
        wxLogError(wxT("Couldn't create a connection object!"));
        return PGCONN_BAD;
    }

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
