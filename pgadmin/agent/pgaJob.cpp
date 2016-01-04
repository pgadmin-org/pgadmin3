//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgaJob.h - PostgreSQL Agent Job
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "schema/pgObject.h"
#include "schema/pgCollection.h"
#include "schema/pgDatabase.h"
#include "agent/pgaJob.h"
#include "agent/pgaStep.h"
#include "agent/pgaSchedule.h"

pgaJob::pgaJob(const wxString &newName)
	: pgServerObject(jobFactory, newName)
{
}

wxString pgaJob::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on pgAgent job");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing pgAgent job");
			break;
		case PROPERTIESREPORT:
			message = _("pgAgent job properties report");
			break;
		case PROPERTIES:
			message = _("pgAgent job properties");
			break;
		case DDLREPORT:
			message = _("pgAgent job DDL report");
			break;
		case DEPENDENCIESREPORT:
			message = _("pgAgent job dependencies report");
			break;
		case DEPENDENCIES:
			message = _("pgAgent job dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("pgAgent job dependents report");
			break;
		case DEPENDENTS:
			message = _("pgAgent job dependents");
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop job \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPTITLE:
			message = _("Drop job?");
			break;
	}

	if (!message.IsEmpty() && !(kindOfMessage == DROPEXCLUDINGDEPS || kindOfMessage == DROPTITLE))
		message += wxT(" - ") + GetName();

	return message;
}

int pgaJob::GetIconId()
{
	if (GetEnabled())
		return jobFactory.GetIconId();
	else
		return jobFactory.GetDisabledId();
}


wxMenu *pgaJob::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();
	if (1) // check priv.
	{
		stepFactory.AppendMenu(menu);
		scheduleFactory.AppendMenu(menu);
	}
	return menu;
}


bool pgaJob::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetConnection()->ExecuteVoid(wxT("DELETE FROM pgagent.pga_job WHERE jobid=") + NumToStr(GetRecId()));
}


void pgaJob::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);

		// Log
		wxLogInfo(wxT("Adding child objects to Job."));

		browser->AppendCollection(this, scheduleFactory);
		browser->AppendCollection(this, stepFactory);
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("ID"), GetRecId());
		properties->AppendYesNoItem(_("Enabled"), GetEnabled());
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

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgaJob::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *job = 0;

	pgObject *obj = browser->GetObject(browser->GetItemParent(item));
	if (obj && obj->IsCollection())
		job = jobFactory.CreateObjects((pgCollection *)obj, 0, wxT("\n   WHERE j.jobid=") + NumToStr(GetRecId()));

	return job;
}



pgObject *pgaJobFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgaJob *job = 0;

	pgSet *jobs = collection->GetConnection()->ExecuteSet(
	                  wxT("SELECT j.*, cl.*, ag.*, sub.jlgstatus AS joblastresult ")
	                  wxT("  FROM pgagent.pga_job j JOIN")
	                  wxT("  pgagent.pga_jobclass cl ON cl.jclid=jobjclid LEFT OUTER JOIN")
	                  wxT("  pgagent.pga_jobagent ag ON ag.jagpid=jobagentid LEFT OUTER JOIN")
	                  wxT("  (SELECT DISTINCT ON (jlgjobid) jlgstatus, jlgjobid")
	                  wxT("   FROM pgagent.pga_joblog")
	                  wxT("  ORDER BY jlgjobid, jlgid desc) sub ON sub.jlgjobid = j.jobid ")
	                  + restriction +
	                  wxT("ORDER BY jobname;"));

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
			job->iSetServer(collection->GetServer());
			job->iSetRecId(jobs->GetLong(wxT("jobid")));
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
				browser->AppendObject(collection, job);
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
		wxLogInfo(wxT("Displaying statistics for job %s"), GetFullIdentifier().c_str());

		// Add the statistics view columns
		statistics->ClearAll();
		statistics->AddColumn(_("Run"), 50);
		statistics->AddColumn(_("Status"), 60);
		statistics->AddColumn(_("Start time"), 95);
		statistics->AddColumn(_("End time"), 95);
		statistics->AddColumn(_("Duration"), 70);

		pgSet *stats = GetConnection()->ExecuteSet(sql);
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

				long pos = statistics->AppendItem(stats->GetVal(0), status, startTime.Format());
				if (stats->GetVal(4).Length() > 0)
					statistics->SetItem(pos, 3, endTime.Format());
				statistics->SetItem(pos, 4, stats->GetVal(3));

				stats->MoveNext();
			}
			delete stats;
		}
	}
}

bool pgaJob::RunNow()
{
	if (!GetConnection()->ExecuteVoid(wxT("UPDATE pgagent.pga_job SET jobnextrun = now() WHERE jobid=") + NumToStr(GetRecId())))
		return false;

	return true;
}


pgaJobCollection::pgaJobCollection(pgaFactory *factory, pgServer *sv)
	: pgServerObjCollection(factory, sv)
{
}


wxString pgaJobCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on pgAgent jobs");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing pgAgent jobs");
			break;
		case OBJECTSLISTREPORT:
			message = _("pgAgent jobs list report");
			break;
	}

	return message;
}


pgaJobObject::pgaJobObject(pgaJob *_job, pgaFactory &factory, const wxString &newName)
	: pgServerObject(factory, newName)
{
	job = _job;
	server = job->GetServer();
}


pgaJobObjCollection::pgaJobObjCollection(pgaFactory *factory, pgaJob *_job)
	: pgServerObjCollection(factory, _job->GetServer())
{
	job = _job;
}


bool pgaJobObjCollection::CanCreate()
{
	return job->CanCreate();
}


pgCollection *pgaJobObjFactory::CreateCollection(pgObject *obj)
{
	return new pgaJobObjCollection(GetCollectionFactory(), (pgaJob *)obj);
}


/////////////////////////////


#include "images/job.pngc"
#include "images/jobs.pngc"
#include "images/jobdisabled.pngc"

pgaJobFactory::pgaJobFactory()
	: pgServerObjFactory(__("pgAgent Job"), __("New Job"), __("Create a new Job."), job_png_img)
{
	metaType = PGM_JOB;
	disabledId = addIcon(jobdisabled_png_img);
}


pgCollection *pgaJobFactory::CreateCollection(pgObject *obj)
{
	return new pgaJobCollection(GetCollectionFactory(), (pgServer *)obj);
}


pgaJobFactory jobFactory;
static pgaCollectionFactory cf(&jobFactory, __("Jobs"), jobs_png_img);

runNowFactory::runNowFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Run now"), _("Reschedule the job to run now."));
}


wxWindow *runNowFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (!((pgaJob *)(obj))->RunNow())
	{
		wxLogError(_("Failed to reschedule the job."));
	}

	form->Refresh(obj);

	return 0;
}


bool runNowFactory::CheckEnable(pgObject *obj)
{
	if (obj)
	{
		if (obj->GetMetaType() == PGM_JOB && !obj->IsCollection())
			return true;
	}
	return false;
}
