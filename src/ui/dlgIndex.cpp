//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
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


#define cbType          CTRL_COMBOBOX("cbType")
#define chkUnique       CTRL_CHECKBOX("chkUnique")
#define txtWhere        CTRL_TEXT("txtWhere")

#define btnAddCol       CTRL_BUTTON("btnAddCol")
#define btnRemoveCol    CTRL_BUTTON("btnRemoveCol")



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

    CreateListColumns(lstColumns, _("Columns"), wxT(""), 0);
}


dlgIndexBase::dlgIndexBase(frmMain *frame, const wxString &resName, ctlListView *colList)
: dlgCollistProperty(frame, resName, colList)
{
    SetIcon(wxIcon(index_xpm));
    index=0;
    
    CreateListColumns(lstColumns, _("Columns"), wxT(""), 0);
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
		txtComment->SetValue(index->GetComment());

	    int pos = 0;
        wxStringTokenizer cols(index->GetColumns(), wxT(","));
        while (cols.HasMoreTokens())
        {
            wxString str=cols.GetNextToken();
            if (str.Strip() == wxT(""))
                str.RemoveLast();       // there's a space
            lstColumns->InsertItem(pos++, str, PGICON_COLUMN);
        }
    }
    else
    {
        // create mode
    }

    return dlgCollistProperty::Go(modal);
}


void dlgIndexBase::OnAddCol(wxCommandEvent &ev)
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


void dlgIndexBase::OnRemoveCol(wxCommandEvent &ev)
{
    long pos=lstColumns->GetSelection();
    if (pos >= 0)
    {
        wxString col=lstColumns->GetItemText(pos);
        lstColumns->DeleteItem(pos);
        cbColumns->Append(col);

        wxNotifyEvent event;
        OnChange(event);
    }
}


void dlgIndexBase::OnChange(wxCommandEvent &ev)
{
    if (index)
    {
        btnOK->Enable(txtComment->GetValue() != index->GetComment());
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
    EVT_TEXT(XRCID("txtName"),                      dlgIndex::OnChange)
END_EVENT_TABLE();

        
dlgIndex::dlgIndex(frmMain *frame, pgIndex *index, pgTable *parentNode)
: dlgIndexBase(frame, wxT("dlgIndex"), index, parentNode)
{
}


void dlgIndex::OnChange(wxCommandEvent &ev)
{
    if (index)
    {
        btnOK->Enable(txtComment->GetValue() != index->GetComment());
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
            sql += wxT("INDEX ") + qtIdent(GetName());
            sql += wxT("\n   ON ") + table->GetQuotedFullIdentifier();
            AppendIfFilled(sql, wxT(" USING "), cbType->GetValue());
            sql += wxT(" (") + GetColumns()
                + wxT(")");


            AppendIfFilled(sql, wxT(" WHERE "), txtWhere->GetValue());
            sql +=  wxT(";\n");
        }
        AppendComment(sql, wxT("INDEX"), table->GetSchema(), index);
    }
    return sql;
}


pgObject *dlgIndex::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=pgIndex::ReadObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}

