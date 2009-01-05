//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
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

BEGIN_EVENT_TABLE(dlgIndexConstraint, dlgIndexBase)
    EVT_BUTTON(XRCID("btnAddCol"),                  dlgIndexConstraint::OnAddCol)
    EVT_BUTTON(XRCID("btnRemoveCol"),               dlgIndexConstraint::OnRemoveCol)
#ifdef __WXMAC__
    EVT_SIZE(                                       dlgIndexConstraint::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgIndexConstraint::dlgIndexConstraint(pgaFactory *f, frmMain *frame, const wxString &resName, pgIndexBase *index, pgTable *parentNode)
: dlgIndexBase(f, frame, resName, index, parentNode)
{
    lstColumns->CreateColumns(0, _("Columns"), wxT(""), 0);
}


dlgIndexConstraint::dlgIndexConstraint(pgaFactory *f, frmMain *frame, const wxString &resName, ctlListView *colList)
: dlgIndexBase(f, frame, resName, colList)
{
    lstColumns->CreateColumns(0, _("Columns"), wxT(""), 0);
}


wxString dlgIndexConstraint::GetColumns()
{
    wxString sql;

    int pos;
    // iterate cols
    for (pos=0 ; pos < lstColumns->GetItemCount() ; pos++)
    {
        if (pos)
            sql += wxT(", ");

        sql += qtIdent(lstColumns->GetItemText(pos));
    }
    return sql;
}


int dlgIndexConstraint::Go(bool modal)
{
    PrepareTablespace(cbTablespace);

    if (index)
    {
        pgIndexConstraint *idc=(pgIndexConstraint*)index;

        wxArrayString colsArr = index->GetColumnList();
        for (int colIdx=0,colsCount=colsArr.Count(); colIdx<colsCount; colIdx++)
            lstColumns->InsertItem(colIdx, colsArr.Item(colIdx), columnFactory.GetIconId());

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

    int returnCode = dlgIndexBase::Go(modal);

    #ifdef __WXMAC__
    wxSizeEvent event(wxSize(GetSize().GetWidth() - 25, GetSize().GetHeight() + 200));
    OnChangeSize(event);
    #endif

    return returnCode;
}


void dlgIndexConstraint::OnAddCol(wxCommandEvent &ev)
{
    wxString col=cbColumns->GetValue();
    if (!col.IsEmpty())
    {
        lstColumns->InsertItem(lstColumns->GetItemCount(), col, columnFactory.GetIconId());
        cbColumns->Delete(cbColumns->GetCurrentSelection());
        if (cbColumns->GetCount())
            cbColumns->SetSelection(0);

        CheckChange();
        if (!cbColumns->GetCount())
            btnAddCol->Disable();
    }
}


void dlgIndexConstraint::OnRemoveCol(wxCommandEvent &ev)
{
    long pos=lstColumns->GetSelection();
    if (pos >= 0)
    {
        wxString col=lstColumns->GetItemText(pos);
        lstColumns->DeleteItem(pos);
        cbColumns->Append(col);

        CheckChange();
        btnRemoveCol->Disable();
    }
}

#ifdef __WXMAC__
void dlgIndexConstraint::OnChangeSize(wxSizeEvent &ev)
{
    lstColumns->SetSize(wxDefaultCoord, wxDefaultCoord,
        ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
    if (GetAutoLayout())
    {
        Layout();
    }
}
#endif


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
