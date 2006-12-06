//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgIndexConstraint.cpp - PostgreSQL IndexConstraint Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frmMain.h"
#include "dlgIndexConstraint.h"
#include "pgIndex.h"
#include "pgColumn.h"
#include "pgTable.h"
#include "pgIndexConstraint.h"



#define cbTablespace    CTRL_COMBOBOX("cbTablespace")
#define chkDeferrable   CTRL_CHECKBOX("chkDeferrable")
#define chkDeferred     CTRL_CHECKBOX("chkDeferred")
#define stDeferred      CTRL_STATIC("stDeferred")



BEGIN_EVENT_TABLE(dlgIndexConstraint, dlgIndexBase)
    EVT_CHECKBOX(XRCID("chkDeferrable"),        dlgIndexConstraint::OnCheckDeferrable)
END_EVENT_TABLE();


dlgIndexConstraint::dlgIndexConstraint(pgaFactory *f, frmMain *frame, const wxString &resName, pgIndexBase *index, pgTable *parentNode)
: dlgIndexBase(f, frame, resName, index, parentNode)
{
}


dlgIndexConstraint::dlgIndexConstraint(pgaFactory *f, frmMain *frame, const wxString &resName, ctlListView *colList)
: dlgIndexBase(f, frame, resName, colList)
{
}


int dlgIndexConstraint::Go(bool modal)
{
    wxNotifyEvent event;
    PrepareTablespace(cbTablespace);

    if (index)
    {
        pgIndexConstraint *idc=(pgIndexConstraint*)index;

        chkDeferrable->SetValue(idc->GetDeferrable());
        chkDeferred->SetValue(idc->GetDeferred());
        chkDeferrable->Disable();
        chkDeferred->Disable();
        if (!idc->GetTablespace().IsEmpty())
            cbTablespace->SetValue(idc->GetTablespace());
        cbTablespace->Enable(connection->BackendMinimumVersion(7, 5));
    }
    else
    {
        txtComment->Disable();
        if (!table)
        {
            cbClusterSet->Disable();
            cbClusterSet = 0;
        }
    }

    OnCheckDeferrable(event);

    return dlgIndexBase::Go(modal);
}


wxString dlgIndexConstraint::GetDefinition()
{
    wxString sql;

    sql = wxT("(") + GetColumns() + wxT(")");
    AppendIfFilled(sql, wxT(" USING INDEX TABLESPACE "),qtIdent(cbTablespace->GetValue()));
    if (chkDeferrable->GetValue())
    {
        sql += wxT(" DEFERRABLE INITIALLY ");
        if (chkDeferred->Enable())
            sql += wxT("DEFERRED");
        else
            sql += wxT("IMMEDIATE");
    }
    return sql;
}


void dlgIndexConstraint::OnCheckDeferrable(wxCommandEvent &ev)
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

        sql +=wxT(" ") + wxString(factory->GetTypeName()).Upper() + wxT(" ") + GetDefinition()
            + wxT(";\n");
    }

    if (!name.IsEmpty())
        AppendComment(sql, wxT("CONSTRAINT ") + qtIdent(name) 
            + wxT(" ON ") + table->GetQuotedFullIdentifier(), index);

    return sql;
}



dlgProperty *pgPrimaryKeyFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgPrimaryKey(this, frame, (pgPrimaryKey*)node, (pgTable*)parent);
}




dlgPrimaryKey::dlgPrimaryKey(pgaFactory *f, frmMain *frame, pgPrimaryKey *index, pgTable *parentNode)
: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), index, parentNode)
{
}


dlgPrimaryKey::dlgPrimaryKey(pgaFactory *f, frmMain *frame, ctlListView *colList)
: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), colList)
{
}


pgObject *dlgPrimaryKey::CreateObject(pgCollection *collection)
{
    wxString name=GetName();
    if (name.IsEmpty())
        return 0;

    pgObject *obj=primaryKeyFactory.CreateObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtDbString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());

    return obj;
}



dlgProperty *pgUniqueFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgUnique(this, frame, (pgUnique*)node, (pgTable*)parent);
}


dlgUnique::dlgUnique(pgaFactory *f, frmMain *frame, pgUnique *index, pgTable *parentNode)
: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), index, parentNode)
{
}


dlgUnique::dlgUnique(pgaFactory *f, frmMain *frame, ctlListView *colList)
: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), colList)
{
}


pgObject *dlgUnique::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=uniqueFactory.CreateObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtDbString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}
