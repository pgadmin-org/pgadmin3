//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgType.cpp - PostgreSQL TYPE Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// Images
#include "images/type.xpm"

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgType.h"
#include "pgSchema.h"
#include "pgType.h"
#include "pgDatatype.h"


// pointer to controls
#define rdbType         CTRL("rdbType",             wxRadioBox)

#define cbInput         CTRL("cbInput",             wxComboBox)
#define cbOutput        CTRL("cbOutput",            wxComboBox)
#define chkVariable     CTRL("chkVariable",         wxCheckBox)
#define txtLength       CTRL("txtLength",           wxTextCtrl)
#define txtDefault      CTRL("txtDefault",          wxTextCtrl)
#define cbElement       CTRL("cbElement",           wxComboBox)
#define txtDelimiter    CTRL("txtDelimiter",        wxTextCtrl)
#define chkByValue      CTRL("chkByValue",          wxCheckBox)
#define cbAlignment     CTRL("cbAlignment",         wxComboBox)
#define cbStorage       CTRL("cbStorage",           wxComboBox)
#define lstMembers      CTRL("lstMembers",          wxListCtrl)
#define txtMembername   CTRL("txtMembername",       wxTextCtrl)
#define btnAdd          CTRL("btnAdd",              wxButton)
#define btnRemove       CTRL("btnRemove",           wxButton)

BEGIN_EVENT_TABLE(dlgType, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgType::OnChange)
    EVT_RADIOBOX(XRCID("rdbType"),                  dlgType::OnTypeChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgType::OnChange)

    EVT_COMBOBOX(XRCID("cbInput"),                  dlgType::OnChange)
    EVT_COMBOBOX(XRCID("cbOutput"),                 dlgType::OnChange)
    EVT_TEXT(XRCID("txtLength"),                    dlgType::OnChange)
    EVT_CHECKBOX(XRCID("chkVariable"),              dlgType::OnChange)
    
    EVT_BUTTON(XRCID("btnAdd"),                     dlgType::OnVarAdd)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgType::OnVarRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgType::OnVarSelChange)
    EVT_COMBOBOX(XRCID("cbDatatype"),               dlgType::OnSelChangeTyp)
    EVT_TEXT(XRCID("txtMembername"),                dlgType::OnChangeMember)
    EVT_TEXT(XRCID("txtLength"),                    dlgType::OnSelChangeTyp)
END_EVENT_TABLE();



dlgType::dlgType(frmMain *frame, pgType *node, pgSchema *sch)
: dlgTypeProperty(frame, wxT("dlgType"))
{
    type=node;
    schema=sch;
    SetIcon(wxIcon(type_xpm));
    CreateListColumns(lstMembers, _("Member"), _("Data type"), -1);


    wxWindow *defPage=cbInput->GetParent();
    externalWindows = defPage->GetChildren();
    compositeWindows =  lstMembers->GetParent()->GetChildren();

    int i=compositeWindows.GetCount();
    while (i--)
        compositeWindows.Item(i)->GetData()->Reparent(defPage);

    nbNotebook->DeletePage(2);

    wxNotifyEvent event;
    OnTypeChange(event);
    txtOID->Disable();
}


void dlgType::OnChangeMember(wxNotifyEvent &ev)
{
    btnAdd->Enable(!txtMembername->GetValue().Strip(wxString::both).IsEmpty() 
        && cbDatatype->GetSelection() >= 0);
}


void dlgType::OnTypeChange(wxNotifyEvent &ev)
{
    bool isComposite = !rdbType->GetSelection();
    int i;
    
    i=externalWindows.GetCount();
    while (i--)
        externalWindows.Item(i)->GetData()->Show(!isComposite);

    i=compositeWindows.GetCount();
    while (i--)
        compositeWindows.Item(i)->GetData()->Show(isComposite);

    OnChange(ev);
}


pgObject *dlgType::GetObject()
{
    return type;
}


int dlgType::Go(bool modal)
{

    if (type)
    {
        // Edit Mode
        txtName->SetValue(type->GetIdentifier());
        txtOID->SetValue(NumToStr((long)type->GetOid()));

        txtName->Disable();
        txtOID->Disable();
        rdbType->Disable();
        cbDatatype->Disable();
        txtMembername->Disable();
        txtLength->Disable();
        btnAdd->Disable();
        btnRemove->Disable();
    }
    else
    {
        // Create mode

        pgSet *set=connection->ExecuteSet(
            wxT("SELECT proname, nspname\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE proargtypes[0] <> 0 AND proargtypes[1] = 0"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString nsp=set->GetVal(wxT("nspname"));
                if (nsp == wxT("pg_catalog"))
                    nsp = wxT("");
                else
                    nsp += wxT(".");
                cbInput->Append(nsp+set->GetVal(wxT("proname")));
                cbOutput->Append(nsp+set->GetVal(wxT("proname")));
                set->MoveNext();
            }
            delete set;
        }
        FillDatatype(cbDatatype, cbElement);
        txtLength->SetValidator(numericValidator);
    }
    return dlgTypeProperty::Go(modal);
}


void dlgType::OnSelChangeTyp(wxNotifyEvent &ev)
{
    if (!type)
    {
        CheckLenEnable();
        txtLength->Enable(isVarLen);
        OnChange(ev);
    }
}


void dlgType::OnChange(wxNotifyEvent &ev)
{
    if (type)
    {
        btnOK->Enable(txtComment->GetValue() != type->GetComment());
    }
    else
    {
        wxString name=GetName();
        bool isComposite=!rdbType->GetSelection();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));
        CheckValid(enable, !name.StartsWith(wxT("_")), wxT("Name may not start with '_'."));

        if (isComposite)
        {
            CheckValid(enable, lstMembers->GetItemCount() > 1, _("Please specify at least two members."));
        }
        else
        {
            txtLength->Enable(!chkVariable->GetValue());
            CheckValid(enable, cbInput->GetSelection() >= 0, _("Please specify input conversion function."));
            CheckValid(enable, cbOutput->GetSelection() >= 0, _("Please specify output conversion function."));
            CheckValid(enable, chkVariable->GetValue() || StrToLong(txtLength->GetValue()) > 0, _("Please specify internal storage length."));
        }
        EnableOK(enable);
    }
}




void dlgType::OnVarSelChange(wxListEvent &ev)
{
    long pos=GetListSelected(lstMembers);
    if (pos >= 0)
    {
        txtMembername->SetValue(lstMembers->GetItemText(pos));
        cbDatatype->SetValue(GetListText(lstMembers, pos, 1));
    }
}


void dlgType::OnVarAdd(wxNotifyEvent &ev)
{
    wxString name=txtMembername->GetValue().Strip(wxString::both);
    wxString type=cbDatatype->GetValue();

    if (!name.IsEmpty())
    {
        long pos=lstMembers->FindItem(-1, name);
        if (pos < 0)
        {
            pos = lstMembers->GetItemCount();
            lstMembers->InsertItem(pos, name, 0);
            memberTypes.Add(GetTypeInfo(cbDatatype->GetSelection()));
        }
        else
        {
            memberTypes.Insert(GetTypeInfo(cbDatatype->GetSelection()), pos);
            memberTypes.RemoveAt(pos+1);
        }
        lstMembers->SetItem(pos, 1, type);
    }
    OnChange(ev);
}


void dlgType::OnVarRemove(wxNotifyEvent &ev)
{
    long pos=GetListSelected(lstMembers);

    if (pos >= 0)
    {
        lstMembers->DeleteItem(pos);
        memberTypes.RemoveAt(pos);
    }
    OnChange(ev);
}


pgObject *dlgType::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=0; //pgType::ReadObjects(collection, 0, wxT("\n WHERE usename=") + qtString(name));
    return obj;
}


wxString dlgType::GetSql()
{
    wxString sql;
    
    if (type)
    {
        // Edit Mode
    }
    else
    {
        // Create Mode
        sql = wxT("CREATE TYPE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(GetName());
        if (rdbType->GetSelection())
        {
            sql += wxT("\n   (INPUT=");
            AppendQuoted(sql, cbInput->GetValue());
            sql += wxT(", OUTPUT=");
            AppendQuoted(sql, cbOutput->GetValue());
            sql += wxT(",\n    INTERNALLENGTH=");
            if (chkVariable->GetValue())
                sql += wxT("VARIABLE");
            else
                sql += txtLength->GetValue();
            AppendIfFilled(sql, wxT(",\n    DEFAULT="), txtDefault->GetValue());
            if (!cbElement->GetValue().IsEmpty())
            {
                sql += wxT(",\n    ELEMENT=");
                AppendQuoted(sql, cbElement->GetValue());
                AppendIfFilled(sql, wxT(", DELIMITER="), qtString(txtDelimiter->GetValue().Strip(wxString::both)));
            }
            if (chkByValue->GetValue())
                sql += wxT(",\n    PASSEDBYVALUE");
            AppendIfFilled(sql, wxT(",\n    ALIGNMENT="), cbAlignment->GetValue());
            AppendIfFilled(sql, wxT(",\n    STORAGE="), cbStorage->GetValue());
        }
        else
        {
            sql += wxT("\n   AS (");

            int i;
            for (i=0 ; i < lstMembers->GetItemCount() ; i++)
            {
                if (i)
                    sql += wxT(", ");
                sql += lstMembers->GetItemText(i) + wxT(" ")
                    + memberTypes.Item(i).AfterFirst(':');
            }
        }
    }
    sql += wxT(");\n");
    AppendComment(sql, wxT("TYPE"), schema, type);

    return sql;
}

