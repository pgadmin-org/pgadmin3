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



BEGIN_EVENT_TABLE(dlgColumn, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgColumn::OnChange)
    EVT_TEXT(XRCID("txtLength"),                    dlgColumn::OnChange)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgColumn::OnChange)
    EVT_TEXT(XRCID("Default"),                      dlgColumn::OnChange)
    EVT_CHECKBOX(XRCID("chkNotNull"),               dlgColumn::OnChange)
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
        cbDatatype->Append(column->GetVarTypename());
        cbDatatype->SetValue(column->GetVarTypename());
        types.Add(column->GetVarTypename());
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
        FillDatatype(cbDatatype);
        cbDatatype->Append(wxT("serial"));
    }
    return dlgTypeProperty::Go(modal);
}


wxString dlgColumn::GetSql()
{
    wxString sql;

    if (table)
    {
        sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
            + wxT("\n   ADD ") + qtIdent(GetName())
            + wxT(" ") + GetDefinition()
            + wxT(";\n");

        AppendComment(sql, wxT("COLUMN ") + table->GetQuotedFullIdentifier() 
                + wxT(".") + qtIdent(GetName()), column);
    }
    return sql;
}


wxString dlgColumn::GetDefinition()
{
    wxString sql;

    sql = GetQuotedTypename();
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
    if (!column)
    {
        CheckLenEnable();
        txtLength->Enable(isVarLen);
        OnChange(ev);
    }
}


void dlgColumn::OnChange(wxNotifyEvent &ev)
{
    if (!column)
    {
        long varlen=StrToLong(txtLength->GetValue()), 
             varprec=StrToLong(txtPrecision->GetValue());
        txtPrecision->Enable(isVarPrec && varlen > 0);

        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbDatatype->GetSelection() >= 0, _("Please select a datatype."));
        CheckValid(enable, isVarLen || txtLength->GetValue().IsEmpty() || varlen >0,
            _("Please specify valid length."));
        CheckValid(enable, !txtPrecision->IsEnabled() || (varprec >= 0 && varprec <= varlen),
            _("Please specify valid numeric precision (0..") + NumToStr(varlen) + wxT(")."));

        EnableOK(enable);
    }
}


