//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgIndex.cpp - PostgreSQL Index Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "dlgIndex.h"
#include "pgIndex.h"
#include "pgColumn.h"
#include "pgTable.h"

// Images
#include "images/index.xpm"


#define cbType          CTRL("cbType", wxComboBox)
#define chkUnique       CTRL("chkUnique", wxCheckBox)
#define txtWhere        CTRL("txtWhere", wxTextCtrl)

#define btnAddCol       CTRL("btnAddCol", wxButton)
#define btnRemoveCol    CTRL("btnRemoveCol", wxButton)



BEGIN_EVENT_TABLE(dlgIndexBase, dlgCollistProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgIndexBase::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgIndexBase::OnChange)
    EVT_BUTTON(XRCID("btnAddCol"),                  dlgIndexBase::OnAddCol)
    EVT_BUTTON(XRCID("btnRemoveCol"),               dlgIndexBase::OnRemoveCol)
END_EVENT_TABLE();


dlgIndexBase::dlgIndexBase(frmMain *frame, const wxString &resName, pgIndex *node, pgTable *parentNode)
: dlgCollistProperty(frame, resName, parentNode)
{
    SetIcon(wxIcon(index_xpm));
    index=node;
    wxASSERT(!table || table->GetType() == PG_TABLE);

    CreateListColumns(lstColumns, wxT("Columns"), wxT(""), 0);
}


dlgIndexBase::dlgIndexBase(frmMain *frame, const wxString &resName, wxListCtrl *colList)
: dlgCollistProperty(frame, resName, colList)
{
    SetIcon(wxIcon(index_xpm));
    index=0;
    
    CreateListColumns(lstColumns, wxT("Columns"), wxT(""), 0);
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
        txtName->SetValue(index->GetName());
        txtName->Disable();

        btnAddCol->Disable();
        btnRemoveCol->Disable();
        cbColumns->Disable();

        btnOK->Disable();

        int pos=0;
        wxStringTokenizer cols(index->GetColumns(), ',');
        while (cols.HasMoreTokens())
        {
            wxString str=cols.GetNextToken();
            if (pos)
                str.RemoveLast();       // there's a space
            lstColumns->InsertItem(pos++, str, PGICON_COLUMN);
        }
    }
    else
    {
        // create mode
        btnOK->Disable();
    }

    return dlgCollistProperty::Go(modal);
}


void dlgIndexBase::OnAddCol(wxNotifyEvent &ev)
{
    wxString col=cbColumns->GetValue();
    if (!col.IsEmpty())
    {
        lstColumns->InsertItem(lstColumns->GetItemCount(), col, PGICON_COLUMN);
        cbColumns->Delete(cbColumns->GetSelection());
        if (cbColumns->GetCount())
            cbColumns->SetSelection(0);

        wxNotifyEvent event;
        OnChange(event);
    }
}


void dlgIndexBase::OnRemoveCol(wxNotifyEvent &ev)
{
    long pos=lstColumns->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (pos >= 0)
    {
        wxString col=lstColumns->GetItemText(pos);
        lstColumns->DeleteItem(pos);
        cbColumns->Append(col);

        wxNotifyEvent event;
        OnChange(event);
    }
}


void dlgIndexBase::OnChange(wxNotifyEvent &ev)
{
    if (index)
    {
        btnOK->Enable(txtComment->GetValue() != index->GetComment());
    }
    else
        btnOK->Enable(!txtName->GetValue().IsEmpty() && lstColumns->GetItemCount() > 0);
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
    EVT_TEXT(XRCID("txtName"),                      dlgIndex::OnChange)
END_EVENT_TABLE();

        
dlgIndex::dlgIndex(frmMain *frame, pgIndex *index, pgTable *parentNode)
: dlgIndexBase(frame, wxT("dlgIndex"), index, parentNode)
{
}


void dlgIndex::OnChange(wxNotifyEvent &ev)
{
    if (index)
    {
        btnOK->Enable(txtComment->GetValue() != index->GetComment());
    }
    else
    {
        txtComment->Enable(!txtName->GetValue().IsEmpty());
        btnOK->Enable(lstColumns->GetItemCount() > 0);
    }
}


int dlgIndex::Go(bool modal)
{
    if (index)
    {
        // edit mode: view only
        
        cbType->Append(index->GetIndexType());
        chkUnique->SetValue(index->GetIsUnique());
        txtWhere->SetValue(index->GetConstraint());
        cbType->SetSelection(0);
        cbType->Disable();
        txtWhere->Disable();
        chkUnique->Disable();
    }
    else
    {
        // create mode
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
        txtComment->Disable();
    }
    return dlgIndexBase::Go(modal);
}
       

wxString dlgIndex::GetSql()
{
    wxString sql;

    if (table)
    {
        if (!index)
        {
            sql = wxT("CREATE ");
            if (chkUnique->GetValue())
                sql += wxT("UNIQUE ");
            sql += wxT("INDEX ") + qtIdent(txtName->GetValue());
            AppendIfFilled(sql, wxT(" USING "), cbType->GetValue());
            sql += wxT("\n   ON ") + table->GetFullIdentifier() 
                + wxT("(") + GetColumns()
                + wxT(")");


            AppendIfFilled(sql, wxT(" WHERE "), txtWhere->GetValue());
            sql +=  wxT(";\n");
        }
        wxString cmt=txtComment->GetValue();
        AppendComment(sql, wxT("INDEX"), index);
    }
    return sql;
}


pgObject *dlgIndex::CreateObject(pgCollection *collection)
{
    wxString name=txtName->GetValue();

    pgObject *obj=pgIndex::ReadObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}

