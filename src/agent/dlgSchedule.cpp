//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSchedule.cpp - PostgreSQL Schedule Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include "calbox.h"
#include "timespin.h"

// App headers
#include "misc.h"
#include "dlgSchedule.h"
#include "pgaSchedule.h"

// Images
#include "images/schedule.xpm"


// pointer to controls
#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define cbKind              CTRL_COMBOBOX("cbKind")
#define timInterval         CTRL_TIME("timInterval")
#define calStart            CTRL_CALENDAR("calStart")
#define timStart            CTRL_TIME("timStart")
#define calEnd              CTRL_CALENDAR("calEnd")
#define timEnd              CTRL_TIME("timEnd")
#define calSchedule         CTRL_CALENDAR("calSchedule")
#define timSchedule         CTRL_TIME("timSchedule")
#define pnlSchedule         CTRL_PANEL("pnlSchedule")
#define lstIntervals        CTRL_TIME("lstIntervals")
#define timOffset           CTRL_TIME("timOffset")
#define btnAddInterval      CTRL_BUTTON("btnAddInterval")
#define btnChangeInterval   CTRL_BUTTON("btnChangeInterval")
#define btnRemoveInterval   CTRL_BUTTON("btnRemoveInterval")


BEGIN_EVENT_TABLE(dlgSchedule, dlgOidProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgSchedule::OnChange)
    EVT_CHECKBOX(XRCID("chkEnabled"),               dlgSchedule::OnChange)
    EVT_COMBOBOX(XRCID("cbKind"),                   dlgSchedule::OnChangeKind)
    EVT_SPIN(XRCID("timInterval"),                  dlgSchedule::OnChange)
    EVT_CALENDAR_SEL_CHANGED(XRCID("calStart"),     dlgSchedule::OnChange)
    EVT_SPIN(XRCID("timStart"),                     dlgSchedule::OnChange)
    EVT_CALENDAR_SEL_CHANGED(XRCID("calEnd"),       dlgSchedule::OnChange)
    EVT_SPIN(XRCID("timEnd"),                       dlgSchedule::OnChange)
    EVT_CALENDAR_SEL_CHANGED(XRCID("calSchedule"),  dlgSchedule::OnChange)
    EVT_SPIN(XRCID("timSchedule"),                  dlgSchedule::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgSchedule::OnChange)
    EVT_LIST_ITEM_SELECTED(XRCID("lstIntervals"),   dlgSchedule::OnSelChangeInterval)
    EVT_BUTTON(XRCID("btnAddInterval"),             dlgSchedule::OnAddInterval)
    EVT_BUTTON(XRCID("btnChangeInterval"),          dlgSchedule::OnChangeInterval)
    EVT_BUTTON(XRCID("btnRemoveInterval"),          dlgSchedule::OnRemoveInterval)
END_EVENT_TABLE();


dlgSchedule::dlgSchedule(frmMain *frame, pgaSchedule *node, pgaJob *j)
: dlgOidProperty(frame, wxT("dlgSchedule"))
{
    SetIcon(wxIcon(schedule_xpm));
    objectType=PGA_SCHEDULE;
    schedule=node;
    job=j;
    if (job)
        jobOid=job->GetOid();
    else
        jobOid=0;

    timInterval->SetMax(365*24*60*60 -1, true);
    txtOID->Disable();

    btnChangeInterval->Disable();
    btnRemoveInterval->Disable();
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
        txtName->SetValue(schedule->GetName());
        txtOID->SetValue(NumToStr((long)schedule->GetOid()));
        chkEnabled->SetValue(schedule->GetEnabled());
        cbKind->SetSelection(wxString(wxT("nsdwmy")).Find(schedule->GetKindChar()));
        calStart->SetDate(schedule->GetStart());
        timStart->SetTime(schedule->GetStart());
        if (schedule->GetEnd().IsValid())
        {
            calEnd->SetDate(schedule->GetEnd());
            timEnd->SetTime(schedule->GetEnd());
        }
        else
            timEnd->Disable();
        calSchedule->SetDate(schedule->GetSchedule());
        timSchedule->SetTime(schedule->GetSchedule());
        timInterval->SetValue(schedule->GetIntervalList().Item(0));

        txtComment->SetValue(schedule->GetComment());
        wxNotifyEvent ev;
        OnChangeKind(ev);
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

    pgObject *obj=pgaSchedule::ReadObjects(job, 0, wxT("   AND sc.oid=") + NumToStr(oid) + wxT("\n"));
    return obj;
}


void dlgSchedule::OnChangeKind(wxNotifyEvent &ev)
{
    switch (cbKind->GetSelection())
    {
        case 0: // repeat
            timInterval->Enable();
            pnlSchedule->Disable();
            break;
        case 1: // single
            timInterval->Disable();
            pnlSchedule->Disable();
            break;
        case 2: // day
            timOffset->SetMax(24*60*60-1);
            timInterval->Disable();
            pnlSchedule->Enable();
            break;
        case 3: // week
            timOffset->SetMax(7*24*60*60-1, true);
            timInterval->Disable();
            pnlSchedule->Enable();
            break;
        case 4: // month
            timOffset->SetMax(31*24*60*60-1, true);
            timInterval->Disable();
            pnlSchedule->Enable();
            break;
        case 5: // year
            timOffset->SetMax(365*24*60*60-1, true);
            timInterval->Disable();
            pnlSchedule->Enable();
            break;
        default:
            break;
    }
    OnChange(ev);
}


void dlgSchedule::OnChange(wxNotifyEvent &ev)
{
    timEnd->Enable(calEnd->GetDate().IsValid());

    wxString name=GetName();
    bool enable;
    if (schedule)
    {
        enable  =  name != schedule->GetName()
                || chkEnabled->GetValue() != schedule->GetEnabled()
                || cbKind->GetSelection() != wxString(wxT("nsdwmy")).Find(schedule->GetKindChar())
                || txtComment->GetValue() != schedule->GetComment();
    }
    else
    {
        enable=true;
    }
    CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
    EnableOK(enable);
}


void dlgSchedule::OnSelChangeInterval(wxNotifyEvent &ev)
{
    btnChangeInterval->Enable();
    btnRemoveInterval->Enable();
}


void dlgSchedule::OnAddInterval(wxNotifyEvent &ev)
{
}


void dlgSchedule::OnChangeInterval(wxNotifyEvent &ev)
{
}


void dlgSchedule::OnRemoveInterval(wxNotifyEvent &ev)
{
    btnChangeInterval->Disable();
    btnRemoveInterval->Disable();
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
        wxString kind = wxT("nsdwmy")[cbKind->GetSelection()];
        wxString jscjoboid, list=wxT("NULL");
        if (jobOid)
            jscjoboid = NumToStr(jobOid);
        else
            jscjoboid = wxT("<Oid>");
        sql = wxT("INSERT INTO pga_jobschedule (jscjoboid, jscname, jscdesc, jscenabled, jsckind, ")
              wxT("jscstart, jscend, jscschedule, jsclist)\n")
              wxT("VALUES(") + jscjoboid + wxT(", ") + qtString(name) + wxT(", ") + qtString(txtComment->GetValue()) + wxT(", ")
                + BoolToStr(chkEnabled->GetValue()) + wxT(", ") + qtString(kind) + wxT(", ") 
                + DateToAnsiStr(calStart->GetDate() + timStart->GetValue()) + wxT(", ")
                + DateToAnsiStr(calEnd->GetDate() + timEnd->GetValue()) + wxT(", ")
                + DateToAnsiStr(calSchedule->GetDate() + timSchedule->GetValue()) + wxT(", ")
                + list + wxT(")");

    }
    return sql;
}


wxString dlgSchedule::GetUpdateSql()
{
    wxString sql, name;
    name=GetName();

    if (job)
    {
        // edit mode
    }
    else
    {
        // create mode

    }
    return sql;
}
