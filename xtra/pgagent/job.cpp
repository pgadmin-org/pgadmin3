//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// job.cpp - pgAgent job
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"



Job::Job(DBconn *conn, const string &jid)
{
    threadConn=conn;
    jobid=jid;
    status="";

    int rc=threadConn->ExecuteVoid(
        "UPDATE pgagent.pga_job SET jobagentid=" + backendPid + ", joblastrun=now() "
        " WHERE jobagentid IS NULL AND jobid=" + jobid);

    if (rc == 1)
    {
		DBresult *id=threadConn->Execute(
			"SELECT nextval('pgagent.pga_joblog_jlgid_seq') AS id");
		if (id)
		{
			logid=id->GetString("id");

			DBresult *res=threadConn->Execute(
				"INSERT INTO pgagent.pga_joblog(jlgid, jlgjobid, jlgstatus) "
	            "VALUES (" + logid + ", " + jobid + ", 'r')");
			if (res)
			{
				status="r";
	            delete res;
			}
			delete id;
		}
    }
}


Job::~Job()
{
    if (status != "")
    {
        threadConn->ExecuteVoid(
            "UPDATE pgagent.pga_joblog "
            "   SET jlgstatus='" + status + "', jlgduration=now() - jlgstart "
            " WHERE jlgid=" + logid + ";\n"

            "UPDATE pgagent.pga_job "
            "   SET jobagentid=NULL, jobnextrun=NULL "
            " WHERE jobid=" + jobid
            );
    }
	threadConn->Return();
}


int Job::Execute()
{
    int rc=0;
    DBresult *steps=threadConn->Execute(
        "SELECT jstid, jstkind, jstdbname, jstcode, jstonerror "
        "  FROM pgagent.pga_jobstep "
        " WHERE jstenabled "
        "   AND jstjobid=" + jobid +
        " ORDER BY jstname, jstid");

    if (!steps)
    {
        status='i';
        return -1;
    }

    while (steps->HasData())
    {
        DBconn *stepConn;
        string jslid, stepid, jpecode;

        stepid = steps->GetString("jstid");
        
		DBresult *id=threadConn->Execute(
			"SELECT nextval('pgagent.pga_jobsteplog_jslid_seq') AS id");
		if (id)
		{
			jslid=id->GetString("id");
			DBresult *res=threadConn->Execute(
				"INSERT INTO pgagent.pga_jobsteplog(jslid, jsljlgid, jsljstid, jslstatus) "
				"SELECT " + jslid + ", " + logid + ", " + stepid + ", 'r'"
				"  FROM pgagent.pga_jobstep WHERE jstid=" + stepid);

			if (res)
			{
				rc=res->RowsAffected();
				delete res;
			}
			else
	            rc = -1;
		}
		delete id;

        if (rc != 1)
        {
            status='i';
            return -1;
        }

        switch ((int) steps->GetString("jstkind")[0])
        {
            case 's':
            {
                stepConn=DBconn::Get(steps->GetString("jstdbname"));
                if (stepConn)
                {
                    LogMessage("Executing step " + stepid + " on database " + steps->GetString("jstdbname"), LOG_DEBUG);
                    rc=stepConn->ExecuteVoid(steps->GetString("jstcode"));
					stepConn->Return();
                }
                else
                    rc=-1;

                break;
            }
            case 'b':
            {
                // batch not jet implemented
                break;
            }
            default:
            {
                status='i';
                return -1;
            }
        }

        string stepstatus;
        if (rc >= 0)
            stepstatus = "s";
        else
            stepstatus = steps->GetString("jstonerror");

        rc=threadConn->ExecuteVoid(
            "UPDATE pgagent.pga_jobsteplog "
            "   SET jslduration = now() - jslstart, "
            "       jslresult = " + NumToStr(rc) + ", jslstatus = '" + stepstatus + "' "
            " WHERE jslid=" + jslid);
        if (rc != 1 || stepstatus == "f")
        {
            status = 'f';
            return -1;
        }
        steps->MoveNext();
    }
    delete steps;

    status = 's';
    return 0;
}
