//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "pgIndexConstraint.h"



#define chkDeferrable   CTRL("chkDeferrable",   wxCheckBox)
#define chkDeferred     CTRL("chkDeferred",     wxCheckBox)
#define stDeferred      CTRL("stDeferred",      wxStaticText)



BEGIN_EVENT_TABLE(dlgIndexConstraint, dlgIndexBase)
    EVT_CHECKBOX(XRCID("chkDeferrable"),        dlgIndexConstraint::OnCheckDeferrable)
END_EVENT_TABLE();



dlgIndexConstraint::dlgIndexConstraint(frmMain *frame, const wxString &resName, pgIndex *index, pgTable *parentNode)
: dlgIndexBase(frame, resName, index, parentNode)
{
}


dlgIndexConstraint::dlgIndexConstraint(frmMain *frame, const wxString &resName, wxListCtrl *colList)
: dlgIndexBase(frame, resName, colList)
{
}


int dlgIndexConstraint::Go(bool modal)
{
    wxNotifyEvent event;
    OnCheckDeferrable(event);

    if (index)
    {
        pgIndexConstraint *idc=(pgIndexConstraint*)index;

        chkDeferrable->SetValue(idc->GetDeferrable());
        chkDeferred->SetValue(idc->GetDeferred());
        chkDeferrable->Disable();
        chkDeferred->Disable();
    }
    else
    {
        txtComment->Disable();
    }

    return dlgIndexBase::Go(modal);
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


wxString dlgIndexConstraint::GetSql()
{
    wxString sql;
    wxString name=GetName();

    if (!index)
    {
        sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
            + wxT(" ADD");
        AppendIfFilled(sql, wxT(" CONSTRAINT "), qtIdent(name));
        sql +=wxT(" PRIMARY KEY ") + GetDefinition()
            + wxT(";\n");
    }
    else
    {
        AppendComment(sql, wxT("CONSTRAINT ") + table->GetSchema()->GetQuotedIdentifier()
                +  wxT(".") + qtIdent(name), index);
    }
    return sql;
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


pgObject *dlgPrimaryKey::CreateObject(pgCollection *collection)
{
    wxString name=GetName();
    if (name.IsEmpty())
        return 0;

    pgObject *obj=pgPrimaryKey::ReadObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());

    return obj;
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


pgObject *dlgUnique::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=pgUnique::ReadObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}
