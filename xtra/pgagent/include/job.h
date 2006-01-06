//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2002 - 2006 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// job.h - agent job
//
//////////////////////////////////////////////////////////////////////////


#ifndef JOB_H
#define JOB_H

#include <wx/wx.h>


class Job
{
public:
    Job(DBconn *conn, const wxString &jid);
    ~Job();

    int Execute();
    bool Runnable() { return status == wxT("r"); }

protected:
    DBconn *threadConn;
    wxString jobid, logid;
    wxString status;
};


class JobThread : public wxThread
{
public:
    JobThread(const wxString &jid);
    ~JobThread();
	bool Runnable() { return runnable; }

	virtual void *Entry();

private:
	wxString jobid;
	bool runnable;
	Job *job;
};

#endif // JOB_H

