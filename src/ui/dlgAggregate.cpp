//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgAggregate.cpp - PostgreSQL Operator Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "dlgAggregate.h"
#include "pgSchema.h"
#include "pgAggregate.h"
#include "pgDatatype.h"

// Images
#include "images/aggregate.xpm"


// pointer to controls
#define txtOwner            CTRL("txtOwner", wxTextCtrl)
#define cbBaseType          CTRL("cbBaseType", wxComboBox)
#define cbStateType         CTRL("cbStateType", wxComboBox)
#define cbStateFunc         CTRL("cbStateFunc", wxComboBox)
#define cbFinalFunc         CTRL("cbFinalFunc", wxComboBox)
#define txtInitial          CTRL("txtInitial", wxTextCtrl)



BEGIN_EVENT_TABLE(dlgAggregate, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgAggregate::OnChange)
    EVT_COMBOBOX(XRCID("cbBaseType"),               dlgAggregate::OnChangeType)
    EVT_COMBOBOX(XRCID("cbStateType"),              dlgAggregate::OnChangeType)
    EVT_COMBOBOX(XRCID("cbStateFunc"),              dlgAggregate::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgAggregate::OnChange)
END_EVENT_TABLE();


dlgAggregate::dlgAggregate(frmMain *frame, pgAggregate *node, pgSchema *sch)
: dlgTypeProperty(frame, wxT("dlgAggregate"))
{
    SetIcon(wxIcon(aggregate_xpm));
    schema=sch;
    aggregate=node;

    txtOID->Disable();
    txtOwner->Disable();
}


pgObject *dlgAggregate::GetObject()
{
    return aggregate;
}


int dlgAggregate::Go(bool modal)
{
    if (aggregate)
    {
        // edit mode
        txtName->SetValue(aggregate->GetName());
        txtOID->SetValue(NumToStr(aggregate->GetOid()));
        txtOwner->SetValue(aggregate->GetOwner());
        cbBaseType->Append(aggregate->GetInputType());
        cbBaseType->SetSelection(0);
        cbStateType->Append(aggregate->GetStateType());
        cbStateType->SetSelection(0);

        cbStateFunc->Append(aggregate->GetStateFunction());
        cbStateFunc->SetSelection(0);
        cbFinalFunc->Append(aggregate->GetFinalFunction());
        cbFinalFunc->SetSelection(0);

        txtInitial->SetValue(aggregate->GetInitialCondition());

        cbBaseType->Disable();
        cbStateType->Disable();
        cbStateFunc->Disable();
        cbFinalFunc->Disable();
        txtInitial->Disable();
    }
    else
    {
        // create mode
        AddType(wxT(" "), PGOID_TYPE_ANY, wxT("any"));
        cbBaseType->Append(wxT("ANY"));
        FillDatatype(cbBaseType, cbStateType, false);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgAggregate::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgAggregate::ReadObjects(collection, 0,
         wxT("\n   AND proname=") + qtString(GetName()) +
         wxT("\n   AND pronamespace=") + schema->GetOidStr());

    return obj;
}


void dlgAggregate::OnChange(wxNotifyEvent &ev)
{
    if (aggregate)
    {
        btnOK->Enable(txtComment->GetValue() != aggregate->GetComment());
    }
    else
    {
        wxString name=GetName();
        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbBaseType->GetSelection() >=0, _("Please select base datatype."));
        CheckValid(enable, cbStateType->GetSelection() >=0, _("Please select state datatype."));
        CheckValid(enable, cbStateFunc->GetSelection() >= 0, _("Please specify state function."));

        EnableOK(enable);
    }
}


void dlgAggregate::OnChangeType(wxNotifyEvent &ev)
{
    procedures.Clear();
    cbStateFunc->Clear();
    cbFinalFunc->Clear();

    if (cbBaseType->GetSelection() >= 0 && cbStateType->GetSelection() >= 0)
    {
        wxString qry=
            wxT("SELECT proname, nspname, prorettype\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_type t ON t.oid=p.prorettype\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE proargtypes[2] = 0");


        pgSet *set=connection->ExecuteSet(qry +
            wxT("\n   AND prorettype = ") + GetTypeOid(cbStateType->GetSelection()+1) +
            wxT("\n   AND proargtypes[0] = ") + GetTypeOid(cbStateType->GetSelection()+1) +
            wxT("\n   AND (proargtypes[1]= 0 OR proargtypes[1]= ") 
            + GetTypeOid(cbBaseType->GetSelection()) + wxT(")"));

        if (set)
        {
            while (!set->Eof())
            {
                wxString nsp=set->GetVal(wxT("nspname"));
                if (nsp == wxT("public") || nsp == wxT("pg_catalog"))
                    nsp=wxT("");
                else
                    nsp += wxT(".");

                procedures.Add(qtIdent(set->GetVal(wxT("nspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("proname"))));
                cbStateFunc->Append(nsp + set->GetVal(wxT("proname")));

                set->MoveNext();
            }
            delete set;
        }


        procedures.Add(wxEmptyString);
        cbFinalFunc->Append(wxT(" "));

        set=connection->ExecuteSet(qry +
            wxT("\n   AND proargtypes[0] = ") + GetTypeOid(cbStateType->GetSelection()+1) +
            wxT("\n   AND proargtypes[1]= 0"));

        if (set)
        {
            while (!set->Eof())
            {
                wxString nsp=set->GetVal(wxT("nspname"));
                if (nsp == wxT("public") || nsp == wxT("pg_catalog"))
                    nsp=wxT("");
                else
                    nsp += wxT(".");

                procedures.Add(qtIdent(set->GetVal(wxT("nspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("proname"))));
                cbFinalFunc->Append(nsp + set->GetVal(wxT("proname")));

                set->MoveNext();
            }
            delete set;
        }
    }
    OnChange(ev);
}


wxString dlgAggregate::GetSql()
{
    wxString sql, name;
    name=GetName();

    if (aggregate)
    {
        // edit mode
    }
    else
    {
        // create mode
        sql = wxT("CREATE AGGREGATE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(name)
            + wxT("(\n   BASETYPE=") + GetQuotedTypename(cbBaseType->GetSelection())
            + wxT(",\n   SFUNC=") + procedures.Item(cbStateFunc->GetSelection())
            + wxT(", STYPE=") + GetQuotedTypename(cbStateType->GetSelection() +1); // skip "any" type

        if (cbFinalFunc->GetSelection() > 0)
        {
            sql += wxT(",\n   FINALFUNC=") 
                + procedures.Item(cbFinalFunc->GetSelection() + cbStateFunc->GetCount());
        }
        wxString initial=txtInitial->GetValue().Strip(wxString::both);
        if (!initial.IsEmpty())
            sql += wxT(",\n   INITCOND=") + qtString(initial);
        
        sql += wxT(");\n");
    }
    AppendComment(sql, wxT("AGGREGATE"), schema, aggregate);

    return sql;
}
