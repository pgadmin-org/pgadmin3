//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgIndexConstraint.cpp - PostgreSQL IndexConstraint Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "dlgIndexConstraint.h"
#include "pgIndex.h"
#include "pgColumn.h"
#include "pgTable.h"


#define chkDeferrable   CTRL("chkDeferrable",   wxCheckBox)
#define chkDeferred     CTRL("chkDeferred",     wxCheckBox)
#define stDeferred      CTRL("stDeferred",      wxStaticText)



BEGIN_EVENT_TABLE(dlgIndexConstraint, dlgIndexBase)
    EVT_CHECKBOX(XRCID("chkDeferrable"),        dlgIndexConstraint::OnCheckDeferrable)
END_EVENT_TABLE();



dlgIndexConstraint::dlgIndexConstraint(frmMain *frame, const wxString &resName, pgIndex *index, pgTable *parentNode)
: dlgIndexBase(frame, resName, index, parentNode)
{
    stDeferred->Disable();
    chkDeferred->Disable();
    columns=0;
}


dlgIndexConstraint::dlgIndexConstraint(frmMain *frame, const wxString &resName, wxListCtrl *colList)
: dlgIndexBase(frame, resName, 0, 0)
{
    stDeferred->Disable();
    chkDeferred->Disable();
    columns=colList;
}


int dlgIndexConstraint::Go(bool modal)
{
    if (columns)
    {
        int pos;
        // iterate cols
        for (pos=0 ; pos < columns->GetItemCount() ; pos++)
            cbColumns->Append(columns->GetItemText(pos));
    }
    return dlgIndexBase::Go(modal);
}


wxString dlgIndexConstraint::GetName()
{
    return txtName->GetValue();
}


wxString dlgIndexConstraint::GetDefinition()
{
    wxString sql;

    sql = wxT("(") + GetColumns() + wxT(")");

    return sql;
}

void dlgIndexConstraint::OnCheckDeferrable(wxNotifyEvent &ev)
{
    bool canDef=chkDeferrable->GetValue();
    stDeferred->Enable(canDef);
    if (!canDef)
        chkDeferred->SetValue(false);
    chkDeferred->Enable(canDef);
}


dlgPrimaryKey::dlgPrimaryKey(frmMain *frame, pgIndex *index, pgTable *parentNode)
: dlgIndexConstraint(frame, wxT("dlgIndexConstraint"), index, parentNode)
{
}


dlgPrimaryKey::dlgPrimaryKey(frmMain *frame, wxListCtrl *colList)
: dlgIndexConstraint(frame, wxT("dlgIndexConstraint"), colList)
{
    objectType=PG_PRIMARYKEY;
}


wxString dlgPrimaryKey::GetSql()
{
    wxString sql;

    sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
        + wxT(" ADD CONSTRAINT ") + qtIdent(txtName->GetValue())
        + wxT(" PRIMARY KEY ") + GetDefinition()
        + wxT(";\n");

    return sql;
}


pgObject *dlgPrimaryKey::CreateObject(pgCollection *collection)
{
    return 0;
}


dlgUnique::dlgUnique(frmMain *frame, pgIndex *index, pgTable *parentNode)
: dlgIndexConstraint(frame, wxT("dlgIndexConstraint"), index, parentNode)
{
}


dlgUnique::dlgUnique(frmMain *frame, wxListCtrl *colList)
: dlgIndexConstraint(frame, wxT("dlgIndexConstraint"), colList)
{
    objectType = PG_UNIQUE;
}


wxString dlgUnique::GetSql()
{
    wxString sql;

    sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
        + wxT(" ADD CONSTRAINT ") + qtIdent(txtName->GetValue())
        + wxT(" UNIQUE ") + GetDefinition()
        + wxT(";\n");

    return sql;
}


pgObject *dlgUnique::CreateObject(pgCollection *collection)
{
    return 0;
}
