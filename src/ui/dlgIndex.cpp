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

#define lstColumns      CTRL("lstColumns", wxListCtrl)
#define btnAddCol       CTRL("btnAddCol", wxButton)
#define btnRemoveCol    CTRL("btnRemoveCol", wxButton)



BEGIN_EVENT_TABLE(dlgIndexBase, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgIndexBase::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgIndexBase::OnChange)
    EVT_BUTTON(XRCID("btnAddCol"),                  dlgIndexBase::OnAddCol)
    EVT_BUTTON(XRCID("btnRemoveCol"),               dlgIndexBase::OnRemoveCol)
END_EVENT_TABLE();


dlgIndexBase::dlgIndexBase(frmMain *frame, const wxString &resName, pgIndex *node, pgTable *parentNode)
: dlgProperty(frame, resName)
{
    SetIcon(wxIcon(index_xpm));
    index=node;
    table=parentNode;
    wxASSERT(!table || table->GetType() == PG_TABLE);

    CreateListColumns(lstColumns, wxT("Columns"), wxT(""), 0);
}


pgObject *dlgIndexBase::GetObject()
{
    return index;
}


int dlgIndexBase::Go(bool modal)
{
    if (table)
    {
        long cookie;
        pgObject *data;
        wxTreeItemId columnsItem=mainForm->GetBrowser()->GetFirstChild(table->GetId(), cookie);
        while (columnsItem)
        {
            data=(pgObject*)mainForm->GetBrowser()->GetItemData(columnsItem);
            if (data->GetType() == PG_COLUMNS)
                break;
            item=mainForm->GetBrowser()->GetNextChild(table->GetId(), cookie);
        }

        if (columnsItem)
        {
            long cookie;
            pgColumn *column;
            wxTreeItemId item=mainForm->GetBrowser()->GetFirstChild(columnsItem, cookie);

            // check columns
            while (item)
            {
                column=(pgColumn*)mainForm->GetBrowser()->GetItemData(item);
                if (column->GetType() == PG_COLUMN)
                {
                    if (column->GetColNumber() > 0)
                    {
                        cbColumns->Append(column->GetName());
                    }
                }
        
                item=mainForm->GetBrowser()->GetNextChild(columnsItem, cookie);
            }
        }
    }
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

    return dlgProperty::Go(modal);
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
        btnOK->Enable(!index && !txtName->GetValue().IsEmpty() && lstColumns->GetItemCount() > 0);
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


dlgIndex::dlgIndex(frmMain *frame, pgIndex *index, pgTable *parentNode)
: dlgIndexBase(frame, wxT("dlgIndex"), index, parentNode)
{
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
        if (!index || index->GetComment() != cmt)
        {
            if (!cmt.IsEmpty())
                sql += wxT("COMMENT ON INDEX ") + qtIdent(txtName->GetValue())
                    + wxT("\n   IS ") + qtString(cmt)
                    + wxT(";\n");
        }
    }
    return sql;
}


pgObject *dlgIndex::CreateObject(pgCollection *collection)
{
    return 0;
}

