//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgStep.cpp - PostgreSQL Step Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgStep.h"
#include "pgaStep.h"

// Images
#include "images/step.xpm"


// pointer to controls
#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define cbDatabase          CTRL_COMBOBOX("cbDatabase")
#define rbxKind             CTRL_RADIOBOX("rbxKind")
#define rbxOnError          CTRL_RADIOBOX("rbxOnError")
#define pnlDefinition       CTRL_PANEL("pnlDefinition")
#define txtSqlBox           CTRL_TEXT("txtSqlBox")

#define CTL_SQLBOX  188

BEGIN_EVENT_TABLE(dlgStep, dlgAgentProperty)
    EVT_CHECKBOX(XRCID("chkEnabled"),               dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbDatabase"),               dlgProperty::OnChange)
    EVT_RADIOBOX(XRCID("rbxKind"),                  dlgProperty::OnChange)
    EVT_RADIOBOX(XRCID("rbxOnError"),               dlgProperty::OnChange)
    EVT_STC_MODIFIED(CTL_SQLBOX,                    dlgProperty::OnChangeStc)
END_EVENT_TABLE();


dlgStep::dlgStep(frmMain *frame, pgaStep *node, pgaJob *j)
: dlgAgentProperty(frame, wxT("dlgStep"))
{
    SetIcon(wxIcon(step_xpm));
    objectType=PGA_STEP;
    step=node;
    job=j;
    if (job)
        jobId=job->GetId();
    else
        jobId=0;

    sqlBox=new ctlSQLBox(pnlDefinition, CTL_SQLBOX, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);

    wxWindow *placeholder=CTRL_TEXT("txtSqlBox");
    wxSizer *sizer=placeholder->GetContainingSizer();
    sizer->Add(sqlBox, 1, wxRIGHT|wxGROW, 5);
    sizer->Remove(placeholder);
    delete placeholder;
    sizer->Layout();
}


pgObject *dlgStep::GetObject()
{
    return step;
}


int dlgStep::Go(bool modal)
{
    cbDatabase->Append(wxT(" "));
    cbDatabase->SetSelection(0);

    pgSet *db=connection->ExecuteSet(wxT("SELECT datname FROM pg_database"));
    if (db)
    {
        while (!db->Eof())
        {
            cbDatabase->Append(db->GetVal(0));
            db->MoveNext();
        }
        delete db;
    }

    if (step)
    {
        // edit mode
        if (step->GetDbname().IsEmpty())
            cbDatabase->SetSelection(0);
        else
            cbDatabase->SetValue(step->GetDbname());
        rbxKind->SetSelection(wxString(wxT("sb")).Find(step->GetKindChar()));
        rbxOnError->SetSelection(wxString(wxT("fsi")).Find(step->GetOnErrorChar()));
        sqlBox->SetText(step->GetCode());

        chkEnabled->SetValue(step->GetEnabled());
    }
    else
    {
        // create mode
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgStep::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=pgaStep::ReadObjects(job, 0, wxT("   AND jstid=") + NumToStr(id) + wxT("\n"));
    return obj;
}


void dlgStep::CheckChange()
{
    wxString name=GetName();
    bool enable;
    if (step)
    {
        enable  =  name != step->GetName()
                || chkEnabled->GetValue() != step->GetEnabled()
                || cbDatabase->GetValue().Trim() != step->GetDbname()
                || rbxKind->GetSelection() != wxString(wxT("sb")).Find(step->GetKindChar())
                || rbxOnError->GetSelection() != wxString(wxT("fsi")).Find(step->GetOnErrorChar())
                || txtComment->GetValue() != step->GetComment()
                || sqlBox->GetText() != step->GetCode();
    }
    else
    {
        enable=true;
    }
    CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
    CheckValid(enable, sqlBox->GetLength() > 0, _("Please specify code to execute."));
    EnableOK(enable);
}



wxString dlgStep::GetComment()
{
    return txtComment->GetValue();
}



wxString dlgStep::GetInsertSql()
{
    wxString sql;

    if (!step)
    {
        wxString name=GetName();
        wxString kind = wxT("sb")[rbxKind->GetSelection()];
        wxString onerror = wxT("fsi")[rbxOnError->GetSelection()];
        wxString db;
        wxString jstjobid;
        if (jobId)
            jstjobid = NumToStr(jobId);
        else
            jstjobid = wxT("<id>");

        if (!cbDatabase->GetSelection())
            db = wxT("NULL");
        else
            db = qtString(cbDatabase->GetValue());

        sql = wxT("INSERT INTO pgadmin.pga_jobstep (jstjobid, jstname, jstdesc, jstenabled, jstkind, jstonerror, jstcode, jstdbname)\n")
              wxT("SELECT ") + jstjobid + wxT(", ") + qtString(name) + wxT(", ") + qtString(txtComment->GetValue()) + wxT(", ")
                + BoolToStr(chkEnabled->GetValue()) + wxT(", ") + qtString(kind) + wxT(", ") 
                + qtString(onerror) + wxT(", ") + qtString(sqlBox->GetText()) + wxT(", ") + db;
    }
    return sql;
}


wxString dlgStep::GetUpdateSql()
{
    wxString sql;

    if (step)
    {
        // edit mode

        wxString name=GetName();

        wxString vars;
        if (name != step->GetName())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jstname=") + qtString(name));
        }
        if (chkEnabled->GetValue() != step->GetEnabled())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jstenabled=") + BoolToStr(chkEnabled->GetValue()));
        }
        if (cbDatabase->GetValue().Trim() != step->GetDbname())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            
            if (!cbDatabase->GetSelection())
                vars.Append(wxT("jstdbname=NULL"));
            else
                vars.Append(wxT("jstdbname=") + qtString(cbDatabase->GetValue()) + wxT(")"));
        }
        if (rbxKind->GetSelection() != wxString(wxT("sb")).Find(step->GetKindChar()))
        {
            wxString kind = wxT("sb")[rbxKind->GetSelection()];
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jstkind=") + qtString(kind));
        }
        if (rbxOnError->GetSelection() != wxString(wxT("fsi")).Find(step->GetOnErrorChar()))
        {
            wxString onerror = wxT("fsi")[rbxOnError->GetSelection()];
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jstname=") + qtString(name));
        }
        if (txtComment->GetValue() != step->GetComment())
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jstdesc=") + qtString(txtComment->GetValue()));
        }
        if (sqlBox->GetText() != step->GetCode())
        {
        {
            if (!vars.IsEmpty())
                vars.Append(wxT(", "));
            vars.Append(wxT("jstcode=") + qtString(sqlBox->GetText()));
        }
        }

        if (!vars.IsEmpty())
            sql = wxT("UPDATE pgadmin.pga_jobstep\n")
                  wxT("   SET ") + vars + wxT("\n")
                  wxT(" WHERE jstid=") + NumToStr(step->GetId());
    }
    else
    {
        // create mode; handled by GetInsertSql()
    }
    return sql;
}
