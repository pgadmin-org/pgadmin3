//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "pgDatatype.h"

// Images
#include "images/domain.xpm"


// pointer to controls
#define cbOwner             CTRL("cbOwner", wxComboBox)
#define chkNotNull          CTRL("chkNotNull", wxCheckBox)
#define txtDefault          CTRL("txtDefault", wxTextCtrl)
#define txtCheck            CTRL("txtCheck", wxTextCtrl)

BEGIN_EVENT_TABLE(dlgDomain, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgDomain::OnChange)
    EVT_COMBOBOX(XRCID("cbOwner"),                  dlgDomain::OnChange)
    EVT_TEXT(XRCID("txtLength"),                    dlgDomain::OnChange)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgDomain::OnChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgDomain::OnSelChangeTyp)
    EVT_TEXT(XRCID("txtComment"),                   dlgDomain::OnChange)
    EVT_TEXT(XRCID("txLength"),                     dlgDomain::OnChange)
    EVT_TEXT(XRCID("txtDefault"),                   dlgDomain::OnChange)
    EVT_CHECKBOX(XRCID("chkNotNull"),               dlgDomain::OnChange)
END_EVENT_TABLE();


dlgDomain::dlgDomain(frmMain *frame, pgDomain *node, pgSchema *sch)
: dlgTypeProperty(frame, wxT("dlgDomain"))
{
    SetIcon(wxIcon(domain_xpm));
    schema=sch;
    domain=node;

    txtOID->Disable();
    txtLength->Disable();
    txtPrecision->Disable();
}


pgObject *dlgDomain::GetObject()
{
    return domain;
}


int dlgDomain::Go(bool modal)
{
    if (!domain)
        cbOwner->Append(wxT(""));

    pgSet *set=connection->ExecuteSet(wxT("SELECT usename FROM pg_user ORDER BY usename"));

    if (set)
    {
        while (!set->Eof())
        {
            cbOwner->Append(set->GetVal(0));
            set->MoveNext();
        }
        delete set;
    }

    if (domain)
    {
        // edit mode
        txtName->SetValue(domain->GetName());
        txtOID->SetValue(NumToStr((long)domain->GetOid()));
        cbDatatype->Append(domain->GetBasetype());
        AddType(wxT(" "), 0, domain->GetBasetype());
        cbDatatype->SetSelection(0);
        if (domain->GetLength() >= 0)
        {
            txtLength->SetValue(NumToStr(domain->GetLength()));
            if (domain->GetPrecision() >= 0)
                txtPrecision->SetValue(NumToStr(domain->GetPrecision()));
        }
        chkNotNull->SetValue(domain->GetNotNull());
        txtDefault->SetValue(domain->GetDefault());
        txtCheck->SetValue(domain->GetCheck());
        cbOwner->SetValue(domain->GetOwner());

        txtName->Disable();
        cbDatatype->Disable();
        txtCheck->Disable();

        if (!connection->BackendMinimumVersion(7, 4))
        {
            cbOwner->Disable();
            txtDefault->Disable();
            chkNotNull->Disable();
        }
    }
    else
    {
        // create mode
        if (!connection->BackendMinimumVersion(7, 4))
            txtCheck->Disable();
        FillDatatype(cbDatatype, false);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgDomain::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

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
        EnableOK(txtDefault->GetValue() != domain->GetDefault()
            || chkNotNull->GetValue() != domain->GetNotNull()
            || cbOwner->GetValue() != domain->GetOwner()
            || txtComment->GetValue() != domain->GetComment());
    }
    else
    {
        wxString name=GetName();
        long varlen=StrToLong(txtLength->GetValue()), 
             varprec=StrToLong(txtPrecision->GetValue());

        txtPrecision->Enable(isVarPrec && varlen > 0);

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbDatatype->GetSelection() >=0, _("Please select a datatype."));
        CheckValid(enable, !isVarLen || txtLength->GetValue().IsEmpty()
            || (varlen >= minVarLen && varlen <= maxVarLen && NumToStr(varlen) == txtLength->GetValue()),
            _("Please specify valid length."));
        CheckValid(enable, !txtPrecision->IsEnabled() 
            || (varprec >= 0 && varprec <= varlen && NumToStr(varprec) == txtPrecision->GetValue()),
            _("Please specify valid numeric precision (0..") + NumToStr(varlen) + wxT(")."));

        EnableOK(enable);
    }
}



void dlgDomain::OnSelChangeTyp(wxNotifyEvent &ev)
{
    if (!domain)
    {
        CheckLenEnable();
        txtLength->Enable(isVarLen);
        OnChange(ev);
    }
}


wxString dlgDomain::GetSql()
{
    wxString sql, name;
    name=GetName();

    if (domain)
    {
        // edit mode
        if (chkNotNull->GetValue() != domain->GetNotNull())
        {
            sql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier();
            if (chkNotNull->GetValue())
                sql += wxT(" SET NOT NULL;\n");
            else
                sql += wxT(" DROP NOT NULL;\n");
        }
        if (txtDefault->GetValue() != domain->GetDefault())
        {
            sql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier();
            if (txtDefault->GetValue().IsEmpty())
                sql += wxT(" DROP DEFAULT;\n");
            else
                sql += wxT(" SET DEFAULT ") + txtDefault->GetValue() + wxT(";\n");
        }
        if (cbOwner->GetValue() != domain->GetOwner())
        {
            sql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier()
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue())
                + wxT(";\n");
        }
    }
    else
    {
        // create mode
        sql = wxT("CREATE DOMAIN ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(name)
            + wxT("\n   AS ") + GetQuotedTypename(cbDatatype->GetSelection());
        
        AppendIfFilled(sql, wxT("\n   DEFAULT "), txtDefault->GetValue());
        if (chkNotNull->GetValue())
            sql += wxT("\n   NOT NULL");
        if (!txtCheck->GetValue().IsEmpty())
            sql += wxT("\n   CHECK (") + txtCheck->GetValue() + wxT(")");
        sql += wxT(";\n");

        if (cbOwner->GetSelection() > 0)
        {
            sql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier()
                +  wxT(" USER TO ") + qtIdent(cbOwner->GetValue())
                + wxT(";\n");
        }
    }
    AppendComment(sql, wxT("DOMAIN"), schema, domain);

    return sql;
}

