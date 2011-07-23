//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgCheck.cpp - PostgreSQL Check Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "schema/pgTable.h"
#include "schema/pgCheck.h"
#include "dlg/dlgCheck.h"

#define txtWhere        CTRL_TEXT("txtWhere")
#define chkDontValidate CTRL_CHECKBOX("chkDontValidate")


BEGIN_EVENT_TABLE(dlgCheck, dlgProperty)
	EVT_TEXT(XRCID("txtWhere"),                 dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkDontValidate"),      dlgCheck::OnChangeValidate)
END_EVENT_TABLE();


dlgProperty *pgCheckFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgCheck(this, frame, (pgCheck *)node, (pgTable *)parent);
}


dlgCheck::dlgCheck(pgaFactory *f, frmMain *frame, pgCheck *node, pgTable *parentNode)
	: dlgProperty(f, frame, wxT("dlgCheck"))
{
	check = node;
	table = parentNode;
}

void dlgCheck::CheckChange()
{
	bool enable = true;
	if (check)
	{
		enable = txtComment->GetValue() != check->GetComment();
		if (connection->BackendMinimumVersion(9, 2) && !check->GetValid() && !chkDontValidate->GetValue())
			enable = true;
		EnableOK(enable);
	}
	else
	{
		txtComment->Enable(!GetName().IsEmpty());
		CheckValid(enable, !txtWhere->GetValue().IsEmpty(), _("Please specify condition."));
		EnableOK(enable);
	}
}


pgObject *dlgCheck::GetObject()
{
	return check;
}


pgObject *dlgCheck::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	if (name.IsEmpty())
		return 0;

	pgObject *obj = checkFactory.CreateObjects(collection, 0, wxT(
	                    "\n   AND conname=") + qtDbString(name) + wxT(
	                    "\n   AND relnamespace=") + table->GetSchema()->GetOidStr());
	return obj;
}


int dlgCheck::Go(bool modal)
{
	if (check)
	{
		// edit mode: view only
		txtName->Disable();

		txtWhere->SetValue(check->GetDefinition());
		txtWhere->Disable();

		if (connection->BackendMinimumVersion(9, 2))
			chkDontValidate->SetValue(!check->GetValid());
		else
			chkDontValidate->SetValue(true);
		chkDontValidate->Enable(connection->BackendMinimumVersion(9, 2) && !check->GetDefinition().IsEmpty() && !check->GetValid());
	}
	else
	{
		// create mode
		txtComment->Disable();
		if (!table)
		{
			cbClusterSet->Disable();
			cbClusterSet = 0;
		}
		chkDontValidate->Enable(connection->BackendMinimumVersion(9, 2));
	}

	return dlgProperty::Go(modal);
}


void dlgCheck::OnChangeValidate(wxCommandEvent &ev)
{
	CheckChange();
}


wxString dlgCheck::GetSql()
{
	wxString sql;
	wxString name = GetName();

	if (!check)
	{
		sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
		      + wxT("\n  ADD");
		AppendIfFilled(sql, wxT(" CONSTRAINT "), qtIdent(name));
		sql += wxT("\n  CHECK ") + GetDefinition()
		       + wxT(";\n");
	}
	else if (!chkDontValidate->GetValue())
	{
		sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
		      + wxT(" VALIDATE CONSTRAINT ") + qtIdent(name) + wxT(";\n");
	}

	if (!name.IsEmpty())
		AppendComment(sql, wxT("CONSTRAINT ") + qtIdent(name)
		              + wxT(" ON ") + table->GetQuotedFullIdentifier(), check);
	return sql;
}


wxString dlgCheck::GetDefinition()
{
	wxString sql;

	sql = wxT("(") + txtWhere->GetValue() + wxT(")");

	if (chkDontValidate->GetValue())
		sql += wxT(" NOT VALID");

	return sql;
}
