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
#define chkWeekDays			CTRL_CHECKLISTBOX("chkWeekDays")
#define chkMonthDays		CTRL_CHECKLISTBOX("chkMonthDays")
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
        jobId=job->GetJobId();
    else
        jobId=0;

    btnChangeException->Disable();
    btnRemoveException->Disable();

	txtID->Disable();
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
		txtID->SetValue(NumToStr(schedule->GetScheduleId()));
        chkEnabled->SetValue(schedule->GetEnabled());
        calStart->SetDate(schedule->GetStart());
        timStart->SetTime(schedule->GetStart());
        if (schedule->GetEnd().IsValid())
        {
            calEnd->SetDate(schedule->GetEnd());
            timEnd->SetTime(schedule->GetEnd());
        }
        else
            timEnd->Disable();

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
    wxString name=GetName();

    pgObject *obj=pgaSchedule::ReadObjects(collection, 0, wxT("   AND jscid=") + NumToStr(jobId) + wxT("\n"));
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
    timEnd->Enable(calEnd->GetDate().IsValid());

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
            jscjobid = wxT("<id>");
        sql = wxT("INSERT INTO pgagent.pga_jobschedule (jscjobid, jscname, jscdesc, jscenabled, jscstart, jscend)\n")
              wxT("VALUES(") + jscjobid + wxT(", ") + qtString(name) + wxT(", ") + qtString(txtComment->GetValue()) + wxT(", ")
                + BoolToStr(chkEnabled->GetValue()) 
				+ DateToAnsiStr(calStart->GetDate() + timStart->GetValue()) + wxT(", ") 
				+ DateToAnsiStr(calEnd->GetDate() + timEnd->GetValue())
                + wxT(")");

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
    }
    else
    {
        // create mode

    }
    return sql;
}
