//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaStep.h - PostgreSQL Agent Step
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


pgaStep::pgaStep(pgaJob *_job, const wxString& newName)
: pgaJobObject(_job, PGA_STEP, newName)
{
    wxLogInfo(wxT("Creating a pgaStep object"));
}

pgaStep::~pgaStep()
{
    wxLogInfo(wxT("Destroying a pgaStep object"));
}


bool pgaStep::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DELETE FROM pg_admin.pga_jobstep WHERE oid=") + GetOidStr());
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
        properties->AppendItem(_("OID"), GetOid());
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
        pgaJob *obj=(pgaJob*)browser->GetItemData(parentItem);
        if (obj->GetType() == PGA_JOB)
            Step = ReadObjects(obj, 0);
    }
    return Step;
}



pgObject *pgaStep::ReadObjects(pgaJob *job, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaStep *step=0;

    pgSet *steps= job->GetDatabase()->ExecuteSet(
       wxT("SELECT st.oid, st.*, datname FROM pg_admin.pga_jobstep st\n")
       wxT("  LEFT OUTER JOIN pg_database db ON db.oid=st.jstdboid")
       wxT(" WHERE st.jstjoboid=") + job->GetOidStr() + wxT("\n")
       + restriction +
       wxT(" ORDER BY st.oid"));

    if (steps)
    {
        while (!steps->Eof())
        {

            step = new pgaStep(job, steps->GetVal(wxT("jstname")));
            step->iSetOid(steps->GetOid(wxT("oid")));
            step->iSetDatabase(job->GetDatabase());
            step->iSetDbname(steps->GetVal(wxT("datname")));
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
                job->AppendBrowserItem(browser, step);
				steps->MoveNext();
            }
            else
                break;
        }

		delete steps;
    }
    return step;
}
