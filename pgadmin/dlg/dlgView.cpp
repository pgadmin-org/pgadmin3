//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgView.cpp - PostgreSQL View Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "ctl/ctlSQLBox.h"
#include "dlg/dlgView.h"
#include "schema/pgView.h"
#include "schema/pgSchema.h"



// pointer to controls
#define pnlDefinition   CTRL_PANEL("pnlDefinition")
#define txtSqlBox       CTRL_SQLBOX("txtSqlBox")



BEGIN_EVENT_TABLE(dlgView, dlgSecurityProperty)
	EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgProperty::OnChangeStc)
	EVT_BUTTON(wxID_APPLY,                          dlgView::OnApply)
END_EVENT_TABLE();


dlgProperty *pgViewFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgView(this, frame, (pgView *)node, (pgSchema *)parent);
}

dlgView::dlgView(pgaFactory *f, frmMain *frame, pgView *node, pgSchema *sch)
	: dlgSecurityProperty(f, frame, node, wxT("dlgView"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
	schema = sch;
	view = node;
}


pgObject *dlgView::GetObject()
{
	return view;
}


int dlgView::Go(bool modal)
{
	AddGroups(cbOwner);
	AddUsers(cbOwner);

	if (view)
	{
		// edit mode

		oldDefinition = view->GetFormattedDefinition();
		txtSqlBox->SetText(oldDefinition);
	}
	else
	{
		// create mode
	}

	// Find, and disable the RULE ACL option if we're 8.2
	if (connection->BackendMinimumVersion(8, 2))
	{
		// Disable the checkbox
		if (!DisablePrivilege(wxT("RULE")))
			wxLogError(_("Failed to disable the RULE privilege checkbox!"));
	}

	return dlgSecurityProperty::Go(modal);
}


pgObject *dlgView::CreateObject(pgCollection *collection)
{
	pgObject *obj = viewFactory.CreateObjects(collection, 0,
	                wxT("\n   AND c.relname=") + qtDbString(txtName->GetValue()) +
	                wxT("\n   AND c.relnamespace=") + schema->GetOidStr());
	return obj;
}


void dlgView::CheckChange()
{
	wxString name = GetName();
	if (name)
	{
		bool enable = true;
		if (view)
			enable = txtComment->GetValue() != view->GetComment()
			         || txtSqlBox->GetText().Trim(true).Trim(false) != oldDefinition.Trim(true).Trim(false)
			         || cbOwner->GetValue() != view->GetOwner()
			         || name != view->GetName();
		enable &= !txtSqlBox->GetText().Trim(true).IsEmpty();
		EnableOK(enable);
	}
	else
	{
		bool enable = true;

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, txtSqlBox->GetText().Trim(true).Trim(false).Length() > 0 , _("Please enter function definition."));

		EnableOK(enable);
	}
}


wxString dlgView::GetSql()
{
	wxString sql, name = GetName();

	if (view)
	{
		// edit mode

		if (name != view->GetName())
		{
			sql += wxT("ALTER TABLE ") + view->GetQuotedFullIdentifier()
			       +  wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
		}
	}

	if (!view || txtSqlBox->GetText().Trim(true).Trim(false) != oldDefinition.Trim(true).Trim(false))
	{
		sql += wxT("CREATE OR REPLACE VIEW ") + schema->GetQuotedPrefix() + qtIdent(name) + wxT(" AS\n")
		       + txtSqlBox->GetText().Trim(true).Trim(false)
		       + wxT(";\n");
	}

	if (view)
		AppendOwnerChange(sql, wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));
	else
		AppendOwnerNew(sql, wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));


	sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));

	AppendComment(sql, wxT("VIEW"), schema, view);
	return sql;
}

bool dlgView::IsUpToDate()
{
	if (view && !view->IsUpToDate())
		return false;
	else
		return true;
}

void dlgView::OnApply(wxCommandEvent &ev)
{
	dlgProperty::OnApply(ev);

	wxString sql = wxT("SELECT xmin FROM pg_class WHERE oid = ") + view->GetOidStr();
	view->iSetXid(StrToOid(connection->ExecuteScalar(sql)));
}

