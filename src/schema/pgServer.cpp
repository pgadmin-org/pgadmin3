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
#include "dlgConnect.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"
#include "pgfeatures.h"


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
#ifdef WIN32
    scmHandle = 0;
    serviceHandle = 0;
#endif
}

pgServer::~pgServer()
{
	// Keith 2003.03.05
	// This was not being deleted and was causing memory leaksd
	if (conn)
		delete conn;

#ifdef WIN32
    if (serviceHandle)
        CloseServiceHandle(serviceHandle);
    if (scmHandle)
        CloseServiceHandle(scmHandle);
#endif
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


bool pgServer::StartService()
{
    bool done=false;
#ifdef WIN32
    if (serviceHandle)
    {
        done = (::StartService(serviceHandle, 0, 0) != 0);
        if (!done)
        {
            DWORD rc = ::GetLastError();
            if (rc == ERROR_SERVICE_ALREADY_RUNNING)
                return true;

            // report error
            wxLogError(__("Failed to start server %s: Errcode=%d\nCheck event log for details."),
                serviceId.c_str(), rc);
        }
        else
            GetServerRunning();     // ignore result, just to wait for startup
    }
#else
	wxString res = ExecProcess(serviceId + wxT(" start"));
	done = (res.Find(wxT("tarting")) > 0);
#endif
    return done;
}


bool pgServer::StopService()
{
    bool done=false;
#ifdef WIN32
    if (serviceHandle)
    {
        SERVICE_STATUS st;
        done = (::ControlService(serviceHandle, SERVICE_CONTROL_STOP, &st) != 0);
        if (!done)
        {
            DWORD rc = ::GetLastError();
            // report error
            wxLogError(__("Failed to stop server %s: Errcode=%d\nCheck event log for details."),
                serviceId.c_str(), rc);
        }
    }
#else
	wxString res = ExecProcess(serviceId + wxT(" stop"));
	done = (res.Find(wxT("stopped")) > 0);
#endif
    return done;
}


bool pgServer::GetServerRunning()
{
    bool done=false;
#ifdef WIN32
    if (serviceHandle)
    {
        SERVICE_STATUS st;
        int loops;

        for (loops=0 ; loops < 20 ; loops++)
        {
            if (::QueryServiceStatus(serviceHandle, &st) == 0)
            {
                DWORD rc = ::GetLastError();
                CloseServiceHandle(serviceHandle);
                CloseServiceHandle(scmHandle);
                serviceHandle=0;
                scmHandle=0;

                return false;
            }
            done = (st.dwCurrentState == SERVICE_RUNNING);
            if (st.dwCurrentState == SERVICE_START_PENDING)
                Sleep(100);
            else
                break;
        }
    }
#else

	wxString res = ExecProcess(serviceId + wxT(" status"));
	done = (res.Find(wxT("PID: ")) > 0);

#endif
    return done;
}


void pgServer::iSetServiceID(const wxString& s)
{
    serviceId = s;
#ifdef WIN32
    if (serviceId.Find('\\') < 0)
        scmHandle = OpenSCManager(0, SERVICES_ACTIVE_DATABASE, GENERIC_EXECUTE);
    else
        scmHandle = OpenSCManager(wxT("\\\\") + serviceId.BeforeFirst('\\'), SERVICES_ACTIVE_DATABASE, GENERIC_EXECUTE|GENERIC_READ);

    if (scmHandle)
        serviceHandle=OpenService(scmHandle, serviceId.AfterLast('\\'), GENERIC_EXECUTE|GENERIC_READ);
#endif
}


bool pgServer::GetServerControllable()
{
#ifdef WIN32
    return serviceHandle != 0;
#else
    return !serviceId.IsEmpty();
#endif
}

    
int pgServer::Connect(frmMain *form, bool askPassword, const wxString &pwd)
{
    wxLogInfo(wxT("Attempting to create a connection object..."));

    if (!conn || conn->GetStatus() != PGCONN_OK)
    {
        if (conn)
        {
            delete conn;
            conn=0;
        }
        if (askPassword)
        {
            if (GetNeedPwd())
            {
                wxString txt;
                txt.Printf(_("Please enter password for user %s\non server %s (%s)"), username.c_str(), description.c_str(), GetName().c_str());
                dlgConnect dlg(form, txt, GetNeedPwd());

	            switch (dlg.Go())
                {
		            case wxID_OK:
			            break;
		            case wxID_CANCEL:
                    case -1:
	                    return PGCONN_ABORTED;
		            default:
	                    wxLogError(__("Couldn't create a connection dialogue!"));
		                return PGCONN_BAD;
	            }

                iSetNeedPwd(dlg.GetNeedPwd());

                if (GetNeedPwd())
                    iSetPassword(dlg.GetPassword());
            }
        }
        else
            iSetPassword(pwd);

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

            wxString sql = wxT("SELECT usecreatedb, usesuper");
            if (conn->HasFeature(FEATURE_POSTMASTER_STARTTIME))
                sql += wxT(", CASE WHEN usesuper THEN pg_postmaster_starttime() ELSE NULL END as upsince");

            pgSet *set=ExecuteSet(sql + wxT("\n  FROM pg_user WHERE usename=current_user"));
            if (set)
            {
                iSetCreatePrivilege(set->GetBool(wxT("usecreatedb")));
                iSetSuperUser(set->GetBool(wxT("usesuper")));
                if (conn->HasFeature(FEATURE_POSTMASTER_STARTTIME))
                    iSetUpSince(set->GetDateTime(wxT("upsince")));
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
    if (conn)
    {
        if (error != wxT(""))
        {
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
        properties->AppendItem(_("Need password?"), GetNeedPwd());
        if (GetConnected())
        {
            properties->AppendItem(_("Version string"), GetVersionString());
            properties->AppendItem(_("Version number"), GetVersionNumber());
            properties->AppendItem(_("Last system OID"), GetLastSystemOID());
        }
        properties->AppendItem(_("Connected?"), GetConnected());
        if (GetUpSince().IsValid())
            properties->AppendItem(_("Up since"), GetUpSince());
        if (GetServerControllable())
            properties->AppendItem(_("Running?"), GetServerRunning());
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
