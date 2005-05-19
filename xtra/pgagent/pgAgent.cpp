//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAgent.cpp - pgAgent main entry
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"

#include <wx/thread.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

string connectString;
string serviceDBname;
string backendPid;
long longWait=30;
long shortWait=10;
long minLogLevel=LOG_ERROR;


int MainRestartLoop(DBconn *serviceConn)
{
    // clean up old jobs

    int rc;

	LogMessage("Clearing zombies", LOG_DEBUG);
    rc=serviceConn->ExecuteVoid(
        "CREATE TEMP TABLE pga_tmp_zombies(jagpid int4)");

    rc=serviceConn->ExecuteVoid("DELETE FROM pga_tmp_zombies");

    rc = serviceConn->ExecuteVoid(
        "INSERT INTO pga_tmp_zombies (jagpid) "
        "SELECT jagpid "
        "  FROM pgagent.pga_jobagent AG "
        "  LEFT JOIN pg_stat_activity PA ON jagpid=procpid "
        " WHERE procpid IS NULL"
        );

    if (rc > 0)
    {
        // There are orphaned agent entries
        // mark the jobs as aborted
        rc=serviceConn->ExecuteVoid(
            "UPDATE pgagent.pga_joblog SET jlgstatus='d' "
            "  FROM pga_tmp_zombies Z "
            "  JOIN pgagent.pga_job J ON jobagentid=jagpid "
            "  JOIN pgagent.pga_joblog LG ON jlgjobid=J.jobid "
            " WHERE LG.jlgstatus='r';\n"

            "UPDATE pgagent.pga_job SET jobagentid=NULL, jobnextrun=NULL "
            "  FROM pga_tmp_zombies Z "
            "  JOIN pgagent.pga_job J ON jobagentid=jagpid;\n"
            
            "DELETE FROM pgagent.pga_jobagent "
            "  WHERE jagpid IN (SELECT jagpid FROM pga_tmp_zombies);\n"
            );
    }

    char hostname[255];
    gethostname(hostname, 255);

    rc=serviceConn->ExecuteVoid(
        "INSERT INTO pgagent.pga_jobagent (jagpid, jagstation) SELECT pg_backend_pid(), '" + string(hostname) + "'");
    if (rc < 0)
        return rc;

    while (1)
    {
        bool foundJobToExecute=false;

		LogMessage("Checking for jobs to run", LOG_DEBUG);
        DBresult *res=serviceConn->Execute(
            "SELECT J.jobid "
            "  FROM pgagent.pga_job J "
            " WHERE jobenabled "
            "   AND jobagentid IS NULL "
            "   AND jobnextrun <= now() "
            "   AND jobhostagent = '' OR jobhostagent = '" + string(hostname) + "'"
            " ORDER BY jobnextrun");

        if (res)
        {
            string jobid=res->GetString("jobid");
            delete res;

            if (jobid != "")
            {
				DBconn *threadConn=DBconn::Get(serviceDBname);
                Job job(threadConn, jobid);

                if (job.Runnable())
                {
                    foundJobToExecute=true;
                    LogMessage("Running job: " + jobid, LOG_DEBUG);
                    job.Execute();
					LogMessage("Completed job: " + jobid, LOG_DEBUG);
                }
            }
            else
            {
				LogMessage("No jobs to run - sleeping...", LOG_DEBUG);
                WaitAWhile();
            }
        }
        else
        {
            LogMessage("Failed to query jobs table!", LOG_ERROR);
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
	    LogMessage("Creating primary connection", LOG_DEBUG);
        DBconn *serviceConn=DBconn::InitConnection(connectString);
    
		if (serviceConn && serviceConn->IsValid())
        {
            serviceDBname = serviceConn->GetDBname();

			// Basic sanity check, and a chance to get the serviceConn's PID
	        LogMessage("Database sanity check", LOG_DEBUG);
            DBresult *res=serviceConn->Execute("SELECT count(*) As count, pg_backend_pid() AS pid FROM pg_class cl JOIN pg_namespace ns ON ns.oid=relnamespace WHERE relname='pga_job' AND nspname='pgagent'");
            if (res)
			{
                string val=res->GetString("count");
            
                if (val == "0")
                    LogMessage("Could not find the table 'pgagent.pga_job'. Have you run pgagent.sql on this database?", LOG_ERROR);
    
	    	    backendPid=res->GetString("pid");
			}
        
            MainRestartLoop(serviceConn);
        }

		LogMessage("Couldn't create connection: " + serviceConn->GetLastError(), LOG_WARNING);
        DBconn::ClearConnections(true);
        WaitAWhile(true);
    }
    while (1);
}
