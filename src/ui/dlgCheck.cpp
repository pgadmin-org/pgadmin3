//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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


#define chkDeferrable   CTRL_CHECKBOX("chkDeferrable")
#define chkDeferred     CTRL_CHECKBOX("chkDeferred")
#define stDeferred      CTRL_STATIC("stDeferred")
#define txtWhere        CTRL_TEXT("txtWhere")


BEGIN_EVENT_TABLE(dlgCheck, dlgProperty)
    EVT_TEXT(XRCID("txtWhere"),                 dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkDeferrable"),        dlgCheck::OnCheckDeferrable)
END_EVENT_TABLE();



dlgCheck::dlgCheck(frmMain *frame, pgCheck *node, pgTable *parentNode)
: dlgProperty(frame, wxT("dlgCheck"))
{
    check=node;
    table=parentNode;
    objectType=PG_CHECK;
}


void dlgCheck::OnCheckDeferrable(wxCommandEvent &ev)
{
    bool canDef=chkDeferrable->GetValue();
    stDeferred->Enable(canDef);
    if (!canDef)
        chkDeferred->SetValue(false);
    chkDeferred->Enable(canDef);
}


void dlgCheck::CheckChange()
{
    if (check)
    {
        EnableOK(txtComment->GetValue() != check->GetComment());
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
        txtName->Disable();

        txtWhere->SetValue(check->GetDefinition());
        txtWhere->Disable();

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
