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
#include "pgCollection.h"
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
        InsertListItem(properties, pos++, _("Created"), GetCreated());
        InsertListItem(properties, pos++, _("Changed"), GetChanged());
        InsertListItem(properties, pos++, _("Next run"), GetNextrun());
        InsertListItem(properties, pos++, _("Last run"), GetLastrun());
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
        if (obj->GetType() == PGA_JOBS)
            job = ReadObjects((pgCollection*)obj, 0);
    }
    return job;
}



pgObject *pgaJob::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaJob *job=0;

    pgSet *jobs= collection->GetDatabase()->ExecuteSet(
       wxT("SELECT j.oid, * from pg_job j\n")
       wxT("  JOIN pg_jobclass cl ON cl.oid=jobjcloid\n")
       + restriction +
       wxT(" ORDER BY jobname"));

    if (jobs)
    {
        while (!jobs->Eof())
        {

            job = new pgaJob(jobs->GetVal(wxT("jobname")));
            job->iSetOid(jobs->GetOid(wxT("oid")));
            job->iSetDatabase(collection->GetDatabase());
            job->iSetComment(jobs->GetVal(wxT("jobdesc")));

            job->iSetEnabled(jobs->GetBool(wxT("jobenabled")));
            job->iSetCreated(jobs->GetDateTime(wxT("jobcreated")));
            job->iSetChanged(jobs->GetDateTime(wxT("jobchanged")));
            job->iSetNextrun(jobs->GetDateTime(wxT("jobnextrun")));
            job->iSetLastrun(jobs->GetDateTime(wxT("joblastrun")));
            job->iSetAgentId(jobs->GetLong(wxT("jobagentid")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, job);
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
: pgDatabaseObject(PGA_JOB, newName)
{
    job=_job;
}
