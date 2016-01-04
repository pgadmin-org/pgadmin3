//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRule.cpp - PostgreSQL View Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "ctl/ctlSQLBox.h"
#include "dlg/dlgRule.h"
#include "schema/pgRule.h"
#include "schema/pgTable.h"
#include "schema/pgCollection.h"



// pointer to controls
#define rbxEvent        CTRL_RADIOBOX("rbxEvent")
#define chkDoInstead    CTRL_CHECKBOX("chkDoInstead")
#define txtCondition    CTRL_TEXT("txtCondition")

#define pnlDefinition   CTRL_PANEL("pnlDefinition")
#define txtSqlBox       CTRL_SQLBOX("txtSqlBox")


BEGIN_EVENT_TABLE(dlgRule, dlgProperty)
	EVT_TEXT(XRCID("txtCondition"),                 dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkSelect"),                dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkInsert"),                dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkUpdate"),                dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkDelete"),                dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkDoInstead"),             dlgProperty::OnChange)
	EVT_RADIOBOX(XRCID("rbxEvent"),                 dlgProperty::OnChange)
	EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgProperty::OnChangeStc)
END_EVENT_TABLE();


dlgProperty *pgRuleFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRule(this, frame, (pgRule *)node, (pgTable *)parent);
}



dlgRule::dlgRule(pgaFactory *f, frmMain *frame, pgRule *node, pgTable *tab)
	: dlgProperty(f, frame, wxT("dlgRule"))
{
	table = tab;
	rule = node;
}


pgObject *dlgRule::GetObject()
{
	return rule;
}


int dlgRule::Go(bool modal)
{
	if (rule)
	{
		// edit mode

		oldDefinition = rule->GetFormattedDefinition();
		if (!oldDefinition.IsEmpty())
		{
			int doPos = oldDefinition.Find(wxT(" DO INSTEAD "));
			if (doPos > 0)
				oldDefinition = oldDefinition.Mid(doPos + 12).Strip(wxString::both);
			else
			{
				doPos = oldDefinition.Find(wxT(" DO "));
				if (doPos > 0)
					oldDefinition = oldDefinition.Mid(doPos + 4).Strip(wxString::both);
			}
		}
		chkDoInstead->SetValue(rule->GetDoInstead());
		rbxEvent->SetStringSelection(rule->GetEvent());
		txtCondition->SetValue(rule->GetCondition());
		txtSqlBox->SetText(oldDefinition);

		txtName->Disable();
	}
	else
	{
		// create mode
	}

	// Reset the labels as they XRC values will have been localised :-(
	rbxEvent->SetString(0, wxT("SELECT"));
	rbxEvent->SetString(1, wxT("INSERT"));
	rbxEvent->SetString(2, wxT("UPDATE"));
	rbxEvent->SetString(3, wxT("DELETE"));

	return dlgProperty::Go(modal);
}


pgObject *dlgRule::CreateObject(pgCollection *collection)
{
	pgObject *obj = ruleFactory.CreateObjects(collection, 0,
	                wxT("\n   AND rulename=") + qtDbString(GetName()) +
	                wxT("\n   AND rw.ev_class=") + table->GetOidStr());
	return obj;
}


bool dlgRule::didChange()
{
	if (!rule)
		return true;

	if (GetName() != rule->GetName())
		return true;
	if (txtSqlBox->GetText().Strip(wxString::both) != oldDefinition)
		return true;
	if (chkDoInstead->GetValue() != rule->GetDoInstead())
		return true;
	if (rbxEvent->GetStringSelection() != rule->GetEvent())
		return true;
	if (txtCondition->GetValue() != rule->GetCondition())
		return true;
	if (txtSqlBox->GetText() != oldDefinition)
		return true;

	return false;
}


void dlgRule::CheckChange()
{
	if (rule)
	{
		EnableOK(didChange() || txtSqlBox->GetText() != oldDefinition || txtComment->GetValue() != rule->GetComment());
	}
	else
	{
		wxString name = GetName();

		bool enable = true;

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, rbxEvent->GetSelection() >= 0,
		           _("Please select at an event."));
		CheckValid(enable, !txtSqlBox->GetTextLength() || txtSqlBox->GetTextLength() > 6 , _("Please enter function definition."));

		EnableOK(enable);
	}
}


wxString dlgRule::GetSql()
{
	wxString sql, name = GetName();


	if (!rule || didChange())
	{
		sql += wxT("CREATE OR REPLACE RULE ") + qtIdent(name)
		       + wxT(" AS\n   ON ") + rbxEvent->GetStringSelection()
		       + wxT(" TO ") + table->GetQuotedFullIdentifier();
		AppendIfFilled(sql, wxT("\n   WHERE ") , txtCondition->GetValue());

		sql += wxT("\n   DO ");

		if (chkDoInstead->GetValue())
			sql += wxT("INSTEAD ");

		if (txtSqlBox->GetTextLength())
		{
			sql += wxT("\n") + txtSqlBox->GetText().Strip(wxString::both);
			if (sql.Right(1) != wxT(";"))
				sql += wxT(";");
		}
		else
			sql += wxT("NOTHING;");

		sql += wxT("\n");
	}
	AppendComment(sql, wxT("RULE ") + qtIdent(name)
	              + wxT(" ON ") + table->GetQuotedFullIdentifier(), rule);
	return sql;
}

