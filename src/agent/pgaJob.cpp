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
#include "pgCollection.h"
#include "pgDatabase.h"
#include "pgaJob.h"
#include "pgaStep.h"
#include "pgaSchedule.h"

extern sysSettings *settings;

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


bool pgaJob::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(wxT("DELETE FROM pgagent.pga_job WHERE jobid=") + NumToStr(GetRecId()));
}


void pgaJob::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);

        // Log
        wxLogInfo(wxT("Adding child objects to Job."));
        pgCollection *collection;

        // Schedules
        collection = new pgCollection(PGA_SCHEDULES, this);
        AppendBrowserItem(browser, collection);

        // Steps
        collection = new pgCollection(PGA_STEPS, this);
        AppendBrowserItem(browser, collection);

    }

    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("ID"), GetRecId());
        properties->AppendItem(_("Enabled"), GetEnabled());
		properties->AppendItem(_("Host agent"), GetHostAgent());
        properties->AppendItem(_("Job class"), GetJobclass());
        properties->AppendItem(_("Created"), GetCreated());
        properties->AppendItem(_("Changed"), GetChanged());
        properties->AppendItem(_("Next run"), GetNextrun());
        properties->AppendItem(_("Last run"), GetLastrun());
        properties->AppendItem(_("Last result"), GetLastresult());
		if (!GetCurrentAgent().IsEmpty())
			properties->AppendItem(_("Running at"), GetCurrentAgent());
		else
			properties->AppendItem(_("Running at"), _("Not currently running"));

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
        if (obj->GetType() == PGA_JOBS)
            job = ReadObjects((pgCollection*)obj, 0, wxT("\n   WHERE j.jobid=") + NumToStr(GetRecId()));
    }
    return job;
}



pgObject *pgaJob::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaJob *job=0;

    pgSet *jobs= collection->GetDatabase()->ExecuteSet(
       wxT("SELECT *, ")
	   wxT("(SELECT jlgstatus FROM pgagent.pga_joblog jl WHERE jl.jlgjobid = j.jobid ORDER BY jlgid DESC LIMIT 1) AS joblastresult ")
	   wxT("FROM pgagent.pga_job j\n")
       wxT("  JOIN pgagent.pga_jobclass cl ON cl.jclid=jobjclid\n")
       wxT("  LEFT OUTER JOIN pgagent.pga_jobagent ag ON ag.jagpid=jobagentid\n")
       + restriction +
       wxT(" ORDER BY jobname"));

    if (jobs)
    {
        while (!jobs->Eof())
        {
			wxString status;
			if (jobs->GetVal(wxT("joblastresult")) == wxT("r"))
                status = _("Running");
		    else if (jobs->GetVal(wxT("joblastresult")) == wxT("s"))
                status = _("Successful");
			else if (jobs->GetVal(wxT("joblastresult")) == wxT("f"))
                status = _("Failed");
			else if (jobs->GetVal(wxT("joblastresult")) == wxT("d"))
                status = _("Aborted");
        	else if (jobs->GetVal(wxT("joblastresult")) == wxT("i"))
                status = _("No steps");
			else
                status = _("Unknown");

            job = new pgaJob(jobs->GetVal(wxT("jobname")));
            job->iSetRecId(jobs->GetLong(wxT("jobid")));
            job->iSetDatabase(collection->GetDatabase());
            job->iSetComment(jobs->GetVal(wxT("jobdesc")));

            job->iSetEnabled(jobs->GetBool(wxT("jobenabled")));
            job->iSetJobclass(jobs->GetVal(wxT("jclname")));
			job->iSetHostAgent(jobs->GetVal(wxT("jobhostagent")));
            job->iSetCreated(jobs->GetDateTime(wxT("jobcreated")));
            job->iSetChanged(jobs->GetDateTime(wxT("jobchanged")));
            job->iSetNextrun(jobs->GetDateTime(wxT("jobnextrun")));
            job->iSetLastrun(jobs->GetDateTime(wxT("joblastrun")));
            job->iSetLastresult(status);
			job->iSetCurrentAgent(jobs->GetVal(wxT("jagstation")));

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

void pgaJob::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    wxString sql =
        wxT("SELECT jlgid")
		     wxT(", jlgstatus")
             wxT(", jlgstart")
             wxT(", jlgduration")
			 wxT(", (jlgstart + jlgduration) AS endtime")
             wxT("  FROM pgagent.pga_joblog\n")
             wxT(" WHERE jlgjobid = ") + NumToStr(GetRecId()) +
			 wxT(" ORDER BY jlgstart DESC") +
			 wxT(" LIMIT ") + NumToStr(settings->GetMaxRows());

    if (statistics)
    {
        wxLogInfo(wxT("Displaying statistics for job %s"), GetFullIdentifier());

        // Add the statistics view columns
		statistics->ClearAll();
		statistics->AddColumn(_("Run"), 50);
        statistics->AddColumn(_("Status"), 60);
		statistics->AddColumn(_("Start time"), 95);
		statistics->AddColumn(_("End time"), 95);
		statistics->AddColumn(_("Duration"), 70);

        pgSet *stats = database->ExecuteSet(sql);
		wxString status;
		wxDateTime startTime;
		wxDateTime endTime;

        if (stats)
        {
            while (!stats->Eof())
            {
				if (stats->GetVal(1) == wxT("r"))
                    status = _("Running");
				else if (stats->GetVal(1) == wxT("s"))
                    status = _("Successful");
				else if (stats->GetVal(1) == wxT("f"))
                    status = _("Failed");
				else if (stats->GetVal(1) == wxT("d"))
                    status = _("Aborted");
				else if (stats->GetVal(1) == wxT("i"))
                    status = _("No steps");
				else
                    status = _("Unknown");

				startTime.ParseDateTime(stats->GetVal(2));
				endTime.ParseDateTime(stats->GetVal(4));

                long pos=statistics->AppendItem(stats->GetVal(0), status, startTime.Format());
				if (stats->GetVal(4).Length() > 0)
                    statistics->SetItem(pos, 3, endTime.Format());
				statistics->SetItem(pos, 4, stats->GetVal(3));

				stats->MoveNext();
            }
            delete stats;
        }
    }
}

pgaJobObject::pgaJobObject(pgaJob *_job, int newType, const wxString& newName)
: pgDatabaseObject(newType, newName)
{
    job=_job;
}

