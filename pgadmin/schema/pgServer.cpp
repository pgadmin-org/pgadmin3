//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgServer.cpp - PostgreSQL Server
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/busyinfo.h>

// App headers
#include "frm/menu.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "frm/frmHint.h"
#include "dlg/dlgConnect.h"
#include "schema/pgDatabase.h"
#include "schema/pgTablespace.h"
#include "schema/pgGroup.h"
#include "schema/pgUser.h"
#include "schema/pgRole.h"
#include "agent/pgaJob.h"
#include "utils/utffile.h"
#include "utils/pgfeatures.h"
#include "frm/frmReport.h"
#include "dlg/dlgServer.h"

#define DEFAULT_PG_DATABASE wxT("postgres")

pgServer::pgServer(const wxString& newName, const wxString& newDescription, const wxString& newDatabase, const wxString& newUsername, int newPort, bool _storePwd, bool _restore, int _ssl)
: pgObject(serverFactory, newName)
{  
	description = newDescription;
    database = newDatabase;
    username = newUsername;
    port = newPort;
    ssl=_ssl;
    serverIndex=0;

    connected = false;
    lastSystemOID = 0;

    conn = NULL;
    passwordValid=true;
    storePwd=_storePwd;
	restore=_restore;
    superUser=false;
    createPrivilege=false;
#ifdef WIN32
    scmHandle = 0;
    serviceHandle = 0;
#endif
}

pgServer::~pgServer()
{
	if (conn)
		delete conn;

#ifdef WIN32
    if (serviceHandle)
        CloseServiceHandle(serviceHandle);
    if (scmHandle)
        CloseServiceHandle(scmHandle);
#endif
}


int pgServer::GetIconId()
{
    if (GetConnected())
        return serverFactory.GetIconId();
    else
        return serverFactory.GetClosedIconId();
}


wxMenu *pgServer::GetNewMenu()
{
    wxMenu *menu=0;
    if (connected && (GetSuperUser() || GetCreateRole()))
    {
        menu=new wxMenu();
        if (settings->GetDisplayOption(wxT("Tablespaces")))
            tablespaceFactory.AppendMenu(menu);
        if (GetConnection()->BackendMinimumVersion(8, 1))
        {
            if (settings->GetDisplayOption(wxT("Groups/group roles")))
                groupRoleFactory.AppendMenu(menu);
            if (settings->GetDisplayOption(wxT("Users/login roles")))
                loginRoleFactory.AppendMenu(menu);
        }
        else
        {
            if (settings->GetDisplayOption(wxT("Groups/group roles")))
                groupFactory.AppendMenu(menu);
            if (settings->GetDisplayOption(wxT("Users/login roles")))
                userFactory.AppendMenu(menu);
        }
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


wxString pgServer::GetFullName()
{
    if (GetDescription().Length() > 0)
      return GetDescription() + wxT(" (") + GetIdentifier() + wxT(")");
    else
      return wxT("(") + GetIdentifier() + wxT(")");
}

wxString pgServer::GetFullIdentifier() 
{
    return GetFullName();
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
    UpdateIcon(form->GetBrowser());
    return true;
}


bool pgServer::GetCanHint()
{
    return connected && conn->BackendMinimumVersion(8, 1) && !autovacuumRunning;
}


void pgServer::ShowHint(frmMain *form, bool force)
{
    wxArrayString hints;

    if (!autovacuumRunning)
        hints.Add(HINT_AUTOVACUUM);
    
    if (force || !hintShown)
        frmHint::ShowHint(form, hints, GetFullIdentifier(), force);
    hintShown=true;
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


wxString pgServer::passwordFilename()
{
	wxString fname = sysSettings::GetConfigFile(sysSettings::PGPASS);

    wxLogInfo(wxT("Using password file %s"), fname.c_str());
    return fname;
}



bool pgServer::GetPasswordIsStored()
{
    wxString fname=passwordFilename();


    if (!wxFile::Exists(fname))
        return false;

    wxUtfFile file(fname, wxFile::read, wxFONTENCODING_SYSTEM);

    if (file.IsOpened())
    {
        wxString before;
        file.Read(before);

        wxStringTokenizer lines(before, wxT("\n\r"));

        wxString seekStr= GetName() + wxT(":") 
                        + NumToStr((long)GetPort()) + wxT(":*:") 
                        + username + wxT(":") ;

        wxString seekStr2= wxString(GetName().mb_str(wxConvUTF8), wxConvLibc) + wxT(":") 
                        + NumToStr((long)GetPort()) + wxT(":*:") 
                        + wxString(username.mb_str(wxConvUTF8), wxConvLibc) + wxT(":") ;

        while (lines.HasMoreTokens())
        {
            wxString str=lines.GetNextToken();
            if (str.Left(seekStr.Length()) == seekStr)
                return true;

            if (str.Left(seekStr2.Length()) == seekStr2)
                return true;
        }
    }
    
    return false;
}


void pgServer::StorePassword()
{
    wxString fname=passwordFilename();


    wxUtfFile file;
    if (!wxFile::Exists(fname))
    {
        int fd=creat(fname.ToAscii(), S_IREAD | S_IWRITE);
        if (fd > 0)
            close(fd);
    }

	// Don't try to read and write in one OP - it doesn't work well
	wxString before;
	file.Open(fname, wxFile::read, wxFONTENCODING_SYSTEM);
	file.Read(before);
	file.Close();

	file.Open(fname, wxFile::write, wxFONTENCODING_SYSTEM);

    if (file.IsOpened())
    {
        wxString after;

        wxString passwd;
        wxString seekStr;
        
        if (GetConnection()->GetNeedUtfConnectString())
        {
            passwd = wxString(password.mb_str(wxConvUTF8), wxConvLibc);
            seekStr = wxString(GetName().mb_str(wxConvUTF8), wxConvLibc) + wxT(":") 
                    + NumToStr((long)GetPort()) + wxT(":*:") 
                    + wxString(username.mb_str(wxConvUTF8), wxConvLibc) + wxT(":") ;
        }
        else
        {
            passwd = password;
            seekStr = GetName() + wxT(":") 
                    + NumToStr((long)GetPort()) + wxT(":*:") 
                    + username + wxT(":") ;
        }

        file.Read(before);
        wxStringTokenizer lines(before, wxT("\n\r"));

        file.Seek(0);
        bool found=false;
        while (lines.HasMoreTokens())
        {
            wxString str=lines.GetNextToken();
            if (str.Left(seekStr.Length()) == seekStr)
            {
                // entry found
                found=true;
                if (storePwd)
                    file.Write(seekStr + passwd + END_OF_LINE);
            }
            else
                file.Write(str + END_OF_LINE);
        }
        if (!found && storePwd)
            file.Write(seekStr + passwd + END_OF_LINE);

        file.Close();
    }
}

    
int pgServer::Connect(frmMain *form, bool askPassword, const wxString &pwd, bool forceStorePassword)
{
    wxLogInfo(wxT("Attempting to create a connection object..."));

    bool storePassword = false;

    if (!conn || conn->GetStatus() != PGCONN_OK)
    {
        if (conn)
        {
            delete conn;
            conn=0;
        }
        if (askPassword)
        {
            if (!passwordValid || !GetPasswordIsStored() || !GetStorePwd())
            {
                wxString txt;
                txt.Printf(_("Please enter password for user %s\non server %s (%s)"), username.c_str(), description.c_str(), GetName().c_str());
                dlgConnect dlg(form, txt, GetStorePwd());

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

                iSetStorePwd(dlg.GetStorePwd());
                password = dlg.GetPassword();
                storePassword = true;
            }
        }
        else
            iSetPassword(pwd);

        form->StartMsg(_("Connecting to database"));

        if (database.IsEmpty())
        {
            conn = new pgConn(GetName(), DEFAULT_PG_DATABASE, username, password, port, ssl);
            if (conn->GetStatus() == PGCONN_OK)
                database=DEFAULT_PG_DATABASE;
            else if (conn->GetStatus() == PGCONN_BAD && conn->GetLastError().Find(
                                wxT("database \"") DEFAULT_PG_DATABASE wxT("\" does not exist")) >= 0)
            {
                delete conn;
                conn = new pgConn(GetName(), wxT("template1"), username, password, port, ssl);
                if (conn && conn->GetStatus() == PGCONN_OK)
                    database=wxT("template1");
            }
        }
        else
        {
            conn = new pgConn(GetName(), database, username, password, port, ssl);
            if (!conn)
            {
                form->EndMsg(false);
                wxLogError(__("Couldn't create a connection object!"));
                return PGCONN_BAD;
            }
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
            bool hasUptime=false;

            wxString sql = wxT("SELECT usecreatedb, usesuper");
            if (conn->BackendMinimumVersion(8, 1))
            {
                hasUptime=true;
                sql += wxT(", CASE WHEN usesuper THEN pg_postmaster_start_time() ELSE NULL END as upsince");
            }
            else if (conn->HasFeature(FEATURE_POSTMASTER_STARTTIME))
            {
                hasUptime=true;
                sql += wxT(", CASE WHEN usesuper THEN pg_postmaster_starttime() ELSE NULL END as upsince");
            }

            pgSet *set=ExecuteSet(sql + wxT("\n  FROM pg_user WHERE usename=current_user"));
            if (set)
            {
                iSetCreatePrivilege(set->GetBool(wxT("usecreatedb")));
                iSetSuperUser(set->GetBool(wxT("usesuper")));
                if (hasUptime)
                    iSetUpSince(set->GetDateTime(wxT("upsince")));
                delete set;
            }

			if (conn->BackendMinimumVersion(8, 1))
			{
				set=ExecuteSet(wxT("SELECT rolcreaterole, rolcreatedb FROM pg_roles WHERE rolname = current_user;"));
	
				if (set)
				{
					iSetCreatePrivilege(set->GetBool(wxT("rolcreatedb")));
					iSetCreateRole(set->GetBool(wxT("rolcreaterole")));
					delete set;
				}
			}
			else
				iSetCreateRole(false);

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

            UpdateIcon(form->GetBrowser());
            if (storePassword || forceStorePassword)
                StorePassword();
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

    passwordValid = connected;
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
    sql.Printf(wxT("ALTER USER %s WITH ENCRYPTED PASSWORD %s;"), qtIdent(username).c_str(), qtDbString(conn->EncryptPassword(username, newVal)).c_str());
    bool executed = conn->ExecuteVoid(sql);
    if (executed)
    {
        password = newVal;
        StorePassword();
        return true;
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



void pgServer::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    // Add child nodes if necessary
    if (GetConnected())
    {
        // Reset password menu option
//        form->fileMenu->Enable(MNU_PASSWORD, true);

        if (!expandedKids)
        {
            expandedKids=true;
            // Log
            
            wxLogInfo(wxT("Adding child object to server ") + GetIdentifier());
    
			if (settings->GetDisplayOption(wxT("Databases")))
				browser->AppendCollection(this, databaseFactory);

            if (conn->BackendMinimumVersion(8, 0) && settings->GetDisplayOption(wxT("Tablespaces")))
                browser->AppendCollection(this, tablespaceFactory);

            // Jobs
            // We only add the Jobs node if the appropriate objects are the initial DB.
			if (settings->GetDisplayOption(wxT("pgAgent jobs")))
			{
				wxString exists = conn->ExecuteScalar(
					wxT("SELECT cl.oid FROM pg_class cl JOIN pg_namespace ns ON ns.oid=relnamespace\n")
					wxT(" WHERE relname='pga_job' AND nspname='pgagent'"));

				if (!exists.IsNull())
					browser->AppendCollection(this, jobFactory);
			}

            if (conn->BackendMinimumVersion(8, 1))
            {
				if (settings->GetDisplayOption(wxT("Groups/group roles")))
					browser->AppendCollection(this, groupRoleFactory);
				if (settings->GetDisplayOption(wxT("Users/login roles")))
                browser->AppendCollection(this, loginRoleFactory);
            }
            else
            {
				if (settings->GetDisplayOption(wxT("Groups/group roles")))
					browser->AppendCollection(this, groupFactory);
				if (settings->GetDisplayOption(wxT("Users/login roles")))
					browser->AppendCollection(this, userFactory);
            }
        }

        autovacuumRunning=true;
        pgSetIterator set(conn, 
            wxT("SELECT setting FROM pg_settings\n")
            wxT(" WHERE name IN ('autovacuum', 'stats_start_collector', 'stats_row_level')"));

        while (autovacuumRunning && set.RowsLeft())
            autovacuumRunning = set.GetBool(wxT("setting"));
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

        properties->AppendItem(_("Maintenance database"), GetDatabaseName());
        properties->AppendItem(_("Username"), GetUsername());
        properties->AppendItem(_("Store password?"), GetStorePwd());
		properties->AppendItem(_("Restore environment?"), GetRestore());
        if (GetConnected())
        {
            properties->AppendItem(_("Version string"), GetVersionString());
            properties->AppendItem(_("Version number"), GetVersionNumber());
            properties->AppendItem(_("Last system OID"), GetLastSystemOID());
        }
        properties->AppendItem(_("Connected?"), GetConnected());
        if (GetConnected())
        {
            if (GetUpSince().IsValid())
                properties->AppendItem(_("Up since"), GetUpSince());
            if (conn->BackendMinimumVersion(8,1))
                properties->AppendItem(wxT("Autovacuum"), (autovacuumRunning ? _("running") : _("not running")));
        }
        if (GetServerControllable())
            properties->AppendItem(_("Running?"), GetServerRunning());

        if (!GetDbRestriction().IsEmpty())
            properties->AppendItem(_("DB restriction"), GetDbRestriction());
    }

    if(!GetConnected())
        return;

    if (form && GetCanHint() && !hintShown)
    {
        ShowHint(form, false);
    }
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

void pgServer::ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &wh)
{
    Dependencies->ClearAll();
    Dependencies->AddColumn(_("Type"), 60);
    Dependencies->AddColumn(_("Name"), 100);
    Dependencies->AddColumn(_("Restriction"), 50);
}


void pgServer::ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &wh)
{
    referencedBy->ClearAll();
    referencedBy->AddColumn(_("Type"), 60);
    referencedBy->AddColumn(_("Name"), 100);
    referencedBy->AddColumn(_("Restriction"), 50);
}

pgServerCollection::pgServerCollection(pgaFactory *factory)
 : pgCollection(factory)
{
}


pgServerObjCollection::pgServerObjCollection(pgaFactory *factory, pgServer *sv)
: pgCollection(factory)
{
    server = sv;
}


bool pgServerObjCollection::CanCreate()
{
    if (server->GetMetaType() == PGM_DATABASE)
        return (GetServer()->GetCreatePrivilege() || GetServer()->GetSuperUser());
    else
	{
		if (server->GetConnection()->BackendMinimumVersion(8, 1) && GetMetaType() == PGM_ROLE)
			return (server->GetCreateRole() || server->GetSuperUser());
		else if (server->GetConnection()->BackendMinimumVersion(8, 1) && GetMetaType() == PGM_DATABASE)
			return (server->GetCreatePrivilege() || server->GetSuperUser());
		else
			return server->GetSuperUser();
	}
}


pgObject *pgServerFactory::CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr)
{
    long numServers=settings->Read(wxT("Servers/Count"), 0L);

    long loop, port, ssl=0;
    wxString key, servername, description, database, username, lastDatabase, lastSchema, storePwd, restore, serviceID, dbRestriction;
    pgServer *server=0;

    wxArrayString servicedServers;

	// Get the hostname for later...
	char buf[255];
	gethostname(buf, 255); 
    wxString hostname = wxString(buf, wxConvUTF8);

    for (loop = 1; loop <= numServers; ++loop)
    {
        key.Printf(wxT("Servers/%d/"), loop);
        
        settings->Read(key + wxT("Server"), &servername, wxEmptyString);
        settings->Read(key + wxT("ServiceID"), &serviceID, wxEmptyString);
        settings->Read(key + wxT("Description"), &description, wxEmptyString);
        settings->Read(key + wxT("StorePwd"), &storePwd, wxEmptyString);
	    settings->Read(key + wxT("Restore"), &restore, wxT("true"));
        settings->Read(key + wxT("Port"), &port, 0);
        settings->Read(key + wxT("Database"), &database, wxEmptyString);
        settings->Read(key + wxT("Username"), &username, wxEmptyString);
        settings->Read(key + wxT("LastDatabase"), &lastDatabase, wxEmptyString);
        settings->Read(key + wxT("LastSchema"), &lastSchema, wxEmptyString);
        settings->Read(key + wxT("DbRestriction"), &dbRestriction, wxEmptyString);

        // SSL mode
#ifdef SSL
        settings->Read(key + wxT("SSL"), &ssl, 0);
#endif

        // Add the Server node
        server = new pgServer(servername, description, database, username, port, StrToBool(storePwd), StrToBool(restore), ssl);
        server->iSetLastDatabase(lastDatabase);
        server->iSetLastSchema(lastSchema);
        server->iSetServiceID(serviceID);
		server->iSetDiscovered(false);
        server->iSetDbRestriction(dbRestriction);
        server->iSetServerIndex(loop);
        browser->AppendItem(obj->GetId(), server->GetFullName(), server->GetIconId(), -1, server);
	browser->SortChildren(obj->GetId());


#ifdef WIN32
        int bspos = serviceID.Find('\\');
        if (bspos >= 0)
        {
            if (serviceID.Left(2) != wxT(".\\") && !serviceID.Matches(wxGetHostName() + wxT("\\*")))
                serviceID = wxEmptyString;
        }
        if (!serviceID.IsEmpty())
            servicedServers.Add(serviceID);
#endif

    }

#ifdef WIN32

	// Add local servers. Will currently only work on Win32 with >= BETA3 
	// of the Win32 PostgreSQL installer.
	wxRegKey *pgKey = new wxRegKey(wxT("HKEY_LOCAL_MACHINE\\Software\\PostgreSQL\\Services"));

	if (pgKey->Exists())
	{

		wxString svcName, temp;
		long cookie = 0;
		long *tmpport = 0;
		bool flag = false;

		flag = pgKey->GetFirstKey(svcName, cookie);

		while (flag != false)
		{
            if (servicedServers.Index(svcName, false) < 0)
            {
			    key.Printf(wxT("HKEY_LOCAL_MACHINE\\Software\\PostgreSQL\\Services\\%s"), svcName);
			    wxRegKey *svcKey = new wxRegKey(key);

                servername = wxT("localhost");
                database = wxEmptyString;
			    svcKey->QueryValue(wxT("Display Name"), description);
			    svcKey->QueryValue(wxT("Database Superuser"), username);
                svcKey->QueryValue(wxT("Port"), &port);

			    // Add the Server node
			    server = new pgServer(servername, description, database, username, port, false, 0);
			    server->iSetDiscovered(true);
			    server->iSetServiceID(svcName);
			    browser->AppendItem(obj->GetId(), server->GetFullName(), server->GetIconId(), -1, server);
			    browser->SortChildren(obj->GetId());
            }
			// Get the next one...
			flag = pgKey->GetNextKey(svcName, cookie);
		}
	}
#endif //WIN32

    return server;
}


#include "images/servers.xpm"
#include "images/server.xpm"
#include "images/server-sm.xpm"
#include "images/serverbad.xpm"
#include "images/serverbad-sm.xpm"

pgServerFactory::pgServerFactory() 
: pgaFactory(__("Server"), __("New Server Registration"), __("Create a new Server registration."), server_xpm, server_sm_xpm)
{
    metaType = PGM_SERVER;
    closedId = addIcon(serverbad_xpm);
    smallClosedId = addIcon(serverbad_sm_xpm);
}

pgCollection *pgServerFactory::CreateCollection(pgObject *obj)
{
    return new pgCollection(GetCollectionFactory());
}

pgCollection *pgServerObjFactory::CreateCollection(pgObject *obj)
{
    return new pgServerObjCollection(GetCollectionFactory(), (pgServer*)obj);
}

pgServerFactory serverFactory;
static pgaCollectionFactory cf(&serverFactory, __("Servers"), servers_xpm);

#include "images/connect.xpm"
addServerFactory::addServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&Add Server..."), _("Add a connection to a server."));
    toolbar->AddTool(id, _("Add Server"), wxBitmap(connect_xpm), _("Add a connection to a server."), wxITEM_NORMAL);
}


wxWindow *addServerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    int rc = PGCONN_BAD;
    
    dlgServer dlg(&serverFactory, form, 0);
    dlg.CenterOnParent();

    ctlTree *browser=form->GetBrowser();

    while (rc != PGCONN_OK)
    {
        if (dlg.GoNew() != wxID_OK)
            return 0;

        pgServer *server=(pgServer*)dlg.CreateObject(0);

        if (dlg.GetTryConnect())
        {
            wxBusyInfo waiting(wxString::Format(_("Connecting to server %s (%s:%d)"),
                server->GetDescription().c_str(), server->GetName().c_str(), server->GetPort()), form);
            rc = server->Connect(form, false, dlg.GetPassword(), true);
        }
        else
        {
            rc = PGCONN_OK;
            server->InvalidatePassword();
        }
        switch (rc)
        {
            case PGCONN_OK:
            {
                int icon;
                if (server->GetConnected())
                    icon = serverFactory.GetIconId();
                else
                    icon = serverFactory.GetClosedIconId();
                wxLogInfo(wxT("pgServer object initialised as required."));
                browser->AppendItem(form->GetServerCollection()->GetId(), server->GetFullName(), 
                    icon, -1, server);
		browser->SortChildren(form->GetServerCollection()->GetId());

                browser->Expand(form->GetServerCollection()->GetId());
                wxString label;
                label.Printf(_("Servers (%d)"), form->GetBrowser()->GetChildrenCount(form->GetServerCollection()->GetId(), false));
                browser->SetItemText(form->GetServerCollection()->GetId(), label);
                form->StoreServers();
                return 0;
            }
            case PGCONN_DNSERR:
            {
                delete server;
                break;
            }
            case PGCONN_BAD:
            case PGCONN_BROKEN:
            {
                form->ReportConnError(server);
                delete server;

                break;
            }
            default:
            {
                wxLogInfo(__("pgServer object didn't initialise because the user aborted."));
                delete server;
                return 0;
            }
        }
    }
    return 0;
}


startServiceFactory::startServiceFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Start Service"), _("Start PostgreSQL Service"));
}


wxWindow *startServiceFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server= (pgServer*)obj;
    form->StartMsg(_("Starting Service"));
    bool rc = server->StartService();
    if (rc)
        form->execSelChange(server->GetId(), true);
    form->EndMsg(rc);
    return 0;
}


bool startServiceFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
    {
        pgServer *server=(pgServer*)obj;
        return server->GetServerControllable() && !server->GetServerRunning();
    }
    return false;
}


stopServiceFactory::stopServiceFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Stop Service"), _("Stop PostgreSQL Service"));
}


wxWindow *stopServiceFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server= (pgServer*)obj;
	wxMessageDialog msg(form, _("Are you sure you wish to shutdown this server?"),
            _("Stop Service"), wxYES_NO | wxICON_QUESTION);
    if (msg.ShowModal() == wxID_YES)
    {
        form->StartMsg(_("Stopping service"));

        bool done=server->StopService();

        if (done)
	    {
            if (server->Disconnect(form))
            {
                form->GetBrowser()->DeleteChildren(server->GetId());
                form->execSelChange(server->GetId(), true);
            }
	    }
        form->EndMsg(done);
    }
    return 0;
}


bool stopServiceFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
    {
        pgServer *server=(pgServer*)obj;
        return server->GetServerControllable() && server->GetServerRunning();
    }
    return false;
}


connectServerFactory::connectServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Connect"), _("Connect to the selected server."));
}


wxWindow *connectServerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server = (pgServer *)obj;
    form->ReconnectServer(server);
    return 0;
}


bool connectServerFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
        return !((pgServer*)obj)->GetConnected();

    return false;
}


disconnectServerFactory::disconnectServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Disconnec&t"), _("Disconnect from the selected server."));
}


wxWindow *disconnectServerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server=(pgServer*)obj;
    server->Disconnect(form);
    server->UpdateIcon(form->GetBrowser());
    form->GetBrowser()->DeleteChildren(obj->GetId());
    form->execSelChange(obj->GetId(), true);
    return 0;
}


bool disconnectServerFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
        return ((pgServer*)obj)->GetConnected();

    return false;
}
