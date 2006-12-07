//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgAggregate.cpp - PostgreSQL Operator Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgAggregate.h"
#include "pgSchema.h"
#include "pgAggregate.h"
#include "pgDatatype.h"

// Images


// pointer to controls
#define cbBaseType          CTRL_COMBOBOX2("cbBaseType")
#define cbStateType         CTRL_COMBOBOX2("cbStateType")
#define cbStateFunc         CTRL_COMBOBOX("cbStateFunc")
#define cbFinalFunc         CTRL_COMBOBOX("cbFinalFunc")
#define cbSortOp            CTRL_COMBOBOX("cbSortOp")
#define txtInitial          CTRL_TEXT("txtInitial")



BEGIN_EVENT_TABLE(dlgAggregate, dlgTypeProperty)
    EVT_TEXT(XRCID("cbBaseType"),                   dlgAggregate::OnChangeTypeBase)
    EVT_COMBOBOX(XRCID("cbBaseType"),               dlgAggregate::OnChangeType)
    EVT_TEXT(XRCID("cbStateType"),                  dlgAggregate::OnChangeTypeState)
    EVT_COMBOBOX(XRCID("cbStateType"),              dlgAggregate::OnChangeType)
    EVT_COMBOBOX(XRCID("cbStateFunc"),              dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbSortOp"),                 dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbStateFunc"),                  dlgProperty::OnChange)
END_EVENT_TABLE();


dlgProperty *pgAggregateFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgAggregate(this, frame, (pgAggregate*)node, (pgSchema*)parent);
}


dlgAggregate::dlgAggregate(pgaFactory *f, frmMain *frame, pgAggregate *node, pgSchema *sch)
: dlgTypeProperty(f, frame, wxT("dlgAggregate"))
{
    SetIcon(wxIcon(aggregateFactory.GetImage()));
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

    if (!connection->BackendMinimumVersion(8, 1))
        cbSortOp->Disable();

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

        cbSortOp->Append(aggregate->GetSortOp());
        cbSortOp->SetSelection(0);

        txtInitial->SetValue(aggregate->GetInitialCondition());

        if (!connection->BackendMinimumVersion(7, 4))
            txtName->Disable();
        cbBaseType->Disable();
        cbStateType->Disable();
        cbFinalFunc->Disable();
        cbSortOp->Disable();
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
    pgObject *obj=aggregateFactory.CreateObjects(collection, 0,
         wxT("\n   AND proname=") + qtDbString(GetName()) +
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
        CheckValid(enable, cbStateFunc->GetCurrentSelection() >= 0, _("Please specify state function."));

        EnableOK(enable);
    }
}


void dlgAggregate::OnChangeTypeBase(wxCommandEvent &ev)
{
    cbBaseType->GuessSelection(ev);
    OnChangeType(ev);
}

void dlgAggregate::OnChangeTypeState(wxCommandEvent &ev)
{
    cbStateType->GuessSelection(ev);
    OnChangeType(ev);
}

void dlgAggregate::OnChangeType(wxCommandEvent &ev)
{
    cbStateFunc->Clear();
    cbFinalFunc->Clear();
    cbSortOp->Clear();

    if (cbBaseType->GetGuessedSelection() >= 0 && cbStateType->GetGuessedSelection() >= 0)
    {
        wxString qry=
            wxT("SELECT proname, nspname, prorettype\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_type t ON t.oid=p.prorettype\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE COALESCE(proargtypes[2],0) = 0");


        pgSet *set=connection->ExecuteSet(qry +
            wxT("\n   AND prorettype = ") + GetTypeOid(cbStateType->GetGuessedSelection()+1) +
            wxT("\n   AND proargtypes[0] = ") + GetTypeOid(cbStateType->GetGuessedSelection()+1) +
            wxT("\n   AND (COALESCE(proargtypes[1],0) = 0 OR proargtypes[1]= ") 
            + GetTypeOid(cbBaseType->GetGuessedSelection()) + wxT(")"));

        if (set)
        {
            while (!set->Eof())
            {
                cbStateFunc->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")),
                                    database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));

                set->MoveNext();
            }
            delete set;
        }


        cbFinalFunc->Append(wxT(" "));

        set=connection->ExecuteSet(qry +
            wxT("\n   AND proargtypes[0] = ") + GetTypeOid(cbStateType->GetGuessedSelection()+1) +
            wxT("\n   AND COALESCE(proargtypes[1],0)= 0"));

        if (set)
        {
            while (!set->Eof())
            {
                cbFinalFunc->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")),
                                    database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));

                set->MoveNext();
            }
            delete set;
        }

        cbSortOp->Append(wxT(" "), wxEmptyString);

        set=connection->ExecuteSet(
            wxT("SELECT oprname, nspname\n")
            wxT("  FROM pg_operator op\n")
            wxT("  JOIN pg_namespace nsp on nsp.oid=oprnamespace\n")
            wxT(" WHERE oprleft = ") + GetTypeOid(cbBaseType->GetGuessedSelection()) + wxT("\n")
            wxT("   AND oprright = ") + GetTypeOid(cbBaseType->GetGuessedSelection()) + wxT("\n"));

        if (set)
        {
            while (!set->Eof())
            {
                wxString key = database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("oprname")));
                wxString txt = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("oprname"));
                cbSortOp->Append(txt, key);
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
        AppendNameChange(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(aggregate->GetName()) + 
							   wxT("(") + GetQuotedTypename(cbBaseType->GetGuessedSelection()) + wxT(")"));
        AppendOwnerChange(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(GetName()) + 
							   wxT("(") + GetQuotedTypename(cbBaseType->GetGuessedSelection()) + wxT(")"));
    }
    else
    {
        // create mode
        name=GetName();
        sql = wxT("CREATE AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name)
            + wxT("(\n   BASETYPE=") + GetQuotedTypename(cbBaseType->GetGuessedSelection())
            + wxT(",\n   SFUNC=") + cbStateFunc->GetStringKey()
            + wxT(", STYPE=") + GetQuotedTypename(cbStateType->GetGuessedSelection() +1); // skip "any" type

        if (cbFinalFunc->GetCurrentSelection() > 0)
        {
            sql += wxT(",\n   FINALFUNC=") 
                + cbFinalFunc->GetStringKey();
        }
        wxString initial=txtInitial->GetValue().Strip(wxString::both);
        if (!initial.IsEmpty())
            sql += wxT(",\n   INITCOND=") + qtDbString(initial);

        wxString opr=cbSortOp->GetStringKey();
        if (!opr.IsEmpty())
            sql += wxT(",\n   SORTOP=") + opr;

        sql += wxT(");\n");

        AppendOwnerNew(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name)+ 
			                wxT("(") + GetQuotedTypename(cbBaseType->GetGuessedSelection()) + wxT(")"));
    }
    AppendComment(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(GetName())
                  + wxT("(") + GetQuotedTypename(cbBaseType->GetGuessedSelection())
                  + wxT(")"), aggregate);

    return sql;
}
