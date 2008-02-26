//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgType.cpp - PostgreSQL TYPE Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>


// App headers
#include "utils/misc.h"
#include "dlg/dlgType.h"
#include "schema/pgSchema.h"
#include "schema/pgType.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define rdbType                 CTRL_RADIOBOX("rdbType")

#define cbInput                 CTRL_COMBOBOX("cbInput")
#define cbOutput                CTRL_COMBOBOX("cbOutput")
#define cbReceive               CTRL_COMBOBOX("cbReceive")
#define cbSend                  CTRL_COMBOBOX("cbSend")
#define chkVariable             CTRL_CHECKBOX("chkVariable")
#define txtIntLength            CTRL_TEXT("txtIntLength")
#define txtDefault              CTRL_TEXT("txtDefault")
#define cbElement               CTRL_COMBOBOX2("cbElement")
#define txtDelimiter            CTRL_TEXT("txtDelimiter")
#define chkByValue              CTRL_CHECKBOX("chkByValue")
#define cbAlignment             CTRL_COMBOBOX("cbAlignment")
#define cbStorage               CTRL_COMBOBOX("cbStorage")
#define lstMembers              CTRL_LISTVIEW("lstMembers")
#define txtMembername           CTRL_TEXT("txtMembername")
#define lstLabels               CTRL_LISTVIEW("lstLabels")
#define txtLabel                CTRL_TEXT("txtLabel")
#define btnAddMember            CTRL_BUTTON("btnAddMember")
#define btnRemoveMember         CTRL_BUTTON("btnRemoveMember")
#define btnAddLabel             CTRL_BUTTON("btnAddLabel")
#define btnRemoveLabel          CTRL_BUTTON("btnRemoveLabel")
#define pnlDefinition           CTRL_PANEL("pnlDefinition")
#define pnlDefinitionExtern     CTRL_PANEL("pnlDefinitionExtern")
#define pnlDefinitionComposite  CTRL_PANEL("pnlDefinitionComposite")
#define pnlDefinitionEnum       CTRL_PANEL("pnlDefinitionEnum")


BEGIN_EVENT_TABLE(dlgType, dlgTypeProperty)
    EVT_RADIOBOX(XRCID("rdbType"),                  dlgType::OnTypeChange)

    EVT_TEXT(XRCID("cbInput"),                      dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbInput"),                  dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbOutput"),                     dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbOutput"),                 dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtIntLength"),                 dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkVariable"),              dlgProperty::OnChange)
    
    EVT_BUTTON(XRCID("btnAddMember"),               dlgType::OnMemberAdd)
    EVT_BUTTON(XRCID("btnRemoveMember"),            dlgType::OnMemberRemove)
    EVT_BUTTON(XRCID("btnAddLabel"),                dlgType::OnLabelAdd)
    EVT_BUTTON(XRCID("btnRemoveLabel"),             dlgType::OnLabelRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstMembers"),     dlgType::OnMemberSelChange)
    EVT_LIST_ITEM_SELECTED(XRCID("lstLabels"),      dlgType::OnLabelSelChange)
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
    lstLabels->InsertColumn(0, _("Label"), wxLIST_FORMAT_LEFT, GetClientSize().GetWidth());

    wxNotifyEvent event;
    OnTypeChange(event);
}


void dlgType::OnChangeMember(wxCommandEvent &ev)
{
    btnAddMember->Enable(!txtMembername->GetValue().Strip(wxString::both).IsEmpty() 
        && cbDatatype->GetGuessedSelection() >= 0);
}

void dlgType::showDefinition(int panel)
{
    pnlDefinitionExtern->Show(false);
    pnlDefinitionComposite->Show(false);
    pnlDefinitionEnum->Show(false);

    switch (panel)
    {
        case 0:
            pnlDefinitionComposite->Show(true);
            break;
        case 1:
            pnlDefinitionEnum->Show(true);
            break;
        case 2:
            pnlDefinitionExtern->Show(true);
            break;
    }
}


void dlgType::OnTypeChange(wxCommandEvent &ev)
{
    showDefinition(rdbType->GetSelection());
    
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
        rdbType->SetSelection(type->GetTypeClass());
        rdbType->Disable();

        showDefinition(type->GetTypeClass());

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
        btnAddMember->Disable();
        btnRemoveMember->Disable();

        txtLabel->Disable();
        btnAddLabel->Disable();
        btnRemoveLabel->Disable();

        wxArrayString elements=type->GetTypesArray();
        size_t i;
		for (i=0 ; i < elements.GetCount() ; i+=2)
            lstMembers->AppendItem(0, elements.Item(i), elements.Item(i+1));

        cbDatatype->Disable();
        txtLength->Disable();

        // Load the enum labels
        elements=type->GetLabelArray();
		for (i=0 ; i < elements.GetCount() ; i++)
            lstLabels->AppendItem(0, elements.Item(i));

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

        if (!connection->BackendMinimumVersion(8, 3))
            rdbType->Enable(TYPE_ENUM, false);

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
                    cbReceive->Append(pn);
                    cbSend->Append(pn);
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

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, !name.StartsWith(wxT("_")), _("Name may not start with '_'."));

        if (rdbType->GetSelection() == TYPE_COMPOSITE)
        {
            CheckValid(enable, lstMembers->GetItemCount() > 1, _("Please specify at least two members."));
        }
        else if (rdbType->GetSelection() == TYPE_ENUM)
        {
            CheckValid(enable, lstLabels->GetItemCount() >= 1, _("Please specify at least one label."));
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


void dlgType::OnMemberSelChange(wxListEvent &ev)
{
    long pos=lstMembers->GetSelection();
    if (pos >= 0)
    {
        txtMembername->SetValue(lstMembers->GetText(pos));
        cbDatatype->SetValue(lstMembers->GetText(pos, 1));
    }
}


void dlgType::OnMemberAdd(wxCommandEvent &ev)
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


void dlgType::OnMemberRemove(wxCommandEvent &ev)
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


void dlgType::OnLabelSelChange(wxListEvent &ev)
{
    long pos=lstLabels->GetSelection();
    if (pos >= 0)
    {
        txtLabel->SetValue(lstLabels->GetText(pos));
    }
}


void dlgType::OnLabelAdd(wxCommandEvent &ev)
{
    wxString label=txtLabel->GetValue().Strip(wxString::both);

    if (!label.IsEmpty())
    {
        long pos=lstLabels->FindItem(-1, label);
        if (pos < 0)
        {
            pos = lstLabels->GetItemCount();
            lstLabels->InsertItem(pos, label, 0);
        }
    }
    txtLabel->SetValue(wxEmptyString);
    CheckChange();
}


void dlgType::OnLabelRemove(wxCommandEvent &ev)
{
    long pos=lstLabels->GetSelection();

    if (pos >= 0)
        lstLabels->DeleteItem(pos);

    CheckChange();
}


pgObject *dlgType::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=0; //pgType::ReadObjects(collection, 0, wxT("\n WHERE usename=") + qtDbString(name));
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

        if (rdbType->GetSelection() == TYPE_COMPOSITE)
        {
            sql += wxT(" AS\n   (");

            int i;
            for (i=0 ; i < lstMembers->GetItemCount() ; i++)
            {
                if (i)
                    sql += wxT(",\n    ");
                sql += qtIdent(lstMembers->GetItemText(i)) + wxT(" ")
                    + memberTypes.Item(i).AfterFirst(':')
                    + memberSizes.Item(i);
            }
        }
        else if (rdbType->GetSelection() == TYPE_ENUM)
        {
            sql += wxT(" AS ENUM\n   (");

            int i;
            for (i=0 ; i < lstLabels->GetItemCount() ; i++)
            {
                if (i)
                    sql += wxT(",\n    ");
                sql += connection->qtDbString(lstLabels->GetItemText(i));
            }
        }
        else
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
                AppendIfFilled(sql, wxT(", DELIMITER="), qtDbString(txtDelimiter->GetValue().Strip(wxString::both)));
            }
            if (chkByValue->GetValue())
                sql += wxT(",\n    PASSEDBYVALUE");
            AppendIfFilled(sql, wxT(",\n    ALIGNMENT="), cbAlignment->GetValue());
            AppendIfFilled(sql, wxT(",\n    STORAGE="), cbStorage->GetValue());
        }

        sql += wxT(");\n");
    }
    AppendComment(sql, wxT("TYPE"), schema, type);

    return sql;
}

