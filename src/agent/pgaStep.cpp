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


pgaStep::pgaStep(pgCollection *_collection, const wxString& newName)
: pgaJobObject(_collection->GetJob(), PGA_STEP, newName)
{
    wxLogInfo(wxT("Creating a pgaStep object"));
}

pgaStep::~pgaStep()
{
    wxLogInfo(wxT("Destroying a pgaStep object"));
}


bool pgaStep::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DELETE FROM pgagent.pga_jobstep WHERE jstid=") + NumToStr(GetJobId()));
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
        properties->AppendItem(_("ID"), GetJobId());
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
            Step = ReadObjects(obj, 0);
    }
    return Step;
}



pgObject *pgaStep::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaStep *step=0;

    pgSet *steps= collection->GetDatabase()->ExecuteSet(
       wxT("SELECT * FROM pgagent.pga_jobstep\n")
       wxT(" WHERE jstjobid=") + NumToStr(collection->GetJob()->GetJobId()) + wxT("\n")
       + restriction +
       wxT(" ORDER BY jstid"));

    if (steps)
    {
        while (!steps->Eof())
        {

            step = new pgaStep(collection, steps->GetVal(wxT("jstname")));
            step->iSetJobId(steps->GetLong(wxT("jstid")));
            step->iSetDatabase(collection->GetDatabase());
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
