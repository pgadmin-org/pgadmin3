//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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

    connected = false;
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


pgConn *pgServer::CreateConn(wxString dbName, OID oid)
{
    if (!connected)
        return 0;

    if (dbName.IsEmpty())
    {
        dbName = GetDatabaseName();
        oid = dbOid;
    }
    pgConn *conn=new pgConn(GetName(), dbName, username, password, port, ssl, oid);

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


bool pgServer::Disconnect(frmMain *form)
{
    if (conn)
    {
        delete conn;
        conn=0;
        connected=false;
        expandedKids=false;
    }
    form->GetBrowser()->SetItemImage(GetId(), PGICON_SERVERBAD, wxTreeItemIcon_Normal);

    form->GetBrowser()->SetItemImage(GetId(), PGICON_SERVERBAD, wxTreeItemIcon_Selected);
    return true;
}


#define SERVICEBUFSIZE  10000
#define QUERYBUFSIZE    256     

#ifdef WIN32
wxArrayString pgServer::GetDependentServices(SC_HANDLE handle)
{
    wxArrayString services;
    LPENUM_SERVICE_STATUS sbuf = (LPENUM_SERVICE_STATUS) new char[SERVICEBUFSIZE];

    DWORD servicesReturned=0, bytesNeeded;
    ::EnumDependentServices(handle, SERVICE_STATE_ALL, sbuf, SERVICEBUFSIZE, &bytesNeeded, &servicesReturned);


    DWORD i;
    for (i=0 ; i < servicesReturned ; i++)
    {
        SC_HANDLE h=::OpenService(scmHandle, sbuf[i].lpServiceName, SERVICE_QUERY_CONFIG);
        if (h)
        {
            char buffer[QUERYBUFSIZE];
            LPQUERY_SERVICE_CONFIG qsc=(LPQUERY_SERVICE_CONFIG)buffer;
            if(::QueryServiceConfig(h, qsc, QUERYBUFSIZE, &bytesNeeded))
            {
                if (qsc->dwStartType != SERVICE_DISABLED)
                    services.Add(sbuf[i].lpServiceName);
            }

            ::CloseServiceHandle(h);
        }
    }
    delete[] sbuf;

    return services;
}
#endif


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
            {
                GetServerRunning();
                return true;
            }
            // report error
            wxLogError(__("Failed to start server %s: Errcode=%d\nCheck event log for details."),
                serviceId.c_str(), rc);
        }
        else
        {
            GetServerRunning();     // ignore result, just to wait for startup

            wxArrayString services=GetDependentServices(serviceHandle);

            if (services.GetCount() > 0)
            {
                size_t i;
                wxString serviceString;
                for (i=0 ; i < services.GetCount() ; i++)
                    serviceString += wxT("   ") + services.Item(i) + wxT("\n");

                wxMessageDialog msg(0, _("There are dependent services configured:\n\n")
                        + serviceString + _("\nStart dependent services too?"), _("Dependent services"),
                            wxICON_EXCLAMATION | wxYES_NO | wxYES_DEFAULT);
                
                if (msg.ShowModal() == wxID_YES)
                {
                    for (i=0 ; i < services.GetCount() ; i++)
                    {
                        SC_HANDLE h=::OpenService(scmHandle, services.Item(i), GENERIC_EXECUTE|GENERIC_READ);
                        if (h)
                        {
                            if (!::StartService(h, 0, 0))
                                done=false;
                            CloseServiceHandle(h);
                        }
                        else
                            done=false;
                    }
                    if (!done)
                    {
                        wxMessageDialog msg(0, _("One or more dependent services didn't start; see the eventlog for details."), _("Service start problem"),
                                    wxICON_EXCLAMATION |wxOK);
                        msg.ShowModal();
                        done=true;
                    }
                }
            }
        }
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
            if (::GetLastError() == ERROR_DEPENDENT_SERVICES_RUNNING)
            {
                LPENUM_SERVICE_STATUS sbuf = (LPENUM_SERVICE_STATUS) new char[SERVICEBUFSIZE];
                DWORD bytesNeeded, servicesReturned=0;
                ::EnumDependentServices(serviceHandle, SERVICE_ACTIVE, sbuf, SERVICEBUFSIZE, &bytesNeeded, &servicesReturned);
                
                done=true;

                if (servicesReturned)
                {
                    DWORD i;
                    wxString services;
                    for (i=0 ; i < servicesReturned ; i++)
                        services += wxT("   ") + wxString(sbuf[i].lpDisplayName) + wxT("\n");

                    wxMessageDialog msg(0, _("There are dependent services running:\n\n")
                            + services + _("\nStop dependent services?"), _("Dependent services"),
                                wxICON_EXCLAMATION | wxYES_NO | wxYES_DEFAULT);
                    if (msg.ShowModal() != wxID_YES)
                        return false;

                    for (i=0 ; done && i < servicesReturned ; i++)
                    {
                        SC_HANDLE h=::OpenService(scmHandle, sbuf[i].lpServiceName, GENERIC_EXECUTE|GENERIC_READ);
                        if (h)
                        {
                            done = (::ControlService(h, SERVICE_CONTROL_STOP, &st) != 0);
                            CloseServiceHandle(h);
                        }
                        else
                            done=false;
                    }
                    if (done)
                        done = (::ControlService(serviceHandle, SERVICE_CONTROL_STOP, &st) != 0);
                }
            }
            // report error

            if (!done)
                wxLogError(__("Failed to stop server %s: Errcode=%d\nCheck event log for details."),
                    serviceId.c_str(), ::GetLastError());
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
						wxTheApp->Yield();
			            break;
		            case wxID_CANCEL:
                    case -1:
						wxTheApp->Yield();
	                    return PGCONN_ABORTED;
		            default:
						wxTheApp->Yield();
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

        if (!conn)
        {
            form->EndMsg(false);
            wxLogError(__("Couldn't create a connection object!"));
            return PGCONN_BAD;
        }
    }
    int status = conn->GetStatus();
    if (status == PGCONN_OK)
    {
        dbOid = conn->GetDbOid();

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

            wxString version, allVersions;
            version.Printf(wxT("%d.%d"), conn->GetMajorVersion(), conn->GetMinorVersion());
            allVersions = settings->Read(wxT("Updates/pgsql-Versions"), wxEmptyString);
            if (allVersions.Find(version) < 0)
            {
                if (!allVersions.IsEmpty())
                    allVersions += wxT(", ");
                allVersions += version;
                settings->Write(wxT("Updates/pgsql-Versions"), allVersions);
            }
            if (conn->IsSSLconnected())
                settings->Write(wxT("Updates/UseSSL"), true);

            form->GetBrowser()->SetItemImage(GetId(), PGICON_SERVER, wxTreeItemIcon_Normal);
            form->GetBrowser()->SetItemImage(GetId(), PGICON_SERVER, wxTreeItemIcon_Selected);
        }
        else
        {
            error.Printf(_("The PostgreSQL server must be at least version %2.1f!"), SERVER_MIN_VERSION);
            connected = false;
            status = PGCONN_BAD;
        }

    }
    else
    {
        connected = false;
    }

    form->EndMsg(connected && status == PGCONN_OK);
    return status;
}


wxString pgServer::GetIdentifier() const
{
    wxString idstr;
    if (GetName().IsEmpty())
        idstr.Printf(wxT("local:.s.PGSQL.%d"), port);
	else if (GetName().StartsWith(wxT("/")))
        idstr.Printf(wxT("local:%s/.s.PGSQL.%d"), GetName().c_str(), port);
	else
	    idstr.Printf(wxT("%s:%d"), GetName().c_str(), port);
    return idstr;
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
    if (executed)
    {
        password = newVal;
        return false;
    }
    else
        return false;
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
//        form->fileMenu->Enable(MNU_PASSWORD, true);

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

        properties->AppendItem(_("Description"), GetDescription());
        if (GetName().IsEmpty() || GetName().StartsWith(wxT("/")))
		{
            properties->AppendItem(_("Hostname"), wxT("local:") + GetName());
            properties->AppendItem(_("Port"), (long)GetPort());
		}
        else
        {
            properties->AppendItem(_("Hostname"), GetName());
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
        }
        if (!serviceId.IsEmpty())
            properties->AppendItem(_("Service"), serviceId);

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
        if (GetConnected() && GetUpSince().IsValid())
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
        if (GetConnection()->BackendMinimumVersion(8, 1))
        {
            statistics->AddColumn(_("Backend start"), 70);
            statistics->AddColumn(_("Client"), 70);
        }
        statistics->AddColumn(_("Current Query"), 300);

        pgSet *stats = ExecuteSet(wxT("SELECT * FROM pg_stat_activity"));
        if (stats)
        {
            int pos=0;
            while (!stats->Eof())
            {
                statistics->InsertItem(pos, stats->GetVal(wxT("procpid")), 0);
                int colpos=1;
                statistics->SetItem(pos, colpos++, stats->GetVal(wxT("usename")));
                statistics->SetItem(pos, colpos++, stats->GetVal(wxT("datname")));
                if (GetConnection()->BackendMinimumVersion(8, 1))
                {
                    statistics->SetItem(pos, colpos++, stats->GetVal(wxT("backend_start")));
                    wxString client=stats->GetVal(wxT("client_addr")) + wxT(":") + stats->GetVal(wxT("client_port"));
                    if (client == wxT(":-1"))
                        client = _("local pipe");
                    statistics->SetItem(pos, colpos++, client);
                }
                statistics->SetItem(pos, colpos++, stats->GetVal(wxT("current_query")));

                stats->MoveNext();
                pos++;
            }

	        delete stats;
        }
    }
}
