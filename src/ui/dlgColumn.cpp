//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#define txtDefault          CTRL_TEXT("txtDefault")
#define chkNotNull          CTRL_CHECKBOX("chkNotNull")
#define txtAttstattarget    CTRL_TEXT("txtAttstattarget")
#define cbSequence          CTRL_COMBOBOX("cbSequence")



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
    txtAttstattarget->SetValidator(numericValidator);
    cbSequence->Disable();
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

        cbSequence->Append(database->GetSchemaPrefix(column->GetSerialSchema()) + column->GetSerialSequence());
        cbSequence->SetSelection(0);

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

        if (table)
        {
            sequences.Add(wxEmptyString);
            cbSequence->Append(_("<new sequence>"));

            wxString sysRestr;
            if (!settings->GetShowSystemObjects())
                sysRestr = wxT("   AND ") + connection->SystemNamespaceRestriction(wxT("nspname"));

            pgSet *set=connection->ExecuteSet(
                wxT("SELECT nspname, relname\n")
                wxT("  FROM pg_class cl\n")
                wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
                wxT(" WHERE cl.relkind='S'\n")
                + sysRestr + wxT("\n")
                + wxT(" ORDER BY CASE WHEN ") + connection->SystemNamespaceRestriction(wxT("nspname")) 
                + wxT(" THEN 0 ELSE 1 END, nspname, relname"));

            if (set)
            {
                while (!set->Eof())
                {
                    sequences.Add(qtIdent(set->GetVal(wxT("nspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("relname"))));
                    cbSequence->Append(set->GetVal(wxT("nspname")) + wxT(".") + set->GetVal(wxT("relname")));
                    set->MoveNext();
                }
                delete set;
            }
        }
        else
            cbSequence->Append(wxT(" "));

        cbSequence->SetSelection(0);
    }
    return dlgTypeProperty::Go(modal);
}


wxString dlgColumn::GetSql()
{
    wxString sql;
    wxString name=GetName();

    bool isSerial = (cbDatatype->GetValue() == wxT("serial") || cbDatatype->GetValue() == wxT("bigserial"));

    if (table)
    {
        if (column)
        {
            if (name != column->GetName())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" RENAME ") + qtIdent(column->GetName())
                    +  wxT("  TO ") + qtIdent(name)
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
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(name);
                if (txtDefault->GetValue().IsEmpty())
                    sql += wxT(" DROP DEFAULT");
                else
                    sql += wxT(" SET DEFAULT ") + txtDefault->GetValue();

                sql += wxT(";\n");
            }
            if (chkNotNull->GetValue() != column->GetNotNull())
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(name);
                if (chkNotNull->GetValue())
                    sql += wxT(" SET");
                else
                    sql += wxT(" DROP");

                sql += wxT(" NOT NULL;\n");
            }
            if (txtAttstattarget->GetValue() != NumToStr(column->GetAttstattarget()))
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(name);
                if (txtAttstattarget->GetValue().IsEmpty())
                    sql += wxT(" SET STATISTICS -1");
                else
                    sql += wxT(" SET STATISTICS ") + txtAttstattarget->GetValue();
                sql += wxT(";\n");
            }
        }
        else
        {
            if (isSerial)
            {
                wxString typname;
                if (cbDatatype->GetValue() == wxT("serial"))
                    typname = wxT("int4");
                else
                    typname = wxT("int8");

                wxString sequence;
                bool newSequence = (cbSequence->GetSelection() <= 0);

                if (newSequence)
                {
                    sequence = qtIdent(table->GetSchema()->GetName()) + wxT(".") +
                               qtIdent(table->GetName() + wxT("_") + name + wxT("_seq"));

                    sql = wxT("CREATE SEQUENCE ") + sequence + wxT(";\n");
                }
                else
                    sequence=sequences.Item(cbSequence->GetSelection());

                sql +=wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ADD COLUMN ") + qtIdent(name)
                    + wxT(" ") + typname + wxT(";\n")
                    + wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ALTER COLUMN ") + qtIdent(name)
                    + wxT(" SET NOT NULL;\n")
                    + wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ALTER COLUMN ") + qtIdent(name)
                    + wxT(" SET DEFAULT nextval('") + sequence + wxT("'::text);\n");

                if (newSequence)
                {
                    sql += wxT("INSERT INTO pg_depend(classid, objid, objsubid, refclassid, refobjid, refobjsubid, deptype)\n")
                        wxT("SELECT cl.oid, seq.oid, 0, cl.oid, ") + table->GetOidStr() + wxT(", attnum, 'i'\n")
                        wxT("  FROM pg_class cl, pg_attribute, pg_class seq\n")
                        wxT("  JOIN pg_namespace sn ON sn.OID=seq.relnamespace\n")
                        wxT(" WHERE cl.relname='pg_class'\n")
                        wxT("  AND seq.relname=") + qtString(table->GetName() + wxT("_") + name + wxT("_seq")) + wxT("\n")
                        wxT("  AND sn.nspname=") + qtString(table->GetSchema()->GetName()) + wxT("\n")
                        wxT("  AND attrelid=") + table->GetOidStr() + wxT(" AND attname=") + qtString(name) + wxT(";\n");
                }
            }
            else
            {
                sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ADD COLUMN ") + qtIdent(name)
                    + wxT(" ") + GetQuotedTypename(cbDatatype->GetSelection())
                    + wxT(";\n");

                if (!isSerial && chkNotNull->GetValue())
                    sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                        + wxT("\n   ALTER COLUMN ") + qtIdent(name)
                        + wxT(" SET NOT NULL;\n");
            
                if (!isSerial && !txtDefault->GetValue().IsEmpty())
                    sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                        + wxT("\n   ALTER COLUMN ") + qtIdent(name)
                        + wxT(" SET DEFAULT ") + txtDefault->GetValue() 
                        + wxT(";\n");
            }
            if (!txtAttstattarget->GetValue().IsEmpty())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ALTER COLUMN ") + qtIdent(name)
                    + wxT(" SET STATISTICS ") + txtAttstattarget->GetValue()
                    + wxT(";\n");
        }


        AppendComment(sql, wxT("COLUMN ") + table->GetQuotedFullIdentifier() 
                + wxT(".") + qtIdent(name), column);
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
        wxT("\n   AND cl.relname=") + qtString(table->GetName()) +
        wxT("\n   AND cl.relnamespace=") + table->GetSchema()->GetOidStr() +
        wxT("\n"));
    return obj;
}


void dlgColumn::OnSelChangeTyp(wxCommandEvent &ev)
{
    CheckLenEnable();
    if (column && column->GetLength() <= 0)
    {
        isVarLen=false;
        isVarPrec=false;
    }
    txtLength->Enable(isVarLen);

    bool isSerial = (cbDatatype->GetValue() == wxT("serial") || cbDatatype->GetValue() == wxT("bigserial"));
    cbSequence->Enable(isSerial && table!=0);
    chkNotNull->Enable(!isSerial);
    txtDefault->Enable(!isSerial);

    OnChange(ev);
}


void dlgColumn::OnChange(wxCommandEvent &ev)
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


