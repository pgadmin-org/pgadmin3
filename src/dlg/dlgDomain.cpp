//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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


// pointer to controls
#define chkNotNull          CTRL_CHECKBOX("chkNotNull")
#define txtDefault          CTRL_TEXT("txtDefault")
#define txtCheck            CTRL_TEXT("txtCheck")

BEGIN_EVENT_TABLE(dlgDomain, dlgTypeProperty)
    EVT_TEXT(XRCID("txtLength"),                    dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgDomain::OnSelChangeTyp)
    EVT_COMBOBOX(XRCID("cbDatatype"),               dlgDomain::OnSelChangeTyp)
    EVT_TEXT(XRCID("txLength"),                     dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtDefault"),                   dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkNotNull"),               dlgProperty::OnChange)
END_EVENT_TABLE();


dlgProperty *pgDomainFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgDomain(this, frame, (pgDomain*)node, (pgSchema*)parent);
}


dlgDomain::dlgDomain(pgaFactory *f, frmMain *frame, pgDomain *node, pgSchema *sch)
: dlgTypeProperty(f, frame, wxT("dlgDomain"))
{
    schema=sch;
    domain=node;

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

    pgObject *obj=domainFactory.CreateObjects(collection, 0, 
        wxT("   AND d.typname=") + qtDbString(name) + 
        wxT("\n   AND d.typnamespace=") + schema->GetOidStr() +
        wxT("\n"));
    return obj;
}


void dlgDomain::CheckChange()
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
        CheckValid(enable, cbDatatype->GetGuessedSelection() >=0, _("Please select a datatype."));
        CheckValid(enable, !isVarLen || txtLength->GetValue().IsEmpty()
            || (varlen >= minVarLen && varlen <= maxVarLen && NumToStr(varlen) == txtLength->GetValue()),
            _("Please specify valid length."));
        CheckValid(enable, !txtPrecision->IsEnabled() 
            || (varprec >= 0 && varprec <= varlen && NumToStr(varprec) == txtPrecision->GetValue()),
            _("Please specify valid numeric precision (0..") + NumToStr(varlen) + wxT(")."));

        EnableOK(enable);
    }
}



void dlgDomain::OnSelChangeTyp(wxCommandEvent &ev)
{
    if (!domain)
    {
        cbDatatype->GuessSelection(ev);
        CheckLenEnable();
        txtLength->Enable(isVarLen);
        CheckChange();
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
        AppendOwnerChange(sql, wxT("DOMAIN ") + qtIdent(name));
    }
    else
    {
        // create mode
        sql = wxT("CREATE DOMAIN ") + schema->GetQuotedPrefix() + qtIdent(name)
            + wxT("\n   AS ") + GetQuotedTypename(cbDatatype->GetGuessedSelection());
        
        AppendIfFilled(sql, wxT("\n   DEFAULT "), txtDefault->GetValue());
        if (chkNotNull->GetValue())
            sql += wxT("\n   NOT NULL");
        if (!txtCheck->GetValue().IsEmpty())
            sql += wxT("\n   CHECK (") + txtCheck->GetValue() + wxT(")");
        sql += wxT(";\n");

        AppendOwnerNew(sql, wxT("DOMAIN ") + qtIdent(name));
    }
    AppendComment(sql, wxT("DOMAIN"), schema, domain);

    return sql;
}

