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
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/process.h>

#ifdef WIN32
#define popen _popen
#define pclose _pclose
#endif


Job::Job(DBconn *conn, const wxString &jid)
{
    threadConn=conn;
    jobid=jid;
    status=wxT("");

    LogMessage(wxString::Format(_("Starting job: %s"), jobid.c_str()), LOG_DEBUG);

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

    LogMessage(wxString::Format(_("Completed job: %s"), jobid.c_str()), LOG_DEBUG);
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
                    LogMessage(wxString::Format(_("Executing SQL step %s (part of job %s)"), stepid.c_str(), jobid.c_str()), LOG_DEBUG);
                    rc=stepConn->ExecuteVoid(steps->GetString(wxT("jstcode")));
                    output = stepConn->GetLastError();
                    stepConn->Return();
                }
                else
                {
                    output = _("Couldn't get a connection to the database!");
                    rc=-1;
                }


                break;
            }
            case 'b':
            {
                // Batch jobs are more complex thank SQL, for obvious reasons...
                LogMessage(wxString::Format(_("Executing batch step %s (part of job %s)"), stepid.c_str(), jobid.c_str()), LOG_DEBUG);

                // Get a temporary filename, then reuse it to create an empty directory.
                wxString dirname = wxFileName::CreateTempFileName(wxT("pga_"));
                if (dirname.Length() == 0)
                {
                    output = _("Couldn't get a temporary filename!");
                    LogMessage(_("Couldn't get a temporary filename!"), LOG_WARNING);
                    rc=-1;
                    break;
                }

                ;
                if (!wxRemoveFile(dirname))
                {
                    output.Printf(_("Couldn't remove temporary file: %s"), dirname.c_str());
                    LogMessage(output, LOG_WARNING);
                    rc=-1;
                    break;
                }

                if (!wxMkdir(dirname, 0700))
                {
                    output.Printf(_("Couldn't create temporary directory: %s"), dirname.c_str());
                    LogMessage(output, LOG_WARNING);
                    rc=-1;
                    break;
                }

#ifdef WIN32
                wxString filename = dirname + wxT("\\") + jobid + wxT("_") + stepid + wxT(".bat");
#else
                wxString filename = dirname + wxT("/") + jobid + wxT("_") + stepid + wxT(".scr");
#endif

                // Write the script
                wxFile *file = new wxFile();

                if (!file->Create(filename, true, wxS_IRUSR | wxS_IWUSR | wxS_IXUSR))
                {
                    output.Printf(_("Couldn't create temporary script file: %s"), filename.c_str());
                    LogMessage(output, LOG_WARNING);
                    rc=-1;
                    break;
                }

                if (!file->Open(filename, wxFile::write))
                {
                    output.Printf(_("Couldn't open temporary script file: %s"), filename.c_str());
                    LogMessage(output, LOG_WARNING);
                    wxRemoveFile(filename);
                    wxRmdir(dirname);
                    rc=-1;
                    break;
                }

                wxString code = steps->GetString(wxT("jstcode"));

                // Cleanup the code. If we're on Windows, we need to make all line ends \r\n, 
                // If we're on Unix, we need \n
                code.Replace(wxT("\r\n"), wxT("\n"));
#ifdef WIN32
                code.Replace(wxT("\n"), wxT("\r\n"));
#endif

                if (!file->Write(code))
                {
                    output.Printf(_("Couldn't write to temporary script file: %s"), filename.c_str());
                    LogMessage(output, LOG_WARNING);
                    wxRemoveFile(filename);
                    wxRmdir(dirname);
                    rc=-1;
                    break;
                }

                file->Close();
                LogMessage(wxString::Format(_("Executing script file: %s"), filename.c_str()), LOG_DEBUG);

                // Execute the file and capture the output
                FILE *fp_script;
                char buf[128];
                fp_script = popen(filename.mb_str(wxConvUTF8), "r");
                if (!fp_script)
                {
                    output.Printf(_("Couldn't execute script: %s"), filename.c_str());
                    LogMessage(output, LOG_WARNING);
                    wxRemoveFile(filename);
                    wxRmdir(dirname);
                    rc=-1;
                    break;
                }


               while(!feof(fp_script))
               {
                   if (fgets(buf, 128, fp_script) != NULL)
                       output += wxString::FromAscii(buf);
               }

               rc=pclose(fp_script);
		rc = rc / 256; // Get the return value out of the top 8 bits

                // Delete the file/directory. If we fail, don't overwrite the script output in the log, just throw warnings.
                if (!wxRemoveFile(filename))
                {
                    LogMessage(wxString::Format(_("Couldn't remove temporary script file: %s"), filename.c_str()), LOG_WARNING);
                    wxRmdir(dirname);
                    break;
                }

                if (!wxRmdir(dirname))
                {
                    LogMessage(wxString::Format(_("Couldn't remove temporary directory: "), dirname.c_str()), LOG_WARNING);
                    break;
                }

                break;
            }
            default:
            {
                output = _("Invalid step type!");
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
    LogMessage(wxString::Format(_("Creating job thread for job %s"), jid.c_str()), LOG_DEBUG);

    runnable = false;
    jobid = jid;

    DBconn *threadConn=DBconn::Get(serviceDBname);
    job = new Job(threadConn, jobid);

    if (job->Runnable())
        runnable = true;

}


JobThread::~JobThread()
{
    LogMessage(wxString::Format(_("Destroying job thread for job %s"), jobid.c_str()), LOG_DEBUG);
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
