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
#define txtOwner            CTRL("txtOwner", wxTextCtrl)
#define chkNotNull          CTRL("chkNotNull", wxCheckBox)
#define txtDefault          CTRL("txtDefault", wxTextCtrl)


BEGIN_EVENT_TABLE(dlgDomain, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgDomain::OnChange)
    EVT_TEXT(XRCID("txtLength"),                    dlgDomain::OnChange)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgDomain::OnChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgDomain::OnSelChangeTyp)
    EVT_TEXT(XRCID("txtComment"),                   dlgDomain::OnChange)
    EVT_TEXT(XRCID("txLength"),                     dlgDomain::OnChange)
END_EVENT_TABLE();


dlgDomain::dlgDomain(frmMain *frame, pgDomain *node, pgSchema *sch)
: dlgTypeProperty(frame, wxT("dlgDomain"))
{
    SetIcon(wxIcon(domain_xpm));
    schema=sch;
    domain=node;

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

        txtName->Disable();
        cbDatatype->Disable();
        txtDefault->Disable();
    }
    else
    {
        // create mode
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
        EnableOK(txtComment->GetValue() != domain->GetComment());
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
    }
    else
    {
        // create mode
        sql = wxT("CREATE DOMAIN ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(name)
            + wxT("\n   AS ") + GetQuotedTypename(cbDatatype->GetSelection());
        
        AppendIfFilled(sql, wxT("\n   DEFAULT "), txtDefault->GetValue());
        if (chkNotNull->GetValue())
            sql += wxT("\n   NOT NULL");
        sql += wxT(";\n");

    }
    AppendComment(sql, wxT("DOMAIN"), schema, domain);

    return sql;
}

