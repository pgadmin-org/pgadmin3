//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaStep.cpp - PostgreSQL Agent Step
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
#include "pgaStep.h"
#include "pgaStep.h"
#include "pgaSchedule.h"

extern sysSettings *settings;

pgaStep::pgaStep(pgCollection *_collection, const wxString& newName)
: pgaJobObject(_collection->GetJob(), PGA_STEP, newName)
{
    wxLogInfo(wxT("Creating a pgaStep object"));
}

pgaStep::~pgaStep()
{
    wxLogInfo(wxT("Destroying a pgaStep object"));
}


bool pgaStep::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    return GetConnection()->ExecuteVoid(wxT("DELETE FROM pgagent.pga_jobstep WHERE jstid=") + NumToStr(GetRecId()));
}


void pgaStep::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;
    }

    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("ID"), GetRecId());
        properties->AppendItem(_("Enabled"), GetEnabled());
        properties->AppendItem(_("Kind"), GetKind());
        properties->AppendItem(_("Database"), GetDbname());
        properties->AppendItem(_("Code"), GetCode());
        properties->AppendItem(_("On error"), GetOnError());

        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgaStep::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *Step=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgCollection *obj=(pgCollection*)browser->GetItemData(parentItem);
        if (obj->GetType() == PGA_STEPS)
            Step = ReadObjects(obj, 0, wxT("\n   AND jstid=") + NumToStr(GetRecId()));
    }
    return Step;
}



pgObject *pgaStep::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaStep *step=0;

    pgSet *steps= collection->GetConnection()->ExecuteSet(
       wxT("SELECT * FROM pgagent.pga_jobstep\n")
       wxT(" WHERE jstjobid=") + NumToStr(collection->GetJob()->GetRecId()) + wxT("\n")
       + restriction +
       wxT(" ORDER BY jstname"));

    if (steps)
    {
        while (!steps->Eof())
        {

            step = new pgaStep(collection, steps->GetVal(wxT("jstname")));
            step->iSetRecId(steps->GetLong(wxT("jstid")));
            step->iSetDbname(steps->GetVal(wxT("jstdbname")));
            step->iSetCode(steps->GetVal(wxT("jstcode")));
            step->iSetEnabled(steps->GetBool(wxT("jstenabled")));

            wxChar kindc = *steps->GetVal(wxT("jstkind")).c_str();
            wxString kinds;
            switch (kindc)
            {
                case 'b':   kinds = _("Batch");         break;
                case 's':   kinds = wxT("SQL");         break;
            }
            step->iSetKindChar(kindc);
            step->iSetKind(kinds);

            wxChar onerrc = *steps->GetVal(wxT("jstonerror")).c_str();
            wxString onerrs;
            switch (onerrc)
            {
                case 's':   onerrs = _("Succeed");      break;
                case 'f':   onerrs = _("Fail");         break;
                case 'i':   onerrs = _("Ignore");       break;
    

            }

            step->iSetOnErrorChar(onerrc);
            step->iSetOnError(onerrs);
            step->iSetComment(steps->GetVal(wxT("jstdesc")));


            if (browser)
            {
                collection->AppendBrowserItem(browser, step);
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
        wxLogInfo(wxT("Displaying statistics for job %s"), GetFullIdentifier());

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
				else if (stats->GetVal(1) == wxT("i"))
                    status = _("Aborted");
				else
                    status = _("Unknown");

				startTime.ParseDateTime(stats->GetVal(3));
				endTime.ParseDateTime(stats->GetVal(5));

                long pos=statistics->AppendItem(stats->GetVal(0), status, stats->GetVal(2));
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