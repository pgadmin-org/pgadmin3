//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
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
#include "ctl/ctlSeclabelPanel.h"


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

	seclabelPage = new ctlSeclabelPanel(nbNotebook);
}


pgObject *dlgView::GetObject()
{
	return view;
}


int dlgView::Go(bool modal)
{
	if (!view)
		cbOwner->Append(wxEmptyString);
	AddGroups(cbOwner);
	AddUsers(cbOwner);

	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(view);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgView::OnChange));
	}
	else
		seclabelPage->Disable();

	if (view)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(8, 1));
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
		{
			wxLogError(_("Failed to disable the RULE privilege checkbox!"));
		}
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
	bool enable = true;
	wxString name = GetName();
	if(!name.IsEmpty())
	{
		if (view)
			enable = txtComment->GetValue() != view->GetComment()
			         || txtSqlBox->GetText().Trim(true).Trim(false) != oldDefinition.Trim(true).Trim(false)
			         || cbOwner->GetValue() != view->GetOwner()
			         || cbSchema->GetValue() != view->GetSchema()->GetName()
			         || name != view->GetName();
		enable &= !txtSqlBox->GetText().Trim(true).IsEmpty();
		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	else
	{
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, txtSqlBox->GetText().Trim(true).Trim(false).Length() > 0 , _("Please enter function definition."));
	}

	EnableOK(enable);
}


wxString dlgView::GetSql()
{
	wxString sql;
	wxString name;

	if (view)
	{
		// edit mode
		name = GetName();

		if (name != view->GetName())
		{
			if (connection->BackendMinimumVersion(8, 3))
				AppendNameChange(sql, wxT("VIEW ") + view->GetQuotedFullIdentifier());
			else
				AppendNameChange(sql, wxT("TABLE ") + view->GetQuotedFullIdentifier());
		}

		if (connection->BackendMinimumVersion(8, 4) && cbSchema->GetName() != view->GetSchema()->GetName())
			AppendSchemaChange(sql, wxT("VIEW " + qtIdent(view->GetSchema()->GetName()) + wxT(".") + qtIdent(name)));
		else
			AppendSchemaChange(sql, wxT("TABLE " + qtIdent(view->GetSchema()->GetName()) + wxT(".") + qtIdent(name)));
	}

	if (!view || txtSqlBox->GetText().Trim(true).Trim(false) != oldDefinition.Trim(true).Trim(false))
	{
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());

		sql += wxT("CREATE OR REPLACE VIEW ") + name + wxT(" AS\n")
		       + txtSqlBox->GetText().Trim(true).Trim(false)
		       + wxT(";\n");
	}

	if (view)
		AppendOwnerChange(sql, wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));
	else
		AppendOwnerNew(sql, wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));


	sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	AppendComment(sql, wxT("VIEW ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), view);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("VIEW"), qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

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

void dlgView::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
