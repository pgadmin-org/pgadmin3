//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgDomain.cpp - PostgreSQL Domain Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "dlgDomain.h"
#include "pgSchema.h"
#include "pgDomain.h"

// Images
#include "images/domain.xpm"


// pointer to controls
#define txtOwner            CTRL("txtOwner", wxTextCtrl)
#define cbBasetype          CTRL("cbBasetype", wxComboBox)
#define txtLength           CTRL("txtLength", wxTextCtrl)
#define txtPrecision        CTRL("txtPrecision", wxTextCtrl)
#define chkNotNull          CTRL("chkNotNull", wxCheckBox)
#define txtDefault          CTRL("txtDefault", wxTextCtrl)


BEGIN_EVENT_TABLE(dlgDomain, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgDomain::OnChange)
    EVT_TEXT(XRCID("txtLength"),                    dlgDomain::OnChange)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgDomain::OnChange)
    EVT_TEXT(XRCID("cbBasetype"),                   dlgDomain::OnSelChangeTyp)
    EVT_TEXT(XRCID("txtComment"),                   dlgDomain::OnChange)
    EVT_TEXT(XRCID("txLength"),                     dlgDomain::OnChange)
END_EVENT_TABLE();


dlgDomain::dlgDomain(frmMain *frame, pgDomain *node, pgSchema *sch)
: dlgProperty(frame, wxT("dlgDomain"))
{
    SetIcon(wxIcon(domain_xpm));
    schema=sch;
    domain=node;
    isVarLen=false;
    isVarPrec=false;

    txtOID->Disable();
    txtOwner->Disable();
    txtLength->Disable();
    txtPrecision->Disable();
}


pgObject *dlgDomain::GetObject()
{
    return domain;
}


int dlgDomain::Go(bool modal)
{
    if (domain)
    {
        // edit mode
        txtName->SetValue(domain->GetName());
        txtOID->SetValue(NumToStr((long)domain->GetOid()));
        txtOwner->SetValue(domain->GetOwner());
        cbBasetype->Append(domain->GetBasetype());
        cbBasetype->SetSelection(0);
        if (domain->GetLength() >= 0)
        {
            txtLength->SetValue(NumToStr(domain->GetLength()));
            if (domain->GetPrecision() >= 0)
                txtPrecision->SetValue(NumToStr(domain->GetPrecision()));
        }
        chkNotNull->SetValue(domain->GetNotNull());

        txtName->Disable();
        cbBasetype->Disable();
    }
    else
    {
        // create mode
        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT CASE WHEN COALESCE(t.typelem, 0) = 0 OR SUBSTR(t.typname, 1,1) <> '_' THEN t.typname ELSE b.typname || '[]' END AS typname, t.typlen, t.oid, nspname\n"
            "  FROM pg_type t\n"
            "  JOIN pg_namespace nsp ON t.typnamespace=nsp.oid\n"
            "  LEFT OUTER JOIN pg_type b ON t.typelem=b.oid\n"
            " WHERE t.typisdefined AND t.typtype IN ('b')\n"
            " ORDER BY t.typtype DESC, (t.typelem>0)::bool, COALESCE(b.typname, t.typname)"));

        if (set)
        {
            while (!set->Eof())
            {
                wxString nsp=set->GetVal(wxT("nspname"));
                if (nsp == wxT("public") || nsp == wxT("pg_catalog"))
                    nsp = wxT("");
                else
                    nsp += wxT(".");

                typmods.Add(set->GetVal(1) + wxT(":") + set->GetVal(2));
                cbBasetype->Append(nsp + set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }
        txtLength->SetValidator(numericValidator);
        txtPrecision->SetValidator(numericValidator);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgDomain::CreateObject(pgCollection *collection)
{
    wxString name=txtName->GetValue();

    pgObject *obj=pgDomain::ReadObjects(collection, 0, 
        wxT("   AND d.typname=") + qtString(name) + 
        wxT("\n   AND d.typnamespace=") + schema->GetOidStr() +
        wxT("\n"));
    return obj;
}


void dlgDomain::OnChange(wxNotifyEvent &ev)
{
    if (domain)
    {
    }
    else
    {
        wxString name=txtName->GetValue();
        long varlen=StrToLong(txtLength->GetValue()), 
             varprec=StrToLong(txtPrecision->GetValue());

        txtPrecision->Enable(isVarPrec && varlen > 0);

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));
        CheckValid(enable, cbBasetype->GetSelection() >=0, wxT("Please select a datatype."));
        CheckValid(enable, isVarLen || txtLength->GetValue().IsEmpty() || varlen >0,
            wxT("Please specify valid length."));
        CheckValid(enable, !txtPrecision->IsEnabled() || (varprec >= 0 && varprec <= varlen),
            wxT("Please specify valid numeric precision (0..") + NumToStr(varlen) + wxT(")."));

        EnableOK(enable);
    }
}



void dlgDomain::OnSelChangeTyp(wxNotifyEvent &ev)
{
    int sel=cbBasetype->GetSelection();
    if (sel >= 0)
    {
        wxString typmod=typmods.Item(sel);
        isVarLen = (StrToLong(typmod.BeforeFirst(':')) == -1);
        if (isVarLen)
        {
            Oid oid=StrToLong(typmod.Mid(typmod.Find(':')+1));
            switch (oid)
            {
                case PGOID_TYPE_NUMERIC_ARRAY:
                case PGOID_TYPE_NUMERIC:
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
    OnChange(ev);
}


wxString dlgDomain::GetSql()
{
    wxString sql, name;
    name=txtName->GetValue();

    if (domain)
    {
        // edit mode
    }
    else
    {
        // create mode
        sql = wxT("CREATE DOMAIN ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(name)
            + wxT("\n   AS ");
        AppendQuoted(sql, cbBasetype->GetValue());

        if (StrToLong(txtLength->GetValue()) > 0)
        {
            sql += wxT("(") + txtLength->GetValue();
            if (isVarPrec)
            {
                wxString varprec=txtPrecision->GetValue();
                if (!varprec.IsEmpty())
                    sql += wxT(", ") + varprec;
            }
            sql += wxT(")");
        }

        AppendIfFilled(sql, wxT("\n   DEFAULT "), txtDefault->GetValue());
        sql += wxT(";\n");

    }
    AppendComment(sql, wxT("DOMAIN"), domain);

    return sql;
}

