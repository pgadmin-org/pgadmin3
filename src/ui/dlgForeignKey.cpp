//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgForeignKey.cpp - PostgreSQL Foreign Key Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "pgTable.h"
#include "pgForeignKey.h"
#include "dlgForeignKey.h"


#define chkDeferrable   CTRL("chkDeferrable",   wxCheckBox)
#define chkDeferred     CTRL("chkDeferred",     wxCheckBox)
#define stDeferred      CTRL("stDeferred",      wxStaticText)
#define cbReferences    CTRL("cbReferences",    wxComboBox)

#define cbRefColumns    CTRL("cbRefColumns",    wxComboBox)
#define btnAddRef       CTRL("btnAddRef",       wxButton)
#define btnRemoveRef    CTRL("btnRemoveRef",    wxButton)

#define rbOnUpdate      CTRL("rbOnUpdate",      wxRadioBox)
#define rbOnDelete      CTRL("rbOnDelete",      wxRadioBox)


BEGIN_EVENT_TABLE(dlgForeignKey, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                  dlgForeignKey::OnChange)
    EVT_TEXT(XRCID("txtComment"),               dlgForeignKey::OnChange)
    EVT_CHECKBOX(XRCID("chkDeferrable"),        dlgForeignKey::OnCheckDeferrable)
    EVT_LIST_ITEM_SELECTED(XRCID("lstColumns"), dlgForeignKey::OnSelChangeCol)
    EVT_COMBOBOX(XRCID("cbReferences"),         dlgForeignKey::OnSelChangeRef)
    EVT_COMBOBOX(XRCID("cbColumns"),            dlgForeignKey::OnSelChangeRefCol)
    EVT_COMBOBOX(XRCID("cbRefColumns"),         dlgForeignKey::OnSelChangeRefCol)
    EVT_BUTTON(XRCID("btnAddRef"),              dlgForeignKey::OnAddRef)
    EVT_BUTTON(XRCID("btnRemoveRef"),           dlgForeignKey::OnRemoveRef)
END_EVENT_TABLE();



dlgForeignKey::dlgForeignKey(frmMain *frame, pgForeignKey *node, pgTable *parentNode)
: dlgCollistProperty(frame, wxT("dlgForeignKey"), parentNode)
{
    foreignKey=node;
}


dlgForeignKey::dlgForeignKey(frmMain *frame, wxListCtrl *colList)
: dlgCollistProperty(frame, wxT("dlgForeignKey"), colList)
{
    foreignKey=0;
    objectType=PG_FOREIGNKEY;
}

void dlgForeignKey::OnCheckDeferrable(wxNotifyEvent &ev)
{
    bool canDef=chkDeferrable->GetValue();
    stDeferred->Enable(canDef);
    if (!canDef)
        chkDeferred->SetValue(false);
    chkDeferred->Enable(canDef);
}


void dlgForeignKey::OnChange(wxNotifyEvent &ev)
{
    if (foreignKey)
    {
        btnOK->Enable(txtComment->GetValue() != foreignKey->GetComment());
        txtComment->Enable(!txtName->GetValue().IsEmpty());
    }
    else
        btnOK->Enable(lstColumns->GetItemCount() > 0);
}


void dlgForeignKey::OnSelChangeCol(wxNotifyEvent &ev)
{
    btnRemoveRef->Enable();
}


void dlgForeignKey::OnSelChangeRefCol(wxNotifyEvent &ev)
{
    btnAddRef->Enable(cbColumns->GetSelection() >= 0 && cbRefColumns->GetSelection() >= 0);
}


void dlgForeignKey::OnSelChangeRef(wxNotifyEvent &ev)
{
    cbRefColumns->Clear();

    wxString tab=cbReferences->GetValue();
    wxString nsp;
    if (tab.Find('.') >= 0)
    {
        nsp=tab.BeforeFirst('.');
        tab=tab.Mid(nsp.Length()+1);
    }
    else
        nsp=wxT("public");

    pgSet *set=connection->ExecuteSet(wxT(
        "SELECT attname\n"
        "  FROM pg_attribute att, pg_class cl, pg_namespace nsp\n"
        " WHERE attrelid=cl.oid AND relnamespace=nsp.oid\n"
        "   AND nspname=") + qtString(nsp)
        + wxT("\n   AND relname=") + qtString(tab)
        + wxT("\n   AND attnum > 0\n"
              "\n ORDER BY attnum"));
    if (set)
    {
        while (!set->Eof())
        {
            cbRefColumns->Append(set->GetVal(0));
            set->MoveNext();
        }
        delete set;
    }
}


void dlgForeignKey::OnAddRef(wxNotifyEvent &ev)
{
    wxString col=cbColumns->GetValue();
    wxString ref=cbRefColumns->GetValue();
    if (!col.IsEmpty() && !ref.IsEmpty())
    {
        AppendListItem(lstColumns, col, ref, PGICON_COLUMN);
        cbColumns->Delete(cbColumns->GetSelection());
        cbRefColumns->Delete(cbRefColumns->GetSelection());
        cbReferences->Disable();
        btnAddRef->Disable();

        wxNotifyEvent event;
        OnChange(event);
    }
}


void dlgForeignKey::OnRemoveRef(wxNotifyEvent &ev)
{
    long pos=GetListSelected(lstColumns);

    if (pos >= 0)
    {
        wxString col=lstColumns->GetItemText(pos);
        wxString ref=GetListText(lstColumns, pos, 1);
        cbColumns->Append(col);
        cbRefColumns->Append(ref);

        lstColumns->DeleteItem(pos);
        cbReferences->Enable(lstColumns->GetItemCount() == 0);
        btnRemoveRef->Disable();
    }
}


pgObject *dlgForeignKey::GetObject()
{
    return foreignKey;
}


pgObject *dlgForeignKey::CreateObject(pgCollection *collection)
{
    wxString name=txtName->GetValue();
    if (name.IsEmpty())
        return 0;

    pgObject *obj=pgForeignKey::ReadObjects(collection, 0, wxT(
        "\n   AND conname=") + qtString(name) + wxT(
        "\n   AND cl.relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}


int dlgForeignKey::Go(bool modal)
{
    CreateListColumns(lstColumns, wxT("Local"), wxT("Referenced"), -1);

    btnAddRef->Disable();
    btnRemoveRef->Disable();


    if (foreignKey)
    {
        // edit mode: view only
        txtName->SetValue(foreignKey->GetName());
        txtName->Disable();
        cbReferences->Append(foreignKey->GetReferences());
        cbReferences->SetValue(foreignKey->GetReferences());
        cbReferences->Disable();

        chkDeferrable->SetValue(foreignKey->GetDeferrable());
        chkDeferred->SetValue(foreignKey->GetDeferred());
        chkDeferrable->Disable();
        chkDeferred->Disable();

        rbOnUpdate->SetStringSelection(foreignKey->GetOnUpdate());
        rbOnDelete->SetStringSelection(foreignKey->GetOnDelete());
        rbOnUpdate->Disable();
        rbOnDelete->Disable();

        btnAddRef->Disable();
        btnRemoveRef->Disable();
        cbColumns->Disable();
        cbRefColumns->Disable();

        btnOK->Disable();

        int pos=0;
        wxStringTokenizer cols(foreignKey->GetFkColumns(), ',');
        wxStringTokenizer refs(foreignKey->GetRefColumns(), ',');
        while (cols.HasMoreTokens())
        {
            wxString col=cols.GetNextToken();
            wxString ref=refs.GetNextToken();
            if (pos++)
            {
                col.RemoveLast();       // there's a space
                ref.RemoveLast();
            }
            AppendListItem(lstColumns, col, ref, PGICON_COLUMN);
        }
    }
    else
    {
        // create mode
        btnOK->Disable();
        txtComment->Disable();

        wxString systemRestriction;
        if (!settings->GetShowSystemObjects())
            systemRestriction = wxT("   AND nsp.oid >= 100\n");

        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT nspname, relname FROM pg_namespace nsp, pg_class cl\n"
            " WHERE relnamespace=nsp.oid AND relkind='r'\n"
            "   AND nsp.nspname NOT LIKE 'pg\\_temp\\_%'\n") 
            + systemRestriction
            + wxT("\n ORDER BY nsp.oid, relname"));

        if (set)
        {
            while (!set->Eof())
            {
                wxString nsp=set->GetVal(0);
                if (nsp == wxT("public"))
                    nsp=wxT("");
                else
                    nsp += wxT(".");
                cbReferences->Append(nsp + set->GetVal(1));
                set->MoveNext();
            }
            delete set;
        }
    }

    return dlgCollistProperty::Go(modal);
}


wxString dlgForeignKey::GetSql()
{
    wxString sql;
    wxString name=txtName->GetValue();

    if (!foreignKey)
    {
        sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
            + wxT(" ADD");
        AppendIfFilled(sql, wxT(" CONSTRAINT "), qtIdent(name));
        sql +=wxT(" FOREIGN KEY ") + GetDefinition()
            + wxT(";\n");
    }
    else
    {
        wxString cmt=txtComment->GetValue();
        if (foreignKey->GetComment() != cmt)
            sql += wxT("COMMENT ON CONSTRAINT ") + table->GetSchema()->GetQuotedIdentifier()
                +  wxT(".") + qtIdent(name)
                +  wxT(" IS ") + qtString(cmt)
                + wxT(";\n");
    }
    return sql;
}


wxString dlgForeignKey::GetDefinition()
{
    wxString sql;
    wxString cols, refs;

    int pos;

    for (pos=0 ; pos < lstColumns->GetItemCount() ; pos++)
    {
        if (pos)
        {
            cols += wxT(", ");
            refs += wxT(", ");
        }
        cols += lstColumns->GetItemText(pos);
        refs += GetListText(lstColumns, pos, 1);
    }

    wxString tab=cbReferences->GetValue();
    wxString nsp;
    if (tab.Find('.') >= 0)
    {
        nsp = tab.BeforeFirst('.');
        tab = tab.Mid(nsp.Length()+1);
        nsp = qtIdent(nsp) + wxT(".");
    }

    sql = wxT("(") + cols 
        + wxT(") REFERENCES ") + nsp + qtIdent(tab)
        + wxT(" (") + refs
        + wxT(")"
          "\n   ON UPDATE ") + rbOnUpdate->GetStringSelection()
        + wxT(" ON DELETE ") + rbOnDelete->GetStringSelection();

    return sql;
}
