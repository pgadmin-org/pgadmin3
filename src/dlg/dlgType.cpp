//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgType.cpp - PostgreSQL TYPE Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>


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
#define cbElement       CTRL_COMBOBOX2("cbElement")
#define txtDelimiter    CTRL_TEXT("txtDelimiter")
#define chkByValue      CTRL_CHECKBOX("chkByValue")
#define cbAlignment     CTRL_COMBOBOX("cbAlignment")
#define cbStorage       CTRL_COMBOBOX("cbStorage")
#define lstMembers      CTRL_LISTVIEW("lstMembers")
#define txtMembername   CTRL_TEXT("txtMembername")
#define btnAdd          CTRL_BUTTON("wxID_ADD")
#define btnRemove       CTRL_BUTTON("wxID_REMOVE")
#define pnlDefinition   CTRL_PANEL("pnlDefinition")
#define pnlDefinitionExtern     CTRL_PANEL("pnlDefinitionExtern")
#define pnlDefinitionComposite  CTRL_PANEL("pnlDefinitionComposite")


BEGIN_EVENT_TABLE(dlgType, dlgTypeProperty)
    EVT_RADIOBOX(XRCID("rdbType"),                  dlgType::OnTypeChange)

    EVT_TEXT(XRCID("cbInput"),                      dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbInput"),                  dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbOutput"),                     dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbOutput"),                 dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtIntLength"),                 dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkVariable"),              dlgProperty::OnChange)
    
    EVT_BUTTON(wxID_ADD,                            dlgType::OnVarAdd)
    EVT_BUTTON(wxID_REMOVE,                         dlgType::OnVarRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgType::OnVarSelChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgType::OnSelChangeTyp)
    EVT_COMBOBOX(XRCID("cbDatatype"),               dlgType::OnSelChangeTyp)
    EVT_TEXT(XRCID("txtMembername"),                dlgType::OnChangeMember)
    EVT_TEXT(XRCID("txtLength"),                    dlgType::OnSelChangeTypOrLen)
END_EVENT_TABLE();


dlgProperty *pgTypeFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgType(this, frame, (pgType*)node, (pgSchema*)parent);
}


dlgType::dlgType(pgaFactory *f, frmMain *frame, pgType *node, pgSchema *sch)
: dlgTypeProperty(f, frame, wxT("dlgType"))
{
    type=node;
    schema=sch;
    lstMembers->CreateColumns(0, _("Member"), _("Data type"), -1);

    wxNotifyEvent event;
    OnTypeChange(event);
}


void dlgType::OnChangeMember(wxCommandEvent &ev)
{
    btnAdd->Enable(!txtMembername->GetValue().Strip(wxString::both).IsEmpty() 
        && cbDatatype->GetGuessedSelection() >= 0);
}

void dlgType::showDefinition(bool isComposite)
{
    pnlDefinitionExtern->Show(!isComposite);
    pnlDefinitionComposite->Show(isComposite);
}


void dlgType::OnTypeChange(wxCommandEvent &ev)
{
    showDefinition(!rdbType->GetSelection());
    
    CheckChange();
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
        txtName->Disable();
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

        txtMembername->Disable();
        btnAdd->Disable();
        btnRemove->Disable();

        wxArrayString elements=type->GetTypesArray();
        size_t i;
		for (i=0 ; i < elements.GetCount() ; i+=2)
            lstMembers->AppendItem(0, elements.Item(i), elements.Item(i+1));

        cbDatatype->Disable();
        txtLength->Disable();

        if (!connection->BackendMinimumVersion(7, 5))
            cbOwner->Disable();
    }
    else
    {
        // Create mode
        cbOwner->Append(wxEmptyString);
        cbOwner->Disable();

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
    cbDatatype->GuessSelection(ev);
    OnSelChangeTypOrLen(ev);
}


void dlgType::OnSelChangeTypOrLen(wxCommandEvent &ev)
{
    if (!type)
    {
        CheckLenEnable();
        txtLength->Enable(isVarLen);
        txtPrecision->Enable(isVarPrec);
        CheckChange();
        OnChangeMember(ev);
    }
}


void dlgType::CheckChange()
{
    if (type)
    {
        EnableOK(txtComment->GetValue() != type->GetComment()
            || cbOwner->GetValue() != type->GetOwner());
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
            CheckValid(enable, cbInput->GetCurrentSelection() >= 0, _("Please specify input conversion function."));
            CheckValid(enable, cbOutput->GetCurrentSelection() >= 0, _("Please specify output conversion function."));
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

    if ((txtLength->GetValue() != wxT("") && txtLength->IsEnabled()) && (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled()))
        type += wxT("(") + txtLength->GetValue() + wxT(", ") + txtPrecision->GetValue() + wxT(")");
    else if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
        type += wxT("(") + txtLength->GetValue() + wxT(")");

    if (!name.IsEmpty())
    {
        long pos=lstMembers->FindItem(-1, name);
        if (pos < 0)
        {
            pos = lstMembers->GetItemCount();
            lstMembers->InsertItem(pos, name, 0);
            memberTypes.Add(GetTypeInfo(cbDatatype->GetGuessedSelection()));

            if ((txtLength->GetValue() != wxT("") && txtLength->IsEnabled()) && (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled()))
                memberSizes.Add(wxT("(") + txtLength->GetValue() + wxT(", ") + txtPrecision->GetValue() + wxT(")"));
            else if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
                memberSizes.Add(wxT("(") + txtLength->GetValue() + wxT(")"));
            else
                memberSizes.Add(wxT(""));
        }
        else
        {
            memberTypes.Insert(GetTypeInfo(cbDatatype->GetGuessedSelection()), pos);
            memberTypes.RemoveAt(pos+1);

            if ((txtLength->GetValue() != wxT("")  && txtLength->IsEnabled()) && (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled()))
                memberSizes.Insert(wxT("(") + txtLength->GetValue() + wxT(", ") + txtPrecision->GetValue() + wxT(")"), pos);
            else if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
                memberSizes.Insert(wxT("(") + txtLength->GetValue() + wxT(")"), pos);
            else
                memberSizes.Insert(wxT(""), pos);
            memberSizes.RemoveAt(pos+1);
        }
        lstMembers->SetItem(pos, 1, type);
    }

    CheckChange();
}


void dlgType::OnVarRemove(wxCommandEvent &ev)
{
    long pos=lstMembers->GetSelection();

    if (pos >= 0)
    {
        lstMembers->DeleteItem(pos);
        memberTypes.RemoveAt(pos);
        memberSizes.RemoveAt(pos);
    }
    CheckChange();
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
        AppendOwnerChange(sql, wxT("TYPE ") + type->GetQuotedFullIdentifier());
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
                if (cbReceive->GetCurrentSelection() > 0 || cbSend->GetCurrentSelection() > 0)
                {
                    if (cbReceive->GetCurrentSelection() > 0)
                    {
                        sql += wxT(",\n   RECEIVE=");
                        AppendQuoted(sql, cbReceive->GetValue());
                        if (cbSend->GetCurrentSelection() > 0)
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
                sql += qtIdent(lstMembers->GetItemText(i)) + wxT(" ")
                    + memberTypes.Item(i).AfterFirst(':')
                    + memberSizes.Item(i);
            }
        }
        sql += wxT(");\n");
    }
    AppendComment(sql, wxT("TYPE"), schema, type);

    return sql;
}

