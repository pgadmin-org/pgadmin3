//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#define cbTypmodin              CTRL_COMBOBOX("cbTypmodin")
#define cbTypmodout             CTRL_COMBOBOX("cbTypmodout")
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
#define btnChangeMember         CTRL_BUTTON("btnChangeMember")
#define btnRemoveMember         CTRL_BUTTON("btnRemoveMember")
#define btnAddAfterLabel        CTRL_BUTTON("btnAddAfterLabel")
#define btnAddBeforeLabel       CTRL_BUTTON("btnAddBeforeLabel")
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
    EVT_BUTTON(XRCID("btnChangeMember"),            dlgType::OnMemberChange)
    EVT_BUTTON(XRCID("btnRemoveMember"),            dlgType::OnMemberRemove)
    EVT_BUTTON(XRCID("btnAddBeforeLabel"),          dlgType::OnLabelAddBefore)
    EVT_BUTTON(XRCID("btnAddAfterLabel"),           dlgType::OnLabelAddAfter)
    EVT_BUTTON(XRCID("btnRemoveLabel"),             dlgType::OnLabelRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstMembers"),     dlgType::OnMemberSelChange)
    EVT_LIST_ITEM_SELECTED(XRCID("lstLabels"),      dlgType::OnLabelSelChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgType::OnSelChangeTyp)
    EVT_COMBOBOX(XRCID("cbDatatype"),               dlgType::OnSelChangeTyp)
    EVT_TEXT(XRCID("txtMembername"),                dlgType::OnChangeMember)
    EVT_TEXT(XRCID("txtLength"),                    dlgType::OnSelChangeTypOrLen)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgType::OnSelChangeTypOrLen)
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

    queriesToBeSplitted = false;

    wxNotifyEvent event;
    OnTypeChange(event);
}


void dlgType::OnChangeMember(wxCommandEvent &ev)
{
    btnAddMember->Enable(
        ((type && connection->BackendMinimumVersion(9, 1)) || !type)
        && !txtMembername->GetValue().Strip(wxString::both).IsEmpty()
        && cbDatatype->GetGuessedSelection() >= 0);
    btnChangeMember->Enable(true);
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

    pnlDefinitionComposite->GetParent()->Layout();
    // we don't need to call GetParent()->Layout() for all three panels
    // because they all share the same parent
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
    pgSet *set;

    FillDatatype(cbDatatype, cbElement);

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
        cbTypmodin->Append(type->GetTypmodinFunction()); cbTypmodin->SetSelection(0); cbTypmodin->Disable();
        cbTypmodout->Append(type->GetTypmodoutFunction()); cbTypmodout->SetSelection(0); cbTypmodout->Disable();

        chkVariable->SetValue(type->GetInternalLength() < 0);
        chkVariable->Disable();
        if (type->GetInternalLength() > 0)
            txtIntLength->SetValue(NumToStr(type->GetInternalLength())); 
        txtIntLength->Disable();
        txtDefault->SetValue(type->GetDefault());
        txtDefault->Disable();
        cbElement->Append(type->GetElement());
        cbElement->SetSelection(0);
        cbElement->Disable();
        txtDelimiter->SetValue(type->GetDelimiter());
        txtDelimiter->Disable();
        chkByValue->SetValue(type->GetPassedByValue());
        chkByValue->Disable();
        cbAlignment->SetValue(type->GetAlignment());
        cbAlignment->Disable();
        cbStorage->SetValue(type->GetStorage());
        cbStorage->Disable();

        bool changeok = connection->BackendMinimumVersion(9, 1);
        txtMembername->Enable(changeok);
        btnAddMember->Enable(changeok);
        btnChangeMember->Enable(false);
        btnRemoveMember->Enable(false);

        txtLabel->Enable(connection->BackendMinimumVersion(9, 1));
        btnAddBeforeLabel->Enable(connection->BackendMinimumVersion(9, 1));
        btnAddAfterLabel->Enable(connection->BackendMinimumVersion(9, 1));
        btnRemoveLabel->Disable();

        wxArrayString elements=type->GetTypesArray();
        wxString fullType, typeName, typeLength, typePrecision;
        size_t pos;
        size_t i;
		for (i=0 ; i < elements.GetCount() ; i+=2)
        {
            lstMembers->AppendItem(0, elements.Item(i), elements.Item(i+1));

            fullType = elements.Item(i+1);
            typeName = fullType;
            typeLength = wxEmptyString;
            typePrecision = wxEmptyString;

            if (fullType.Find(wxT("(")) > 0)
            {
                // there is at least a length
                typeName = fullType.BeforeFirst('(');
                if (fullType.Find(wxT(",")) > 0)
                {
                    // there is also a precision
                    typeLength = fullType.AfterFirst('(').BeforeFirst(',');
                    typePrecision = fullType.AfterFirst(',').BeforeFirst(')');
                }
                else
                    typeLength = fullType.AfterFirst('(').BeforeFirst(')');
            }

            for (pos = 0; pos < cbDatatype->GetCount() - 1; pos++)
            {
                if (cbDatatype->GetString(pos) == typeName)
                {
                    memberTypes.Add(GetTypeInfo(pos));
                    break;
                }
            }
            memberLengths.Add(typeLength);
            memberPrecisions.Add(typePrecision);
        }

        cbDatatype->Enable(changeok);
        txtLength->Enable(changeok);

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
        bool hasTypmod = connection->BackendMinimumVersion(8, 3);

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

        if (hasTypmod)
        {
            cbTypmodin->Append(wxEmptyString);
            cbTypmodout->Append(wxEmptyString);
        }
        else
        {
            cbTypmodin->Disable();
            cbTypmodout->Disable();
        }

        if (!connection->BackendMinimumVersion(8, 3))
            rdbType->Enable(TYPE_ENUM, false);

        set = connection->ExecuteSet(
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
                if (hasTypmod)
                {
                    cbTypmodin->Append(pn);
                    cbTypmodout->Append(pn);
                }
                set->MoveNext();
            }
            delete set;
        }

        if (hasTypmod)
        {
            set = connection->ExecuteSet(
                wxT("SELECT proname, nspname\n")
                wxT("  FROM pg_proc p\n")
                wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
                wxT("  WHERE prorettype=(SELECT oid FROM pg_type WHERE typname='int4')")
                wxT("    AND proargtypes[0]=(SELECT oid FROM pg_type WHERE typname='_cstring')")
                wxT("    AND proargtypes[1] IS NULL")
                wxT("  ORDER BY nspname, proname"));

            if (set)
            {
                while (!set->Eof())
                {
                    wxString pn = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

                    cbTypmodin->Append(pn);
                    set->MoveNext();
                }
                delete set;
            }

            set = connection->ExecuteSet(
                wxT("SELECT proname, nspname\n")
                wxT("  FROM pg_proc p\n")
                wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
                wxT("  WHERE prorettype=(SELECT oid FROM pg_type WHERE typname='cstring')")
                wxT("    AND proargtypes[0]=(SELECT oid FROM pg_type WHERE typname='int4')")
                wxT("    AND proargtypes[1] IS NULL")
                wxT("  ORDER BY nspname, proname"));

            if (set)
            {
                while (!set->Eof())
                {
                    wxString pn = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

                    cbTypmodout->Append(pn);
                    set->MoveNext();
                }
                delete set;
            }
        }
    }

    txtLength->SetValidator(numericValidator);

    return dlgTypeProperty::Go(modal);
}


void dlgType::OnSelChangeTyp(wxCommandEvent &ev)
{
    txtLength->SetValue(wxEmptyString);
    txtPrecision->SetValue(wxEmptyString);
    cbDatatype->GuessSelection(ev);
    OnSelChangeTypOrLen(ev);
}


void dlgType::OnSelChangeTypOrLen(wxCommandEvent &ev)
{
    if ((type && connection->BackendMinimumVersion(9, 1)) || !type)
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
            || cbOwner->GetValue() != type->GetOwner()
            || (rdbType->GetSelection() == TYPE_COMPOSITE && GetSqlForTypes() != wxEmptyString)
            || (GetSql().Length() > 0 && connection->BackendMinimumVersion(9, 1)));
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
        cbDatatype->SetValue(memberTypes.Item(pos).AfterFirst(':'));
        txtLength->SetValue(memberLengths.Item(pos));
        txtLength->Enable(((type && connection->BackendMinimumVersion(9, 1)) || !type) && !txtLength->GetValue().IsEmpty());
        txtPrecision->SetValue(memberPrecisions.Item(pos));
        txtPrecision->Enable(((type && connection->BackendMinimumVersion(9, 1)) || !type) && !txtPrecision->GetValue().IsEmpty());
        btnChangeMember->Enable((type && connection->BackendMinimumVersion(9, 1)) || !type);
        btnRemoveMember->Enable((type && connection->BackendMinimumVersion(9, 1)) || !type);
    }
}


void dlgType::OnMemberAdd(wxCommandEvent &ev)
{
    wxString name = txtMembername->GetValue().Strip(wxString::both);
    wxString type = cbDatatype->GetValue();
    wxString length = wxEmptyString;
    wxString precision = wxEmptyString;

    if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
       length = txtLength->GetValue();
    if (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled())
       precision = txtPrecision->GetValue();

    if (!length.IsEmpty())
    {
        type += wxT("(") + length;
        if (!precision.IsEmpty())
            type += wxT(",") + precision;
        type += wxT(")");
    }

    if (!name.IsEmpty())
    {
        size_t pos = lstMembers->GetItemCount();
        lstMembers->InsertItem(pos, name, 0);
        lstMembers->SetItem(pos, 1, type);
        memberTypes.Add(GetTypeInfo(cbDatatype->GetGuessedSelection()));
        memberLengths.Add(length);
        memberPrecisions.Add(precision);
    }

    CheckChange();
}


void dlgType::OnMemberChange(wxCommandEvent &ev)
{
    wxString name = txtMembername->GetValue().Strip(wxString::both);
    wxString type = cbDatatype->GetValue();
    wxString length = wxEmptyString;
    wxString precision = wxEmptyString;

    if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
       length = txtLength->GetValue();
    if (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled())
       precision = txtPrecision->GetValue();

    if (!length.IsEmpty())
    {
        type += wxT("(") + length;
        if (!precision.IsEmpty())
            type += wxT(",") + precision;
        type += wxT(")");
    }

    if (!name.IsEmpty())
    {
        long pos=lstMembers->GetFirstSelected();
        if (pos >= 0)
        {
            lstMembers->SetItem(pos, 0, name);
            lstMembers->SetItem(pos, 1, type);
            memberTypes.Insert(GetTypeInfo(cbDatatype->GetGuessedSelection()), pos);
            memberLengths.Insert(length, pos);
            memberPrecisions.Insert(precision, pos);
            memberTypes.RemoveAt(pos+1);
            memberLengths.RemoveAt(pos+1);
            memberPrecisions.RemoveAt(pos+1);
        }
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
        memberLengths.RemoveAt(pos);
        memberPrecisions.RemoveAt(pos);
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


void dlgType::OnLabelAddBefore(wxCommandEvent &ev)
{
    wxString label=txtLabel->GetValue().Strip(wxString::both);

    if (!label.IsEmpty())
    {
        long pos=lstLabels->FindItem(-1, label);
        if (pos < 0)
        {
            if (lstLabels->GetFirstSelected() >= 0)
                pos = lstLabels->GetFirstSelected();
            else
                pos = 0;
            lstLabels->InsertItem(pos, label, 0);
        }
    }
    txtLabel->SetValue(wxEmptyString);
    CheckChange();
}


void dlgType::OnLabelAddAfter(wxCommandEvent &ev)
{
    wxString label=txtLabel->GetValue().Strip(wxString::both);

    if (!label.IsEmpty())
    {
        long pos=lstLabels->FindItem(-1, label);
        if (pos < 0)
        {
            if (lstLabels->GetFirstSelected() >= 0)
                pos = lstLabels->GetFirstSelected() + 1;
            else
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
    wxString sql,direction;
    size_t existingitems_index, listitems_index, offset;
    
    if (type)
    {
        // Edit Mode
        AppendOwnerChange(sql, wxT("TYPE ") + type->GetQuotedFullIdentifier());
        sql += GetSqlForTypes();
        if (rdbType->GetSelection() == TYPE_ENUM && connection->BackendMinimumVersion(9, 1))
        {
            wxArrayString elements=type->GetLabelArray();
            existingitems_index = 0;
            for (listitems_index=0 ; listitems_index < (size_t)lstLabels->GetItemCount() ; listitems_index++)
            {
                if (existingitems_index >= elements.GetCount() || lstLabels->GetItemText(listitems_index) != elements.Item(existingitems_index))
                {
                    queriesToBeSplitted = true;
                    if (listitems_index == 0)
                    {
                        direction = wxT("BEFORE");
                        offset = 0;
                    }
                    else
                    {
                        direction = wxT("AFTER");
                        offset = -1;
                    }

                    sql += wxT("ALTER TYPE ") + type->GetQuotedFullIdentifier()
                        +  wxT(" ADD ") + connection->qtDbString(lstLabels->GetItemText(listitems_index))
                        +  wxT(" ") + direction + wxT(" ")
                        + connection->qtDbString(elements.Item(existingitems_index + offset))
                        + wxT(";\n");
                }
                else
                    existingitems_index++;
            }
        }
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
                    + GetFullTypeName(i);
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
            if (connection->BackendMinimumVersion(8, 3))
            {
                if (cbTypmodin->GetCurrentSelection() > 0 || cbTypmodout->GetCurrentSelection() > 0)
                {
                    if (cbTypmodin->GetCurrentSelection() > 0)
                    {
                        sql += wxT(",\n   TYPMOD_IN=");
                        AppendQuoted(sql, cbTypmodin->GetValue());
                        if (cbTypmodout->GetCurrentSelection() > 0)
                        {
                            sql += wxT(", TYPMOD_OUT=");
                            AppendQuoted(sql, cbTypmodout->GetValue());
                        }
                    }
                    else
                    {
                        sql += wxT(",\n   TYPMOD_OUT=");
                        AppendQuoted(sql, cbTypmodout->GetValue());
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

wxString dlgType::GetFullTypeName(int type)
{
    wxString typname = memberTypes.Item(type).AfterFirst(':');

    if (!memberLengths.Item(type).IsEmpty())
    {
        typname += wxT("(") + memberLengths.Item(type);
        if (!memberPrecisions.Item(type).IsEmpty())
            typname += wxT(",") + memberPrecisions.Item(type);
        typname += wxT(")");
    }

    return typname;
}

wxString dlgType::GetSqlForTypes()
{
    wxString sql = wxEmptyString;
    wxString old_name, old_type, new_name, new_type;
    wxArrayString elements=type->GetTypesArray();
    bool modified = lstMembers->GetItemCount()*2 != (int)elements.GetCount();
    size_t i;

    // Check if there is a change
    for (int i=0 ; i < lstMembers->GetItemCount() && !modified; i++)
    {
        old_name = elements.Item(i*2);
        old_type = elements.Item(i*2+1);
        new_name = lstMembers->GetItemText(i);
        new_type = GetFullTypeName(i);
        modified = modified || old_name != new_name || old_type != new_type;
    }

    if (modified)
    {
        // Drop all old attributes
		for (i=0 ; i < elements.GetCount() ; i+=2)
        {
            old_name = elements.Item(i);
            sql += wxT("ALTER TYPE type DROP ATTRIBUTE ") + old_name + wxT(";\n");
        }

        // Add all new attributes
        for (int i=0 ; i < lstMembers->GetItemCount() ; i++)
        {
            new_name = lstMembers->GetItemText(i);
            new_type = GetFullTypeName(i);
            sql += wxT("ALTER TYPE type ADD ATTRIBUTE ")
                   + new_name + wxT(" ") + new_type + wxT(";\n");
        }
    }

    return sql;
}
