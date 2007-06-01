//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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
#include "schema/pgSchema.h"
#include "schema/pgAggregate.h"
#include "schema/pgDatatype.h"

// Images


// pointer to controls
#define cbInputType         CTRL_COMBOBOX2("cbInputType")
#define lstInputTypes       CTRL_LISTCTRL("lstInputTypes")
#define cbStateType         CTRL_COMBOBOX2("cbStateType")
#define cbStateFunc         CTRL_COMBOBOX("cbStateFunc")
#define cbFinalFunc         CTRL_COMBOBOX("cbFinalFunc")
#define cbSortOp            CTRL_COMBOBOX("cbSortOp")
#define txtInitial          CTRL_TEXT("txtInitial")
#define btnAddType          CTRL_BUTTON("btnAddType")
#define btnRemoveType       CTRL_BUTTON("btnRemoveType")


BEGIN_EVENT_TABLE(dlgAggregate, dlgTypeProperty)
    EVT_TEXT(XRCID("cbInputType"),                  dlgAggregate::OnChangeTypeBase)
    EVT_TEXT(XRCID("cbStateType"),                  dlgAggregate::OnChangeTypeState)
    EVT_COMBOBOX(XRCID("cbStateType"),              dlgAggregate::OnChangeType)
    EVT_COMBOBOX(XRCID("cbStateFunc"),              dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbSortOp"),                 dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbStateFunc"),                  dlgProperty::OnChange)
    EVT_BUTTON(XRCID("btnAddType"),                 dlgAggregate::OnAddInputType)
    EVT_BUTTON(XRCID("btnRemoveType"),              dlgAggregate::OnRemoveInputType)
    EVT_LIST_ITEM_SELECTED(XRCID("lstInputTypes"),  dlgAggregate::OnSelectInputType)
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
    if (!connection->BackendMinimumVersion(8, 0))
        cbOwner->Disable();

    if (!connection->BackendMinimumVersion(8, 1))
        cbSortOp->Disable();

    lstInputTypes->InsertColumn(0, _("Input types"), wxLIST_FORMAT_LEFT, lstInputTypes->GetSize().x - 10);

    if (aggregate)
    {
        // edit mode
        for (unsigned int x = 0; x < aggregate->GetInputTypesArray().Count(); x++ )
        {
            lstInputTypes->InsertItem(x, aggregate->GetInputTypesArray()[x]);
            AddType(wxT(" "), 0, aggregate->GetInputTypesArray()[x]);
        }

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

        cbInputType->Disable();
        btnAddType->Disable();
        btnRemoveType->Disable();
        cbStateFunc->Disable();
        cbStateType->Disable();
        cbFinalFunc->Disable();
        cbSortOp->Disable();
        txtInitial->Disable();
    }
    else
    {
        // create mode
        AddType(wxT(" "), PGOID_TYPE_ANY, wxT("\"any\""));
        cbInputType->Append(wxT("ANY"));

        FillDatatype(cbInputType, cbStateType, false);
        btnRemoveType->Disable();
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
        // For pre 8.2 servers, we can only have one input type.
        if (!connection->BackendMinimumVersion(8, 2))
        {
            if (lstInputTypes->GetItemCount() >= 1)
                btnAddType->Disable();
            else
                btnAddType->Enable();
        }

        // Multi parameter aggregates cannot have sort ops.
        if (connection->BackendMinimumVersion(8, 2))
        {
            if (lstInputTypes->GetItemCount() > 1)
            {
                cbSortOp->SetValue(wxEmptyString);
                cbSortOp->Disable();
            }
            else
                cbSortOp->Enable();
        }

        wxString name=GetName();
        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbStateType->GetGuessedSelection() >=0, _("Please select state datatype."));
        CheckValid(enable, cbStateFunc->GetCurrentSelection() >= 0, _("Please specify state function."));

        EnableOK(enable);
    }
}


void dlgAggregate::OnChangeTypeBase(wxCommandEvent &ev)
{
    cbInputType->GuessSelection(ev);
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

    pgSet *set;
    wxString qry;

    // Get the possible state functions. They must return the state type, and take
    // input_types + 1 parameters, the first of which is of the state type.
    // If there are no input_types specified, assume "ANY" for a count(*) style aggregate.

    if (lstInputTypes->GetItemCount() > 0 && cbStateType->GetGuessedSelection() >= 0)
    {
        set=connection->ExecuteSet(
            wxT("SELECT proname, nspname, prorettype\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_type t ON t.oid=p.prorettype\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE prorettype = ") + GetTypeOid(cbStateType->GetGuessedSelection()+1) +
            wxT("\n   AND proisagg = FALSE")
            wxT("\n   AND proargtypes = '") + GetTypeOid(cbStateType->GetGuessedSelection()+1) + wxT(" ") + GetInputTypesOidList() + wxT("'"));

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

        // Get the possible final_func options. This may be nothing, or a 
        // function taking an argument of state_type
        cbFinalFunc->Append(wxT(" "));

        set=connection->ExecuteSet(
            wxT("SELECT proname, nspname, prorettype\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_type t ON t.oid=p.prorettype\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE proisagg = FALSE")
            wxT("\n   AND proargtypes = '") + GetTypeOid(cbStateType->GetGuessedSelection()+1) + wxT("'"));

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

        // Get the sort operators. This is only valid for a single arguement operator.
        cbSortOp->Append(wxT(" "), wxEmptyString);

        set=connection->ExecuteSet(
            wxT("SELECT oprname, nspname\n")
            wxT("  FROM pg_operator op\n")
            wxT("  JOIN pg_namespace nsp on nsp.oid=oprnamespace\n")
            wxT(" WHERE oprleft = ") + NumToStr(GetInputTypeOid(0)) + wxT("\n")
            wxT("   AND oprright = ") + NumToStr(GetInputTypeOid(0)) + wxT("\n"));

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
							   wxT("(") + GetInputTypesList() + wxT(")"));
        AppendOwnerChange(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(GetName()) + 
							   wxT("(") + GetInputTypesList() + wxT(")"));
    }
    else
    {
        // create mode
        name=GetName();
        if (connection->BackendMinimumVersion(8, 2))
        {
            sql = wxT("CREATE AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name)
                + wxT("(") + GetInputTypesList() + wxT(") (\n");
        }
        else
        {
            sql = wxT("CREATE AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name)
                + wxT("(\n   BASETYPE=") + GetInputTypesList() + wxT(",\n");
        }

        sql += wxT("   SFUNC=") + cbStateFunc->GetStringKey()
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

        sql += wxT("\n);\n");

        AppendOwnerNew(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(name)+ 
			                wxT("(") + GetInputTypesList() + wxT(")"));
    }
    AppendComment(sql, wxT("AGGREGATE ") + schema->GetQuotedPrefix() + qtIdent(GetName())
                  + wxT("(") + GetInputTypesList()
                  + wxT(")"), aggregate);

    return sql;
}

// Return the list of input types
wxString dlgAggregate::GetInputTypesList()
{
    wxString types;

    for (int i=0; i < lstInputTypes->GetItemCount(); i++)
    {
        if (i > 0)
            types += wxT(", ");

        if (lstInputTypes->GetItemText(i) == wxT("ANY") || lstInputTypes->GetItemText(i) == wxT("*"))
            return wxT("*");
        else
            types += qtTypeIdent(lstInputTypes->GetItemText(i));
    }
    return types;
}

void dlgAggregate::OnAddInputType(wxCommandEvent &ev)
{
    if (cbInputType->GetValue() != wxEmptyString)
    {
        if (cbInputType->GetValue() == wxT("ANY") && lstInputTypes->GetItemCount() > 0)
        {
            wxLogError(_("The ANY pseudo-datatype cannot be used in multi parameter aggregates."));
            return;
        }

        wxListItem itm;
        itm.SetMask(wxLIST_MASK_DATA | wxLIST_MASK_TEXT);

        itm.SetData(StrToLong(GetTypeOid(cbInputType->GetGuessedSelection())));
        itm.SetText(cbInputType->GetValue());

        lstInputTypes->InsertItem(itm);
        
        if (cbInputType->GetValue() == wxT("ANY"))
            btnAddType->Disable();
        
        OnChangeType(ev);
    }
}

void dlgAggregate::OnRemoveInputType(wxCommandEvent &ev)
{
    long pos=lstInputTypes->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);;
    if (pos >= 0)
    {
        lstInputTypes->DeleteItem(pos);

        OnChangeType(ev);
        btnRemoveType->Disable();

        // Re-enable the Add button, in case it was disabled because 
        // ANY was specified.
        btnAddType->Enable();
    }
}

void dlgAggregate::OnSelectInputType(wxListEvent &ev)
{
    if (!aggregate)
        btnRemoveType->Enable();
}

// Returnt the datatype OID for the given parameter number
long dlgAggregate::GetInputTypeOid(int param)
{
    wxListItem itm;
    itm.SetMask(wxLIST_MASK_DATA | wxLIST_MASK_TEXT);
    itm.SetId(param);

    lstInputTypes->GetItem(itm); 
    
    return itm.GetData();
}

// Return the list of datatype OIDs
wxString dlgAggregate::GetInputTypesOidList()
{
    wxString types;

    for (int i=0; i < lstInputTypes->GetItemCount(); i++)
    {
        if (i > 0)
            types += wxT(" ");

        types += NumToStr(GetInputTypeOid(i));
    }
    return types;
}