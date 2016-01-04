//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgaSchedule.cpp - PostgreSQL Agent Schedule
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/arrimpl.cpp>

// App headers
#include "utils/misc.h"
#include "schema/pgObject.h"
#include "schema/pgDatabase.h"
#include "schema/pgCollection.h"
#include "agent/pgaSchedule.h"
#include "agent/pgaStep.h"
#include "agent/pgaSchedule.h"


pgaSchedule::pgaSchedule(pgCollection *_collection, const wxString &newName)
	: pgaJobObject(_collection->GetJob(), scheduleFactory, newName)
{
}

wxString pgaSchedule::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on pgAgent schedule");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing pgAgent schedule");
			break;
		case PROPERTIESREPORT:
			message = _("pgAgent schedule properties report");
			break;
		case PROPERTIES:
			message = _("pgAgent schedule properties");
			break;
		case DDLREPORT:
			message = _("pgAgent schedule DDL report");
			break;
		case DEPENDENCIESREPORT:
			message = _("pgAgent schedule dependencies report");
			break;
		case DEPENDENCIES:
			message = _("pgAgent schedule dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("pgAgent schedule dependents report");
			break;
		case DEPENDENTS:
			message = _("pgAgent schedule dependents");
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop schedule \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPTITLE:
			message = _("Drop schedule?");
			break;
	}

	if (!message.IsEmpty() && !(kindOfMessage == DROPEXCLUDINGDEPS || kindOfMessage == DROPTITLE))
		message += wxT(" - ") + GetName();

	return message;
}

bool pgaSchedule::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetConnection()->ExecuteVoid(wxT("DELETE FROM pgagent.pga_schedule WHERE jscid=") + NumToStr(GetRecId()));
}


void pgaSchedule::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
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

		properties->AppendItem(_("Start date"), GetStart());
		properties->AppendItem(_("End date"), GetEnd());
		properties->AppendItem(_("Minutes"), GetMinutesString());
		properties->AppendItem(_("Hours"), GetHoursString());
		properties->AppendItem(_("Weekdays"), GetWeekdaysString());
		properties->AppendItem(_("Monthdays"), GetMonthdaysString());
		properties->AppendItem(_("Months"), GetMonthsString());
		properties->AppendItem(_("Exceptions"), GetExceptionsString());

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgaSchedule::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *schedule = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		schedule = scheduleFactory.CreateObjects(coll, 0, wxT("\n   AND jscid=") + NumToStr(GetRecId()));

	return schedule;
}



pgObject *pgaScheduleFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgaSchedule *schedule = 0;
	wxString tmp;

	pgSet *schedules = collection->GetConnection()->ExecuteSet(
	                       wxT("SELECT * FROM pgagent.pga_schedule\n")
	                       wxT(" WHERE jscjobid=") + NumToStr(collection->GetJob()->GetRecId()) + wxT("\n")
	                       + restriction +
	                       wxT(" ORDER BY jscname"));

	if (schedules)
	{
		while (!schedules->Eof())
		{

			schedule = new pgaSchedule(collection, schedules->GetVal(wxT("jscname")));
			schedule->iSetRecId(schedules->GetLong(wxT("jscid")));
			schedule->iSetStart(schedules->GetDateTime(wxT("jscstart")));
			schedule->iSetEnd(schedules->GetDateTime(wxT("jscend")));
			schedule->iSetEnabled(schedules->GetBool(wxT("jscenabled")));

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

			pgSet *exceptions =  collection->GetConnection()->ExecuteSet(
			                         wxT("SELECT * FROM pgagent.pga_exception\n")
			                         wxT(" WHERE jexscid=") + NumToStr(schedule->GetRecId()) + wxT("\n"));

			tmp.Empty();
			if (exceptions)
			{
				while (!exceptions->Eof())
				{
					tmp += exceptions->GetVal(wxT("jexid"));
					tmp += wxT("|");
					tmp += exceptions->GetVal(wxT("jexdate"));
					tmp += wxT("|");
					tmp += exceptions->GetVal(wxT("jextime"));
					tmp += wxT("|");

					exceptions->MoveNext();
				}
			}
			schedule->iSetExceptions(tmp);
			delete exceptions;

			if (browser)
			{
				browser->AppendObject(collection, schedule);
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
	wxString res, tmp;

	for (x = 0; x <= minutes.Length(); x++)
	{
		if (minutes[x] == 't')
		{
			tmp.Printf(wxT("%.2d, "), (int)x);
			res += tmp;
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
	wxString res, tmp;

	for (x = 0; x <= hours.Length(); x++)
	{
		if (hours[x] == 't')
		{
			tmp.Printf(wxT("%.2d, "), (int)x);
			res += tmp;
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

	for (x = 0; x <= weekdays.Length(); x++)
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
	wxString res, tmp;

	for (x = 0; x <= monthdays.Length(); x++)
	{
		if (monthdays[x] == 't')
		{
			if (x < 31)
			{
				tmp.Printf(wxT("%.2d, "), (int)(x + 1));
				res += tmp;
			}
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

	for (x = 0; x <= months.Length(); x++)
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

wxString pgaSchedule::GetExceptionsString()
{
	wxString tmp, token, dateToken, timeToken;
	wxDateTime val;
	wxStringTokenizer tkz(exceptions, wxT("|"));

	while (tkz.HasMoreTokens() )
	{

		dateToken.Empty();
		timeToken.Empty();

		// First is the ID which can be ignored
		token = tkz.GetNextToken();

		// Look for a date
		if (tkz.HasMoreTokens())
			dateToken = tkz.GetNextToken();

		// Look for a time
		if (tkz.HasMoreTokens())
			timeToken = tkz.GetNextToken();

		if (tmp.IsEmpty())
			tmp += wxT("[");
		else
			tmp += wxT(", [");

		if (!dateToken.IsEmpty() && !timeToken.IsEmpty())
		{
			val.ParseDate(dateToken);
			val.ParseTime(timeToken);
			tmp += val.Format();
		}
		else if (!dateToken.IsEmpty() && timeToken.IsEmpty())
		{
			val.ParseDate(dateToken);
			tmp += val.FormatDate();
		}
		else if (dateToken.IsEmpty() && !timeToken.IsEmpty())
		{
			val.ParseTime(timeToken);
			tmp += val.FormatTime();
		}

		tmp += wxT("]");

	}

	return tmp;
}


/////////////////////////////


pgaScheduleCollection::pgaScheduleCollection(pgaFactory *factory, pgaJob *job)
	: pgaJobObjCollection(factory, job)
{
}


wxString pgaScheduleCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on pgAgent schedules");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing pgAgent schedules");
			break;
		case OBJECTSLISTREPORT:
			message = _("pgAgent schedules list report");
			break;
	}

	return message;
}

/////////////////////////////


#include "images/schedule.pngc"
#include "images/schedules.pngc"

pgaScheduleFactory::pgaScheduleFactory()
	: pgaJobObjFactory(__("Schedule"), __("New Schedule"), __("Create a new Schedule."), schedule_png_img)
{
	metaType = PGM_SCHEDULE;
}


pgCollection *pgaScheduleFactory::CreateCollection(pgObject *obj)
{
	return new pgaScheduleCollection(GetCollectionFactory(), (pgaJob *)obj);
}


pgaScheduleFactory scheduleFactory;
static pgaCollectionFactory cf(&scheduleFactory, __("Schedules"), schedules_png_img);
