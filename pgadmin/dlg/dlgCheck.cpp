//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgCheck.cpp - PostgreSQL Check Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "schema/pgTable.h"
#include "schema/pgCheck.h"
#include "dlg/dlgCheck.h"

#define txtWhere        CTRL_TEXT("txtWhere")


BEGIN_EVENT_TABLE(dlgCheck, dlgProperty)
    EVT_TEXT(XRCID("txtWhere"),                 dlgProperty::OnChange)
END_EVENT_TABLE();


dlgProperty *pgCheckFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgCheck(this, frame, (pgCheck*)node, (pgTable*)parent);
}


dlgCheck::dlgCheck(pgaFactory *f, frmMain *frame, pgCheck *node, pgTable *parentNode)
: dlgProperty(f, frame, wxT("dlgCheck"))
{
    check=node;
    table=parentNode;
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

    pgObject *obj=checkFactory.CreateObjects(collection, 0, wxT(
        "\n   AND conname=") + qtDbString(name) + wxT(
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
    }
    else
    {
        // create mode
        txtComment->Disable();
        if (!table)
        {
            cbClusterSet->Disable();
            cbClusterSet = 0;
        }
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
