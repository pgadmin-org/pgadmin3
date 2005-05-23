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

wxSemaphore *getDb;

Job::Job(DBconn *conn, const wxString &jid)
{
    threadConn=conn;
    jobid=jid;
    status=wxT("");

	LogMessage(_("Starting job: ") + jobid, LOG_DEBUG);

    int rc=threadConn->ExecuteVoid(
        wxT("UPDATE pgagent.pga_job SET jobagentid=") + backendPid + wxT(", joblastrun=now() ")
        wxT(" WHERE jobagentid IS NULL AND jobid=") + jobid);

    if (rc == 1)
    {
		DBresult *id=threadConn->Execute(
			wxT("SELECT nextval('pgagent.pga_joblog_jlgid_seq') AS id"));
		if (id)
		{
			logid=id->GetString(wxT("id"));

			DBresult *res=threadConn->Execute(
				wxT("INSERT INTO pgagent.pga_joblog(jlgid, jlgjobid, jlgstatus) ")
	            wxT("VALUES (") + logid + wxT(", ") + jobid + wxT(", 'r')"));
			if (res)
			{
				status=wxT("r");
	            delete res;
			}
			delete id;
		}
    }
}


Job::~Job()
{
    if (status != wxT(""))
    {
        threadConn->ExecuteVoid(
            wxT("UPDATE pgagent.pga_joblog ")
            wxT("   SET jlgstatus='") + status + wxT("', jlgduration=now() - jlgstart ")
            wxT(" WHERE jlgid=") + logid + wxT(";\n")

            wxT("UPDATE pgagent.pga_job ")
            wxT("   SET jobagentid=NULL, jobnextrun=NULL ")
            wxT(" WHERE jobid=") + jobid
            );
    }
	threadConn->Return();

	LogMessage(_("Completed job: ") + jobid, LOG_DEBUG);
}


int Job::Execute()
{
    int rc=0;
    DBresult *steps=threadConn->Execute(
        wxT("SELECT jstid, jstkind, jstdbname, jstcode, jstonerror ")
        wxT("  FROM pgagent.pga_jobstep ")
        wxT(" WHERE jstenabled ")
        wxT("   AND jstjobid=") + jobid +
        wxT(" ORDER BY jstname, jstid"));

    if (!steps)
    {
        status=wxT("i");
        return -1;
    }

    while (steps->HasData())
    {
        DBconn *stepConn;
        wxString jslid, stepid, jpecode, output;

        stepid = steps->GetString(wxT("jstid"));
        
		DBresult *id=threadConn->Execute(
			wxT("SELECT nextval('pgagent.pga_jobsteplog_jslid_seq') AS id"));
		if (id)
		{
			jslid=id->GetString(wxT("id"));
			DBresult *res=threadConn->Execute(
				wxT("INSERT INTO pgagent.pga_jobsteplog(jslid, jsljlgid, jsljstid, jslstatus) ")
				wxT("SELECT ") + jslid + wxT(", ") + logid + wxT(", ") + stepid + wxT(", 'r'")
				wxT("  FROM pgagent.pga_jobstep WHERE jstid=") + stepid);

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
            status=wxT("i");
            return -1;
        }

        switch ((int) steps->GetString(wxT("jstkind"))[0])
        {
            case 's':
            {
                stepConn=DBconn::Get(steps->GetString(wxT("jstdbname")));
                if (stepConn)
                {
                    LogMessage(_("Executing step ") + stepid + _(" (part of job ") + jobid + wxT(")"), LOG_DEBUG);
                    rc=stepConn->ExecuteVoid(steps->GetString(wxT("jstcode")));
					output = stepConn->GetLastError();
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
                status=wxT("i");
                return -1;
            }
        }

        wxString stepstatus;
        if (rc >= 0)
            stepstatus = wxT("s");
        else
            stepstatus = steps->GetString(wxT("jstonerror"));

        rc=threadConn->ExecuteVoid(
            wxT("UPDATE pgagent.pga_jobsteplog ")
            wxT("   SET jslduration = now() - jslstart, ")
            wxT("       jslresult = ") + NumToStr(rc) + wxT(", jslstatus = '") + stepstatus + wxT("', ")
			wxT("       jsloutput = ") + qtString(output) + wxT(" ")
            wxT(" WHERE jslid=") + jslid);
        if (rc != 1 || stepstatus == wxT("f"))
        {
            status = wxT("f");
            return -1;
        }
        steps->MoveNext();
    }
    delete steps;

    status = wxT("s");
    return 0;
}



JobThread::JobThread(const wxString &jid)  
: wxThread(wxTHREAD_DETACHED)
{ 
	LogMessage(_("Creating job thread for job ") + jid, LOG_DEBUG); 
	
	runnable = false;
	jobid = jid; 

	DBconn *threadConn=DBconn::Get(serviceDBname);
    job = new Job(threadConn, jobid);

    if (job->Runnable())
        runnable = true;

}
    

JobThread::~JobThread() 
{ 
	LogMessage(_("Destroying job thread for job ") + jobid, LOG_DEBUG); 
}


void *JobThread::Entry()
{
	if (runnable)
	{
		job->Execute();
		delete job;
	}

	return(NULL);
}
