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


bool pgaJob::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DELETE FROM pg_admin.pga_job WHERE oid=") + GetOidStr());
}


void pgaJob::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
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

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Enabled"), GetEnabled());
        properties->AppendItem(_("Job class"), GetJobclass());
        properties->AppendItem(_("Created"), GetCreated());
        properties->AppendItem(_("Changed"), GetChanged());
        properties->AppendItem(_("Next run"), GetNextrun());
        properties->AppendItem(_("Last run"), GetLastrun());
        properties->AppendItem(_("Last result"), GetLastresult());
        if (GetAgentId())
            properties->AppendItem(_("Running at"), GetAgentId());

        properties->AppendItem(_("Comment"), GetComment());
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
       wxT("SELECT j.oid, *, ''::text AS joblastresult FROM pg_admin.pga_job j\n")
       wxT("  JOIN pg_admin.pga_jobclass cl ON cl.oid=jobjcloid\n")
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
