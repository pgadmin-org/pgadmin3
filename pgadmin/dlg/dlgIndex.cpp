//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgIndex.cpp - PostgreSQL Index Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "dlg/dlgIndex.h"
#include "schema/pgIndex.h"
#include "schema/pgColumn.h"
#include "schema/pgTable.h"


#define cbTablespace    CTRL_COMBOBOX("cbTablespace")
#define cbType          CTRL_COMBOBOX("cbType")
#define chkUnique       CTRL_CHECKBOX("chkUnique")
#define chkClustered    CTRL_CHECKBOX("chkClustered")
#define chkConcurrent   CTRL_CHECKBOX("chkConcurrent")
#define txtWhere        CTRL_TEXT("txtWhere")
#define txtFillFactor   CTRL_TEXT("txtFillFactor")

#define btnAddCol       CTRL_BUTTON("btnAddCol")
#define btnRemoveCol    CTRL_BUTTON("btnRemoveCol")



BEGIN_EVENT_TABLE(dlgIndexBase, dlgCollistProperty)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtFillFactor"),                dlgProperty::OnChange)
    EVT_BUTTON(XRCID("btnAddCol"),                  dlgIndexBase::OnAddCol)
    EVT_BUTTON(XRCID("btnRemoveCol"),               dlgIndexBase::OnRemoveCol)
    EVT_LIST_ITEM_SELECTED(XRCID("lstColumns"),     dlgIndexBase::OnSelectListCol)
    EVT_COMBOBOX(XRCID("cbColumns"),                dlgIndexBase::OnSelectComboCol)
END_EVENT_TABLE();


dlgProperty *pgIndexFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgIndex(this, frame, (pgIndex*)node, (pgTable*)parent);
}


dlgIndexBase::dlgIndexBase(pgaFactory *f, frmMain *frame, const wxString &resName, pgIndexBase *node, pgTable *parentNode)
: dlgCollistProperty(f, frame, resName, parentNode)
{
    index=node;
    wxASSERT(!table || table->GetMetaType() == PGM_TABLE);

    lstColumns->CreateColumns(0, _("Columns"), wxT(""), 0);
}


dlgIndexBase::dlgIndexBase(pgaFactory *f, frmMain *frame, const wxString &resName, ctlListView *colList)
: dlgCollistProperty(f, frame, resName, colList)
{
    index=0;
    
    lstColumns->CreateColumns(0, _("Columns"), wxT(""), 0);
}


pgObject *dlgIndexBase::GetObject()
{
    return index;
}


int dlgIndexBase::Go(bool modal)
{
    if (index)
    {
        // edit mode: view only
        txtName->Disable();
        cbColumns->Disable();

	    int pos = 0;
        wxStringTokenizer cols(index->GetColumns(), wxT(","));
        while (cols.HasMoreTokens())
        {
            wxString str=cols.GetNextToken();
            if (str.Strip() == wxT(""))
                str.RemoveLast();       // there's a space
            lstColumns->InsertItem(pos++, str, columnFactory.GetIconId());
        }

        if (txtFillFactor)
        {
            txtFillFactor->SetValue(index->GetFillFactor());
        }
    }
    else
    {
        // create mode
    }

    if (txtFillFactor)
    {
        txtFillFactor->SetValidator(numericValidator);
        if (connection->BackendMinimumVersion(8, 2))
            txtFillFactor->Enable();
        else
            txtFillFactor->Disable();
    }

    btnAddCol->Disable();
    btnRemoveCol->Disable();

    return dlgCollistProperty::Go(modal);
}


void dlgIndexBase::OnAddCol(wxCommandEvent &ev)
{
    wxString col=cbColumns->GetValue();
    if (!col.IsEmpty())
    {
        lstColumns->InsertItem(lstColumns->GetItemCount(), col, columnFactory.GetIconId());
        cbColumns->Delete(cbColumns->GetCurrentSelection());
        if (cbColumns->GetCount())
            cbColumns->SetSelection(0);

        CheckChange();
        if (cbColumns->GetValue().IsEmpty())
            btnAddCol->Disable();
    }
}


void dlgIndexBase::OnRemoveCol(wxCommandEvent &ev)
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

void dlgIndexBase::OnSelectListCol(wxListEvent &ev)
{
    OnSelectCol();
}

void dlgIndexBase::OnSelectComboCol(wxCommandEvent &ev)
{
    OnSelectCol();
}

void dlgIndexBase::OnSelectCol()
{
    if (lstColumns->GetSelection() != wxNOT_FOUND)
        btnRemoveCol->Enable(true);
    else
        btnRemoveCol->Enable(false);

    if (cbColumns->GetSelection() != wxNOT_FOUND && !cbColumns->GetValue().IsEmpty())
        btnAddCol->Enable(true);
    else
        btnAddCol->Enable(false);
}

void dlgIndexBase::CheckChange()
{
    if (index)
    {
        EnableOK(txtComment->GetValue() != index->GetComment() ||
                 cbTablespace->GetOIDKey() != index->GetTablespaceOid());
    }
    else
    {
        bool enable=true;
        txtComment->Enable(!GetName().IsEmpty());
        CheckValid(enable, lstColumns->GetItemCount() > 0, _("Please specify columns."));
        EnableOK(enable);
    }
}


wxString dlgIndexBase::GetColumns()
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


BEGIN_EVENT_TABLE(dlgIndex, dlgIndexBase)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkClustered"),             dlgProperty::OnChange)
END_EVENT_TABLE();

        
dlgIndex::dlgIndex(pgaFactory *f, frmMain *frame, pgIndex *index, pgTable *parentNode)
: dlgIndexBase(f, frame, wxT("dlgIndex"), index, parentNode)
{
}


void dlgIndex::CheckChange()
{
    bool fill=false;

    if (index)
    {
        if (txtFillFactor)
        {
            fill = txtFillFactor->GetValue() != index->GetFillFactor();
        }

        EnableOK(fill || 
                 txtComment->GetValue() != index->GetComment() || 
                 chkClustered->GetValue() != index->GetIsClustered() ||
                 cbTablespace->GetOIDKey() != index->GetTablespaceOid());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, lstColumns->GetItemCount() > 0, _("Please specify columns."));
        EnableOK(enable);
    }
}


int dlgIndex::Go(bool modal)
{
    if (!connection->BackendMinimumVersion(7, 4))
        chkClustered->Disable();

    if (index)
    {
        // edit mode: view only
        
        cbType->Append(index->GetIndexType());
        chkUnique->SetValue(index->GetIsUnique());
        chkClustered->SetValue(index->GetIsClustered());
        txtWhere->SetValue(index->GetConstraint());
        cbType->SetSelection(0);
        cbType->Disable();
        txtWhere->Disable();
        chkUnique->Disable();
        chkConcurrent->Disable();
        PrepareTablespace(cbTablespace, index->GetTablespaceOid());
    }
    else
    {
        // create mode
        PrepareTablespace(cbTablespace);
        cbType->Append(wxT(""));
        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT amname FROM pg_am"));
        if (set)
        {
            while (!set->Eof())
            {
                cbType->Append(set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }

        if (!this->database->BackendMinimumVersion(8, 2))
            chkConcurrent->Disable();
    }
    return dlgIndexBase::Go(modal);
}
       

wxString dlgIndex::GetSql()
{
    wxString sql;

    if (table)
    {
        wxString name=GetName();
        if (!index)
        {
            sql = wxT("CREATE ");
            if (chkUnique->GetValue())
                sql += wxT("UNIQUE ");

            sql += wxT("INDEX ");

            if (chkConcurrent->GetValue())
                sql += wxT("CONCURRENTLY ");

            sql += qtIdent(name);

            sql += wxT("\n   ON ") + table->GetQuotedFullIdentifier();
            AppendIfFilled(sql, wxT(" USING "), cbType->GetValue());
            sql += wxT(" (") + GetColumns()
                + wxT(")");

            AppendIfFilled(sql, wxT("\n       TABLESPACE "), qtIdent(cbTablespace->GetValue()));

            if (txtFillFactor)
            {
                if (connection->BackendMinimumVersion(8, 2) && txtFillFactor->GetValue().Length() > 0)
                    sql += wxT("\n  WITH (FILLFACTOR=") + txtFillFactor->GetValue() + wxT(")");
            }

            AppendIfFilled(sql, wxT(" WHERE "), txtWhere->GetValue());
            sql +=  wxT(";\n");
        }
        else
        {
            if (cbTablespace->GetOIDKey() != index->GetTablespaceOid())
                sql += wxT("ALTER INDEX ") + qtIdent(index->GetSchema()->GetName()) + wxT(".") + qtIdent(name) 
                    +  wxT(" SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
                    + wxT(";\n");
        }
        if (connection->BackendMinimumVersion(7, 4))
        {
            if (index && index->GetIsClustered() && !chkClustered->GetValue())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" SET WITHOUT CLUSTER;\n");
            else if (chkClustered->GetValue() && (!index || !index->GetIsClustered()))
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" CLUSTER ON ") + qtIdent(name) + wxT(";\n");
        }
        if (index && connection->BackendMinimumVersion(8, 2) && txtFillFactor->GetValue().Length() > 0)
            sql += wxT("ALTER INDEX ") + qtIdent(name)
                +  wxT(" SET (FILLFACTOR=") + txtFillFactor->GetValue() + wxT(");\n")
                +  wxT("REINDEX INDEX ") + qtIdent(name) + wxT(";\n");
        AppendComment(sql, wxT("INDEX"), table->GetSchema(), index);
    }
    return sql;
}


pgObject *dlgIndex::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=indexFactory.CreateObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtDbString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}

