//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgColumns.cpp - PostgreSQL Columns Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgColumn.h"
#include "pgColumn.h"
#include "pgTable.h"

// Images
#include "images/column.xpm"


// pointer to controls
#define txtName         CTRL("txtName", wxTextCtrl)
#define cbDatatype      CTRL("cbDatatype", wxComboBox)
#define txtLength       CTRL("txtLength", wxTextCtrl)
#define txtPrecision    CTRL("txtPrecision", wxTextCtrl)
#define txtDefault      CTRL("txtDefault", wxTextCtrl)
#define chkNotNull      CTRL("chkNotNull", wxCheckBox)
#define txtComment      CTRL("txtComment", wxTextCtrl)



BEGIN_EVENT_TABLE(dlgColumn, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgColumn::OnChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgColumn::OnChange)
END_EVENT_TABLE();


dlgColumn::dlgColumn(frmMain *frame, pgColumn *node, pgTable *parentNode)
: dlgProperty(frame, wxT("dlgColumn"))
{
    SetIcon(wxIcon(column_xpm));
    column=node;
    table=parentNode;
    wxASSERT(!table || table->GetType() == PG_TABLE);

    isVarLen=false;
    isVarPrec=false;
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
        cbDatatype->SetValue(column->GetFullType());
        if (column->GetLength() >= 0)
            txtLength->SetValue(NumToStr(column->GetLength()));
        if (column->GetPrecision() >= 0)
            txtPrecision->SetValue(NumToStr(column->GetPrecision()));
        txtDefault->SetValue(column->GetDefault());
        chkNotNull->SetValue(column->GetNotNull());
        txtComment->SetValue(column->GetComment());

        cbDatatype->Disable();
        txtLength->Disable();
        txtPrecision->Disable();

        previousDefinition=GetDefinition();
    }
    else
    {
        // create mode
        btnOK->Disable();

        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT CASE WHEN COALESCE(t.typelem, 0) = 0 THEN t.typname ELSE b.typname || '[]' END AS typname, t.typlen, t.oid\n"
            "  FROM pg_type t\n"
            "  LEFT OUTER JOIN pg_type b ON t.typelem=b.oid\n"
            " WHERE t.typisdefined AND t.typtype IN ('b', 'd')\n"
            " ORDER BY t.typtype DESC, (t.typelem>0)::bool, COALESCE(b.typname, t.typname)"));

        if (set)
        {
            while (!set->Eof())
            {
                cbDatatype->Append(set->GetVal(0));
                typmods.Add(set->GetVal(1) + wxT(":") + set->GetVal(2));
                set->MoveNext();
            }
            delete set;
        }
        txtLength->SetValidator(numericValidator);
        txtPrecision->SetValidator(numericValidator);
    }
    return dlgProperty::Go(modal);
}


wxString dlgColumn::GetSql()
{
    wxString sql;

    if (table)
    {
        sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
            + wxT("\n   ADD ") + qtIdent(txtName->GetValue())
            + wxT(" ") + GetDefinition()
            + wxT(";\n");

            wxString cmt=txtComment->GetValue();
        if (!cmt.IsEmpty())
            sql += wxT("COMMENT ON COLUMN ") + table->GetQuotedFullIdentifier() 
                + wxT(".") + txtName->GetValue()
                + wxT("\n   IS ") + qtString(cmt)
                + wxT(";\n");
    }
    return sql;
}


wxString dlgColumn::GetName()
{
    return txtName->GetValue();
}


wxString dlgColumn::GetFullType()
{
    wxString sql;
    sql = cbDatatype->GetValue();
    if (isVarLen)
    {
        wxString len=txtLength->GetValue();
        if (StrToLong(len) > 0)
        {
            sql += wxT("(") + len;
            if (isVarPrec)
                sql += wxT(", ") + txtPrecision->GetValue();
            sql += wxT(")");
        }
    }

    return sql;
}


wxString dlgColumn::GetDefinition()
{
    wxString sql;

    sql = GetFullType();
    if (chkNotNull->GetValue())
        sql += wxT(" NOT NULL");

    AppendIfFilled(sql, wxT(" DEFAULT "), txtDefault->GetValue());

    return sql;
}


pgObject *dlgColumn::CreateObject(pgCollection *collection)
{
    return 0;
}



void dlgColumn::OnChange(wxNotifyEvent &ev)
{
    if (!column)
    {
        if (ev.GetId() == XRCID("cbDatatype"))
        {
            int sel=cbDatatype->GetSelection();
            if (sel >= 0)
            {
                wxString typmod=typmods.Item(sel);
                isVarLen = (StrToLong(typmod.BeforeFirst(':')) == -1);
                if (isVarLen)
                {
                    Oid oid=StrToLong(typmod.Mid(typmod.Find(':')+1));
                    switch (oid)
                    {
                        case 1231:
                        case 1700:
                            isVarPrec=true;
                            break;
                        default:
                            isVarPrec=false;
                            break;
                    }
                }
                else
                    isVarPrec=false;
            }
            txtLength->Enable(isVarLen);
            txtPrecision->Enable(isVarPrec);
        }

        wxString name=txtName->GetValue();
        bool enable=true;
        if (name.IsEmpty())
            enable=false;
        else if (cbDatatype->GetSelection() < 0)
            enable=false;
        else if (isVarLen && StrToLong(txtLength->GetValue()) < 0)
            enable=false;
        else if (isVarPrec && StrToLong(txtPrecision->GetValue()) < 0)
                enable=false;

        btnOK->Enable(enable);
    }
}


