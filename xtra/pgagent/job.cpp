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
    serviceConn=conn;
    jobid=jid;
    status="";

    int rc=serviceConn->ExecuteVoid(
        "UPDATE pgagent.pga_job SET jobagentid=pg_backend_pid(), joblastrun=now() "
        " WHERE jobagentid IS NULL AND jobid=" + jobid);

    if (rc == 1)
    {
		DBresult *id=serviceConn->Execute(
			"SELECT nextval('pgagent.pga_joblog_jlgid_seq') AS id");
		if (id)
		{
			logid=id->GetString("id");

			DBresult *res=serviceConn->Execute(
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
        serviceConn->ExecuteVoid(
            "UPDATE pgagent.pga_joblog "
            "   SET jlgstatus='" + status + "', jlgduration=now() - jlgstart "
            " WHERE jlgid=" + logid + ";\n"

            "UPDATE pgagent.pga_job "
            "   SET jobagentid=NULL, jobnextrun=NULL "
            " WHERE jobid=" + jobid
            );
    }
}


int Job::Execute()
{
    int rc=0;
    DBresult *steps=serviceConn->Execute(
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
        DBconn *conn;
        string jpsid, jpecode;

		DBresult *id=serviceConn->Execute(
			"SELECT nextval('pgagent.pga_jobsteplog_jslid_seq') AS id");
		if (id)
		{
			jpsid=id->GetString("id");
			DBresult *res=serviceConn->Execute(
				"INSERT INTO pgagent.pga_jobsteplog(jslid, jsljlgid, jslstatus) "
				"SELECT " + jpsid + ", " + jobid + ", 'r'"
				"  FROM pgagent.pga_jobstep WHERE jstid=" + steps->GetString("jstid"));

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
                conn=DBconn::Get(steps->GetString("jstdbname"));
                if (conn)
                {
                    LogMessage("Executing step " + steps->GetString("jstid") + " on database " + steps->GetString("jstdbname"), LOG_DEBUG);
                    rc=conn->ExecuteVoid(steps->GetString("jstcode"));
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

        rc=serviceConn->ExecuteVoid(
            "UPDATE pgagent.pga_jobsteplog "
            "   SET jslduration = now() - jslstart, "
            "       jslresult = " + NumToStr(rc) + ", jslstatus = '" + stepstatus + "' "
            " WHERE jslid=" + jpsid);
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
