//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgOperator.cpp - PostgreSQL Operator Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "dlgOperator.h"
#include "pgSchema.h"
#include "pgOperator.h"
#include "pgDatatype.h"

// Images
#include "images/operator.xpm"


// pointer to controls
#define txtOwner            CTRL_TEXT("txtOwner")
#define cbLeftType          CTRL_COMBOBOX("cbLeftType")
#define cbRightType         CTRL_COMBOBOX("cbRightType")
#define cbLeftType          CTRL_COMBOBOX("cbLeftType")
#define cbProcedure         CTRL_COMBOBOX("cbProcedure")
#define cbRestrict          CTRL_COMBOBOX("cbRestrict")
#define cbJoin              CTRL_COMBOBOX("cbJoin")
#define cbCommutator        CTRL_COMBOBOX("cbCommutator")
#define cbNegator           CTRL_COMBOBOX("cbNegator")
#define cbLeftSort          CTRL_COMBOBOX("cbLeftSort")
#define cbRightSort         CTRL_COMBOBOX("cbRightSort")
#define cbLess              CTRL_COMBOBOX("cbLess")
#define cbGreater           CTRL_COMBOBOX("cbGreater")
#define chkCanHash          CTRL_CHECKBOX("chkCanHash")
#define chkCanMerge         CTRL_CHECKBOX("chkCanMerge")


BEGIN_EVENT_TABLE(dlgOperator, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                  dlgOperator::OnChange)
    EVT_TEXT(XRCID("cbLeftType"),               dlgOperator::OnChangeTypeLeft)
    EVT_TEXT(XRCID("cbRightType"),              dlgOperator::OnChangeTypeRight)
    EVT_TEXT(XRCID("cbProcedure"),              dlgOperator::OnChange)
    EVT_TEXT(XRCID("cbLeftSort") ,              dlgOperator::OnChangeJoin)
    EVT_TEXT(XRCID("cbRightSort") ,             dlgOperator::OnChangeJoin)
    EVT_TEXT(XRCID("cbLess") ,                  dlgOperator::OnChangeJoin)
    EVT_TEXT(XRCID("cbGreater") ,               dlgOperator::OnChangeJoin)
    EVT_TEXT(XRCID("txtComment"),               dlgOperator::OnChange)
END_EVENT_TABLE();


dlgOperator::dlgOperator(frmMain *frame, pgOperator *node, pgSchema *sch)
: dlgTypeProperty(frame, wxT("dlgOperator"))
{
    SetIcon(wxIcon(operator_xpm));
    schema=sch;
    oper=node;

    txtOID->Disable();
    txtOwner->Disable();

    cbRestrict->Disable();
    cbJoin->Disable();
    cbCommutator->Disable();
    cbNegator->Disable();
    cbLeftSort->Disable();
    cbRightSort->Disable();
    cbLess->Disable();
    cbGreater->Disable();
    chkCanHash->Disable();
    chkCanMerge->Disable();
}


pgObject *dlgOperator::GetObject()
{
    return oper;
}


int dlgOperator::Go(bool modal)
{
    if (oper)
    {
        // edit mode
        txtName->SetValue(oper->GetName());
        txtOID->SetValue(NumToStr(oper->GetOid()));
        txtOwner->SetValue(oper->GetOwner());

        cbLeftType->Append(oper->GetLeftType());
        cbLeftType->SetSelection(0);

        cbRightType->Append(oper->GetRightType());
        cbRightType->SetSelection(0);

        cbProcedure->Append(oper->GetOperatorFunction());
        cbProcedure->SetSelection(0);

        AddType(wxT(" "), oper->GetLeftTypeOid());
        AddType(wxT(" "), oper->GetRightTypeOid());

        cbRestrict->Append(oper->GetRestrictFunction());
        cbRestrict->SetSelection(0);

        cbJoin->Append(oper->GetJoinFunction());
        cbJoin->SetSelection(0);

        cbCommutator->Append(oper->GetCommutator());
        cbCommutator->SetSelection(0);

        cbNegator->Append(oper->GetNegator());
        cbNegator->SetSelection(0);

        cbLeftSort->Append(oper->GetLeftSortOperator());
        cbLeftSort->SetSelection(0);

        cbRightSort->Append(oper->GetRightSortOperator());
        cbRightSort->SetSelection(0);

        cbLess->Append(oper->GetLessOperator());
        cbLess->SetSelection(0);

        cbGreater->Append(oper->GetGreaterOperator());
        cbGreater->SetSelection(0);

        chkCanHash->SetValue(oper->GetHashJoins());
        if (!oper->GetLeftSortOperator().IsNull() || !oper->GetRightSortOperator().IsNull() ||
            !oper->GetLessOperator().IsNull() || !oper->GetGreaterOperator().IsNull())
            chkCanMerge->SetValue(TRUE);


	    txtComment->SetValue(oper->GetComment());
        txtName->Disable();
        cbProcedure->Disable();
        cbLeftType->Disable();
        cbRightType->Disable();
    }
    else
    {
        // create mode
        wxStringList incl;
        incl.Add(wxT("+"));
        incl.Add(wxT("-"));
        incl.Add(wxT("*"));
        incl.Add(wxT("/"));
        incl.Add(wxT("<"));
        incl.Add(wxT(">"));
        incl.Add(wxT("="));
        incl.Add(wxT("~"));
        incl.Add(wxT("!"));
        incl.Add(wxT("@"));
        incl.Add(wxT("#"));
        incl.Add(wxT("%"));
        incl.Add(wxT("^"));
        incl.Add(wxT("&"));
        incl.Add(wxT("|"));
        incl.Add(wxT("`"));
        incl.Add(wxT("?"));
        incl.Add(wxT("$"));

        wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
        validator.SetIncludeList(incl);
        txtName->SetValidator(validator);

        AddType(wxT(" "), 0);
        cbLeftType->Append(wxT(" "));
        cbRightType->Append(wxT(" "));
        FillDatatype(cbLeftType, cbRightType, false);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgOperator::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgOperator::ReadObjects(collection, 0,
         wxT("\n   AND op.oprname=") + qtString(GetName()) +
         wxT("\n   AND op.oprnamespace=") + schema->GetOidStr() +
         wxT("\n   AND op.oprleft = ") + GetTypeOid(cbLeftType->GetSelection()) +
         wxT("\n   AND op.oprright = ") + GetTypeOid(cbRightType->GetSelection()));

    return obj;
}


void dlgOperator::OnChange(wxCommandEvent &ev)
{
    if (oper)
    {
        EnableOK(txtComment->GetValue() != oper->GetComment());
    }
    else
    {
        wxString name=GetName();
        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbLeftType->GetSelection()>0 || cbRightType->GetSelection() > 0 , _("Please select left or right datatype."));
        CheckValid(enable, cbProcedure->GetSelection() >= 0, _("Please specify a procedure."));

        EnableOK(enable);
    }
}


void dlgOperator::OnChangeTypeLeft(wxCommandEvent &ev)
{
    cbLeftType->GuessSelection();
    OnChangeType(ev);
}

void dlgOperator::OnChangeTypeRight(wxCommandEvent &ev)
{
    cbRightType->GuessSelection();
    OnChangeType(ev);
}

void dlgOperator::OnChangeType(wxCommandEvent &ev)
{
    bool binaryOp=cbLeftType->GetSelection() > 0 && cbRightType->GetSelection() > 0;

    cbRestrict->Enable(binaryOp);
    cbJoin->Enable(binaryOp);
    cbLeftSort->Enable(binaryOp);
    cbRightSort->Enable(binaryOp);
    cbLess->Enable(binaryOp);
    cbGreater->Enable(binaryOp);
    chkCanHash->Enable(binaryOp);
    chkCanMerge->Enable(binaryOp);

    procedures.Clear();

    cbProcedure->Clear();
    cbJoin->Clear();
    cbRestrict->Clear();
    cbCommutator->Clear();
    cbNegator->Clear();
    cbLeftSort->Clear();
    cbRightSort->Clear();
    cbLess->Clear();
    cbGreater->Clear();

    cbRestrict->Append(wxEmptyString);
    cbJoin->Append(wxEmptyString);
    if (cbRestrict->GetSelection() < 0)
        cbRestrict->SetSelection(0);
    if (cbJoin->GetSelection() < 0)
        cbJoin->SetSelection(0);


    if (cbLeftType->GetSelection() > 0 || cbRightType->GetSelection() > 0)
    {
        wxString qry=
            wxT("SELECT proname, nspname\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
            wxT(" WHERE proargtypes[0] = ");

        if (cbLeftType->GetSelection() > 0)
            qry += GetTypeOid(cbLeftType->GetSelection());

        if (binaryOp)
            qry += wxT("\n   AND proargtypes[1] = ");

        if (cbRightType->GetSelection() > 0)
            qry += GetTypeOid(cbRightType->GetSelection());

        pgSet *set=connection->ExecuteSet(qry);
        if (set)
        {
            while (!set->Eof())
            {
                procedures.Add(database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));
                wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
                cbProcedure->Append(procname);
                if (binaryOp)
                {
                    cbJoin->Append(procname);
                    cbRestrict->Append(procname);
                }

                set->MoveNext();
            }
            delete set;
        }

        qry= wxT("SELECT oprname, nspname\n")
             wxT("  FROM pg_operator o\n")
             wxT("  JOIN pg_namespace n ON n.oid=oprnamespace\n");

        if (cbLeftType->GetSelection() > 0)
            qry += wxT(" WHERE oprleft = ") + GetTypeOid(cbLeftType->GetSelection());


        if (cbRightType->GetSelection() > 0)
        {
            if (binaryOp)
                qry += wxT("\n   AND oprright = ");
            else
                qry += wxT(" WHERE oprright = ");
            qry += GetTypeOid(cbRightType->GetSelection());
        }

        cbCommutator->Append(wxT(" "));
        cbNegator->Append(wxT(" "));
        cbLeftSort->Append(wxT(" "));
        cbRightSort->Append(wxT(" "));
        cbLess->Append(wxT(" "));
        cbGreater->Append(wxT(" "));

        set=connection->ExecuteSet(qry);
        if (set)
        {
            while (!set->Eof())
            {
                wxString opname=database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("oprname"));

                cbCommutator->Append(opname);
                cbNegator->Append(opname);
                if (binaryOp)
                {
                    cbLeftSort->Append(opname);
                    cbRightSort->Append(opname);
                    cbLess->Append(opname);
                    cbGreater->Append(opname);
                }
                set->MoveNext();
            }
            delete set;
        }
    }

    OnChange(ev);
}


void dlgOperator::OnChangeJoin(wxCommandEvent &ev)
{
    bool implicitMerges = (cbLeftSort->GetSelection() > 0 || cbRightSort->GetSelection() > 0
               || cbLess->GetSelection() > 0 || cbGreater->GetSelection() > 0);

    if (implicitMerges)
        chkCanMerge->SetValue(true);
    chkCanMerge->Enable(!implicitMerges);
}



void dlgOperator::AppendFilledOperator(wxString &sql, wxChar *txt, ctlComboBox *cb)
{
    wxString op=cb->GetValue().Trim();
    if (!op.IsNull())
    {
		sql += txt;
        if (op.Find('.') > 0)
            sql += wxT("OPERATOR(") + op + wxT(")");
        else
            sql += op;
    }
}


wxString dlgOperator::GetSql()
{
    wxString sql, name;

    if (oper)
    {
        // edit mode
	name = oper->GetQuotedFullIdentifier()
	    + wxT("(") + oper->GetOperands() + wxT(")");
    }
    else
    {
        // create mode
        name = schema->GetQuotedPrefix() + GetName() + wxT("(");
	if (cbLeftType->GetSelection() > 0)
	    name += GetQuotedTypename(cbLeftType->GetSelection());
	else
	    name += wxT("NONE");
	name += wxT(", ");
	if (cbRightType->GetSelection() > 0)
	    name += GetQuotedTypename(cbRightType->GetSelection());
	else
	    name += wxT("NONE");
	name += wxT(")");


        sql = wxT("CREATE OPERATOR ") + schema->GetQuotedPrefix() + GetName()
            + wxT("(\n   PROCEDURE=") + procedures.Item(cbProcedure->GetSelection());
        
        AppendIfFilled(sql, wxT(",\n   LEFTARG="), GetQuotedTypename(cbLeftType->GetSelection()));
        AppendIfFilled(sql, wxT(",\n   RIGHTARG="), GetQuotedTypename(cbRightType->GetSelection()));
        AppendIfFilled(sql, wxT(",\n   COMMUTATOR="), cbCommutator->GetValue().Trim());
        AppendIfFilled(sql, wxT(",\n   NEGATOR="), cbNegator->GetValue().Trim());
        
        if (cbLeftType->GetSelection() > 0 && cbRightType->GetSelection() > 0)
        {
            if (cbRestrict->GetSelection() > 0)
                sql += wxT(",\n   RESTRICT=") + procedures.Item(cbRestrict->GetSelection()-1);
            if (cbJoin->GetSelection() > 0)
                sql += wxT(",\n   JOIN=") + procedures.Item(cbJoin->GetSelection()-1);

            AppendFilledOperator(sql, wxT(",\n   SORT1="), cbLeftSort);
            AppendFilledOperator(sql, wxT(",\n   SORT2="), cbRightSort);
            AppendFilledOperator(sql, wxT(",\n   LTCMP="), cbLess);
            AppendFilledOperator(sql, wxT(",\n   GTCMP="), cbGreater);

            if (chkCanMerge->GetValue() || chkCanHash->GetValue())
            {
                sql += wxT(",\n   ");
                if (chkCanHash->GetValue())
                {
                    if (chkCanMerge->GetValue())
                        sql += wxT("HASHES, MERGES");
                    else
                        sql += wxT("HASHES");
                }
                else if (chkCanMerge->GetValue())
                    sql += wxT("MERGES");
            }
        }
        sql += wxT(");\n");

    }
    AppendComment(sql, wxT("OPERATOR ") + name, oper);

    return sql;
}
