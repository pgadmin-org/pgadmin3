//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#define cbBaseType          CTRL_COMBOBOX2("cbBaseType")
#define cbStateType         CTRL_COMBOBOX2("cbStateType")
#define cbStateFunc         CTRL_COMBOBOX("cbStateFunc")
#define cbFinalFunc         CTRL_COMBOBOX("cbFinalFunc")
#define txtInitial          CTRL_TEXT("txtInitial")



BEGIN_EVENT_TABLE(dlgAggregate, dlgTypeProperty)
    EVT_TEXT(XRCID("cbBaseType"),                   dlgAggregate::OnChangeTypeBase)
    EVT_TEXT(XRCID("cbStateType"),                  dlgAggregate::OnChangeTypeState)
    EVT_TEXT(XRCID("cbStateFunc"),                  dlgProperty::OnChange)
END_EVENT_TABLE();


dlgAggregate::dlgAggregate(frmMain *frame, pgAggregate *node, pgSchema *sch)
: dlgTypeProperty(frame, wxT("dlgAggregate"))
{
    SetIcon(wxIcon(aggregate_xpm));
    schema=sch;
    aggregate=node;
}


pgObject *dlgAggregate::GetObject()
{
    return aggregate;
}


int dlgAggregate::Go(bool modal)
{
    if (!connection->BackendMinimumVersion(7, 5))
        cbOwner->Disable();

    if (aggregate)
    {
        // edit mode
        cbBaseType->Append(aggregate->GetInputType());
        cbBaseType->SetSelection(0);
        AddType(wxT(" "), 0, aggregate->GetInputType());
        cbStateType->Append(aggregate->GetStateType());
        cbStateType->SetSelection(0);

        cbStateFunc->Append(aggregate->GetStateFunction());
        cbStateFunc->SetSelection(0);
        cbFinalFunc->Append(aggregate->GetFinalFunction());
        cbFinalFunc->SetSelection(0);

        txtInitial->SetValue(aggregate->GetInitialCondition());

        if (!connection->BackendMinimumVersion(7, 4))
            txtName->Disable();
        cbBaseType->Disable();
        cbStateType->Disable();
        cbStateFunc->Disable();
        cbFinalFunc->Disable();
        txtInitial->Disable();
    }
    else
    {
        // create mode
        AddType(wxT(" "), PGOID_TYPE_ANY, wxT("\"any\""));
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


void dlgAggregate::CheckChange()
{
    if (aggregate)
    {
        EnableOK(GetName() != aggregate->GetName() 
            || txtComment->GetValue() != aggregate->GetComment()
            || cbOwner->GetValue() != aggregate->GetOwner());
    }
    else
    {
        wxString name=GetName();
        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbBaseType->GetGuessedSelection() >=0, _("Please select base datatype."));
        CheckValid(enable, cbStateType->GetGuessedSelection() >=0, _("Please select state datatype."));
        CheckValid(enable, cbStateFunc->GetSelection() >= 0, _("Please specify state function."));

        EnableOK(enable);
    }
}


void dlgAggregate::OnChangeTypeBase(wxCommandEvent &ev)
{
    cbBaseType->GuessSelection();
    CheckChange();
}

void dlgAggregate::OnChangeTypeState(wxCommandEvent &ev)
{
    cbStateType->GuessSelection();
    CheckChange();
}

void dlgAggregate::OnChangeType(wxCommandEvent &ev)
{
    procedures.Clear();
    cbStateFunc->Clear();
    cbFinalFunc->Clear();

    if (cbBaseType->GetGuessedSelection() >= 0 && cbStateType->GetGuessedSelection() >= 0)
    {
        wxString qry=
            wxT("SELECT proname, nspname, prorettype\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_type t ON t.oid=p.prorettype\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE proargtypes[2] = 0");


        pgSet *set=connection->ExecuteSet(qry +
            wxT("\n   AND prorettype = ") + GetTypeOid(cbStateType->GetGuessedSelection()+1) +
            wxT("\n   AND proargtypes[0] = ") + GetTypeOid(cbStateType->GetGuessedSelection()+1) +
            wxT("\n   AND (proargtypes[1]= 0 OR proargtypes[1]= ") 
            + GetTypeOid(cbBaseType->GetGuessedSelection()) + wxT(")"));

        if (set)
        {
            while (!set->Eof())
            {
                procedures.Add(database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));
                cbStateFunc->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")));

                set->MoveNext();
            }
            delete set;
        }


        procedures.Add(wxEmptyString);
        cbFinalFunc->Append(wxT(" "));

        set=connection->ExecuteSet(qry +
            wxT("\n   AND proargtypes[0] = ") + GetTypeOid(cbStateType->GetGuessedSelection()+1) +
            wxT("\n   AND proargtypes[1]= 0"));

        if (set)
        {
            while (!set->Eof())
            {
                procedures.Add(database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));
                cbFinalFunc->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")));

                set->MoveNext();
            }
            delete set;
        }
    }
    CheckChange();
}


wxString dlgAggregate::GetSql()
{
    wxString sql, name;

    if (aggregate)
    {
        // edit mode
        AppendNameChange(sql);
        AppendOwnerChange(sql);
    }
    else
    {
        // create mode
        name=GetName();
        sql = wxT("CREATE AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name)
            + wxT("(\n   BASETYPE=") + GetQuotedTypename(cbBaseType->GetGuessedSelection())
            + wxT(",\n   SFUNC=") + procedures.Item(cbStateFunc->GetSelection())
            + wxT(", STYPE=") + GetQuotedTypename(cbStateType->GetGuessedSelection() +1); // skip "any" type

        if (cbFinalFunc->GetSelection() > 0)
        {
            sql += wxT(",\n   FINALFUNC=") 
                + procedures.Item(cbFinalFunc->GetSelection() + cbStateFunc->GetCount());
        }
        wxString initial=txtInitial->GetValue().Strip(wxString::both);
        if (!initial.IsEmpty())
            sql += wxT(",\n   INITCOND=") + qtString(initial);
        
        sql += wxT(");\n");

        AppendOwnerNew(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name));
    }
    AppendComment(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name)
                  + wxT("(") + GetQuotedTypename(cbBaseType->GetGuessedSelection())
                  +wxT(")"), aggregate);

    return sql;
}
