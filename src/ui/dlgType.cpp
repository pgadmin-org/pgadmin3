//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgType.cpp - PostgreSQL TYPE Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// Images
#include "images/type.xpm"

// App headers
#include "misc.h"
#include "dlgType.h"
#include "pgSchema.h"
#include "pgType.h"
#include "pgDatatype.h"


// pointer to controls
#define rdbType         CTRL_RADIOBOX("rdbType")

#define cbInput         CTRL_COMBOBOX("cbInput")
#define cbOutput        CTRL_COMBOBOX("cbOutput")
#define cbReceive       CTRL_COMBOBOX("cbReceive")
#define cbSend          CTRL_COMBOBOX("cbSend")
#define chkVariable     CTRL_CHECKBOX("chkVariable")
#define txtIntLength    CTRL_TEXT("txtIntLength")
#define txtDefault      CTRL_TEXT("txtDefault")
#define cbElement       CTRL_COMBOBOX("cbElement")
#define txtDelimiter    CTRL_TEXT("txtDelimiter")
#define chkByValue      CTRL_CHECKBOX("chkByValue")
#define cbAlignment     CTRL_COMBOBOX("cbAlignment")
#define cbStorage       CTRL_COMBOBOX("cbStorage")
#define lstMembers      CTRL_LISTVIEW("lstMembers")
#define txtMembername   CTRL_TEXT("txtMembername")
#define btnAdd          CTRL_BUTTON("btnAdd")
#define btnRemove       CTRL_BUTTON("btnRemove")

BEGIN_EVENT_TABLE(dlgType, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgType::OnChange)
    EVT_RADIOBOX(XRCID("rdbType"),                  dlgType::OnTypeChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgType::OnChange)

    EVT_TEXT(XRCID("cbInput"),                      dlgType::OnChange)
    EVT_TEXT(XRCID("cbOutput"),                     dlgType::OnChange)
    EVT_TEXT(XRCID("txtIntLength"),                 dlgType::OnChange)
    EVT_CHECKBOX(XRCID("chkVariable"),              dlgType::OnChange)
    
    EVT_BUTTON(XRCID("btnAdd"),                     dlgType::OnVarAdd)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgType::OnVarRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgType::OnVarSelChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgType::OnSelChangeTyp)
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


void dlgType::OnChangeMember(wxCommandEvent &ev)
{
    btnAdd->Enable(!txtMembername->GetValue().Strip(wxString::both).IsEmpty() 
        && cbDatatype->GetSelection() >= 0);
}

void dlgType::showDefinition(bool isComposite)
{
    int i;
    
    i=externalWindows.GetCount();
    while (i--)
        externalWindows.Item(i)->GetData()->Show(!isComposite);

    i=compositeWindows.GetCount();
    while (i--)
        compositeWindows.Item(i)->GetData()->Show(isComposite);
}


void dlgType::OnTypeChange(wxCommandEvent &ev)
{
    showDefinition(!rdbType->GetSelection());
    
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
        txtName->SetValue(type->GetIdentifier()); txtName->Disable();
        txtOID->SetValue(NumToStr((long)type->GetOid())); txtOID->Disable();
        rdbType->SetSelection(type->GetIsComposite() ? 0 : 1); rdbType->Disable();

        showDefinition(type->GetIsComposite());

        cbInput->Append(type->GetInputFunction()); cbInput->SetSelection(0); cbInput->Disable();
        cbOutput->Append(type->GetOutputFunction()); cbOutput->SetSelection(0); cbOutput->Disable();
        cbReceive->Append(type->GetReceiveFunction()); cbReceive->SetSelection(0); cbReceive->Disable();
        cbSend->Append(type->GetSendFunction()); cbSend->SetSelection(0); cbSend->Disable();

        chkVariable->SetValue(type->GetInternalLength() < 0); chkVariable->Disable();
        if (type->GetInternalLength() > 0)
            txtIntLength->SetValue(NumToStr(type->GetInternalLength())); 
        txtIntLength->Disable();
        txtDefault->SetValue(type->GetDefault()); txtDefault->Disable();
        cbElement->Append(type->GetElement()); cbElement->SetSelection(0); cbElement->Disable();
        txtDelimiter->SetValue(type->GetDelimiter()); txtDelimiter->Disable();
        chkByValue->SetValue(type->GetPassedByValue()); chkByValue->Disable();
        cbAlignment->SetValue(type->GetAlignment()); cbAlignment->Disable();
        cbStorage->SetValue(type->GetStorage()); cbStorage->Disable();
        txtComment->SetValue(type->GetComment());

        txtMembername->Disable();
        btnAdd->Disable();
        btnRemove->Disable();

        wxStringTokenizer members(type->GetTypesList(), wxT(","));
        while (members.HasMoreTokens())
        {
            wxString str=members.GetNextToken().Strip(wxString::both);
            AppendListItem(lstMembers, str.BeforeFirst(' '), str.AfterFirst(' '), 0);
        }

        cbDatatype->Disable();
        txtLength->Disable();
    }
    else
    {
        // Create mode

        bool hasSendRcv = connection->BackendMinimumVersion(7, 4);

        if (hasSendRcv)
        {
            cbReceive->Append(wxEmptyString);
            cbSend->Append(wxEmptyString);
        }
        else
        {
            cbReceive->Disable();
            cbSend->Disable();
        }

        pgSet *set=connection->ExecuteSet(
            wxT("SELECT proname, nspname\n")
            wxT("  FROM (\n")
            wxT("        SELECT proname, nspname, max(proargtypes[0]) AS arg0, max(proargtypes[1]) AS arg1\n")
            wxT("          FROM pg_proc p\n")
            wxT("          JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT("         GROUP BY proname, nspname\n")
            wxT("        HAVING count(proname) = 1   ) AS uniquefunc\n")
            wxT(" WHERE arg0 <> 0 AND arg1 = 0"));
        
        if (set)
        {
            while (!set->Eof())
            {
                wxString pn = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

                cbInput->Append(pn);
                cbOutput->Append(pn);
                if (hasSendRcv)
                {
                    cbOutput->Append(pn);
                    cbOutput->Append(pn);
                }
                set->MoveNext();
            }
            delete set;
        }
        FillDatatype(cbDatatype, cbElement);
        txtLength->SetValidator(numericValidator);
    }
    return dlgTypeProperty::Go(modal);
}


void dlgType::OnSelChangeTyp(wxCommandEvent &ev)
{
    if (!type)
    {
        CheckLenEnable();
        txtLength->Enable(isVarLen);
        OnChange(ev);
    }
}


void dlgType::OnChange(wxCommandEvent &ev)
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
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, !name.StartsWith(wxT("_")), _("Name may not start with '_'."));

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
    long pos=lstMembers->GetSelection();
    if (pos >= 0)
    {
        txtMembername->SetValue(lstMembers->GetText(pos));
        cbDatatype->SetValue(lstMembers->GetText(pos, 1));
    }
}


void dlgType::OnVarAdd(wxCommandEvent &ev)
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


void dlgType::OnVarRemove(wxCommandEvent &ev)
{
    long pos=lstMembers->GetSelection();

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
        sql = wxT("CREATE TYPE ") + schema->GetQuotedPrefix() + qtIdent(GetName());
        if (rdbType->GetSelection())
        {
            sql += wxT("\n   (INPUT=");
            AppendQuoted(sql, cbInput->GetValue());
            sql += wxT(", OUTPUT=");
            AppendQuoted(sql, cbOutput->GetValue());

            if (connection->BackendMinimumVersion(7, 4))
            {
                if (cbReceive->GetSelection() > 0 || cbSend->GetSelection() > 0)
                {
                    if (cbReceive->GetSelection() > 0)
                    {
                        sql += wxT(",\n   RECEIVE=");
                        AppendQuoted(sql, cbReceive->GetValue());
                        if (cbSend->GetSelection() > 0)
                        {
                            sql += wxT(", SEND=");
                            AppendQuoted(sql, cbSend->GetValue());
                        }
                    }
                    else
                    {
                        sql += wxT(",\n   SEND=");
                        AppendQuoted(sql, cbSend->GetValue());
                    }
                }

            }
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
        sql += wxT(");\n");
    }
    AppendComment(sql, wxT("TYPE"), schema, type);

    return sql;
}

