//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaJob.h - PostgreSQL Agent Job
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgDatabase.h"
#include "pgaAgent.h"
#include "pgaJob.h"
#include "pgaStep.h"
#include "pgaSchedule.h"


pgaJob::pgaJob(const wxString& newName)
: pgDatabaseObject(PGA_JOB, newName)
{
    wxLogInfo(wxT("Creating a pgaJob object"));
}

pgaJob::~pgaJob()
{
    wxLogInfo(wxT("Destroying a pgaJob object"));
}


wxMenu *pgaJob::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();
    if (1) // check priv.
    {
        AppendMenu(menu, PGA_STEP);
        AppendMenu(menu, PGA_SCHEDULE);
    }
    return menu;
}


void pgaJob::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child objects to Job."));

        pgaStep::ReadObjects(this, browser);
        pgaSchedule::ReadObjects(this, browser);

    }

    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Enabled"), GetEnabled());
        InsertListItem(properties, pos++, _("Job class"), GetJobclass());
        InsertListItem(properties, pos++, _("Created"), GetCreated());
        InsertListItem(properties, pos++, _("Changed"), GetChanged());
        InsertListItem(properties, pos++, _("Next run"), GetNextrun());
        InsertListItem(properties, pos++, _("Last run"), GetLastrun());
        InsertListItem(properties, pos++, _("Last result"), GetLastresult());
        if (GetAgentId())
            InsertListItem(properties, pos++, _("Running at"), GetAgentId());

        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}



pgObject *pgaJob::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *job=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PGA_AGENT)
            job = ReadObjects((pgaAgent*)obj, 0);
    }
    return job;
}



pgObject *pgaJob::ReadObjects(pgaAgent *agent, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaJob *job=0;

    pgSet *jobs= agent->GetDatabase()->ExecuteSet(
       wxT("SELECT j.oid, *, ''::text as joblastresult from pga_job j\n")
       wxT("  JOIN pga_jobclass cl ON cl.oid=jobjcloid\n")
       + restriction +
       wxT(" ORDER BY jobname"));

    if (jobs)
    {
        while (!jobs->Eof())
        {

            job = new pgaJob(jobs->GetVal(wxT("jobname")));
            job->iSetOid(jobs->GetOid(wxT("oid")));
            job->iSetDatabase(agent->GetDatabase());
            job->iSetComment(jobs->GetVal(wxT("jobdesc")));

            job->iSetEnabled(jobs->GetBool(wxT("jobenabled")));
            job->iSetJobclass(jobs->GetVal(wxT("jclname")));
            job->iSetCreated(jobs->GetDateTime(wxT("jobcreated")));
            job->iSetChanged(jobs->GetDateTime(wxT("jobchanged")));
            job->iSetNextrun(jobs->GetDateTime(wxT("jobnextrun")));
            job->iSetLastrun(jobs->GetDateTime(wxT("joblastrun")));
            job->iSetLastresult(jobs->GetVal(wxT("joblastresult")));
            job->iSetAgentId(jobs->GetLong(wxT("jobagentid")));

            if (browser)
            {
                agent->AppendBrowserItem(browser, job);
				jobs->MoveNext();
            }
            else
                break;
        }

		delete jobs;
    }
    return job;
}


pgaJobObject::pgaJobObject(pgaJob *_job, int newType, const wxString& newName)
: pgDatabaseObject(newType, newName)
{
    job=_job;
}
