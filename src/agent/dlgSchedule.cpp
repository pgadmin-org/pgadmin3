//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSchedule.cpp - PostgreSQL Schedule Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "misc.h"
#include "dlgSchedule.h"
#include "pgaSchedule.h"

// Images
#include "images/schedule.xpm"


// pointer to controls
#define	txtID				CTRL_TEXT("txtID")
#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define calStart            CTRL_CALENDAR("calStart")
#define timStart            CTRL_TIME("timStart")
#define calEnd              CTRL_CALENDAR("calEnd")
#define timEnd              CTRL_TIME("timEnd")
#define chkWeekdays			CTRL_CHECKLISTBOX("chkWeekDays")
#define chkMonthdays		CTRL_CHECKLISTBOX("chkMonthDays")
#define chkMonths			CTRL_CHECKLISTBOX("chkMonths")
#define chkHours			CTRL_CHECKLISTBOX("chkHours")
#define chkMinutes			CTRL_CHECKLISTBOX("chkMinutes")
#define lstExceptions       CTRL_LISTCTRL("lstExceptions")
#define timExceptionTime	CTRL_TIME("timExceptionTime")
#define calExceptionTime	CTRL_CALENDAR("calExceptionTime")
#define btnAddException     CTRL_BUTTON("btnAddException")
#define btnChangeException  CTRL_BUTTON("btnChangeException")
#define btnRemoveException  CTRL_BUTTON("btnRemoveException")


BEGIN_EVENT_TABLE(dlgSchedule, dlgAgentProperty)
    EVT_CHECKBOX(XRCID("chkEnabled"),                dlgProperty::OnChange)
    EVT_CALENDAR_SEL_CHANGED(XRCID("calStart"),      dlgSchedule::OnChangeCal)
    EVT_SPIN(XRCID("timStart"),                      dlgSchedule::OnChangeSpin)
    EVT_CALENDAR_SEL_CHANGED(XRCID("calEnd"),        dlgSchedule::OnChangeCal)
    EVT_SPIN(XRCID("timEnd"),                        dlgSchedule::OnChangeSpin)
    EVT_LIST_ITEM_SELECTED(XRCID("lstExceptions"),   dlgSchedule::OnSelChangeException)
    EVT_BUTTON(XRCID("btnAddException"),             dlgSchedule::OnAddException)
    EVT_BUTTON(XRCID("btnChangeException"),          dlgSchedule::OnChangeException)
    EVT_BUTTON(XRCID("btnRemoveException"),          dlgSchedule::OnRemoveException)
END_EVENT_TABLE();


dlgSchedule::dlgSchedule(frmMain *frame, pgaSchedule *node, pgaJob *j)
: dlgAgentProperty(frame, wxT("dlgSchedule"))
{
    SetIcon(wxIcon(schedule_xpm));
    objectType=PGA_SCHEDULE;
    schedule=node;
    job=j;
    if (job)
        jobId=job->GetRecId();
    else
        jobId=0;

	txtID->Disable();

    btnChangeException->Disable();
    btnRemoveException->Disable();


}


pgObject *dlgSchedule::GetObject()
{
    return schedule;
}


int dlgSchedule::Go(bool modal)
{

    if (schedule)
    {
        // edit mode
		recId = schedule->GetRecId();
		txtID->SetValue(NumToStr(recId));
        chkEnabled->SetValue(schedule->GetEnabled());
        calStart->SetValue(schedule->GetStart());
        timStart->SetTime(schedule->GetStart());
        if (schedule->GetEnd().IsValid())
        {
            calEnd->SetValue(schedule->GetEnd());
            timEnd->SetTime(schedule->GetEnd());
        }
        else
            timEnd->Disable();

		unsigned int x;
		for (x=0; x<schedule->GetMonths().Length(); x++ )
			if (schedule->GetMonths()[x] == 't') chkMonths->Check(x, true);

		for (x=0; x<schedule->GetMonthdays().Length(); x++ )
			if (schedule->GetMonthdays()[x] == 't') chkMonthdays->Check(x, true);

		for (x=0; x<schedule->GetWeekdays().Length(); x++ )
			if (schedule->GetWeekdays()[x] == 't') chkWeekdays->Check(x, true);

		for (x=0; x<schedule->GetHours().Length(); x++ )
			if (schedule->GetHours()[x] == 't') chkHours->Check(x, true);

		for (x=0; x<schedule->GetMinutes().Length(); x++ )
			if (schedule->GetMinutes()[x] == 't') chkMinutes->Check(x, true);

        wxNotifyEvent ev;
    }
    else
    {
        // create mode
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgSchedule::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgaSchedule::ReadObjects(collection, 0, wxT("   AND jscid=") + NumToStr(recId) + wxT("\n"));
    return obj;
}


void dlgSchedule::OnChangeCal(wxCalendarEvent &ev)
{
    CheckChange();
}


void dlgSchedule::OnChangeSpin(wxSpinEvent &ev)
{
    CheckChange();
}


void dlgSchedule::CheckChange()
{
    timEnd->Enable(calEnd->GetValue().IsValid());

    wxString name=GetName();
    bool enable;
    if (schedule)
    {
        enable  =  name != schedule->GetName()
                || chkEnabled->GetValue() != schedule->GetEnabled()
                || txtComment->GetValue() != schedule->GetComment();
    }
    else
    {
        enable=true;
    }
    CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
	CheckValid(enable, calStart->GetValue().IsValid(), _("Please specify start date."));
    EnableOK(enable);
}


void dlgSchedule::OnSelChangeException(wxListEvent &ev)
{
    btnChangeException->Enable();
    btnRemoveException->Enable();
}


void dlgSchedule::OnAddException(wxCommandEvent &ev)
{
}


void dlgSchedule::OnChangeException(wxCommandEvent &ev)
{
}


void dlgSchedule::OnRemoveException(wxCommandEvent &ev)
{
    btnChangeException->Disable();
    btnRemoveException->Disable();
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
        wxString name=GetName();
        wxString jscjobid, list=wxT("NULL");
        if (jobId)
            jscjobid = NumToStr(jobId);
        else
            jscjobid = wxT("<JobId>");

		// Build the various arrays of values
        sql = wxT("INSERT INTO pgagent.pga_schedule (jscid, jscjobid, jscname, jscdesc, jscminutes, jschours, jscweekdays, jscmonthdays, jscmonths, jscenabled, jscstart, jscend)\n")
              wxT("VALUES(<SchId>, ") + jscjobid + wxT(", ") + qtString(name) + wxT(", ") + qtString(txtComment->GetValue()) + wxT(", ")
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

		sql += wxT(")");
    }

    return sql;
}


wxString dlgSchedule::GetUpdateSql()
{
    wxString sql, name;
    name=GetName();

    if (schedule)
    {
        // edit mode
        wxString vars;

        if (name != schedule->GetName())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jscname = ") + qtString(name));
        }
        if (txtComment->GetValue() != schedule->GetComment())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jscdesc = ") + qtString(txtComment->GetValue()));
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
                  wxT(" WHERE jscid=") + NumToStr(recId);
    }
    else
    {
        // create mode
		// Handled by GetInsertSQL
    }
    return sql;
}

const wxString dlgSchedule::ChkListBox2PgArray(wxCheckListBox *lb)
{
	wxString res = wxT("{");

	for (int x=0; x<lb->GetCount(); x++)
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

	for (int x=0; x<lb->GetCount(); x++)
	{
		if (lb->IsChecked(x))
			res += wxT("t");
		else
			res += wxT("f");
	}

	return res;
}
