//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgServer.cpp - PostgreSQL Server
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "frmConnect.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"


pgServer::pgServer(const wxString& newName, const wxString& newDescription, const wxString& newDatabase, const wxString& newUsername, int newPort, bool _trusted, int _ssl)
: pgObject(PG_SERVER, newName)
{  
    wxLogInfo(wxT("Creating a pgServer object"));

	description = newDescription;
    database = newDatabase;
    username = newUsername;
    port = newPort;
    ssl=_ssl;

    connected = FALSE;
    lastSystemOID = 0;

    conn = NULL;
    trusted=_trusted;
    superUser=false;
    createPrivilege=false;
}

pgServer::~pgServer()
{
	// Keith 2003.03.05
	// This was not being deleted and was causing memory leaksd
	if (conn)
		delete conn;

    wxLogInfo(wxT("Destroying a pgServer object"));
}


wxMenu *pgServer::GetNewMenu()
{
    wxMenu *menu=0;
    if (connected && GetSuperUser())
    {
        menu=new wxMenu();
        AppendMenu(menu, PG_DATABASE);
        AppendMenu(menu, PG_TABLESPACE);
        AppendMenu(menu, PG_GROUP);
        AppendMenu(menu, PG_USER);
    }
    return menu;
}


pgServer *pgServer::GetServer() const
{
    if (connected)
        return (pgServer*)this;
    return 0;
}


pgConn *pgServer::CreateConn(wxString dbName)
{
    if (!connected)
        return 0;

    if (dbName.IsEmpty())
        dbName = GetDatabaseName();

    pgConn *conn=new pgConn(GetName(), dbName, username, password, port, ssl);

    if (conn && conn->GetStatus() != PGCONN_OK)
    {
        wxLogError(wxT("%s"), conn->GetLastError().c_str());
        delete conn;
        return 0;
    }
    return conn;
}


wxString pgServer::GetFullName() const
{
    if (GetDescription().Length() > 0)
      return GetDescription() + wxT(" (") + GetIdentifier() + wxT(")");
    else
      return wxT("(") + GetIdentifier() + wxT(")");
}


bool pgServer::Disconnect()
{
    if (conn)
    {
        delete conn;
        conn=0;
        connected=false;
        expandedKids=false;
    }
    return true;
}


int pgServer::Connect(frmMain *form, bool lockFields) 
{
    wxLogInfo(wxT("Attempting to create a connection object..."));

    parentWin = form;
    if (!conn || conn->GetStatus() != PGCONN_OK)
    {
        if (conn)
        {
            delete conn;
            conn=0;
        }
        if (!trusted || !lockFields)
        {
            frmConnect winConnect(form, GetName(), description, database, username, port, trusted, ssl);

            if (lockFields) 
		        winConnect.LockFields();

	        switch (winConnect.Go())
            {
		        case wxID_OK:
			        break;
		        case wxID_CANCEL:
	                return PGCONN_ABORTED;
		        default:
	                wxLogError(__("Couldn't create a connection dialogue!"));
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
                ssl=winConnect.GetSSL();
            }
            if (!trusted)
                iSetPassword(winConnect.GetPassword());
        }
        if (password.IsNull())
            form->StartMsg(_("Connecting to database without password"));
        else
            form->StartMsg(_("Connecting to database"));

        conn = new pgConn(GetName(), database, username, password, port, ssl);
        form->EndMsg();

        if (!conn)
        {
            wxLogError(__("Couldn't create a connection object!"));
            return PGCONN_BAD;
        }
    }
    int status = conn->GetStatus();
    if (status == PGCONN_OK)
    {
        // Check the server version
        if (conn->BackendMinimumVersion(7, 3))
        {
            connected = true;
            pgSet *set=ExecuteSet(wxT("SELECT usecreatedb, usesuper from pg_user where usename=current_user"));
            if (set)
            {
                iSetCreatePrivilege(set->GetBool(wxT("usecreatedb")));
                iSetSuperUser(set->GetBool(wxT("usesuper")));
                delete set;
            }
        }
        else
        {
            error.Printf(_("The PostgreSQL server must be at least version %2.1f!"), SERVER_MIN_VERSION);
            connected = false;
            return PGCONN_BAD;
        }

    }
    else
        connected = FALSE;

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
    if (connected)
    {
        if (ver.IsEmpty())
            ver = wxString(conn->GetVersionString());
        return ver;
    }
    else
        return wxEmptyString;
}


wxString pgServer::GetVersionNumber()
{
    if (connected)
    {
        if (versionNum.IsEmpty())
        {
            int major=0, minor=0;
            sscanf(GetVersionString().ToAscii(), "%*s %d.%d", &major, &minor);
            versionNum.Printf(wxT("%d.%d"), major, minor);
        }

    } 
    return versionNum;
}


OID pgServer::GetLastSystemOID()
{
    if (connected)
    {
        if (lastSystemOID == 0)
            lastSystemOID = conn->GetLastSystemOID();
        return lastSystemOID;
    }
    else
        return 0;
}


bool pgServer::SetPassword(const wxString& newVal)
{
    wxString sql;
    sql.Printf(wxT("ALTER USER %s WITH PASSWORD %s;"), qtIdent(username).c_str(), qtString(newVal).c_str());
    bool executed = conn->ExecuteVoid(sql);
    if (executed) {
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
        if (conn->GetLastError() != wxT(""))
        {
            msg.Printf(wxT("%s\n%s"), error.c_str(), conn->GetLastError().c_str());
        }
        else
        {
            msg=error;
        }
    }
    else
    {
        msg=conn->GetLastError();
    }
    return msg;
}



void pgServer::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
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
            pgCollection *collection = new pgCollection(PG_DATABASES, this);
            AppendBrowserItem(browser, collection);

            if (conn->BackendMinimumVersion(7, 5))
            {
                // Tablespaces
                collection = new pgCollection(PG_TABLESPACES, this);
                AppendBrowserItem(browser, collection);
            }
            // Groups
            collection = new pgCollection(PG_GROUPS, this);
            AppendBrowserItem(browser, collection);
    
            // Users
            collection = new pgCollection(PG_USERS, this);
            AppendBrowserItem(browser, collection);
        }
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for server ") + GetIdentifier());

        // Add the properties view columns
        CreateListColumns(properties);

        // Display the Server properties

        properties->AppendItem(_("Hostname"), GetName());
        properties->AppendItem(_("Description"), GetDescription());
        properties->AppendItem(_("Port"), (long)GetPort());

#ifdef SSL
        if (GetConnected())
        {
            properties->AppendItem(_("Encryption"), 
                conn->IsSSLconnected() ? _("SSL encrypted") : _("not encrypted"));
        }
        else
        {
            if (ssl > 0)
            {
                wxString sslMode;
                switch (ssl)
                {
                    case 1: sslMode = _("require"); break;
                    case 2: sslMode = _("prefer"); break;
                    case 3: sslMode = _("allow"); break;
                    case 4: sslMode = _("disable"); break;
                }
                properties->AppendItem(_("SSL Mode"), sslMode);
            }
        }
#endif
        properties->AppendItem(_("Initial database"), GetDatabaseName());
        properties->AppendItem(_("Username"), GetUsername());
        properties->AppendItem(_("Trusted?"), GetTrusted());
        if (GetConnected())
        {
            properties->AppendItem(_("Version string"), GetVersionString());
            properties->AppendItem(_("Version number"), GetVersionNumber());
            properties->AppendItem(_("Last system OID"), GetLastSystemOID());
        }
        properties->AppendItem(_("Connected?"), BoolToYesNo(GetConnected()));
    }

    if(!GetConnected())
        return;
}


void pgServer::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    if (conn)
    {
        wxLogInfo(wxT("Displaying statistics for server ") + GetIdentifier());

        // Add the statistics view columns
        statistics->ClearAll();
        statistics->AddColumn(wxT("PID"), 35);
        statistics->AddColumn(_("User"), 70);
        statistics->AddColumn(_("Database"), 70);
        statistics->AddColumn(_("Current Query"), 300);

        pgSet *stats = ExecuteSet(wxT("SELECT datname, procpid, usename, current_query FROM pg_stat_activity"));
        if (stats)
        {
            int pos=0;
            while (!stats->Eof())
            {
                statistics->InsertItem(pos, stats->GetVal(wxT("procpid")), 0);
                statistics->SetItem(pos, 1, stats->GetVal(wxT("usename")));
                statistics->SetItem(pos, 2, stats->GetVal(wxT("datname")));
                statistics->SetItem(pos, 3, stats->GetVal(wxT("current_query")));
                stats->MoveNext();
                pos++;
            }

	        delete stats;
        }
    }
}