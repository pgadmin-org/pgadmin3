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
#include "pgCollection.h"


pgServer::pgServer(const wxString& newName, const wxString& newDatabase, const wxString& newUsername, int newPort, bool _trusted)
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
    trusted=_trusted;

}

pgServer::~pgServer()
{
	// Keith 2003.03.05
	// This was not being deleted and was causing memory leaksd
	if (conn)
		delete conn;

    wxLogInfo(wxT("Destroying a pgServer object"));
}


wxString pgServer::GetFullName() const
{
    return GetDescription() + wxT("  (") + GetIdentifier() + wxT(")");
}



int pgServer::Connect(wxFrame *form, bool lockFields) 
{
    wxLogInfo(wxT("Attempting to create a connection object..."));

    if (!conn || conn->GetStatus() != PGCONN_OK)
    {
        if (!trusted)
        {
            frmConnect winConnect(form, GetName(), description, database, username, port, trusted);

            if (lockFields) 
		        winConnect.LockFields();

	        switch (winConnect.Go())
            {
		        case wxID_OK:
			        break;
		        case wxID_CANCEL:
	                return PGCONN_ABORTED;
		        default:
	                wxLogError(wxT("Couldn't create a connection dialogue!"));
		            return PGCONN_BAD;
	        }

            if (!lockFields)
            {
                iSetDescription(winConnect.GetDescription());
                iSetName(winConnect.GetServer());
                iSetDatabase(winConnect.GetDatabase());
                iSetUsername(winConnect.GetUsername());
                iSetPort(winConnect.GetPort());
                trusted=winConnect.GetTrusted();
            }
            if (!trusted)
                iSetPassword(winConnect.GetPassword());
        }
        if (password.IsNull())
            StartMsg(wxT("Connecting to database without password"));
        else
            StartMsg(wxT("Connecting to database"));
        conn = new pgConn(GetName(), database, username, password, port);   
        EndMsg();
        if (!conn) {
            wxLogError(wxT("Couldn't create a connection object!"));
            return PGCONN_BAD;
        }
    }
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
            msg=error;
        }
    } else {
        msg=conn->GetLastError();
    }
    return msg;
}



void pgServer::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    // Add child nodes if necessary
    if (GetConnected()) {

        // Reset password menu option
//        form->fileMenu->Enable(MNU_PASSWORD, TRUE);

        if (!expandedKids)
        {
            expandedKids=true;
            // Log
            
            wxLogInfo(wxT("Adding child object to server ") + GetIdentifier());
    
            // Databases
            pgCollection *collection = new pgCollection(PG_DATABASES);
            collection->SetServer(this);
            browser->AppendItem(GetId(), collection->GetTypeName(), 2, -1, collection);
      
            // Groups
            collection = new pgCollection(PG_GROUPS);
            collection->SetServer(this);
            browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_GROUP, -1, collection);
    
            // Users
            collection = new pgCollection(PG_USERS);
            collection->SetServer(this);
            browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_USER, -1, collection);
        }
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for server ") + GetIdentifier());

        // Add the properties view columns
        CreateListColumns(properties);

        // Display the Server properties
        int pos=0;
        InsertListItem(properties, pos++, wxT("Hostname"), GetName());
        InsertListItem(properties, pos++, wxT("Description"), GetDescription());
        InsertListItem(properties, pos++, wxT("Port"), (long)GetPort());
        InsertListItem(properties, pos++, wxT("Initial Database"), GetDatabase());
        InsertListItem(properties, pos++, wxT("Username"), GetUsername());
        InsertListItem(properties, pos++, wxT("Trusted?"), GetTrusted());
        if (GetConnected())
        {
            InsertListItem(properties, pos++, wxT("Version String"), GetVersionString());
            InsertListItem(properties, pos++, wxT("Version Number"), GetVersionNumber());
            InsertListItem(properties, pos++, wxT("Last System OID"), GetLastSystemOID());
        }
        InsertListItem(properties, pos++, wxT("Connected?"), BoolToYesNo(GetConnected()));
    }

    if(!GetConnected())
        return;
    
    if (statistics)
    {
        wxLogInfo(wxT("Displaying statistics for server ") + GetIdentifier());

        // Add the statistics view columns
        statistics->ClearAll();
        statistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
        statistics->InsertColumn(1, wxT("PID"), wxLIST_FORMAT_LEFT, 50);
        statistics->InsertColumn(2, wxT("User"), wxLIST_FORMAT_LEFT, 100);
        statistics->InsertColumn(3, wxT("Current Query"), wxLIST_FORMAT_LEFT, 400);

        pgSet *stats = ExecuteSet(wxT("SELECT datname, procpid, usename, current_query FROM pg_stat_activity"));
        if (stats)
        {
            int pos=0;
            while (!stats->Eof())
            {
                statistics->InsertItem(pos, stats->GetVal(wxT("datname")), 0);
                statistics->SetItem(pos, 1, stats->GetVal(wxT("procpid")));
                statistics->SetItem(pos, 2, stats->GetVal(wxT("usename")));
                statistics->SetItem(pos, 3, stats->GetVal(wxT("current_query")));
                stats->MoveNext();
                pos++;
            }

	        // Keith 2003.03.05
	        // Fixed memory leak
	        delete stats;
        }
    }
}
