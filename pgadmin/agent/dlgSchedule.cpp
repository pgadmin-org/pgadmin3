//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSchedule.cpp - PostgreSQL Schedule Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// App headers
#include "utils/misc.h"
#include "agent/dlgSchedule.h"
#include "agent/pgaSchedule.h"

// image for de/-select all
#include "images/check.pngc"
#include "images/uncheck.pngc"

// pointer to controls
#define	txtID				CTRL_TEXT("txtID")
#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define calStart            CTRL_CALENDAR("calStart")
#define timStart            CTRL_TIME("timStart")
#define calEnd              CTRL_CALENDAR("calEnd")
#define timEnd              CTRL_TIME("timEnd")
#define chkWeekdays			CTRL_CHECKLISTBOX("chkWeekdays")
#define chkMonthdays		CTRL_CHECKLISTBOX("chkMonthdays")
#define chkMonths			CTRL_CHECKLISTBOX("chkMonths")
#define chkHours			CTRL_CHECKLISTBOX("chkHours")
#define chkMinutes			CTRL_CHECKLISTBOX("chkMinutes")
#define lstExceptions       CTRL_LISTVIEW("lstExceptions")
#define timException     	CTRL_TIME("timException")
#define calException    	CTRL_CALENDAR("calException")
#define btnAddException     CTRL_BUTTON("btnAddException")
#define btnChangeException  CTRL_BUTTON("btnChangeException")
#define btnRemoveException  CTRL_BUTTON("btnRemoveException")
#define btnWeekdays  		CTRL_BUTTON("btnWeekdays")
#define btnMonthdays  		CTRL_BUTTON("btnMonthdays")
#define btnMonths  		CTRL_BUTTON("btnMonths")
#define btnHours  			CTRL_BUTTON("btnHours")
#define btnMinutes  		CTRL_BUTTON("btnMinutes")

BEGIN_EVENT_TABLE(dlgSchedule, dlgAgentProperty)
	EVT_CHECKBOX(XRCID("chkEnabled"),                dlgSchedule::OnChangeCom)
	EVT_CALENDAR_SEL_CHANGED(XRCID("calStart"),      dlgSchedule::OnChangeCal)
	EVT_TEXT(XRCID("timStart"),                      dlgSchedule::OnChangeCom)
	EVT_CALENDAR_SEL_CHANGED(XRCID("calEnd"),        dlgSchedule::OnChangeCal)
	EVT_TEXT(XRCID("timEnd"),                        dlgSchedule::OnChangeCom)
	EVT_LIST_ITEM_SELECTED(XRCID("lstExceptions"),   dlgSchedule::OnSelChangeException)
	EVT_BUTTON(XRCID("btnAddException"),             dlgSchedule::OnAddException)
	EVT_BUTTON(XRCID("btnChangeException"),          dlgSchedule::OnChangeException)
	EVT_BUTTON(XRCID("btnRemoveException"),          dlgSchedule::OnRemoveException)
	EVT_BUTTON(XRCID("btnWeekdays"),        	     dlgSchedule::OnSelectAllWeekdays)
	EVT_BUTTON(XRCID("btnMonthdays"),		         dlgSchedule::OnSelectAllMonthdays)
	EVT_BUTTON(XRCID("btnMonths"), 				     dlgSchedule::OnSelectAllMonths)
	EVT_BUTTON(XRCID("btnHours"),  			         dlgSchedule::OnSelectAllHours)
	EVT_BUTTON(XRCID("btnMinutes"),			         dlgSchedule::OnSelectAllMinutes)
	EVT_CHECKLISTBOX(XRCID("chkWeekdays"),           dlgSchedule::OnChangeCom)
	EVT_CHECKLISTBOX(XRCID("chkMonthdays"),          dlgSchedule::OnChangeCom)
	EVT_CHECKLISTBOX(XRCID("chkMonths"),             dlgSchedule::OnChangeCom)
	EVT_CHECKLISTBOX(XRCID("chkHours"),              dlgSchedule::OnChangeCom)
	EVT_CHECKLISTBOX(XRCID("chkMinutes"),            dlgSchedule::OnChangeCom)
#ifdef __WXMAC__
	EVT_SIZE(                                        dlgSchedule::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgProperty *pgaScheduleFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgSchedule(this, frame, (pgaSchedule *)node, (pgaJob *)parent);
}


dlgSchedule::dlgSchedule(pgaFactory *f, frmMain *frame, pgaSchedule *node, pgaJob *j)
	: dlgAgentProperty(f, frame, wxT("dlgSchedule"))
{
	schedule = node;
	job = j;
	if (job)
		jobId = job->GetRecId();
	else
		jobId = 0;

	lstExceptions->CreateColumns(0, _("Date"), _("Time"), 90);
	lstExceptions->AddColumn(wxT("dirty"), 0);
	lstExceptions->AddColumn(wxT("id"), 0);

	txtID->Disable();

	btnChangeException->Disable();
	btnRemoveException->Disable();

	int i;

	for (i = 0 ; i < 24 ; i++)
		chkHours->Append(wxString::Format(wxT("%02d"), i));

	for (i = 0 ; i < 60 ; i++)
		chkMinutes->Append(wxString::Format(wxT("%02d"), i));
}


pgObject *dlgSchedule::GetObject()
{
	return schedule;
}


#ifdef __WXMAC__
void dlgSchedule::OnChangeSize(wxSizeEvent &ev)
{
	lstExceptions->SetSize(wxDefaultCoord, wxDefaultCoord,
	                       ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


int dlgSchedule::Go(bool modal)
{
	int returncode;

	if (schedule)
	{
		// edit mode
		recId = schedule->GetRecId();
		txtID->SetValue(NumToStr(recId));
		chkEnabled->SetValue(schedule->GetEnabled());
		calStart->SetValue(schedule->GetStart().GetDateOnly());
		timStart->SetTime(schedule->GetStart());
		if (schedule->GetEnd().IsValid())
		{
			calEnd->SetValue(schedule->GetEnd().GetDateOnly());
			timEnd->SetTime(schedule->GetEnd());
		}
		else
		{
			calEnd->SetValue(wxInvalidDateTime);
			timEnd->SetTime(wxInvalidDateTime);
			timEnd->Disable();
		}

		unsigned int x;
		for (x = 0; x < schedule->GetMonths().Length(); x++ )
			if (schedule->GetMonths()[x] == 't') chkMonths->Check(x, true);

		for (x = 0; x < schedule->GetMonthdays().Length(); x++ )
			if (schedule->GetMonthdays()[x] == 't') chkMonthdays->Check(x, true);

		for (x = 0; x < schedule->GetWeekdays().Length(); x++ )
			if (schedule->GetWeekdays()[x] == 't') chkWeekdays->Check(x, true);

		for (x = 0; x < schedule->GetHours().Length(); x++ )
			if (schedule->GetHours()[x] == 't') chkHours->Check(x, true);

		for (x = 0; x < schedule->GetMinutes().Length(); x++ )
			if (schedule->GetMinutes()[x] == 't') chkMinutes->Check(x, true);

		wxString id, dateToken, timeToken;
		wxDateTime val;
		long pos = 0;
		wxStringTokenizer tkz(schedule->GetExceptions(), wxT("|"));

		while (tkz.HasMoreTokens() )
		{
			dateToken.Empty();
			timeToken.Empty();

			// First is the ID
			id = tkz.GetNextToken();

			// Look for a date
			if (tkz.HasMoreTokens())
				dateToken = tkz.GetNextToken();

			// Look for a time
			if (tkz.HasMoreTokens())
				timeToken = tkz.GetNextToken();

			if (!dateToken.IsEmpty() && !timeToken.IsEmpty())
			{
				val.ParseDate(dateToken);
				val.ParseTime(timeToken);
				pos = lstExceptions->AppendItem(0, val.FormatDate(), val.FormatTime());
			}
			else if (!dateToken.IsEmpty() && timeToken.IsEmpty())
			{
				val.ParseDate(dateToken);
				pos = lstExceptions->AppendItem(0, val.FormatDate(), _("<any>"));
			}
			else if (dateToken.IsEmpty() && !timeToken.IsEmpty())
			{
				val.ParseTime(timeToken);
				pos = lstExceptions->AppendItem(0, _("<any>"), val.FormatTime());
			}

			lstExceptions->SetItem(pos, 2, BoolToStr(false));
			lstExceptions->SetItem(pos, 3, id);

		}

		wxNotifyEvent ev;
	}
	else
	{
		// create mode
	}

	// setup de-/select buttons
	InitSelectAll();

	returncode = dlgProperty::Go(modal);

	SetSqlReadOnly(true);

	return returncode;
}


pgObject *dlgSchedule::CreateObject(pgCollection *collection)
{
	pgObject *obj = scheduleFactory.CreateObjects(collection, 0, wxT("   AND jscid=") + NumToStr(recId) + wxT("\n"));
	return obj;
}


void dlgSchedule::OnChangeCal(wxCalendarEvent &ev)
{
	CheckChange();
}


void dlgSchedule::OnChangeCom(wxCommandEvent &ev)
{
	CheckChange();
}


void dlgSchedule::CheckChange()
{
	timEnd->Enable(calEnd->GetValue().IsValid());

	wxString name = GetName();
	bool enable = true;

	if (statusBar)
		statusBar->SetStatusText(wxEmptyString);

	InitSelectAll();

	CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
	CheckValid(enable, calStart->GetValue().IsValid(), _("Please specify start date."));

	if (enable)
	{
		EnableOK(!GetSql().IsEmpty());
	}
	else
		EnableOK(false);
}


void dlgSchedule::OnSelChangeException(wxListEvent &ev)
{
	int sel = lstExceptions->GetSelection();
	if (sel >= 0)
	{
		wxString exDate = lstExceptions->GetText(sel, 0);
		wxString exTime = lstExceptions->GetText(sel, 1);
		wxDateTime val, null;

		if (exDate == _("<any>"))
			calException->SetValue(null);
		else
		{
			val.ParseDate(exDate);
			calException->SetValue(val);
		}
		if (exTime == _("<any>"))
			timException->SetTime(null);
		else
		{
			val.ParseTime(exTime);
			timException->SetTime(val);
		}

		btnChangeException->Enable();
		btnRemoveException->Enable();
	}
}


void dlgSchedule::OnAddException(wxCommandEvent &ev)
{
	if (!calException->GetValue().IsValid() && timException->GetValue().IsNull())
	{
		wxMessageBox(_("You must enter a valid date and/or time!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
		return;
	}

	wxString exDate, exTime;

	if (calException->GetValue().IsValid())
		exDate = calException->GetValue().FormatDate();
	else
		exDate = _("<any>");

	if (!timException->GetValue().IsNull())
		exTime = timException->GetValue().Format();
	else
		exTime = _("<any>");

	for (int pos = 0; pos < lstExceptions->GetItemCount(); pos++)
	{

		if (lstExceptions->GetText(pos, 0) == exDate &&
		        lstExceptions->GetText(pos, 1) == exTime)
		{
			wxMessageBox(_("The specified exception already exists!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
			return;
		}

		if (lstExceptions->GetText(pos, 0) == exDate &&
		        lstExceptions->GetText(pos, 1) == _("<any>"))
		{
			wxMessageBox(_("An exception already exists for any time on this date!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
			return;
		}

		if (lstExceptions->GetText(pos, 1) == exTime &&
		        lstExceptions->GetText(pos, 0) == _("<any>"))
		{
			wxMessageBox(_("An exception already exists for this time on any date!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
			return;
		}
	}

	lstExceptions->AppendItem(0, exDate, exTime);
	CheckChange();
}


void dlgSchedule::OnChangeException(wxCommandEvent &ev)
{
	if (!calException->GetValue().IsValid() && timException->GetValue().IsNull())
	{
		wxMessageBox(_("You must enter a valid date and/or time!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
		return;
	}

	wxString exDate, exTime;

	if (calException->GetValue().IsValid())
		exDate = calException->GetValue().FormatDate();
	else
		exDate = _("<any>");

	if (!timException->GetValue().IsNull())
		exTime = timException->GetValue().Format();
	else
		exTime = _("<any>");

	long item = lstExceptions->GetFocusedItem();

	for (int pos = 0; pos < lstExceptions->GetItemCount(); pos++)
	{
		if (item != pos)
		{
			if (lstExceptions->GetText(pos, 0) == exDate &&
			        lstExceptions->GetText(pos, 1) == exTime)
			{
				wxMessageBox(_("The specified exception already exists!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
				return;
			}

			if (lstExceptions->GetText(pos, 0) == exDate &&
			        lstExceptions->GetText(pos, 1) == _("<any>"))
			{
				wxMessageBox(_("An exception already exists for any time on this date!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
				return;
			}

			if (lstExceptions->GetText(pos, 1) == exTime &&
			        lstExceptions->GetText(pos, 0) == _("<any>"))
			{
				wxMessageBox(_("An exception already exists for this time on any date!"), _("Add exception"), wxICON_EXCLAMATION | wxOK, this);
				return;
			}
		}
	}

	lstExceptions->SetItem(item, 0, exDate);
	lstExceptions->SetItem(item, 1, exTime);
	lstExceptions->SetItem(item, 2, BoolToStr(true));
	CheckChange();
}


void dlgSchedule::OnRemoveException(wxCommandEvent &ev)
{
	if (lstExceptions->GetText(lstExceptions->GetFocusedItem(), 3) != wxEmptyString)
	{
		deleteExceptions.Add(lstExceptions->GetText(lstExceptions->GetFocusedItem(), 3));
	}
	lstExceptions->DeleteCurrentItem();

	btnChangeException->Disable();
	btnRemoveException->Disable();

	CheckChange();
}


wxString dlgSchedule::GetComment()
{
	return txtComment->GetValue();
}


wxString dlgSchedule::GetInsertSql()
{
	wxString sql;
	if (!schedule)
	{
		wxString name = GetName();
		wxString jscjobid, list = wxT("NULL");
		if (jobId)
			jscjobid = NumToStr(jobId);
		else
			jscjobid = wxT("<JobId>");

		// Build the various arrays of values
		sql = wxT("INSERT INTO pgagent.pga_schedule (jscid, jscjobid, jscname, jscdesc, jscminutes, jschours, jscweekdays, jscmonthdays, jscmonths, jscenabled, jscstart, jscend)\n")
		      wxT("VALUES(<SchId>, ") + jscjobid + wxT(", ") + qtDbString(name) + wxT(", ") + qtDbString(txtComment->GetValue()) + wxT(", ")
		      + wxT("'") + ChkListBox2PgArray(chkMinutes) + wxT("', ")
		      + wxT("'") + ChkListBox2PgArray(chkHours) + wxT("', ")
		      + wxT("'") + ChkListBox2PgArray(chkWeekdays) + wxT("', ")
		      + wxT("'") + ChkListBox2PgArray(chkMonthdays) + wxT("', ")
		      + wxT("'") + ChkListBox2PgArray(chkMonths) + wxT("', ")
		      + BoolToStr(chkEnabled->GetValue()) + wxT(", ")
		      + wxT("'") + DateToAnsiStr(calStart->GetValue() + timStart->GetValue()) + wxT("'");

		if (calEnd->GetValue().IsValid())
			sql += wxT(", '") + DateToAnsiStr(calEnd->GetValue() + timEnd->GetValue()) + wxT("'");
		else
			sql += wxT(", NULL");

		sql += wxT(");\n");
	}

	return sql;
}


wxString dlgSchedule::GetUpdateSql()
{
	wxString sql, name;
	name = GetName();

	if (schedule)
	{
		// edit mode
		wxString vars;

		if (name != schedule->GetName())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscname = ") + qtDbString(name));
		}
		if (txtComment->GetValue() != schedule->GetComment())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscdesc = ") + qtDbString(txtComment->GetValue()));
		}

		if ((!chkEnabled->IsChecked() && schedule->GetEnabled()) || (chkEnabled->IsChecked() && !schedule->GetEnabled()))
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscenabled = ") + BoolToStr(chkEnabled->IsChecked()));
		}

		if (calStart->GetValue() + timStart->GetValue() != schedule->GetStart())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscstart = '") + DateToAnsiStr(calStart->GetValue() + timStart->GetValue()) + wxT("'"));
		}

		if (calEnd->GetValue().IsValid())
		{
			if (schedule->GetEnd().IsValid())
			{
				if (calEnd->GetValue() + timEnd->GetValue() != schedule->GetEnd())
				{
					if (!vars.IsEmpty())
						vars.Append(wxT(", "));
					vars.Append(wxT("jscend = '") + DateToAnsiStr(calEnd->GetValue() + timEnd->GetValue()) + wxT("'"));
				}
			}
			else
			{
				if (!vars.IsEmpty())
					vars.Append(wxT(", "));
				vars.Append(wxT("jscend = '") + DateToAnsiStr(calEnd->GetValue() + wxTimeSpan()) + wxT("'"));
			}
		}
		else
		{
			if (schedule->GetEnd().IsValid())
			{
				if (!vars.IsEmpty())
					vars.Append(wxT(", "));
				vars.Append(wxT("jscend = NULL"));
			}
		}

		if (ChkListBox2StrArray(chkMinutes) != schedule->GetMinutes())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscminutes = '") + ChkListBox2PgArray(chkMinutes) + wxT("'"));
		}

		if (ChkListBox2StrArray(chkHours) != schedule->GetHours())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jschours = '") + ChkListBox2PgArray(chkHours) + wxT("'"));
		}

		if (ChkListBox2StrArray(chkWeekdays) != schedule->GetWeekdays())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscweekdays = '") + ChkListBox2PgArray(chkWeekdays) + wxT("'"));
		}

		if (ChkListBox2StrArray(chkMonthdays) != schedule->GetMonthdays())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscmonthdays = '") + ChkListBox2PgArray(chkMonthdays) + wxT("'"));
		}

		if (ChkListBox2StrArray(chkMonths) != schedule->GetMonths())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jscmonths = '") + ChkListBox2PgArray(chkMonths) + wxT("'"));
		}

		if (!vars.IsEmpty())
			sql = wxT("UPDATE pgagent.pga_schedule SET ") + vars + wxT("\n")
			      wxT(" WHERE jscid=") + NumToStr(recId) + wxT(";\n");
	}
	else
	{
		// create mode
		// Handled by GetInsertSQL
	}

	unsigned int x = 0;
	int y = 0;
	wxDateTime tmpDateTime;
	wxString newDate, newTime;

	// Remove old exceptions
	for (x = 0; x < deleteExceptions.Count(); x++)
	{
		sql += wxT("DELETE FROM pgagent.pga_exception\n  WHERE jexid = ") + deleteExceptions[x] + wxT(";\n");
	}

	// Update dirty exceptions
	for (y = 0; y < lstExceptions->GetItemCount(); y++)
	{
		if (lstExceptions->GetText(y, 2) == BoolToStr(true) &&
		        lstExceptions->GetText(y, 3) != wxEmptyString)
		{
			if (lstExceptions->GetText(y, 0) == _("<any>"))
				newDate = wxT("null");
			else
			{
				tmpDateTime.ParseFormat(lstExceptions->GetText(y, 0), wxT("%x"));
				newDate = wxT("'") + tmpDateTime.FormatISODate() + wxT("'");
			}

			if (lstExceptions->GetText(y, 1) == _("<any>"))
				newTime = wxT("null");
			else
			{
				tmpDateTime.ParseTime(lstExceptions->GetText(y, 1));
				newTime = wxT("'") + tmpDateTime.FormatISOTime() + wxT("'");
			}

			sql += wxT("UPDATE pgagent.pga_exception SET jexdate = ") + newDate +
			       wxT(", jextime = ") + newTime + wxT("\n  WHERE jexid = ") +
			       lstExceptions->GetText(y, 3) + wxT(";\n");
		}
	}

	// Insert new exceptions
	for (y = 0; y < lstExceptions->GetItemCount(); y++)
	{
		if (lstExceptions->GetText(y, 2) == wxEmptyString &&
		        lstExceptions->GetText(y, 3) == wxEmptyString)
		{
			if (lstExceptions->GetText(y, 0) == _("<any>"))
				newDate = wxT("null");
			else
			{
				tmpDateTime.ParseFormat(lstExceptions->GetText(y, 0), wxT("%x"));
				newDate = wxT("'") + tmpDateTime.FormatISODate() + wxT("'");
			}

			if (lstExceptions->GetText(y, 1) == _("<any>"))
				newTime = wxT("null");
			else
			{
				tmpDateTime.ParseTime(lstExceptions->GetText(y, 1));
				newTime = wxT("'") + tmpDateTime.FormatISOTime() + wxT("'");
			}

			sql += wxT("INSERT INTO pgagent.pga_exception (jexscid, jexdate, jextime)\n  VALUES (")
			       + NumToStr(recId) + wxT(", ") + newDate + wxT(", ") + newTime + wxT(");\n");

		}
	}



	return sql;
}


const wxString dlgSchedule::ChkListBox2PgArray(wxCheckListBox *lb)
{
	wxString res = wxT("{");

	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (lb->IsChecked(x))
			res += wxT("t,");
		else
			res += wxT("f,");
	}
	if (res.Length() > 1)
		res.RemoveLast();

	res += wxT("}");

	return res;
}


const wxString dlgSchedule::ChkListBox2StrArray(wxCheckListBox *lb)
{
	wxString res;

	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (lb->IsChecked(x))
			res += wxT("t");
		else
			res += wxT("f");
	}

	return res;
}


void dlgSchedule::OnSelectAll(wxCommandEvent &ev, int origin)
{
	bool check = false;
	wxBitmapButton *btn;
	wxCheckListBox *lb;
	wxString tooltip;

	switch (origin)
	{
		case 1:
			btn = ((wxBitmapButton *)btnWeekdays);
			lb = chkWeekdays;
			break;
		case 2:
			btn = ((wxBitmapButton *)btnMonthdays);
			lb = chkMonthdays;
			break;
		case 3:
			btn = ((wxBitmapButton *)btnMonths);
			lb = chkMonths;
			break;
		case 4:
			btn = ((wxBitmapButton *)btnHours);
			lb = chkHours;
			break;
		case 5:
			btn = ((wxBitmapButton *)btnMinutes);
			lb = chkMinutes;
			break;
		default:
			return;
			break;
	}

	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (!lb->IsChecked(x))
		{
			check = true;
			break;
		}
	}
	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		lb->Check(x, check);
	}

	CheckChange();
}


void dlgSchedule::InitSelectAll()
{
	bool check = false;
	wxBitmapButton *btn;
	wxCheckListBox *lb;
	wxString tooltip;

	btn = ((wxBitmapButton *)btnWeekdays);
	lb = chkWeekdays;
	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (!lb->IsChecked(x))
		{
			check = true;
			break;
		}
	}

	if (check)
	{
		btn->SetBitmapLabel(*check_png_bmp);
		tooltip = _("Select all week days");
	}
	else
	{
		btn->SetBitmapLabel(*uncheck_png_bmp);
		tooltip = _("Deselect all week days");
	}
	btn->SetToolTip(tooltip);

	check = false;
	btn = ((wxBitmapButton *)btnMonthdays);
	lb = chkMonthdays;
	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (!lb->IsChecked(x))
		{
			check = true;
			break;
		}
	}

	if (check)
	{
		btn->SetBitmapLabel(*check_png_bmp);
		tooltip = _("Select all month days");
	}
	else
	{
		btn->SetBitmapLabel(*uncheck_png_bmp);
		tooltip = _("Deselect all month days");
	}
	btn->SetToolTip(tooltip);

	check = false;
	btn = ((wxBitmapButton *)btnMonths);
	lb = chkMonths;
	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (!lb->IsChecked(x))
		{
			check = true;
			break;
		}
	}

	if (check)
	{
		btn->SetBitmapLabel(*check_png_bmp);
		tooltip = _("Select all months");
	}
	else
	{
		btn->SetBitmapLabel(*uncheck_png_bmp);
		tooltip = _("Deselect all months");
	}
	btn->SetToolTip(tooltip);

	check = false;
	btn = ((wxBitmapButton *)btnHours);
	lb = chkHours;
	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (!lb->IsChecked(x))
		{
			check = true;
			break;
		}
	}

	if (check)
	{
		btn->SetBitmapLabel(*check_png_bmp);
		tooltip = _("Select all hours");
	}
	else
	{
		btn->SetBitmapLabel(*uncheck_png_bmp);
		tooltip = _("Deselect all hours");
	}
	btn->SetToolTip(tooltip);


	check = false;
	btn = ((wxBitmapButton *)btnMinutes);
	lb = chkMinutes;
	for (unsigned int x = 0; x < lb->GetCount(); x++)
	{
		if (!lb->IsChecked(x))
		{
			check = true;
			break;
		}
	}

	if (check)
	{
		btn->SetBitmapLabel(*check_png_bmp);
		tooltip = _("Select all minutes");
	}
	else
	{
		btn->SetBitmapLabel(*uncheck_png_bmp);
		tooltip = _("Deselect all minutes");
	}
	btn->SetToolTip(tooltip);
}
