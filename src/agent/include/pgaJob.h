//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
// Class declarations


class pgaJob : public pgDatabaseObject
{
public:
    pgaJob(const wxString& newName = wxT(""));
    ~pgaJob();

    int GetIcon() { return enabled ? PGAICON_JOB : PGAICON_JOBDISABLED; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxEmptyString);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);

    wxString GetJobclass() const { return jobclass; }
    void iSetJobclass(const wxString &s) { jobclass=s; }
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
    wxString GetLastresult() const { return lastresult; }
    void iSetLastresult(const wxString &s) { lastresult = s; }
    wxString GetCurrentAgent() const { return currentAgent; }
    void iSetCurrentAgent(const wxString &s) { currentAgent=s; }
    wxString GetHostAgent() const { return hostAgent; }
    void iSetHostAgent(const wxString &s) { hostAgent=s; }
    long GetRecId() const { return recId; }
    void iSetRecId(const long l) { recId=l; }

    wxMenu *GetNewMenu();
    bool CanCreate() { return true; }
    bool CanView() { return false; }
    bool CanEdit() { return true; }
    bool CanDrop() { return true; }
    bool WantDummyChild() { return true; }

    wxString GetHelpPage(bool forCreate) const { return wxT("pgagent-jobs"); }

private:
    bool enabled;
    wxDateTime created, changed, nextrun, lastrun;
    wxString lastresult, jobclass, currentAgent, hostAgent;
    long recId;
};


class pgaJobObject : public pgDatabaseObject
{
public:
    pgaJobObject(pgaJob *job, int newType, const wxString& newName);
    pgaJob *GetJob() { return job; }

    bool CanCreate() { return job->CanCreate(); }
    bool CanView() { return false; }
    bool CanEdit() { return job->CanEdit(); }
    bool CanDrop() { return job->CanDrop(); }

protected:
    pgaJob *job;
};

#endif
