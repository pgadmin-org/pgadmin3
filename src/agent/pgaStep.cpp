//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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
: pgaJobObject(job, PGA_STEP, newName)
{
    wxLogInfo(wxT("Creating a pgaStep object"));
}

pgaStep::~pgaStep()
{
    wxLogInfo(wxT("Destroying a pgaStep object"));
}



void pgaStep::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;
    }

    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Kind"), GetKind());
        InsertListItem(properties, pos++, _("Database"), GetDbname());
        InsertListItem(properties, pos++, _("Code"), GetCode());
        InsertListItem(properties, pos++, _("On Error"), GetOnError());

        InsertListItem(properties, pos++, _("Comment"), GetComment());
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
       wxT("SELECT st.oid, st.*, datname from pg_jobstep st\n")
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

            wxChar kindc=steps->GetVal(wxT("jstkind"))[0];
            wxString kinds;
            switch (kindc)
            {
                case 'b':   kinds = _("Batch");         break;
                case 's':   kinds = wxT("SQL");         break;
            }
            step->iSetKindChar(kindc);
            step->iSetKind(kinds);

            wxChar onerrc=steps->GetVal(wxT("jstonerror"))[0];
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
