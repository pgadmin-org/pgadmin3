//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgCast.cpp - PostgreSQL Operator Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "dlgCast.h"
#include "pgCast.h"
#include "pgDatatype.h"

// Images
#include "images/cast.xpm"


// pointer to controls
#define cbSourceType        CTRL_COMBOBOX2("cbSourceType")
#define cbTargetType        CTRL_COMBOBOX2("cbTargetType")
#define cbFunction          CTRL_COMBOBOX("cbFunction")
#define chkImplicit         CTRL_CHECKBOX("chkImplicit")
#define stComment           CTRL_STATIC("stComment")



BEGIN_EVENT_TABLE(dlgCast, dlgTypeProperty)
    EVT_TEXT(XRCID("cbSourceType"),                 dlgCast::OnChangeTypeSrc)
    EVT_TEXT(XRCID("cbTargetType"),                 dlgCast::OnChangeTypeTrg)
END_EVENT_TABLE();


dlgCast::dlgCast(frmMain *frame, pgCast *node)
: dlgTypeProperty(frame, wxT("dlgCast"))
{
    SetIcon(wxIcon(cast_xpm));
    cast=node;

    txtName->Disable();
}


pgObject *dlgCast::GetObject()
{
    return cast;
}


int dlgCast::Go(bool modal)
{

    if (!connection->BackendMinimumVersion(7, 5))
        txtComment->Disable();

    if (cast)
    {
        // edit mode
        cbSourceType->Append(cast->GetSourceType());
        cbSourceType->SetSelection(0);
        cbSourceType->Disable();

        cbTargetType->Append(wxEmptyString);
        cbTargetType->Append(cast->GetTargetType());
        cbTargetType->SetSelection(1);
        cbTargetType->Disable();

        AddType(wxT(" "), cast->GetSourceTypeOid());
        AddType(wxT(" "), cast->GetTargetTypeOid());

        cbFunction->Append(cast->GetCastFunction());
        cbFunction->SetSelection(0);

        cbFunction->Disable();
        chkImplicit->Disable();
    }
    else
    {
        // create mode
        FillDatatype(cbSourceType, cbTargetType, false);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgCast::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgCast::ReadObjects(collection, 0,
         wxT(" WHERE castsource = ") + GetTypeOid(cbSourceType->GetGuessedSelection()) +
         wxT("\n   AND casttarget = ") + GetTypeOid(cbTargetType->GetGuessedSelection()));

    return obj;
}


void dlgCast::CheckChange()
{
    if (cast)
    {
        EnableOK(txtComment->GetValue() != cast->GetComment());
    }
    else
    {
        bool enable=true;
        CheckValid(enable, cbSourceType->GetGuessedSelection() > 0, _("Please specify source datatype."));
        CheckValid(enable, cbTargetType->GetGuessedSelection() > 0, _("Please select target datatype."));

        if (enable)
            txtName->SetValue(cbSourceType->GetValue() + wxT(" -> ") + cbTargetType->GetValue());
        else
            txtName->SetValue(wxEmptyString);

        EnableOK(enable);
    }
}


void dlgCast::OnChangeTypeSrc(wxCommandEvent &ev)
{
    cbSourceType->GuessSelection();
    OnChangeType(ev);
}

void dlgCast::OnChangeTypeTrg(wxCommandEvent &ev)
{
    cbTargetType->GuessSelection();
    OnChangeType(ev);
}


void dlgCast::OnChangeType(wxCommandEvent &ev)
{
    functions.Clear();

    if (cbSourceType->GetGuessedSelection() > 0 && cbTargetType->GetGuessedSelection() > 0)
    {
        functions.Add(wxEmptyString);
        cbFunction->Append(wxT(" "));

        wxString qry=
            wxT("SELECT proname, nspname\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE proargtypes[0] = ")
            +  GetTypeOid(cbSourceType->GetGuessedSelection())
            +  wxT("\n   AND proargtypes[1] = 0")
               wxT("\n   AND prorettype = ")
            +  GetTypeOid(cbTargetType->GetGuessedSelection());

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
    }

    CheckChange();
}


wxString dlgCast::GetSql()
{
    wxString sql;

    if (cast)
    {
        // edit mode
    }
    else
    {
        // create mode
        sql = wxT("CREATE CAST (") + cbSourceType->GetValue()
            + wxT(" AS ") + cbTargetType->GetValue()
            + wxT(")\n   ");
        if (cbFunction->GetSelection() > 0)
            sql += wxT("WITH FUNCTION ") + functions.Item(cbFunction->GetSelection())
                +  wxT("(") + cbSourceType->GetValue() + wxT(")");
        else
            sql += wxT("WITHOUT FUNCTION");

        if (chkImplicit->GetValue())
            sql += wxT("\n   AS IMPLICIT");

        sql += wxT(";\n");

    }
    AppendComment(sql, wxT("CAST (") + cbSourceType->GetValue()
                       + wxT(" AS ") + cbTargetType->GetValue() + wxT(")"), cast);

    return sql;
}
