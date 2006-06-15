//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id: pgAgent.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAgent.cpp - pgAgent main entry
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"

#ifndef __WXMSW__
#include <unistd.h>
#endif

wxString connectString;
wxString serviceDBname;
wxString backendPid;
long longWait=30;
long shortWait=10;
long minLogLevel=LOG_ERROR;

#ifndef __WXMSW__
bool runInForeground = false;
wxString logFile = wxEmptyString;
#endif

int MainRestartLoop(DBconn *serviceConn)
{
    // clean up old jobs

    int rc;

    LogMessage(_("Clearing zombies"), LOG_DEBUG);
    rc=serviceConn->ExecuteVoid(
        wxT("CREATE TEMP TABLE pga_tmp_zombies(jagpid int4)"));

    rc=serviceConn->ExecuteVoid(wxT("DELETE FROM pga_tmp_zombies"));

    rc = serviceConn->ExecuteVoid(
        wxT("INSERT INTO pga_tmp_zombies (jagpid) ")
        wxT("SELECT jagpid ")
        wxT("  FROM pgagent.pga_jobagent AG ")
        wxT("  LEFT JOIN pg_stat_activity PA ON jagpid=procpid ")
        wxT(" WHERE procpid IS NULL")
        );

    if (rc > 0)
    {
        // There are orphaned agent entries
        // mark the jobs as aborted
        rc=serviceConn->ExecuteVoid(
            wxT("UPDATE pgagent.pga_joblog SET jlgstatus='d' WHERE jlgid IN (")
            wxT("SELECT jlgid ")
            wxT("FROM pga_tmp_zombies z, pgagent.pga_job j, pgagent.pga_joblog l ")
            wxT("WHERE z.jagpid=j.jobagentid AND j.jobid = l.jlgjobid AND l.jlgstatus='r');\n")

            wxT("UPDATE pgagent.pga_jobsteplog SET jslstatus='d' WHERE jslid IN ( ")
            wxT("SELECT jslid ")
            wxT("FROM pga_tmp_zombies z, pgagent.pga_job j, pgagent.pga_joblog l, pgagent.pga_jobsteplog s ")
            wxT("WHERE z.jagpid=j.jobagentid AND j.jobid = l.jlgjobid AND l.jlgid = s.jsljlgid AND s.jslstatus='r');\n")

            wxT("UPDATE pgagent.pga_job SET jobagentid=NULL, jobnextrun=NULL ")
            wxT("  WHERE jobagentid IN (SELECT jagpid FROM pga_tmp_zombies);\n")

            wxT("DELETE FROM pgagent.pga_jobagent ")
            wxT("  WHERE jagpid IN (SELECT jagpid FROM pga_tmp_zombies);\n")
            );
    }

    wxString hostname = wxGetFullHostName();

    rc=serviceConn->ExecuteVoid(
        wxT("INSERT INTO pgagent.pga_jobagent (jagpid, jagstation) SELECT pg_backend_pid(), '") + hostname + wxT("'"));
    if (rc < 0)
        return rc;

    while (1)
    {
        bool foundJobToExecute=false;

        LogMessage(_("Checking for jobs to run"), LOG_DEBUG);
        DBresult *res=serviceConn->Execute(
            wxT("SELECT J.jobid ")
            wxT("  FROM pgagent.pga_job J ")
            wxT(" WHERE jobenabled ")
            wxT("   AND jobagentid IS NULL ")
            wxT("   AND jobnextrun <= now() ")
            wxT("   AND jobhostagent = '' OR jobhostagent = '") + hostname + wxT("'")
            wxT(" ORDER BY jobnextrun"));

        if (res)
        {
            while(res->HasData())
            {
                wxString jobid=res->GetString(wxT("jobid"));

                JobThread *jt = new JobThread(jobid);

                if (jt->Runnable())
                {
                    jt->Create();
                    jt->Run();
                    foundJobToExecute = true;
                }
                res->MoveNext();

            }

            delete res;
            LogMessage(_("Sleeping..."), LOG_DEBUG);
            WaitAWhile();
        }
        else
        {
            LogMessage(_("Failed to query jobs table!"), LOG_ERROR);
        }
        if (!foundJobToExecute)
            DBconn::ClearConnections();
    }
    return 0;
}


void MainLoop()
{
    // OK, let's get down to business
    do
    {
        LogMessage(_("Creating primary connection"), LOG_DEBUG);
        DBconn *serviceConn=DBconn::InitConnection(connectString);

        if (serviceConn && serviceConn->IsValid())
        {
            serviceDBname = serviceConn->GetDBname();

            // Basic sanity check, and a chance to get the serviceConn's PID
            LogMessage(_("Database sanity check"), LOG_DEBUG);
            DBresult *res=serviceConn->Execute(wxT("SELECT count(*) As count, pg_backend_pid() AS pid FROM pg_class cl JOIN pg_namespace ns ON ns.oid=relnamespace WHERE relname='pga_job' AND nspname='pgagent'"));
            if (res)
            {
                wxString val=res->GetString(wxT("count"));

                if (val == wxT("0"))
                    LogMessage(_("Could not find the table 'pgagent.pga_job'. Have you run pgagent.sql on this database?"), LOG_ERROR);

                backendPid=res->GetString(wxT("pid"));
            }

            MainRestartLoop(serviceConn);
        }

        LogMessage(wxString::Format(_("Couldn't create connection: %s"), serviceConn->GetLastError().c_str()), LOG_WARNING);
        DBconn::ClearConnections(true);
        WaitAWhile(true);
    }
    while (1);
}
