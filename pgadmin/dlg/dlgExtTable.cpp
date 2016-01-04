//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgExtTable.cpp - Greenplum Externtal Table Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "ctl/ctlSQLBox.h"
#include "dlg/dlgExtTable.h"
#include "schema/gpExtTable.h"
#include "schema/pgSchema.h"



// pointer to controls
#define pnlDefinition   CTRL_PANEL("pnlDefinition")
#define txtSqlBox       CTRL_SQLBOX("txtSqlBox")



BEGIN_EVENT_TABLE(dlgExtTable, dlgSecurityProperty)
	EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgProperty::OnChangeStc)
END_EVENT_TABLE();


dlgProperty *gpExtTableFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgExtTable(this, frame, (gpExtTable *)node, (pgSchema *)parent);
}

dlgExtTable::dlgExtTable(pgaFactory *f, frmMain *frame, gpExtTable *node, pgSchema *sch)
	: dlgSecurityProperty(f, frame, node, wxT("dlgExtTable"), wxT("SELECT"), "r")
{
	schema = sch;
	extTable = node;

}


pgObject *dlgExtTable::GetObject()
{
	return extTable;
}


int dlgExtTable::Go(bool modal)
{
	int returncode;

	if (extTable)
	{
		// edit mode

		// TODO:  Make this more like dlgTable, so that it is easier to use.
		// Right now, this is just dummy code until that code is written.
		txtSqlBox->SetText(wxT("(") + extTable->GetSql(NULL).AfterFirst('('));
		oldDefinition = txtSqlBox->GetText();
		txtSqlBox->Enable(false);
	}
	else
	{
		// create mode
	}

	returncode = dlgSecurityProperty::Go(modal);

	// This fixes a UI glitch on MacOS X and Windows
	// Because of the new layout code, the Privileges pane don't size itself properly
	SetSize(GetSize().GetWidth() + 1, GetSize().GetHeight());
	SetSize(GetSize().GetWidth() - 1, GetSize().GetHeight());

	return returncode;
}


pgObject *dlgExtTable::CreateObject(pgCollection *collection)
{
	pgObject *obj = extTableFactory.CreateObjects(collection, 0,
	                wxT("\n   AND c.relname=") + qtDbString(txtName->GetValue()) +
	                wxT("\n   AND c.relnamespace=") + schema->GetOidStr());
	return obj;
}


void dlgExtTable::CheckChange()
{
	wxString name = GetName();
	if(!name.IsEmpty())
	{
		if (extTable)
			EnableOK(txtComment->GetValue() != extTable->GetComment()
			         || txtSqlBox->GetText() != oldDefinition
			         || cbOwner->GetValue() != extTable->GetOwner()
			         || name != extTable->GetName());
		else
			EnableOK(!txtComment->GetValue().IsEmpty()
			         || !txtSqlBox->GetText().IsEmpty()
			         || !cbOwner->GetValue().IsEmpty());
	}
	else
	{
		bool enable = true;

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, txtSqlBox->GetText().Length() > 0 , _("Please enter external table definition."));

		EnableOK(enable);
	}
}


wxString dlgExtTable::GetSql()
{
	wxString sql, name = GetName();


	if (extTable)
	{
		// edit mode

		if (name != extTable->GetName())
		{
			sql += wxT("ALTER TABLE ") + extTable->GetQuotedFullIdentifier()
			       +  wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
		}
	}

	if (!extTable || txtSqlBox->GetText() != oldDefinition)
	{
		sql += wxT("CREATE EXTERNAL TABLE ") + schema->GetQuotedPrefix() + qtIdent(name) + wxT("\n")
		       + txtSqlBox->GetText()
		       + wxT(";\n");
	}

	if (extTable)
		AppendOwnerChange(sql, wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));
	else
		AppendOwnerNew(sql, wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));


	sql +=  GetGrant(wxT("r"), wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));

	AppendComment(sql, wxT("TABLE"), schema, extTable);
	return sql;
}

bool dlgExtTable::IsUpToDate()
{
	if (extTable && !extTable->IsUpToDate())
		return false;
	else
		return true;
}

