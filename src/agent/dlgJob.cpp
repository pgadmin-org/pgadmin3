//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgJob.cpp - PostgreSQL Job Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgJob.h"
#include "pgaJob.h"

// Images
#include "images/job.xpm"


// pointer to controls
#define chkEnabled          CTRL("chkEnabled", wxCheckBox)
#define cbJobclass          CTRL("cbJobclass", wxComboBox)
#define txtCreated          CTRL("txtCreated", wxTextCtrl)
#define txtChanged          CTRL("txtChanged", wxTextCtrl)
#define txtNextrun          CTRL("txtNextrun", wxTextCtrl)
#define txtLastrun          CTRL("txtLastrun", wxTextCtrl)
#define txtLastresult       CTRL("txtLastResult", wxTextCtrl)

#define lstSteps            CTRL("lstSteps", wxListCtrl)
#define btnChangeStep       CTRL("btnChangeStep", wxButton)
#define btnAddStep          CTRL("btnAddStep", wxButton)
#define btnRemoveStep       CTRL("btnRemoveStep", wxButton)
#define lstSchedules        CTRL("lstSchedules", wxListCtrl)
#define btnChangeSchedule   CTRL("btnChangeSchedule", wxButton)
#define btnAddSchedule      CTRL("btnAddSchedule", wxButton)
#define btnRemoveSchedule   CTRL("btnRemoveSchedule", wxButton)


BEGIN_EVENT_TABLE(dlgJob, dlgOidProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgJob::OnChange)
    EVT_CHECKBOX(XRCID("chkEnabled"),               dlgJob::OnChange)
    EVT_COMBOBOX(XRCID("cbJobclass"),               dlgJob::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgJob::OnChange)

    EVT_BUTTON(XRCID("btnChangeStep"),              dlgJob::OnChangeStep)
    EVT_BUTTON(XRCID("btnAddStep"),                 dlgJob::OnAddStep)
    EVT_BUTTON(XRCID("btnRemoveStep"),              dlgJob::OnRemoveStep)
    EVT_BUTTON(XRCID("btnChangeSchedule"),          dlgJob::OnChangeSchedule)
    EVT_BUTTON(XRCID("btnAddSchedule"),             dlgJob::OnAddSchedule)
    EVT_BUTTON(XRCID("btnRemoveStedule"),           dlgJob::OnRemoveSchedule)
END_EVENT_TABLE();


dlgJob::dlgJob(frmMain *frame, pgaJob *node)
: dlgOidProperty(frame, wxT("dlgJob"))
{
    SetIcon(wxIcon(job_xpm));
    job=node;

    txtOID->Disable();
    txtCreated->Disable();
    txtChanged->Disable();
    txtNextrun->Disable();
    txtLastrun->Disable();
    txtLastresult->Disable();
}


pgObject *dlgJob::GetObject()
{
    return job;
}


int dlgJob::Go(bool modal)
{
    pgSet *jcl=connection->ExecuteSet(wxT("SELECT jclname FROM pga_jobclass"));
    if (jcl)
    {
        while (!jcl->Eof())
        {
            cbJobclass->Append(jcl->GetVal(0));
            jcl->MoveNext();
        }
        delete jcl;
    }

    if (job)
    {
        // edit mode
        cbJobclass->SetValue(job->GetJobclass());
        txtName->SetValue(job->GetName());
        txtOID->SetValue(NumToStr((long)job->GetOid()));
        chkEnabled->SetValue(job->GetEnabled());
        txtCreated->SetValue(DateToStr(job->GetCreated()));
        txtChanged->SetValue(DateToStr(job->GetChanged()));
        txtNextrun->SetValue(DateToStr(job->GetNextrun()));
        txtLastrun->SetValue(DateToStr(job->GetLastrun()));
        txtLastresult->SetValue(job->GetLastresult());
        txtComment->SetValue(job->GetComment());
    }
    else
    {
        // create mode
        cbJobclass->SetSelection(0);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgJob::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgaJob::ReadObjects((pgaAgent*)collection, 0, wxT("   AND j.oid=") + NumToStr(oid) + wxT("\n"));
    return obj;
}


void dlgJob::OnChange(wxNotifyEvent &ev)
{
    bool enable;
    wxString name=GetName();
    if (job)
    {
        enable  =  txtComment->GetValue() != job->GetComment()
                || name != job->GetName()
                || chkEnabled->GetValue() != job->GetEnabled();
    }
    else
    {

    }
    CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
    CheckValid(enable, lstSteps->GetItemCount() > 0, _("Please specify at least one step."));

    EnableOK(enable);
}


void dlgJob::OnChangeStep(wxNotifyEvent &ev)
{
}


void dlgJob::OnAddStep(wxNotifyEvent &ev)
{
}


void dlgJob::OnRemoveStep(wxNotifyEvent &ev)
{
}


void dlgJob::OnChangeSchedule(wxNotifyEvent &ev)
{
}


void dlgJob::OnAddSchedule(wxNotifyEvent &ev)
{
}


void dlgJob::OnRemoveSchedule(wxNotifyEvent &ev)
{
}


wxString dlgJob::GetInsertSql()
{
    wxString sql;

    if (!job)
    {
        sql = wxT("INSERT INTO pga_job (jobjcloid, jobname, jobdesc, jobenabled)\n")
              wxT("SELECT jcl.oid, ") + qtString(GetName()) + 
              wxT(", ") + qtString(txtComment->GetValue()) + wxT(", ") + BoolToStr(chkEnabled->GetValue()) + wxT("\n")
              wxT("  FROM pga_jobclass jcl WHERE jclname=") + qtString(cbJobclass->GetValue());
    }
    return sql;
}


wxString dlgJob::GetUpdateSql()
{
    wxString sql, name;
    name=GetName();

    if (job)
    {
        // edit mode
        wxString vars;

        if (name != job->GetName())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jobname = ") + qtString(name));
        }
        if (cbJobclass->GetValue().Trim() != job->GetJobclass())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jobjcloid= (SELECT oid FROM pga_jobclass WHERE jclname=") + qtString(cbJobclass->GetValue()) + wxT(")"));
        }
        if (chkEnabled->GetValue() != job->GetEnabled())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jobenabled = ") + BoolToStr(chkEnabled->GetValue()));
        }
        if (txtComment->GetValue() != job->GetComment())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jobdesc = ") + qtString(job->GetComment()));
        }

        if (!vars.IsEmpty())
            sql = wxT("UPDATE pga_job SET ") + vars + wxT("\n")
                  wxT(" WHERE oid=") + job->GetOidStr();
    }
    else
    {
        // create mode
        // done by GetInsertSql
    }
    return sql;
}


