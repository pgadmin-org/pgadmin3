//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaSchedule.h - PostgreSQL Agent Schedule
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
#include "pgaSchedule.h"
#include "pgaStep.h"
#include "pgaSchedule.h"


pgaSchedule::pgaSchedule(pgaJob *_job, const wxString& newName)
: pgaJobObject(_job, PGA_SCHEDULE, newName)
{
    wxLogInfo(wxT("Creating a pgaSchedule object"));
}

pgaSchedule::~pgaSchedule()
{
    wxLogInfo(wxT("Destroying a pgaSchedule object"));
}



void pgaSchedule::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
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
        InsertListItem(properties, pos++, _("Enabled"), GetEnabled());
        InsertListItem(properties, pos++, _("Type"), GetKind());
        if (kindChar == 'n' || kindChar == 's')
            InsertListItem(properties, pos++, _("Scheduled"), GetSchedule());
        if (kindChar != 's')
            InsertListItem(properties, pos++, _("Interval"), GetIntervalList());
        InsertListItem(properties, pos++, _("Start date"), GetStart());
        InsertListItem(properties, pos++, _("End date"), GetEnd());

        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}



pgObject *pgaSchedule::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *schedule=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgaJob *obj=(pgaJob*)browser->GetItemData(parentItem);
        if (obj->GetType() == PGA_JOB)
            schedule = ReadObjects(obj, 0);
    }
    return schedule;
}



pgObject *pgaSchedule::ReadObjects(pgaJob *job, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaSchedule *schedule=0;

    pgSet *schedules= job->GetDatabase()->ExecuteSet(
       wxT("SELECT sc.oid, * from pga_schedule sc\n")
       wxT(" WHERE sc.jscjoboid=") + job->GetOidStr() + wxT("\n")
       + restriction +
       wxT(" ORDER BY sc.oid"));

    if (schedules)
    {
        while (!schedules->Eof())
        {

            schedule = new pgaSchedule(job, schedules->GetVal(wxT("jscname")));
            schedule->iSetOid(schedules->GetOid(wxT("oid")));
            schedule->iSetDatabase(job->GetDatabase());
            schedule->iSetStart(schedules->GetDateTime(wxT("jscstart")));
            schedule->iSetEnd(schedules->GetDateTime(wxT("jscend")));
            schedule->iSetSchedule(schedules->GetDateTime(wxT("jscsched")));
            schedule->iSetIntervalList(schedules->GetVal(wxT("jsclist")));

            wxChar kindc = *schedules->GetVal(wxT("jsckind")).c_str();
            wxString kinds;
            switch (kindc)
            {
                case 'n':   kinds = _("repeat");            break;
                case 's':   kinds = _("single");            break;
                case 'd':   kinds = _("daily") ;            break;
                case 'w':   kinds = _("weekly");            break;
                case 'm':   kinds = _("monthly");           break;
                case 'y':   kinds = _("yearly");            break;
            }
            schedule->iSetKind(kinds);
            schedule->iSetKindChar(kindc);

            schedule->iSetComment(schedules->GetVal(wxT("jscdesc")));


            if (browser)
            {
                job->AppendBrowserItem(browser, schedule);
				schedules->MoveNext();
            }
            else
                break;
        }

		delete schedules;
    }
    return schedule;
}
