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

#include "pgServer.h"


class pgaJobFactory : public pgServerObjFactory
{
public:
    pgaJobFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
    int GetDisabledId() { return disabledId; }

protected:
    int disabledId;
};
extern pgaJobFactory jobFactory;

class pgaJob : public pgServerObject
{
public:
    pgaJob(const wxString& newName = wxT(""));
    ~pgaJob();

    int GetIconId();
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);

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


class pgaJobObject : public pgServerObject
{
public:
    pgaJobObject(pgaJob *job, pgaFactory &factory, const wxString& newName);
    pgaJob *GetJob() { return job; }

    bool CanCreate() { return job->CanCreate(); }
    bool CanView() { return false; }
    bool CanEdit() { return job->CanEdit(); }
    bool CanDrop() { return job->CanDrop(); }

protected:
    pgaJob *job;
};

#endif
