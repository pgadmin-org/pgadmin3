//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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
#define txtOwner            CTRL("txtOwner", wxTextCtrl)
#define cbLeftType          CTRL("cbLeftType", wxComboBox)
#define cbRightType         CTRL("cbRightType", wxComboBox)
#define cbLeftType          CTRL("cbLeftType", wxComboBox)
#define cbProcedure         CTRL("cbProcedure", wxComboBox)
#define cbRestrict          CTRL("cbRestrict", wxComboBox)
#define cbJoin              CTRL("cbJoin", wxComboBox)
#define cbCommutator        CTRL("cbCommutator", wxComboBox)
#define cbNegator           CTRL("cbNegator", wxComboBox)
#define cbLeftSort          CTRL("cbLeftSort", wxComboBox)
#define cbRightSort         CTRL("cbRightSort", wxComboBox)
#define cbLess              CTRL("cbLess", wxComboBox)
#define cbGreater           CTRL("cbGreater", wxComboBox)
#define chkCanHash          CTRL("chkCanHash", wxCheckBox)
#define chkCanMerge         CTRL("chkCanMerge", wxCheckBox)



BEGIN_EVENT_TABLE(dlgOperator, dlgTypeProperty)
    EVT_TEXT(XRCID("txtName"),                  dlgOperator::OnChange)
    EVT_TEXT(XRCID("cbLeftType"),               dlgOperator::OnChangeType)
    EVT_TEXT(XRCID("cbRightType"),              dlgOperator::OnChangeType)
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
        cbRightType->Append(oper->GetRightType());
        cbRightType->SetSelection(1);

        AddType(wxT(" "), oper->GetLeftTypeOid());
        AddType(wxT(" "), oper->GetRightTypeOid());

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
         wxT("\n   AND op.oprname=") + GetName() +
         wxT("\n   AND op.oprnamespace=") + schema->GetOidStr() +
         wxT("\n   AND op.oprleft = ") + GetTypeOid(cbLeftType->GetSelection()) +
         wxT("\n   AND op.oprright = ") + GetTypeOid(cbRightType->GetSelection()));

    return obj;
}


void dlgOperator::OnChange(wxNotifyEvent &ev)
{
    if (oper)
    {
        btnOK->Enable(txtComment->GetValue() != oper->GetComment());
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


void dlgOperator::OnChangeType(wxNotifyEvent &ev)
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
                wxString nsp=set->GetVal(wxT("nspname"));
                if (nsp == wxT("public") || nsp == wxT("pg_catalog"))
                    nsp=wxT("");
                else
                    nsp += wxT(".");

                procedures.Add(qtIdent(set->GetVal(wxT("nspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("proname"))));
                wxString procname=nsp + set->GetVal(wxT("proname"));
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
                wxString nsp=set->GetVal(wxT("nspname"));
                if (nsp == wxT("public") || nsp == wxT("pg_catalog"))
                    nsp=wxT("");
                else
                    nsp += wxT(".");

                wxString opname=nsp + set->GetVal(wxT("oprname"));

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


void dlgOperator::OnChangeJoin(wxNotifyEvent &ev)
{
    bool implicitMerges = (cbLeftSort->GetSelection() > 0 || cbRightSort->GetSelection() > 0
               || cbLess->GetSelection() > 0 || cbGreater->GetSelection() > 0);

    if (implicitMerges)
        chkCanMerge->SetValue(true);
    chkCanMerge->Enable(!implicitMerges);
}


wxString dlgOperator::GetSql()
{
    wxString sql, name;
    name=GetName();

    if (oper)
    {
        // edit mode
    }
    else
    {
        // create mode
        sql = wxT("CREATE OPERATOR ") + schema->GetQuotedFullIdentifier() + wxT(".") + name
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
            AppendIfFilled(sql, wxT(",\n   SORT1="), cbLeftSort->GetValue().Trim());
            AppendIfFilled(sql, wxT(",\n   SORT2="), cbRightSort->GetValue().Trim());
            AppendIfFilled(sql, wxT(",\n   LTCMP="), cbLess->GetValue().Trim());
            AppendIfFilled(sql, wxT(",\n   GTCMP="), cbGreater->GetValue().Trim());

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
    AppendComment(sql, wxT("OPERATOR"), schema, oper);

    return sql;
}
