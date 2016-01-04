//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "schema/pgObject.h"
#include "schema/pgCheck.h"
#include "dlg/dlgCheck.h"

#define txtWhere        CTRL_TEXT("txtWhere")
#define chkNoInherit    CTRL_CHECKBOX("chkNoInherit")
#define chkDontValidate CTRL_CHECKBOX("chkDontValidate")


BEGIN_EVENT_TABLE(dlgCheck, dlgProperty)
	EVT_TEXT(XRCID("txtWhere"),                 dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkDontValidate"),      dlgCheck::OnChangeValidate)
END_EVENT_TABLE();


dlgProperty *pgCheckFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgCheck(this, frame, (pgCheck *)node, parent);
}


dlgCheck::dlgCheck(pgaFactory *f, frmMain *frame, pgCheck *node, pgObject *parentNode)
	: dlgProperty(f, frame, wxT("dlgCheck"))
{
	check = node;
	object = parentNode;
}

void dlgCheck::CheckChange()
{
	bool enable = true;
	if (check)
	{
		enable = txtName->GetValue() != check->GetName() || txtComment->GetValue() != check->GetComment();
		if (connection->BackendMinimumVersion(9, 2) && !check->GetValid() && !chkDontValidate->GetValue())
			enable = true;
		EnableOK(enable);
	}
	else
	{
		// We don't allow changing the comment if the dialog is launched from dlgTable or dlgDomain
		// so we check IsModal()
		txtComment->Enable(!GetName().IsEmpty() && !IsModal() && object->GetTypeName().Upper() == wxT("TABLE"));
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
	                    "\n   AND relnamespace=") + object->GetSchema()->GetOidStr());
	return obj;
}


int dlgCheck::Go(bool modal)
{
	if (check)
	{
		// edit mode
		txtName->Enable(connection->BackendMinimumVersion(9, 2));
		txtComment->Enable(object->GetTypeName().Upper() == wxT("TABLE"));

		txtWhere->SetValue(check->GetDefinition());
		txtWhere->Disable();

		if (connection->BackendMinimumVersion(9, 2))
		{
			chkNoInherit->SetValue(check->GetNoInherit());
			chkDontValidate->SetValue(!check->GetValid());
		}
		else
			chkDontValidate->SetValue(true);
		chkDontValidate->Enable(connection->BackendMinimumVersion(9, 2) && !check->GetDefinition().IsEmpty() && !check->GetValid());
	}
	else
	{
		// create mode
		txtComment->Disable();
		if (!object)
		{
			cbClusterSet->Disable();
			cbClusterSet = 0;
		}
		chkDontValidate->Enable(connection->BackendMinimumVersion(9, 2));
	}

	chkNoInherit->Enable(connection->BackendMinimumVersion(9, 2) && !check);

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
		sql = wxT("ALTER ") + object->GetTypeName().Upper() + wxT(" ") + object->GetQuotedFullIdentifier()
		      + wxT("\n  ADD");
		if (name.Length() > 0)
		{
			sql += wxT(" CONSTRAINT ") + qtIdent(name) + wxT("\n ");
		}
		sql += wxT(" CHECK ");
		sql += GetDefinition();
		if (connection->BackendMinimumVersion(9, 2) && chkNoInherit->GetValue())
		{
			sql += wxT(" NO INHERIT");
		}
		sql += wxT(";\n");
	}
	else
	{
		if (check->GetName() != name)
		{
			sql = wxT("ALTER ") + object->GetTypeName().Upper() + wxT(" ") + object->GetQuotedFullIdentifier()
			      + wxT("\n  RENAME CONSTRAINT ") + qtIdent(check->GetName())
			      + wxT(" TO ") + qtIdent(name) + wxT(";\n");
		}
		if (connection->BackendMinimumVersion(9, 2) && !check->GetValid() && !chkDontValidate->GetValue())
		{
			sql += wxT("ALTER ") + object->GetTypeName().Upper() + wxT(" ") + object->GetQuotedFullIdentifier()
			       + wxT("\n  VALIDATE CONSTRAINT ") + qtIdent(name) + wxT(";\n");
		}
	}

	if (!name.IsEmpty())
		AppendComment(sql, wxT("CONSTRAINT ") + qtIdent(name)
		              + wxT(" ON ") + object->GetQuotedFullIdentifier(), check);
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
