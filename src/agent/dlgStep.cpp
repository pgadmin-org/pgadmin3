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

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgStep.h"
#include "pgaStep.h"


// pointer to controls
#define	txtID				CTRL_TEXT("txtID")
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


dlgProperty *pgaStepFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgStep(this, frame, (pgaStep*)node, (pgaJob*)parent);
}


dlgStep::dlgStep(pgaFactory *f, frmMain *frame, pgaStep *node, pgaJob *j)
: dlgAgentProperty(f, frame, wxT("dlgStep"))
{
    step=node;
    job=j;
    if (job)
        jobId=job->GetRecId();
    else
        jobId=0;

    sqlBox=new ctlSQLBox(pnlDefinition, CTL_SQLBOX, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);

    wxWindow *placeholder=CTRL_TEXT("txtSqlBox");
    wxSizer *sizer=placeholder->GetContainingSizer();
    sizer->Add(sqlBox, 1, wxRIGHT|wxGROW, 5);
    sizer->Remove(placeholder);
    delete placeholder;
    sizer->Layout();

	txtID->Disable();
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
		recId = step->GetRecId();
		txtID->SetValue(NumToStr(recId));
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

    pgObject *obj=stepFactory.CreateObjects(collection, 0, wxT("   AND jstid=") + NumToStr(recId) + wxT("\n"));
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

	if (statusBar)
		statusBar->SetStatusText(wxEmptyString);

    CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
    CheckValid(enable, sqlBox->GetLength() > 0, _("Please specify code to execute."));

    // Disable/enable the database combo
    if ((wxT("sb")[rbxKind->GetSelection()]) == 'b')
    {
        cbDatabase->SetSelection(0);
        cbDatabase->Enable(false);
    }
    else
        cbDatabase->Enable(true);

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
            jstjobid = wxT("<JobId>");

        db = qtString(cbDatabase->GetValue().Trim());

        sql = wxT("INSERT INTO pgagent.pga_jobstep (jstid, jstjobid, jstname, jstdesc, jstenabled, jstkind, jstonerror, jstcode, jstdbname)\n")
              wxT("SELECT <StpId>, ") + jstjobid + wxT(", ") + qtString(name) + wxT(", ") + qtString(txtComment->GetValue()) + wxT(", ")
                + BoolToStr(chkEnabled->GetValue()) + wxT(", ") + qtString(kind) + wxT(", ") 
                + qtString(onerror) + wxT(", ") + qtString(sqlBox->GetText()) + wxT(", ") + db + wxT(";\n");
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
                vars.Append(wxT("jstdbname=''"));
            else
                vars.Append(wxT("jstdbname=") + qtString(cbDatabase->GetValue()));
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
            sql = wxT("UPDATE pgagent.pga_jobstep\n")
                  wxT("   SET ") + vars + wxT("\n")
                  wxT(" WHERE jstid=") + NumToStr(step->GetRecId()) +
				  wxT(";\n");
    }
    else
    {
        // create mode; handled by GetInsertSql()
    }
    return sql;
}
