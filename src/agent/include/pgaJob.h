//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaJob.h - PostgreSQL Agent Job
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAJOB_H
#define PGAJOB_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
#include "pgaAgent.h"
// Class declarations


class pgaJob : public pgDatabaseObject
{
public:
    pgaJob(const wxString& newName = wxT(""));
    ~pgaJob();

    int GetIcon() { return enabled ? PGAICON_JOB : PGAICON_JOBDISABLED; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxEmptyString);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

    bool GetEnabled() const { return enabled; }
    void iSetEnabled(const bool b) { enabled=b; }
    wxDateTime GetCreated() const { return created; }
    void iSetCreated(const wxDateTime &d) { created=d; }
    wxDateTime GetChanged() const { return changed; }
    void iSetChanged(const wxDateTime &d) { changed=d; }
    wxDateTime GetNextrun() const { return nextrun; }
    void iSetNextrun(const wxDateTime &d) { nextrun=d; }
    wxDateTime GetLastrun() const { return lastrun; }
    void iSetLastrun(const wxDateTime &d) { lastrun=d; }
    long GetAgentId() const { return agentId; }
    void iSetAgentId(const long l) { agentId=l; }

    bool CanCreate() { return false; }
    bool CanView() { return false; }
    bool CanEdit() { return false; }
    bool CanDrop() { return false; }
    bool WantDummyChild() { return true; }

private:
    bool enabled;
    wxDateTime created, changed, nextrun, lastrun;
    long agentId;
};

class pgaJobObject : public pgDatabaseObject
{
public:
    pgaJobObject(pgaJob *job, int newType, const wxString& newName);
    OID GetJoboid() const { return job->GetOid(); }
    pgaJob *GetJob() { return job; }

    bool CanCreate() { return job->CanCreate(); }
    bool CanView() { return false; }
    bool CanEdit() { return job->CanEdit(); }
    bool CanDrop() { return job->CanDrop(); }

protected:
    pgaJob *job;
};

#endif
