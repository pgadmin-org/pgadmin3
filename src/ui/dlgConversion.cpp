//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgConversion.cpp - PostgreSQL Conversion Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "dlgConversion.h"
#include "pgConversion.h"
#include "pgSchema.h"

// Images
#include "images/conversion.xpm"


// pointer to controls
#define txtOwner            CTRL("txtOwner", wxTextCtrl)
#define cbSourceEncoding    CTRL("cbSourceEncoding", wxComboBox)
#define cbTargetEncoding    CTRL("cbTargetEncoding", wxComboBox)
#define cbFunction          CTRL("cbFunction", wxComboBox)
#define chkDefault          CTRL("chkDefault", wxCheckBox)



BEGIN_EVENT_TABLE(dlgConversion, dlgProperty)
    EVT_TEXT(XRCID("txtName"),              dlgConversion::OnChange)
    EVT_TEXT(XRCID("cbSourceEncoding"),     dlgConversion::OnChange)
    EVT_TEXT(XRCID("cbTargetEncoding"),     dlgConversion::OnChange)
    EVT_COMBOBOX(XRCID("cbFunction"),       dlgConversion::OnChange)
    EVT_TEXT(XRCID("txtComment"),           dlgConversion::OnChange)
END_EVENT_TABLE();


dlgConversion::dlgConversion(frmMain *frame, pgConversion *node, pgSchema *sch)
: dlgProperty(frame, wxT("dlgConversion"))
{
    SetIcon(wxIcon(conversion_xpm));
    conversion=node;
    schema=sch;

    txtOwner->Disable();
    txtOID->Disable();
}


pgObject *dlgConversion::GetObject()
{
    return conversion;
}


int dlgConversion::Go(bool modal)
{
    if (conversion)
    {
        // edit mode
        txtName->SetValue(conversion->GetName());
        txtOID->SetValue(NumToStr(conversion->GetOid()));
        txtOwner->SetValue(conversion->GetOwner());
        cbSourceEncoding->SetValue(conversion->GetForEncoding());
        cbTargetEncoding->SetValue(conversion->GetToEncoding());

        cbFunction->Append(conversion->GetProcNamespace()+wxT(".")+conversion->GetProc());
        cbFunction->SetSelection(0);

        if (!connection->BackendMinimumVersion(7, 4))
            txtName->Disable();
        chkDefault->SetValue(conversion->GetDefaultConversion());
        cbSourceEncoding->Disable();
        cbTargetEncoding->Disable();
        cbFunction->Disable();
        chkDefault->Disable();
    }
    else
    {
        // create mode
        wxString qry=
            wxT("SELECT proname, nspname\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace")
            wxT("\n WHERE prorettype = ") + NumToStr(PGOID_TYPE_VOID) +
            wxT("\n   AND proargtypes[0] = ") + NumToStr(PGOID_TYPE_INT4) +
            wxT("\n   AND proargtypes[1] = ") + NumToStr(PGOID_TYPE_INT4) +
            wxT("\n   AND proargtypes[2] = ") + NumToStr(PGOID_TYPE_CSTRING) +
            wxT("\n   AND proargtypes[3] = ") + NumToStr(PGOID_TYPE_CSTRING) +
            wxT("\n   AND proargtypes[4] = ") + NumToStr(PGOID_TYPE_INT4) +
            wxT("\n   AND proargtypes[5] = 0");

        pgSet *set=connection->ExecuteSet(qry);
        if (set)
        {
            while (!set->Eof())
            {
                wxString nsp=set->GetVal(wxT("nspname"));
                if (nsp == wxT("public") || nsp == wxT("pg_catalog"))
                    nsp=wxT("");
                else
                    nsp += wxT(".");

                functions.Add(qtIdent(set->GetVal(wxT("nspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("proname"))));
                cbFunction->Append(nsp + set->GetVal(wxT("proname")));

                set->MoveNext();
            }
            delete set;
        }
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgConversion::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgConversion::ReadObjects(collection, 0,
         wxT("\n WHERE conname = ") + qtString(GetName()) +
         wxT("\n   AND connamespace = ") + schema->GetOidStr());

    return obj;
}


void dlgConversion::OnChange(wxNotifyEvent &ev)
{
    if (conversion)
    {
        btnOK->Enable(txtName->GetValue() != conversion->GetName() 
            || txtComment->GetValue() != conversion->GetComment());
    }
    else
    {
        bool enable=true;
        CheckValid(enable, !GetName().IsEmpty(), _("Please specify name."));
        CheckValid(enable, !cbSourceEncoding->GetValue().IsEmpty(), _("Please specify source encoding."));
        CheckValid(enable, !cbTargetEncoding->GetValue().IsEmpty(), _("Please specify target encoding."));
        CheckValid(enable, cbFunction->GetSelection() >= 0, _("Please specify conversion function."));

        EnableOK(enable);
    }
}




wxString dlgConversion::GetSql()
{
    wxString sql;
    wxString name=GetName();

    if (conversion)
    {
        // edit mode
        if (txtName->GetValue() != conversion->GetName())
        {
            sql += wxT("ALTER CONVERSION ") + conversion->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + qtIdent(name)
                +  wxT(";\n");
        }
    }
    else
    {
        // create mode
        sql = wxT("CREATE ");
        if (chkDefault->GetValue())
            sql += wxT("DEFAULT ");
        sql += wxT("CONVERSION ") + schema->GetQuotedIdentifier() 
            + wxT(".") + qtIdent(name)
            + wxT("\n   FOR ") + qtString(cbSourceEncoding->GetValue())
            + wxT(" TO ") + qtString(cbTargetEncoding->GetValue())
            + wxT("\n   FROM ") + functions.Item(cbFunction->GetSelection())
            + wxT(";\n");
    }
    AppendComment(sql, wxT("CONVERSION"), schema, conversion);

    return sql;
}
