//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgOperator.cpp - PostgreSQL Operator Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgOperator.h"
#include "schema/pgSchema.h"
#include "schema/pgOperator.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define cbLeftType          CTRL_COMBOBOX2("cbLeftType")
#define cbRightType         CTRL_COMBOBOX2("cbRightType")
#define cbProcedure         CTRL_COMBOBOX2("cbProcedure")
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
	EVT_TEXT(XRCID("cbLeftType"),               dlgOperator::OnChangeTypeLeft)
	EVT_COMBOBOX(XRCID("cbLeftType"),           dlgOperator::OnChangeTypeLeft)
	EVT_TEXT(XRCID("cbRightType"),              dlgOperator::OnChangeTypeRight)
	EVT_COMBOBOX(XRCID("cbRightType"),          dlgOperator::OnChangeTypeRight)
	EVT_TEXT(XRCID("cbProcedure"),              dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbProcedure"),          dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbLeftSort") ,              dlgOperator::OnChangeJoin)
	EVT_COMBOBOX(XRCID("cbLeftSort") ,          dlgOperator::OnChangeJoin)
	EVT_TEXT(XRCID("cbRightSort") ,             dlgOperator::OnChangeJoin)
	EVT_COMBOBOX(XRCID("cbRightSort") ,         dlgOperator::OnChangeJoin)
	EVT_TEXT(XRCID("cbLess") ,                  dlgOperator::OnChangeJoin)
	EVT_COMBOBOX(XRCID("cbLess") ,              dlgOperator::OnChangeJoin)
	EVT_TEXT(XRCID("cbGreater") ,               dlgOperator::OnChangeJoin)
	EVT_COMBOBOX(XRCID("cbGreater") ,           dlgOperator::OnChangeJoin)
END_EVENT_TABLE();



dlgProperty *pgOperatorFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgOperator(this, frame, (pgOperator *)node, (pgSchema *)parent);
}

dlgOperator::dlgOperator(pgaFactory *f, frmMain *frame, pgOperator *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgOperator"))
{
	schema = sch;
	oper = node;

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
		cbSchema->Enable(connection->BackendMinimumVersion(9, 1));

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

		if (!connection->BackendMinimumVersion(8, 3))
		{
			cbLeftSort->Append(oper->GetLeftSortOperator());
			cbLeftSort->SetSelection(0);

			cbRightSort->Append(oper->GetRightSortOperator());
			cbRightSort->SetSelection(0);

			cbLess->Append(oper->GetLessOperator());
			cbLess->SetSelection(0);

			cbGreater->Append(oper->GetGreaterOperator());
			cbGreater->SetSelection(0);
		}

		chkCanHash->SetValue(oper->GetHashJoins());
		chkCanMerge->SetValue(oper->GetMergeJoins());


		txtName->Disable();
		cbProcedure->Disable();
		cbLeftType->Disable();
		cbRightType->Disable();
		if (!connection->BackendMinimumVersion(8, 0))
			cbOwner->Disable();
	}
	else
	{
		// create mode
		wxArrayString incl;
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
		validator.SetIncludes(incl);
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
	pgObject *obj = operatorFactory.CreateObjects(collection, 0,
	                wxT("\n   AND op.oprname=") + qtDbString(GetName()) +
	                wxT("\n   AND op.oprnamespace=") + schema->GetOidStr() +
	                wxT("\n   AND op.oprleft = ") + GetTypeOid(cbLeftType->GetGuessedSelection()) +
	                wxT("\n   AND op.oprright = ") + GetTypeOid(cbRightType->GetGuessedSelection()));

	return obj;
}


void dlgOperator::CheckChange()
{
	if (oper)
	{
		EnableOK(txtComment->GetValue() != oper->GetComment()
		         || cbSchema->GetValue() != oper->GetSchema()->GetName()
		         || cbOwner->GetValue() != oper->GetOwner());
	}
	else
	{
		wxString name = GetName();
		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbLeftType->GetGuessedSelection() > 0 || cbRightType->GetGuessedSelection() > 0 , _("Please select left or right datatype."));
		CheckValid(enable, cbProcedure->GetGuessedSelection() >= 0, _("Please specify a procedure."));

		EnableOK(enable);
	}
}


void dlgOperator::OnChangeTypeLeft(wxCommandEvent &ev)
{
	cbLeftType->GuessSelection(ev);
	CheckChangeType();
}

void dlgOperator::OnChangeTypeRight(wxCommandEvent &ev)
{
	cbRightType->GuessSelection(ev);
	CheckChangeType();
}

void dlgOperator::CheckChangeType()
{
	bool binaryOp = cbLeftType->GetGuessedSelection() > 0 && cbRightType->GetGuessedSelection() > 0;

	cbRestrict->Enable(binaryOp);
	cbJoin->Enable(binaryOp);

	if (!connection->BackendMinimumVersion(8, 3))
	{
		cbLeftSort->Enable(binaryOp);
		cbRightSort->Enable(binaryOp);
		cbLess->Enable(binaryOp);
		cbGreater->Enable(binaryOp);
	}

	chkCanHash->Enable(binaryOp);
	chkCanMerge->Enable(binaryOp);

	procedures.Clear();

	cbProcedure->Clear();
	cbJoin->Clear();
	cbRestrict->Clear();
	cbCommutator->Clear();
	cbNegator->Clear();

	if (!connection->BackendMinimumVersion(8, 3))
	{
		cbLeftSort->Clear();
		cbRightSort->Clear();
		cbLess->Clear();
		cbGreater->Clear();
	}

	cbRestrict->Append(wxEmptyString);
	cbJoin->Append(wxEmptyString);
	if (cbRestrict->GetCurrentSelection() < 0)
		cbRestrict->SetSelection(0);
	if (cbJoin->GetCurrentSelection() < 0)
		cbJoin->SetSelection(0);


	if (cbLeftType->GetGuessedSelection() > 0 || cbRightType->GetGuessedSelection() > 0)
	{
		wxString qry =
		    wxT("SELECT proname, nspname\n")
		    wxT("  FROM pg_proc p\n")
		    wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
		    wxT(" WHERE pronargs = ");

		if (binaryOp)
			qry += wxT("2");
		else
			qry += wxT("1");

		qry += wxT("\n   AND proargtypes[0] = ");

		if (cbLeftType->GetGuessedSelection() > 0)
			qry += GetTypeOid(cbLeftType->GetGuessedSelection());

		if (binaryOp)
			qry += wxT("\n   AND proargtypes[1] = ");

		if (cbRightType->GetGuessedSelection() > 0)
			qry += GetTypeOid(cbRightType->GetGuessedSelection());


		pgSet *set = connection->ExecuteSet(qry);
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

		qry = wxT("SELECT oprname, nspname\n")
		      wxT("  FROM pg_operator o\n")
		      wxT("  JOIN pg_namespace n ON n.oid=oprnamespace\n");

		if (cbLeftType->GetGuessedSelection() > 0)
			qry += wxT(" WHERE oprleft = ") + GetTypeOid(cbLeftType->GetGuessedSelection());


		if (cbRightType->GetGuessedSelection() > 0)
		{
			if (binaryOp)
				qry += wxT("\n   AND oprright = ");
			else
				qry += wxT(" WHERE oprright = ");
			qry += GetTypeOid(cbRightType->GetGuessedSelection());
		}

		cbCommutator->Append(wxT(" "));
		cbNegator->Append(wxT(" "));
		if (!connection->BackendMinimumVersion(8, 3))
		{
			cbLeftSort->Append(wxT(" "));
			cbRightSort->Append(wxT(" "));
			cbLess->Append(wxT(" "));
			cbGreater->Append(wxT(" "));
		}

		set = connection->ExecuteSet(qry);
		if (set)
		{
			while (!set->Eof())
			{
				wxString opname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("oprname"));

				cbCommutator->Append(opname);
				cbNegator->Append(opname);
				if (binaryOp && !connection->BackendMinimumVersion(8, 3))
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

	CheckChange();
}


void dlgOperator::OnChangeJoin(wxCommandEvent &ev)
{
	bool implicitMerges = (cbLeftSort->GetCurrentSelection() > 0 || cbRightSort->GetCurrentSelection() > 0
	                       || cbLess->GetCurrentSelection() > 0 || cbGreater->GetCurrentSelection() > 0);

	if (implicitMerges)
		chkCanMerge->SetValue(true);
	chkCanMerge->Enable(!implicitMerges);
}



void dlgOperator::AppendFilledOperator(wxString &sql, const wxChar *txt, ctlComboBoxFix *cb)
{
	wxString op = cb->GetValue().Trim();
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

		AppendOwnerChange(sql, wxT("OPERATOR ") + name);
		AppendSchemaChange(sql, wxT("OPERATOR ") + name);
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + GetName()
		       + wxT("(") + oper->GetOperands() + wxT(")");
	}
	else
	{
		// create mode
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + GetName() + wxT("(");
		if (cbLeftType->GetGuessedSelection() > 0)
			name += GetQuotedTypename(cbLeftType->GetGuessedSelection());
		else
			name += wxT("NONE");
		name += wxT(", ");
		if (cbRightType->GetGuessedSelection() > 0)
			name += GetQuotedTypename(cbRightType->GetGuessedSelection());
		else
			name += wxT("NONE");
		name += wxT(")");


		sql = wxT("CREATE OPERATOR ") + qtIdent(cbSchema->GetValue()) + wxT(".") + GetName()
		      + wxT("(\n   PROCEDURE=") + procedures.Item(cbProcedure->GetGuessedSelection());

		AppendIfFilled(sql, wxT(",\n   LEFTARG="), GetQuotedTypename(cbLeftType->GetGuessedSelection()));
		AppendIfFilled(sql, wxT(",\n   RIGHTARG="), GetQuotedTypename(cbRightType->GetGuessedSelection()));
		AppendIfFilled(sql, wxT(",\n   COMMUTATOR="), cbCommutator->GetValue().Trim());
		AppendIfFilled(sql, wxT(",\n   NEGATOR="), cbNegator->GetValue().Trim());

		if (cbLeftType->GetGuessedSelection() > 0 && cbRightType->GetGuessedSelection() > 0)
		{
			if (cbRestrict->GetCurrentSelection() > 0)
				sql += wxT(",\n   RESTRICT=") + procedures.Item(cbRestrict->GetCurrentSelection() - 1);
			if (cbJoin->GetCurrentSelection() > 0)
				sql += wxT(",\n   JOIN=") + procedures.Item(cbJoin->GetCurrentSelection() - 1);

			if (!connection->BackendMinimumVersion(8, 3))
			{
				AppendFilledOperator(sql, wxT(",\n   SORT1="), cbLeftSort);
				AppendFilledOperator(sql, wxT(",\n   SORT2="), cbRightSort);
				AppendFilledOperator(sql, wxT(",\n   LTCMP="), cbLess);
				AppendFilledOperator(sql, wxT(",\n   GTCMP="), cbGreater);
			}

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
		AppendOwnerChange(sql, wxT("OPERATOR ") + name);
	}
	AppendComment(sql, wxT("OPERATOR ") + name, oper);

	return sql;
}
