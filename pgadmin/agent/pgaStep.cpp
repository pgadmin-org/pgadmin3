//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgaStep.cpp - PostgreSQL Agent Step
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgObject.h"
#include "schema/pgDatabase.h"
#include "schema/pgCollection.h"
#include "agent/pgaStep.h"
#include "agent/pgaSchedule.h"

pgaStep::pgaStep(pgCollection *_collection, const wxString &newName)
	: pgaJobObject(_collection->GetJob(), stepFactory, newName)
{
}

wxString pgaStep::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on pgAgent step");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing pgAgent step");
			break;
		case PROPERTIESREPORT:
			message = _("pgAgent step properties report");
			break;
		case PROPERTIES:
			message = _("pgAgent step properties");
			break;
		case DDLREPORT:
			message = _("pgAgent step DDL report");
			break;
		case DEPENDENCIESREPORT:
			message = _("pgAgent step dependencies report");
			break;
		case DEPENDENCIES:
			message = _("pgAgent step dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("pgAgent step dependents report");
			break;
		case DEPENDENTS:
			message = _("pgAgent step dependents");
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop step \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPTITLE:
			message = _("Drop step?");
			break;
	}

	if (!message.IsEmpty() && !(kindOfMessage == DROPEXCLUDINGDEPS || kindOfMessage == DROPTITLE))
		message += wxT(" - ") + GetName();

	return message;
}

bool pgaStep::IsUpToDate()
{
	wxString sql = wxT("SELECT xmin FROM pgagent.pga_jobstep WHERE jstid = ") + NumToStr(GetRecId());
	if (!GetConnection() || GetConnection()->ExecuteScalar(sql) != NumToStr(GetXid()))
		return false;
	else
		return true;
}

bool pgaStep::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetConnection()->ExecuteVoid(wxT("DELETE FROM pgagent.pga_jobstep WHERE jstid=") + NumToStr(GetRecId()));
}


void pgaStep::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("ID"), GetRecId());
		properties->AppendYesNoItem(_("Enabled"), GetEnabled());
		properties->AppendItem(_("Kind"), GetKind());
		if (GetConnStr().IsEmpty())
			properties->AppendItem(_("Database"), GetDbname());
		else
			properties->AppendItem(_("Connection String"), GetConnStr());
		properties->AppendItem(_("Code"), GetCode());
		properties->AppendItem(_("On error"), GetOnError());

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgaStep::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *step = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		step = stepFactory.CreateObjects(coll, 0, wxT("\n   AND jstid=") + NumToStr(GetRecId()));

	return step;
}



pgObject *pgaStepFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgaStep *step = 0;

	pgSet *steps = collection->GetConnection()->ExecuteSet(
	                   wxT("SELECT xmin, * FROM pgagent.pga_jobstep\n")
	                   wxT(" WHERE jstjobid=") + NumToStr(collection->GetJob()->GetRecId()) + wxT("\n")
	                   + restriction +
	                   wxT(" ORDER BY jstname"));

	if (steps)
	{
		while (!steps->Eof())
		{

			step = new pgaStep(collection, steps->GetVal(wxT("jstname")));
			step->iSetRecId(steps->GetLong(wxT("jstid")));
			step->iSetXid(steps->GetOid(wxT("xmin")));
			step->iSetDbname(steps->GetVal(wxT("jstdbname")));
			if (steps->HasColumn(wxT("jstconnstr")))
				step->iSetConnStr(steps->GetVal(wxT("jstconnstr")));
			step->iSetCode(steps->GetVal(wxT("jstcode")));
			step->iSetEnabled(steps->GetBool(wxT("jstenabled")));

			wxChar kindc = *steps->GetVal(wxT("jstkind")).c_str();
			wxString kinds;
			switch (kindc)
			{
				case 'b':
					kinds = _("Batch");
					break;
				case 's':
					kinds = wxT("SQL");
					break;
			}
			step->iSetKindChar(kindc);
			step->iSetKind(kinds);

			wxChar onerrc = *steps->GetVal(wxT("jstonerror")).c_str();
			wxString onerrs;
			switch (onerrc)
			{
				case 's':
					onerrs = _("Succeed");
					break;
				case 'f':
					onerrs = _("Fail");
					break;
				case 'i':
					onerrs = _("Ignore");
					break;


			}

			step->iSetOnErrorChar(onerrc);
			step->iSetOnError(onerrs);
			step->iSetComment(steps->GetVal(wxT("jstdesc")));


			if (browser)
			{
				browser->AppendObject(collection, step);
				steps->MoveNext();
			}
			else
				break;
		}

		delete steps;
	}
	return step;
}


void pgaStep::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxString sql =
	    wxT("SELECT jsljlgid")
	    wxT(", jslstatus")
	    wxT(", jslresult")
	    wxT(", jslstart")
	    wxT(", jslduration")
	    wxT(", (jslstart + jslduration) AS endtime")
	    wxT(", jsloutput")
	    wxT("  FROM pgagent.pga_jobsteplog\n")
	    wxT(" WHERE jsljstid = ") + NumToStr(GetRecId()) +
	    wxT(" ORDER BY jslstart DESC")
	    wxT(" LIMIT ") + NumToStr(settings->GetMaxRows());

	if (statistics)
	{
		wxLogInfo(wxT("Displaying statistics for job %s"), GetFullIdentifier().c_str());

		// Add the statistics view columns
		statistics->ClearAll();
		statistics->AddColumn(_("Run"), 50);
		statistics->AddColumn(_("Status"), 60);
		statistics->AddColumn(_("Result"), 60);
		statistics->AddColumn(_("Start time"), 95);
		statistics->AddColumn(_("End time"), 95);
		statistics->AddColumn(_("Duration"), 70);
		statistics->AddColumn(_("Output"), 200);

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
				else if (stats->GetVal(1) == wxT("i"))
					status = _("Ignored");
				else if (stats->GetVal(1) == wxT("d"))
					status = _("Aborted");
				else
					status = _("Unknown");

				startTime.ParseDateTime(stats->GetVal(3));
				endTime.ParseDateTime(stats->GetVal(5));

				long pos = statistics->AppendItem(stats->GetVal(0), status, stats->GetVal(2));
				statistics->SetItem(pos, 3, startTime.Format());
				if (stats->GetVal(5).Length() > 0)
					statistics->SetItem(pos, 4, endTime.Format());
				statistics->SetItem(pos, 5, stats->GetVal(4));
				statistics->SetItem(pos, 6, stats->GetVal(6));

				stats->MoveNext();
			}
			delete stats;
		}
	}
}


/////////////////////////////


pgaStepCollection::pgaStepCollection(pgaFactory *factory, pgaJob *job)
	: pgaJobObjCollection(factory, job)
{
}


wxString pgaStepCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on pgAgent steps");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing pgAgent steps");
			break;
		case OBJECTSLISTREPORT:
			message = _("pgAgent steps list report");
			break;
	}

	return message;
}


/////////////////////////////


#include "images/step.pngc"
#include "images/steps.pngc"

pgaStepFactory::pgaStepFactory()
	: pgaJobObjFactory(__("Step"), __("New Step"), __("Create a new Step."), step_png_img)
{
	metaType = PGM_STEP;
}


pgCollection *pgaStepFactory::CreateCollection(pgObject *obj)
{
	return new pgaStepCollection(GetCollectionFactory(), (pgaJob *)obj);
}


pgaStepFactory stepFactory;
static pgaCollectionFactory cf(&stepFactory, __("Steps"), steps_png_img);
