//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgCheck.cpp - PostgreSQL Check Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "pgTable.h"
#include "pgCheck.h"
#include "dlgCheck.h"


#define chkDeferrable   CTRL("chkDeferrable",   wxCheckBox)
#define chkDeferred     CTRL("chkDeferred",     wxCheckBox)
#define stDeferred      CTRL("stDeferred",      wxStaticText)
#define txtWhere        CTRL("txtWhere",        wxTextCtrl)


BEGIN_EVENT_TABLE(dlgCheck, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                  dlgCheck::OnChange)
    EVT_TEXT(XRCID("txtComment"),                  dlgCheck::OnChange)
    EVT_TEXT(XRCID("txtWhere"),                 dlgCheck::OnChange)
    EVT_CHECKBOX(XRCID("chkDeferrable"),        dlgCheck::OnCheckDeferrable)
END_EVENT_TABLE();



dlgCheck::dlgCheck(frmMain *frame, pgCheck *node, pgTable *parentNode)
: dlgProperty(frame, wxT("dlgCheck"))
{
    check=node;
    table=parentNode;
    objectType=PG_CHECK;
}


void dlgCheck::OnCheckDeferrable(wxNotifyEvent &ev)
{
    bool canDef=chkDeferrable->GetValue();
    stDeferred->Enable(canDef);
    if (!canDef)
        chkDeferred->SetValue(false);
    chkDeferred->Enable(canDef);
}


void dlgCheck::OnChange(wxNotifyEvent &ev)
{
    if (check)
    {
        btnOK->Enable(txtComment->GetValue() != check->GetComment());
    }
    else
    {
        bool enable=true;
        txtComment->Enable(!GetName().IsEmpty());
        CheckValid(enable, !txtWhere->GetValue().IsEmpty(), _("Please specify condition."));
        EnableOK(enable);
    }
}


pgObject *dlgCheck::GetObject()
{
    return check;
}


pgObject *dlgCheck::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    if (name.IsEmpty())
        return 0;

    pgObject *obj=pgCheck::ReadObjects(collection, 0, wxT(
        "\n   AND conname=") + qtString(name) + wxT(
        "\n   AND relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}


int dlgCheck::Go(bool modal)
{
    if (check)
    {
        // edit mode: view only
        txtName->SetValue(check->GetName());
        txtName->Disable();

        txtWhere->SetValue(check->GetDefinition());
        txtWhere->Disable();

        txtComment->SetValue(check->GetComment());

        chkDeferrable->SetValue(check->GetDeferrable());
        chkDeferred->SetValue(check->GetDeferred());
        chkDeferrable->Disable();
        chkDeferred->Disable();
    }
    else
    {
        // create mode
        txtComment->Disable();
    }
    return dlgProperty::Go(modal);
}


wxString dlgCheck::GetSql()
{
    wxString sql;
    wxString name=GetName();

    if (!check)
    {
        sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
            + wxT(" ADD");
        AppendIfFilled(sql, wxT(" CONSTRAINT "), qtIdent(name));
        sql +=wxT(" CHECK ") + GetDefinition()
            + wxT(";\n");
    }
    if (!name.IsEmpty())
        AppendComment(sql, wxT("CONSTRAINT ") + qtIdent(name) 
            + wxT(" ON ") + table->GetQuotedFullIdentifier(), check);
    return sql;
}


wxString dlgCheck::GetDefinition()
{
    wxString sql;

    sql = wxT("(") + txtWhere->GetValue() + wxT(")");

    return sql;
}
