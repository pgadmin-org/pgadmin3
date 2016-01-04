//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTrigger.cpp - PostgreSQL Trigger Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "utils/pgDefs.h"

#include "dlg/dlgTrigger.h"
#include "schema/pgTrigger.h"
#include "schema/pgTable.h"
#include "schema/pgSchema.h"


// pointer to controls
#define chkRow          CTRL_CHECKBOX("chkRow")
#define chkConstraint   CTRL_CHECKBOX("chkConstraint")
#define chkDeferrable   CTRL_CHECKBOX("chkDeferrable")
#define chkDeferred     CTRL_CHECKBOX("chkDeferred")
#define cbFunction      CTRL_COMBOBOX2("cbFunction")
#define txtArguments    CTRL_TEXT("txtArguments")
#define rdbFires        CTRL_RADIOBOX("rdbFires")
#define chkInsert       CTRL_CHECKBOX("chkInsert")
#define chkUpdate       CTRL_CHECKBOX("chkUpdate")
#define chkDelete       CTRL_CHECKBOX("chkDelete")
#define chkTruncate     CTRL_CHECKBOX("chkTruncate")
#define txtWhen         CTRL_TEXT("txtWhen")
#define txtBody         CTRL_SQLBOX("txtBody")
#define btnAddCol       CTRL_BUTTON("btnAddCol")
#define btnRemoveCol    CTRL_BUTTON("btnRemoveCol")
#define lstColumns      CTRL_LISTVIEW("lstColumns")


BEGIN_EVENT_TABLE(dlgTrigger, dlgCollistProperty)
	EVT_RADIOBOX(XRCID("rdbFires"),                 dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkConstraint"),            dlgTrigger::OnChangeConstraint)
	EVT_CHECKBOX(XRCID("chkDeferrable"),            dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkRow"),                   dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkInsert"),                dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkUpdate"),                dlgTrigger::OnChange)
	EVT_CHECKBOX(XRCID("chkDelete"),                dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkTruncate"),              dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbFunction"),                   dlgTrigger::OnChangeFunc)
	EVT_COMBOBOX(XRCID("cbFunction"),               dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtArguments"),                 dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtWhen"),                      dlgProperty::OnChange)
	EVT_STC_MODIFIED(XRCID("txtBody"),              dlgProperty::OnChangeStc)
	EVT_LIST_ITEM_SELECTED(XRCID("lstColumns"),     dlgTrigger::OnSelectListCol)
	EVT_COMBOBOX(XRCID("cbColumns"),                dlgTrigger::OnSelectComboCol)
	EVT_BUTTON(XRCID("btnAddCol"),                  dlgTrigger::OnAddCol)
	EVT_BUTTON(XRCID("btnRemoveCol"),               dlgTrigger::OnRemoveCol)
END_EVENT_TABLE();


dlgProperty *pgTriggerFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTrigger(this, frame, (pgTrigger *)node, (pgTable *)parent);
}


dlgTrigger::dlgTrigger(pgaFactory *f, frmMain *frame, pgTrigger *node, pgTable *parentNode)
	: dlgCollistProperty(f, frame, wxT("dlgTrigger"), parentNode)
{
	trigger = node;
	table = parentNode;
	wxASSERT(!table || table->GetMetaType() == PGM_TABLE || table->GetMetaType() == PGM_VIEW
	         || table->GetMetaType() == GP_PARTITION);

	bool bVal;
	settings->Read(wxT("frmQuery/ShowLineNumber"), &bVal, false);
	if (!bVal)
	{
		txtBody->SetMarginType(1, wxSTC_MARGIN_NUMBER);
		txtBody->SetMarginWidth(1, ConvertDialogToPixels(wxPoint(16, 0)).x);
	}

	lstColumns->AddColumn(_("Column name"));
}


pgObject *dlgTrigger::GetObject()
{
	return trigger;
}


wxString dlgTrigger::GetColumns()
{
	wxString sql;

	int pos;
	// iterate cols
	for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
	{
		if (pos)
			sql += wxT(", ");

		sql += qtIdent(lstColumns->GetItemText(pos));
	}
	return sql;
}


int dlgTrigger::Go(bool modal)
{
	if (trigger)
	{
		// edit mode
		chkRow->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_ROW) != 0);
		chkInsert->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_INSERT) != 0);
		chkUpdate->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_UPDATE) != 0);
		chkDelete->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_DELETE) != 0);
		chkTruncate->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_TRUNCATE) != 0);
		if (trigger->GetTriggerType() & TRIGGER_TYPE_BEFORE)
			rdbFires->SetSelection(0);
		else if (trigger->GetTriggerType() & TRIGGER_TYPE_INSTEAD)
			rdbFires->SetSelection(2);
		else
			rdbFires->SetSelection(1);
		txtArguments->SetValue(trigger->GetArguments());
		txtWhen->SetValue(trigger->GetWhen());
		if (!connection->BackendMinimumVersion(7, 4))
			txtName->Disable();

		if (trigger->GetLanguage() == wxT("edbspl"))
		{
			cbFunction->Append(wxString::Format(wxT("<%s>"), _("Inline EDB-SPL")));
			txtBody->SetText(trigger->GetSource());
		}
		else
		{
			cbFunction->Append(trigger->GetFunction());
			txtBody->Disable();
		}

		cbFunction->SetSelection(0);
		txtArguments->Disable();
		cbFunction->Disable();

		if (!connection->EdbMinimumVersion(8, 0))
		{
			chkRow->Disable();
			rdbFires->Disable();
			chkInsert->Disable();
			chkUpdate->Disable();
			chkDelete->Disable();
			chkTruncate->Disable();
		}
		else if (!connection->BackendMinimumVersion(8, 4))
			chkTruncate->Disable();
		else if (!connection->BackendMinimumVersion(8, 5))
			txtWhen->Disable();

		wxArrayString colsArr = trigger->GetColumnList();
		for (int colIdx = 0, colsCount = colsArr.Count(); colIdx < colsCount; colIdx++)
		{
			lstColumns->InsertItem(colIdx, colsArr.Item(colIdx));
		}

		if (connection->BackendMinimumVersion(8, 2))
		{
			chkConstraint->SetValue(trigger->GetIsConstraint());
			chkDeferrable->SetValue(trigger->GetDeferrable());
			chkDeferred->SetValue(trigger->GetDeferred());
		}
		chkConstraint->Enable(false);
		chkDeferrable->Enable(false);
		chkDeferred->Enable(false);
	}
	else
	{
		// create mode
		if (connection->EdbMinimumVersion(8, 0))
			cbFunction->Append(wxString::Format(wxT("<%s>"), _("Inline EDB-SPL")));

		wxString sysRestr;
		if (!settings->GetShowSystemObjects())
			sysRestr = wxT("   AND ") + connection->SystemNamespaceRestriction(wxT("nspname"));

		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT quote_ident(nspname) || '.' || quote_ident(proname)\n")
		                 wxT("  FROM pg_proc p, pg_namespace n, pg_language l\n")
		                 wxT(" WHERE p.pronamespace = n.oid AND p.prolang = l.oid AND l.lanname != 'edbspl' AND prorettype=") + NumToStr(PGOID_TYPE_TRIGGER) + sysRestr +
		                 wxT(" ORDER BY nspname ASC, proname ASC "));
		if (set)
		{
			while (!set->Eof())
			{
				cbFunction->Append(set->GetVal(0));
				set->MoveNext();
			}
			delete set;
		}
		if (!connection->BackendMinimumVersion(7, 4))
		{
			chkRow->SetValue(true);
			chkRow->Disable();
		}

		txtBody->Disable();

		if (!connection->BackendMinimumVersion(8, 4))
			chkTruncate->Disable();

		if (!connection->BackendMinimumVersion(9, 1) || table->GetMetaType() != PGM_VIEW)
			rdbFires->Enable(2, false);

		chkConstraint->Enable(connection->BackendMinimumVersion(8, 2));
		chkDeferrable->Disable();
		chkDeferred->Disable();
	}

	cbColumns->Disable();
	btnAddCol->Disable();
	btnRemoveCol->Disable();

	// Reset the radio box rbxEvent item values with the keywords to resolve the locale issues.
	rdbFires->SetString(0, wxT("BEFORE"));
	rdbFires->SetString(1, wxT("AFTER"));
	rdbFires->SetString(2, wxT("INSTEAD OF"));

	chkTruncate->SetLabel(wxT("TRUNCATE"));
	chkDelete->SetLabel(wxT("DELETE"));
	chkInsert->SetLabel(wxT("INSERT"));
	chkUpdate->SetLabel(wxT("UPDATE"));

	return dlgCollistProperty::Go(modal);
}


void dlgTrigger::OnAddCol(wxCommandEvent &ev)
{
	wxString colName = cbColumns->GetValue();

	if (!colName.IsEmpty())
	{
		lstColumns->InsertItem(lstColumns->GetItemCount(), colName);

		cbColumns->Delete(cbColumns->GetCurrentSelection());
		if (cbColumns->GetCount())
			cbColumns->SetSelection(0);

		CheckChange();
		if (!cbColumns->GetCount())
			btnAddCol->Disable();
	}
}


void dlgTrigger::OnRemoveCol(wxCommandEvent &ev)
{
	long pos = lstColumns->GetSelection();
	if (pos >= 0)
	{
		wxString colName = lstColumns->GetItemText(pos);

		lstColumns->DeleteItem(pos);
		cbColumns->Append(colName);

		CheckChange();
		btnRemoveCol->Disable();
	}
}

wxString dlgTrigger::GetSql()
{
	wxString sql;
	wxString name = GetName();

	if (trigger)
	{
		if (name != trigger->GetName())
			sql = wxT("ALTER TRIGGER ") + trigger->GetQuotedIdentifier() + wxT(" ON ") + table->GetQuotedFullIdentifier()
			      + wxT("\n  RENAME TO ") + qtIdent(name) + wxT(";\n\n");
	}

	if (!trigger ||
	        (cbFunction->GetValue() == wxString::Format(wxT("<%s>"), _("Inline EDB-SPL"))
	         && (
	             txtBody->GetText() != trigger->GetSource() ||
	             chkRow->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_ROW) ||
	             chkInsert->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_INSERT ? true : false) ||
	             chkUpdate->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_UPDATE ? true : false) ||
	             chkDelete->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_DELETE ? true : false) ||
	             chkTruncate->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_TRUNCATE ? true : false) ||
	             rdbFires->GetSelection() != (trigger->GetTriggerType() & TRIGGER_TYPE_BEFORE ? 0 : TRIGGER_TYPE_INSTEAD ? 2 : 1)
	         )
	        )
	   )
	{
		if (cbFunction->GetValue() == wxString::Format(wxT("<%s>"), _("Inline EDB-SPL")))
			sql += wxT("CREATE OR REPLACE TRIGGER ");
		else if (chkConstraint->GetValue())
			sql += wxT("CREATE CONSTRAINT TRIGGER ");
		else
			sql += wxT("CREATE TRIGGER ");
		sql += qtIdent(name);

		if (rdbFires->GetSelection() == 1)
			sql += wxT(" AFTER");
		else if (rdbFires->GetSelection() == 2)
			sql += wxT(" INSTEAD OF");
		else
			sql += wxT(" BEFORE");
		int actionCount = 0;
		if (chkInsert->GetValue())
		{
			if (actionCount++)
				sql += wxT(" OR");
			sql += wxT(" INSERT");
		}
		if (chkUpdate->GetValue())
		{
			if (actionCount++)
				sql += wxT(" OR");
			sql += wxT(" UPDATE");
			if (lstColumns->GetItemCount() > 0)
				sql += wxT(" OF ") + GetColumns();
		}
		if (chkDelete->GetValue())
		{
			if (actionCount++)
				sql += wxT(" OR");
			sql += wxT(" DELETE");
		}
		if (chkTruncate->GetValue())
		{
			if (actionCount++)
				sql += wxT(" OR");
			sql += wxT(" TRUNCATE");
		}
		sql += wxT("\n   ON ") + table->GetQuotedFullIdentifier();
		if (chkDeferrable->GetValue())
		{
			sql += wxT(" DEFERRABLE");
			if (chkDeferred->GetValue())
				sql += wxT(" INITIALLY DEFERRED");
		}
		sql += wxT(" FOR EACH ");
		if (chkRow->GetValue())
			sql += wxT("ROW");
		else
			sql += wxT("STATEMENT");

		if (connection->BackendMinimumVersion(8, 5) && !txtWhen->GetValue().IsEmpty())
			sql += wxT("\n   WHEN (") + txtWhen->GetValue() + wxT(")");

		if (cbFunction->GetValue() != wxString::Format(wxT("<%s>"), _("Inline EDB-SPL")))
		{
			sql += wxT("\n   EXECUTE PROCEDURE ") + cbFunction->GetValue()
			       + wxT("(") + txtArguments->GetValue()
			       + wxT(");\n");
		}
		else
		{
			sql += wxT("\n") + txtBody->GetText();
			if (!sql.Trim().EndsWith(wxT(";")))
				sql = sql.Trim() + wxT(";");
			sql += wxT("\n");
		}
	}
	AppendComment(sql, wxT("TRIGGER ") + qtIdent(GetName())
	              + wxT(" ON ") + table->GetQuotedFullIdentifier(), trigger);

	return sql;
}


pgObject *dlgTrigger::CreateObject(pgCollection *collection)
{
	pgObject *obj = triggerFactory.CreateObjects(collection, 0,
	                wxT("\n   AND tgname=") + qtDbString(GetName()) +
	                wxT("\n   AND tgrelid=") + table->GetOidStr() +
	                wxT("\n   AND relnamespace=") + table->GetSchema()->GetOidStr());
	return obj;
}


void dlgTrigger::OnChange(wxCommandEvent &ev)
{
	if (chkUpdate->GetValue())
	{
		cbColumns->Enable();
	}
	else
	{
		if (lstColumns->GetItemCount() > 0)
		{
			if (wxMessageBox(_("Removing the UPDATE event will cause the column list to be cleared. Do you wish to continue?"), _("Remove UPDATE event?"), wxYES_NO) != wxYES)
			{
				chkUpdate->SetValue(true);
				return;
			}

			// Move all the columns back to the combo
			for (int pos = lstColumns->GetItemCount(); pos > 0; pos--)
			{
				wxString colName = lstColumns->GetItemText(pos - 1);

				lstColumns->DeleteItem(pos - 1);
				cbColumns->Append(colName);
			}
		}

		cbColumns->Disable();
		btnAddCol->Disable();
		btnRemoveCol->Disable();
	}

	CheckChange();
}


void dlgTrigger::OnChangeFunc(wxCommandEvent &ev)
{
	cbFunction->GuessSelection(ev);

	if (cbFunction->GetValue() == wxString::Format(wxT("<%s>"), _("Inline EDB-SPL")))
	{
		txtArguments->Disable();
		txtBody->Enable();
	}
	else
	{
		txtArguments->Enable();
		txtBody->Disable();
	}

	CheckChange();
}


void dlgTrigger::OnChangeConstraint(wxCommandEvent &ev)
{
	if (chkConstraint->GetValue())
	{
		rdbFires->SetSelection(1);
		rdbFires->Disable();
		chkRow->SetValue(true);
		chkRow->Disable();
		chkDeferrable->Enable();
		chkDeferred->Enable();
	}
	else
	{
		rdbFires->Enable();
		chkRow->Enable();
		chkDeferrable->Disable();
		chkDeferred->Disable();
	}

	CheckChange();
}


void dlgTrigger::CheckChange()
{
	bool enable = true;

	wxString function = cbFunction->GetValue();
	wxString name = GetName();

	// We can only have per-statement TRUNCATE triggers
	if (trigger || connection->BackendMinimumVersion(8, 4))
	{
		if (chkRow->GetValue())
		{
			chkTruncate->Disable();
			chkTruncate->SetValue(false);
		}
		else if (connection->EdbMinimumVersion(8, 0))
			chkTruncate->Enable();
	}

	CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
	CheckValid(enable, !function.IsEmpty(), _("Please specify trigger function."));

	CheckValid(enable, chkInsert->GetValue() || chkUpdate->GetValue() || chkDelete->GetValue() || chkTruncate->GetValue(),
	           _("Please specify at least one action."));

	if (cbFunction->GetValue() == wxString::Format(wxT("<%s>"), _("Inline EDB-SPL")))
		CheckValid(enable, !txtBody->GetText().IsEmpty(), _("Please specify trigger body."));

	if (trigger)
	{
		EnableOK(enable &&
		         (txtComment->GetValue() != trigger->GetComment() ||
		          txtName->GetValue() != trigger->GetName() ||
		          (txtBody->GetText() != trigger->GetSource() && cbFunction->GetValue() == wxString::Format(wxT("<%s>"), _("Inline EDB-SPL"))) ||
		          txtWhen->GetValue() != trigger->GetWhen() ||
		          chkRow->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_ROW ? true : false) ||
		          chkInsert->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_INSERT ? true : false) ||
		          chkUpdate->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_UPDATE ? true : false) ||
		          chkDelete->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_DELETE ? true : false) ||
		          chkTruncate->GetValue() != (trigger->GetTriggerType() & TRIGGER_TYPE_TRUNCATE ? true : false) ||
		          rdbFires->GetSelection() != (trigger->GetTriggerType() & TRIGGER_TYPE_BEFORE ? 0 : (trigger->GetTriggerType() & TRIGGER_TYPE_INSTEAD ? 2 : 1))));
	}
	else
	{
		EnableOK(enable);
	}
}

bool dlgTrigger::IsUpToDate()
{
	if (trigger && !trigger->IsUpToDate())
		return false;
	else
		return true;
}

void dlgTrigger::OnSelectListCol(wxListEvent &ev)
{
	OnSelectCol();
}

void dlgTrigger::OnSelectComboCol(wxCommandEvent &ev)
{
	OnSelectCol();
}

void dlgTrigger::OnSelectCol()
{
	// Can't change the columns on an existing index.
	if (trigger)
		return;

	if (lstColumns->GetSelection() != wxNOT_FOUND)
		btnRemoveCol->Enable(true);
	else
		btnRemoveCol->Enable(false);

	if (cbColumns->GetSelection() != wxNOT_FOUND && !cbColumns->GetValue().IsEmpty())
		btnAddCol->Enable(true);
	else
		btnAddCol->Enable(false);
}


