//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaSchedule.cpp - PostgreSQL Agent Schedule
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/arrimpl.cpp>

// App headers
#include "misc.h"
#include "pgObject.h"
#include "pgDatabase.h"
#include "pgCollection.h"
#include "pgaSchedule.h"
#include "pgaStep.h"
#include "pgaSchedule.h"


pgaSchedule::pgaSchedule(pgCollection *_collection, const wxString& newName)
: pgaJobObject(_collection->GetJob(), PGA_SCHEDULE, newName)
{
    wxLogInfo(wxT("Creating a pgaSchedule object"));
}

pgaSchedule::~pgaSchedule()
{
    wxLogInfo(wxT("Destroying a pgaSchedule object"));
}


bool pgaSchedule::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DELETE FROM pgagent.pga_schedule WHERE jscid=") + NumToStr(GetJobId()));
}


void pgaSchedule::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
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

        properties->AppendItem(_("Start date"), GetStart());
        properties->AppendItem(_("End date"), GetEnd());
		properties->AppendItem(_("Minutes"), GetMinutesString());
		properties->AppendItem(_("Hours"), GetHoursString());
		properties->AppendItem(_("Weekdays"), GetWeekdaysString());
		properties->AppendItem(_("Monthdays"), GetMonthdaysString());
		properties->AppendItem(_("Months"), GetMonthsString());

        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgaSchedule::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *schedule=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgCollection *obj=(pgCollection*)browser->GetItemData(parentItem);
        if (obj->GetType() == PGA_SCHEDULES)
            schedule = ReadObjects(obj, 0);
    }
    return schedule;
}



pgObject *pgaSchedule::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaSchedule *schedule=0;
	wxString tmp;

    pgSet *schedules= collection->GetDatabase()->ExecuteSet(
       wxT("SELECT * FROM pgagent.pga_schedule\n")
       wxT(" WHERE jscjobid=") + NumToStr(collection->GetJob()->GetJobId()) + wxT("\n")
       + restriction +
       wxT(" ORDER BY jscid"));

    if (schedules)
    {
        while (!schedules->Eof())
        {

            schedule = new pgaSchedule(collection, schedules->GetVal(wxT("jscname")));
            schedule->iSetJobId(schedules->GetLong(wxT("jscid")));
            schedule->iSetDatabase(collection->GetDatabase());
            schedule->iSetStart(schedules->GetDateTime(wxT("jscstart")));
            schedule->iSetEnd(schedules->GetDateTime(wxT("jscend")));

			tmp = schedules->GetVal(wxT("jscminutes"));
			tmp.Replace(wxT("{"), wxT(""));
			tmp.Replace(wxT("}"), wxT(""));
			tmp.Replace(wxT(","), wxT(""));
			schedule->iSetMinutes(tmp);

			tmp = schedules->GetVal(wxT("jschours"));
			tmp.Replace(wxT("{"), wxT(""));
			tmp.Replace(wxT("}"), wxT(""));
			tmp.Replace(wxT(","), wxT(""));
			schedule->iSetHours(tmp);

			tmp = schedules->GetVal(wxT("jscweekdays"));
			tmp.Replace(wxT("{"), wxT(""));
			tmp.Replace(wxT("}"), wxT(""));
			tmp.Replace(wxT(","), wxT(""));
			schedule->iSetWeekdays(tmp);

			tmp = schedules->GetVal(wxT("jscmonthdays"));
			tmp.Replace(wxT("{"), wxT(""));
			tmp.Replace(wxT("}"), wxT(""));
			tmp.Replace(wxT(","), wxT(""));
			schedule->iSetMonthdays(tmp);

			tmp = schedules->GetVal(wxT("jscmonths"));
			tmp.Replace(wxT("{"), wxT(""));
			tmp.Replace(wxT("}"), wxT(""));
			tmp.Replace(wxT(","), wxT(""));
			schedule->iSetMonths(tmp);

            schedule->iSetComment(schedules->GetVal(wxT("jscdesc")));


            if (browser)
            {
                collection->AppendBrowserItem(browser, schedule);
				schedules->MoveNext();
            }
            else
                break;
        }

		delete schedules;
    }
    return schedule;
}

wxString pgaSchedule::GetMinutesString()
{
	size_t x = 0;
	bool isWildcard = true;
	wxString res;

	for (x=0; x <= minutes.Length();x++)
	{
		if (minutes[x] == 't')
		{
			res.Printf(wxT("%s%d, "), res, x);
			isWildcard = false;
		}
	}

	if (isWildcard)
	{
		res = _("Every minute");
	}
	else
	{
		if (res.Length() > 2) 
		{
			res.RemoveLast();
			res.RemoveLast();
		}
	}

	return res;
}

wxString pgaSchedule::GetHoursString()
{
	size_t x = 0;
	bool isWildcard = true;
	wxString res;

	for (x=0; x <= hours.Length();x++)
	{
		if (hours[x] == 't')
		{
			res.Printf(wxT("%s%d, "), res, x);
			isWildcard = false;
		}
	}

	if (isWildcard)
	{
		res = _("Every hour");
	}
	else
	{
		if (res.Length() > 2) 
		{
			res.RemoveLast();
			res.RemoveLast();
		}
	}

	return res;
}

wxString pgaSchedule::GetWeekdaysString()
{
	size_t x = 0;
	bool isWildcard = true;
	wxString res;

	for (x=0; x <= weekdays.Length();x++)
	{
		if (weekdays[x] == 't')
		{
			switch (x)
			{
				case 0:
					res += _("Sunday");
					res += wxT(", ");
					break;
				case 1:
					res += _("Monday");
					res += wxT(", ");
					break;
				case 2:
					res += _("Tuesday");
					res += wxT(", ");
					break;
				case 3:
					res += _("Wednesday");
					res += wxT(", ");
					break;
				case 4:
					res += _("Thursday");
					res += wxT(", ");
					break;
				case 5:
					res += _("Friday");
					res += wxT(", ");
					break;
				case 6:
					res += _("Saturday");
					res += wxT(", ");
					break;
				default:
					res += _("The mythical 8th day!");
					res += wxT(", ");
					break;
			}
			isWildcard = false;
		}
	}

	if (isWildcard)
	{
		res = _("Any day of the week");
	}
	else
	{
		if (res.Length() > 2) 
		{
			res.RemoveLast();
			res.RemoveLast();
		}
	}

	return res;
}

wxString pgaSchedule::GetMonthdaysString()
{
	size_t x = 0;
	bool isWildcard = true;
	wxString res;

	for (x=0; x <= monthdays.Length();x++)
	{
		if (monthdays[x] == 't')
		{
			if (x < 31)
				res.Printf(wxT("%s%d, "), res, x + 1);
			else
			{
				res += _("Last day");
				res += wxT(", ");
			}
			isWildcard = false;
		}
	}

	if (isWildcard)
	{
		res = _("Every day");
	}
	else
	{
		if (res.Length() > 2) 
		{
			res.RemoveLast();
			res.RemoveLast();
		}
	}

	return res;
}

wxString pgaSchedule::GetMonthsString()
{
	size_t x = 0;
	bool isWildcard = true;
	wxString res;

	for (x=0; x <= months.Length();x++)
	{
		if (months[x] == 't')
		{
			switch (x)
			{
				case 0:
					res += _("January");
					res += wxT(", ");
					break;
				case 1:
					res += _("February");
					res += wxT(", ");
					break;
				case 2:
					res += _("March");
					res += wxT(", ");
					break;
				case 3:
					res += _("April");
					res += wxT(", ");
					break;
				case 4:
					res += _("May");
					res += wxT(", ");
					break;
				case 5:
					res += _("June");
					res += wxT(", ");
					break;
				case 6:
					res += _("July");
					res += wxT(", ");
					break;
				case 7:
					res += _("August");
					res += wxT(", ");
					break;
				case 8:
					res += _("September");
					res += wxT(", ");
					break;
				case 9:
					res += _("October");
					res += wxT(", ");
					break;
				case 10:
					res += _("November");
					res += wxT(", ");
					break;
				case 11:
					res += _("December");
					res += wxT(", ");
					break;
				default:
					res += _("The mythical 13th month!");
					res += wxT(", ");
					break;
			}
			isWildcard = false;
		}
	}

	if (isWildcard)
	{
		res = _("Every month");
	}
	else
	{
		if (res.Length() > 2) 
		{
			res.RemoveLast();
			res.RemoveLast();
		}
	}

	return res;
}