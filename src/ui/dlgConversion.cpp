//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#define cbSourceEncoding    CTRL_COMBOBOX("cbSourceEncoding")
#define cbTargetEncoding    CTRL_COMBOBOX("cbTargetEncoding")
#define cbFunction          CTRL_COMBOBOX("cbFunction")
#define chkDefault          CTRL_CHECKBOX("chkDefault")



BEGIN_EVENT_TABLE(dlgConversion, dlgProperty)
    EVT_TEXT(XRCID("cbSourceEncoding"),     dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbTargetEncoding"),     dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbFunction"),           dlgProperty::OnChange)
END_EVENT_TABLE();


dlgConversion::dlgConversion(frmMain *frame, pgConversion *node, pgSchema *sch)
: dlgProperty(frame, wxT("dlgConversion"))
{
    SetIcon(wxIcon(conversion_xpm));
    conversion=node;
    schema=sch;
}


pgObject *dlgConversion::GetObject()
{
    return conversion;
}


int dlgConversion::Go(bool modal)
{
    if (!connection->BackendMinimumVersion(7, 4))
        txtComment->Disable();

    if (!connection->BackendMinimumVersion(7, 5))
        cbOwner->Disable();

    if (conversion)
    {
        // edit mode
        cbSourceEncoding->Append(conversion->GetForEncoding());
        cbSourceEncoding->SetSelection(0);
        cbTargetEncoding->Append(conversion->GetToEncoding());
        cbTargetEncoding->SetSelection(0);

        cbFunction->Append(database->GetSchemaPrefix(conversion->GetProcNamespace()) + conversion->GetProc());
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
                functions.Add(database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));
                cbFunction->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")));

                set->MoveNext();
            }
            delete set;
        }

        long encNo=0;
        wxString encStr;
        do
        {
            encStr=connection->ExecuteScalar(
                wxT("SELECT pg_encoding_to_char(") + NumToStr(encNo) + wxT(")"));
            if (!encStr.IsEmpty())
            {
                cbSourceEncoding->Append(encStr);
                cbTargetEncoding->Append(encStr);
            }
            encNo++;
        }
        while (!encStr.IsEmpty());
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgConversion::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgConversion::ReadObjects(collection, 0,
         wxT("\n AND conname = ") + qtString(GetName()));

    return obj;
}


void dlgConversion::CheckChange()
{
    if (conversion)
    {
        EnableOK(txtName->GetValue() != conversion->GetName() 
            || txtComment->GetValue() != conversion->GetComment()
            || cbOwner->GetValue() != conversion->GetOwner());
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

        AppendNameChange(sql);
        AppendOwnerChange(sql, wxT("CONVERSION ") + schema->GetQuotedPrefix() + qtIdent(name));
    }
    else
    {
        // create mode
        sql = wxT("CREATE ");
        if (chkDefault->GetValue())
            sql += wxT("DEFAULT ");
        sql += wxT("CONVERSION ") + schema->GetQuotedPrefix() + qtIdent(name)
            + wxT("\n   FOR ") + qtString(cbSourceEncoding->GetValue())
            + wxT(" TO ") + qtString(cbTargetEncoding->GetValue())
            + wxT("\n   FROM ") + functions.Item(cbFunction->GetSelection())
            + wxT(";\n");

        AppendOwnerNew(sql, wxT("CONVERSION ") + schema->GetQuotedPrefix() + qtIdent(name));
    }
    AppendComment(sql, wxT("CONVERSION"), schema, conversion);

    return sql;
}
