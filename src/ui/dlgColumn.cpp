//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgColumns.cpp - PostgreSQL Columns Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "dlgColumn.h"
#include "pgSchema.h"
#include "pgColumn.h"
#include "pgTable.h"
#include "pgDatatype.h"

// Images
#include "images/column.xpm"


// pointer to controls
#define txtDefault      CTRL("txtDefault", wxTextCtrl)
#define chkNotNull      CTRL("chkNotNull", wxCheckBox)
#define txtAttstattarget      CTRL("txtAttstattarget",      wxTextCtrl)



BEGIN_EVENT_TABLE(dlgColumn, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgColumn::OnChange)
    EVT_TEXT(XRCID("txtLength"),                    dlgColumn::OnChange)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgColumn::OnChange)
    EVT_TEXT(XRCID("txtDefault"),                   dlgColumn::OnChange)
    EVT_CHECKBOX(XRCID("chkNotNull"),               dlgColumn::OnChange)
    EVT_TEXT(XRCID("txtAttstattarget"),             dlgColumn::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgColumn::OnChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgColumn::OnSelChangeTyp)
END_EVENT_TABLE();


dlgColumn::dlgColumn(frmMain *frame, pgColumn *node, pgTable *parentNode)
: dlgTypeProperty(frame, wxT("dlgColumn"))
{
    SetIcon(wxIcon(column_xpm));
    column=node;
    table=parentNode;
    wxASSERT(!table || table->GetType() == PG_TABLE);

    objectType=PG_COLUMN;
}


pgObject *dlgColumn::GetObject()
{
    return column;
}


int dlgColumn::Go(bool modal)
{
    if (column)
    {
        // edit mode
        txtName->SetValue(column->GetName());
        if (column->GetLength() > 0)
            txtLength->SetValue(NumToStr(column->GetLength()));
        if (column->GetPrecision() >= 0)
            txtPrecision->SetValue(NumToStr(column->GetPrecision()));
        txtDefault->SetValue(column->GetDefault());
        chkNotNull->SetValue(column->GetNotNull());
        txtAttstattarget->SetValue(NumToStr(column->GetAttstattarget()));
        txtComment->SetValue(column->GetComment());


        cbDatatype->Append(column->GetRawTypename());
        AddType(wxT("?"), column->GetAttTypId(), column->GetRawTypename());

        pgSet *set=connection->ExecuteSet(
            wxT("SELECT tt.oid, tt.typname\n")
            wxT("  FROM pg_cast\n")
            wxT("  JOIN pg_type tt ON tt.oid=casttarget\n")
            wxT(" WHERE castsource=") + NumToStr(column->GetAttTypId()) + wxT("\n")
            wxT("   AND castfunc=0"));

        if (set)
        {
            while (!set->Eof())
            {
                cbDatatype->Append(set->GetVal(wxT("typname")));
                AddType(wxT("?"), set->GetOid(wxT("oid")), set->GetVal(wxT("typname")));
                set->MoveNext();
            }
        }
        if (cbDatatype->GetCount() <= 1)
            cbDatatype->Disable();

        cbDatatype->SetSelection(0);
        wxNotifyEvent ev;
        OnSelChangeTyp(ev);

        previousDefinition=GetDefinition();
        if (column->GetColNumber() < 0)
        {
            txtName->Disable();
            txtDefault->Disable();
            chkNotNull->Disable();
            txtLength->Disable();
            cbDatatype->Disable();
            txtAttstattarget->Disable();
        }
    }
    else
    {
        // create mode
        FillDatatype(cbDatatype);
        cbDatatype->Append(wxT("serial"));
        cbDatatype->Append(wxT("bigserial"));
        AddType(wxT(" "), 0, wxT("serial"));
        AddType(wxT(" "), 0, wxT("bigserial"));
    }
    return dlgTypeProperty::Go(modal);
}


wxString dlgColumn::GetSql()
{
    wxString sql;

    if (table)
    {
        if (column)
        {
            if (GetName() != column->GetName())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" RENAME ") + qtIdent(column->GetName())
                    +  wxT("  TO ") + qtIdent(GetName())
                    +  wxT(";\n");

            wxString sqlPart;
            if (cbDatatype->GetCount() > 1 && cbDatatype->GetValue() != column->GetRawTypename())
                sqlPart = wxT("atttypid=") + GetTypeOid(cbDatatype->GetSelection());


            if (!sqlPart.IsEmpty() || 
                (isVarLen && StrToLong(txtLength->GetValue()) != column->GetLength()) ||
                (isVarPrec && StrToLong(txtPrecision->GetValue()) != column->GetPrecision()))
            {
                long typmod = pgDatatype::GetTypmod(column->GetRawTypename(), txtLength->GetValue(), txtPrecision->GetValue());

                if (!sqlPart.IsEmpty())
                    sqlPart += wxT(", ");
                sqlPart += wxT("atttypmod=") + NumToStr(typmod);
            }
            if (!sqlPart.IsEmpty())
                sql += wxT("UPDATE pg_attribute\n")
                       wxT("   SET ") + sqlPart + wxT("\n")
                       wxT(" WHERE attrelid=") + table->GetOidStr() +
                       wxT(" AND attnum=") + NumToStr(column->GetColNumber()) + wxT(";\n");

            if (txtDefault->GetValue() != column->GetDefault())
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(column->GetName());
                if (txtDefault->GetValue().IsEmpty())
                    sql += wxT(" DROP DEFAULT");
                else
                    sql += wxT(" SET DEFAULT ") + txtDefault->GetValue();

                sql += wxT(";\n");
            }
            if (chkNotNull->GetValue() != column->GetNotNull())
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(column->GetName());
                if (chkNotNull->GetValue())
                    sql += wxT(" SET");
                else
                    sql += wxT(" DROP");

                sql += wxT(" NOT NULL;\n");
            }
            if (txtAttstattarget->GetValue() != NumToStr(column->GetAttstattarget()))
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(column->GetName());
                if (txtAttstattarget->GetValue().IsEmpty())
                    sql += wxT(" SET STATISTICS -1");
                else
                    sql += wxT(" SET STATISTICS ") + txtAttstattarget->GetValue();
                sql += wxT(";\n");
            }
        }
        else
        {
            sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                + wxT("\n   ADD COLUMN ") + qtIdent(GetName())
                + wxT(" ") + GetQuotedTypename(cbDatatype->GetSelection())
                + wxT(";\n");

            if (chkNotNull->GetValue())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ALTER COLUMN ") + qtIdent(GetName())
                    + wxT(" SET NOT NULL;\n");
            
            if (!txtDefault->GetValue().IsEmpty())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ALTER COLUMN ") + qtIdent(GetName())
                    + wxT(" SET DEFAULT ") + txtDefault->GetValue() 
                    + wxT(";\n");
            if (!txtAttstattarget->GetValue().IsEmpty())
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ALTER COLUMN ") + qtIdent(GetName())
                    + wxT(" SET STATISTICS ") + txtAttstattarget->GetValue()
                    + wxT(";\n");
            }
        }


        AppendComment(sql, wxT("COLUMN ") + table->GetQuotedFullIdentifier() 
                + wxT(".") + qtIdent(GetName()), column);
    }
    return sql;
}


wxString dlgColumn::GetDefinition()
{
    wxString sql, col;

    sql = GetQuotedTypename(cbDatatype->GetSelection());
    if (chkNotNull->GetValue())
        sql += wxT(" NOT NULL");

    AppendIfFilled(sql, wxT(" DEFAULT "), txtDefault->GetValue());

    return sql;
}


pgObject *dlgColumn::CreateObject(pgCollection *collection)
{
    pgObject *obj;
    obj=pgColumn::ReadObjects(collection, 0, 
        wxT("\n   AND attname=") + qtString(GetName()) +
        wxT("\n   AND relname=") + qtString(table->GetName()) +
        wxT("\n   AND relnamespace=") + table->GetSchema()->GetOidStr() +
        wxT("\n"));
    return obj;
}


void dlgColumn::OnSelChangeTyp(wxNotifyEvent &ev)
{
    CheckLenEnable();
    if (column && column->GetLength() <= 0)
    {
        isVarLen=false;
        isVarPrec=false;
    }
    txtLength->Enable(isVarLen);
    OnChange(ev);
}


void dlgColumn::OnChange(wxNotifyEvent &ev)
{
    long varlen=StrToLong(txtLength->GetValue()), 
         varprec=StrToLong(txtPrecision->GetValue());
    txtPrecision->Enable(isVarPrec && varlen > 0);

    if (column)
    {

        bool enable=true;
        EnableOK(enable);   // to get rid of old messages

        CheckValid(enable, !isVarLen || !txtLength->GetValue().IsEmpty() || varlen >= column->GetLength(), 
                _("New length must not be less than old length."));
        CheckValid(enable, !txtPrecision->IsEnabled() || varprec >= column->GetPrecision(), 
                _("New precision must not be less than old precision."));
        CheckValid(enable, !txtPrecision->IsEnabled() || varlen-varprec >= column->GetLength()-column->GetPrecision(), 
                _("New total digits must not be less than old total digits."));

        
        if (enable)
            enable = GetName() != column->GetName()
                    || txtDefault->GetValue() != column->GetDefault()
                    || txtComment->GetValue() != column->GetComment()
                    || chkNotNull->GetValue() != column->GetNotNull()
                    || (cbDatatype->GetCount() > 1 && cbDatatype->GetValue() != column->GetRawTypename())
                    || (isVarLen && varlen != column->GetLength())
                    || (isVarPrec && varprec != column->GetPrecision())
                    || txtAttstattarget->GetValue() != NumToStr(column->GetAttstattarget());
        EnableOK(enable);
    }
    else
    {

        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbDatatype->GetSelection() >= 0, _("Please select a datatype."));
        CheckValid(enable, !isVarLen || txtLength->GetValue().IsEmpty() 
            || (varlen >= minVarLen && varlen <= maxVarLen && NumToStr(varlen) == txtLength->GetValue()),
            _("Please specify valid length."));
        CheckValid(enable, !txtPrecision->IsEnabled() 
            || (varprec >= 0 && varprec <= varlen && NumToStr(varprec) == txtPrecision->GetValue()),
            _("Please specify valid numeric precision (0..") + NumToStr(varlen) + wxT(")."));

        EnableOK(enable);
    }
}


