//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// job.h - agent job
//
//////////////////////////////////////////////////////////////////////////


#ifndef JOB_H
#define JOB_H

class Job
{
public:
    Job(DBconn *conn, const string &jid);
    ~Job();

    int Execute();
    bool Runnable() { return status == "r"; }

protected:
    DBconn *serviceConn;
    string jobid, prtid;
    string status;
};

#endif // JOB_H

