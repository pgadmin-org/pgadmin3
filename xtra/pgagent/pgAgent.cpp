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

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

string connectString;
string serviceDBname;
long longWait=30;
long shortWait=10;




int MainRestartLoop(DBconn *serviceConn)
{
    // clean up old jobs

    int rc;

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
            "UPDATE pgagent.pga_jobprotocol SET jprstatus='d' "
            "  FROM pga_tmp_zombies Z "
            "  JOIN pgagent.pga_job J ON jobagentid=jagpid "
            "  JOIN pgagent.pga_jobprotocol PR ON jprjobid=J.jobid "
            " WHERE PR.jprstatus='r';\n"

            "UPDATE pgagent.pga_job SET jobagentid=NULL, jobnextrun=NULL "
            "  FROM pga_tmp_zombies Z "
            "  JOIN pgagent.pga_job J ON jobagentid=jagpid;\n"
            
            "DELETE FROM pgagent.pga_jobagent "
            "  WHERE jagpid IN (SELECT jagpid FROM pga_tmp_zombies);\n"
            );
    }



	// TODO - station should be the hostname, not '' (?)
	char hostname[255];
	gethostname(hostname, 255);

    rc=serviceConn->ExecuteVoid(
        "INSERT INTO pgagent.pga_jobagent (jagpid, station) SELECT pg_backend_pid(), '" + string(hostname) + "'");
    if (rc < 0)
        return rc;

    while (1)
    {
        bool foundJobToExecute=false;

        DBresult *res=serviceConn->Execute(
            "SELECT J.jobid "
            "  FROM pgagent.pga_job J "
            " WHERE jobenabled AND jobagentid IS NULL "
            "   AND jobnextrun <= now() "
            " ORDER BY jobnextrun");

        if (res)
        {
            string jobid=res->GetString("jobid");
            delete res;

            if (jobid != "")
            {
                Job job(serviceConn, jobid);

                if (job.Runnable())
                {
                    foundJobToExecute=true;
                    job.Execute();
                }
            }
            else
            {
                WaitAWhile();
            }
        }
        else
        {
            // bad err
        }
        if (!foundJobToExecute)
            DBconn::ClearConnections();
    }
    return 0;
}


void MainLoop()
{
    do
    {
        DBconn *serviceConn=DBconn::Get(serviceDBname, true);

        if (serviceConn)
        {
            MainRestartLoop(serviceConn);
        }

        DBconn::ClearConnections(true);
        WaitAWhile(true);
    }
    while (1);
}