//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
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
#include "frm/frmMain.h"
#include "dlg/dlgIndexConstraint.h"
#include "schema/pgIndex.h"
#include "schema/pgColumn.h"
#include "schema/pgTable.h"
#include "schema/pgIndexConstraint.h"



#define cbTablespace    CTRL_COMBOBOX("cbTablespace")


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
    PrepareTablespace(cbTablespace);

    if (index)
    {
        pgIndexConstraint *idc=(pgIndexConstraint*)index;

        if (idc->GetTablespaceOid() != 0)
            cbTablespace->SetKey(idc->GetTablespaceOid());
        cbTablespace->Enable(connection->BackendMinimumVersion(8, 0));
    }
    else
    {
        txtComment->Disable();
        if (!table)
        {
            cbClusterSet->Disable();
            cbClusterSet = 0;
        }

        // Add the default tablespace 
        cbTablespace->Insert(_("<default tablespace>"), 0, (void *)0);
        cbTablespace->SetSelection(0);
    }

    return dlgIndexBase::Go(modal);
}


wxString dlgIndexConstraint::GetDefinition()
{
    wxString sql;

    sql = wxT("(") + GetColumns() + wxT(")");

    if (cbTablespace->GetOIDKey() > 0)
        sql += wxT(" USING INDEX TABLESPACE ") + qtIdent(cbTablespace->GetValue());

    return sql;
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
    else
    {
        if (connection->BackendMinimumVersion(8, 0) && cbTablespace->GetOIDKey() != index->GetTablespaceOid())
        {
            sql += wxT("ALTER INDEX ") + index->GetSchema()->GetQuotedIdentifier() + wxT(".") + qtIdent(name) 
                +  wxT(" SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
                + wxT(";\n");
        }
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
