//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgEventTrigger.cpp - PostgreSQL Trigger Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "utils/pgDefs.h"

#include "dlg/dlgEventTrigger.h"
#include "schema/pgEventTrigger.h"
#include "ctl/ctlSeclabelPanel.h"

// pointer to controls
#define chkEnable		CTRL_CHECKBOX("chkEnable")
#define rdbEnableStatus CTRL_RADIOBOX("rdbEnableStatus")
#define cbFunction		CTRL_COMBOBOX2("cbFunction")
#define rdbEvents		CTRL_RADIOBOX("rdbEvents")
#define txtWhen			CTRL_TEXT("txtWhen")
#define cbOwner			CTRL_COMBOBOX2("cbOwner")

BEGIN_EVENT_TABLE(dlgEventTrigger, dlgProperty)
	EVT_CHECKBOX(XRCID("chkEnable"),				dlgEventTrigger::OnChangeEnable)
	EVT_RADIOBOX(XRCID("rdbEnableStatus"),			dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbFunction"),				dlgProperty::OnChange)
	EVT_RADIOBOX(XRCID("rdbEvents"),				dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtWhen"),						dlgProperty::OnChange)
END_EVENT_TABLE();

dlgEventTrigger::dlgEventTrigger(pgaFactory *factory, frmMain *frame, pgEventTrigger *node, pgObject *parent)
	: dlgProperty(factory, frame, wxT("dlgEventTrigger"))
{
	seclabelPage = new ctlSeclabelPanel(nbNotebook);
	eventTrigger = node;
}

dlgProperty *pgEventTriggerFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgEventTrigger(this, frame, (pgEventTrigger *)node, parent);
}

wxString dlgEventTrigger::GetSql()
{
	wxString sql = wxEmptyString;
	wxString name = GetName();

	if (eventTrigger)
	{
		if (!GetName().IsEmpty() && GetName() != eventTrigger->GetName())
			sql = wxT("ALTER EVENT TRIGGER ") + eventTrigger->GetQuotedFullIdentifier() + wxT("\nRENAME TO ") + qtIdent(GetName()) + wxT(";\n\n");

		if (!cbOwner->GetValue().IsEmpty() && cbOwner->GetValue() != eventTrigger->GetOwner())
			sql += wxT("ALTER EVENT TRIGGER ") + eventTrigger->GetQuotedFullIdentifier() + wxT("\nOWNER TO ") + cbOwner->GetValue() + wxT(";\n\n");

		if (rdbEnableStatus->GetSelection() != 0 && chkEnable->GetValue())
			sql += wxT("ALTER EVENT TRIGGER ") + qtIdent(name) + ((rdbEnableStatus->GetSelection() == 1) ? wxT(" ENABLE REPLICA ;\n\n") : wxT(" ENABLE ALWAYS ;\n\n"));
		else if (!chkEnable->GetValue())
			sql += wxT("ALTER EVENT TRIGGER ") + qtIdent(name) + wxT(" DISABLE ;\n\n");
	}

	if (!eventTrigger ||
	        (
	            cbFunction->GetValue() != (eventTrigger->GetFunction()) ||
	            rdbEvents->GetStringSelection().Lower() != (eventTrigger->GetEventName().Lower()) ||
	            txtWhen->GetValue() != (eventTrigger->GetWhen())
	        )
	   )
	{
		if (eventTrigger)
			sql  = wxT("DROP EVENT TRIGGER IF EXISTS ") + ((eventTrigger) ? qtIdent(eventTrigger->GetName()) : qtIdent(GetName())) + wxT(";\n\n");

		sql += wxT("CREATE EVENT TRIGGER ") + qtIdent(name) + wxT(" ON ");

		if (rdbEvents->GetSelection() == 0)
			sql += wxT(" DDL_COMMAND_START ");
		else if (rdbEvents->GetSelection() == 1)
			sql += wxT(" DDL_COMMAND_END ");
		else
			sql += wxT(" SQL_DROP ");

		if (!txtWhen->IsEmpty())
			sql += wxT("\nWHEN TAG IN (") + txtWhen->GetValue() + wxT(")");

		if (!cbFunction->GetValue().IsEmpty())
			sql += wxT("\nEXECUTE PROCEDURE ") + (cbFunction->GetValue()) + wxT("();\n\n");

		if (rdbEnableStatus->GetSelection() != 0 && chkEnable->GetValue())
			sql += wxT("ALTER EVENT TRIGGER ") + qtIdent(name) + ((rdbEnableStatus->GetSelection() == 1) ? wxT(" ENABLE REPLICA ;\n\n") : wxT(" ENABLE ALWAYS ;\n\n"));
		else if (!chkEnable->GetValue())
			sql += wxT("ALTER EVENT TRIGGER ") + qtIdent(name) + wxT(" DISABLE ;\n\n");

		if (!eventTrigger && !cbOwner->GetValue().IsEmpty())
			sql += wxT("ALTER EVENT TRIGGER ") + qtIdent(GetName()) + wxT("\nOWNER TO ") + cbOwner->GetValue() + wxT(";\n\n");
	}

	AppendComment(sql, wxT("EVENT TRIGGER ") + qtIdent(GetName()), eventTrigger);

	if (seclabelPage)
		sql += seclabelPage->GetSqlForSecLabels(wxT("EVENT TRIGGER"), qtIdent(name));

	return sql;
}

pgObject *dlgEventTrigger::CreateObject(pgCollection *pgcol)
{
	pgObject *obj = eventTriggerFactory.CreateObjects(pgcol, 0,
	                wxT(" \n AND e.evtname = ") + qtDbString(GetName()));
	return obj;
}

pgObject *dlgEventTrigger::GetObject()
{
	return eventTrigger;
}

int dlgEventTrigger::Go(bool modal)
{
	seclabelPage->SetConnection(connection);
	seclabelPage->SetObject(eventTrigger);
	this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgEventTrigger::OnChange));

	if (eventTrigger)
	{
		// Edit mode
		chkEnable->SetValue(eventTrigger->GetEnabled());

		if (eventTrigger->GetEnableStatus() == wxT("enabled"))
			rdbEnableStatus->SetSelection(0);
		else if(eventTrigger->GetEnableStatus() == wxT("replica"))
			rdbEnableStatus->SetSelection(1);
		else if(eventTrigger->GetEnableStatus() == wxT("always"))
			rdbEnableStatus->SetSelection(2);
		else
			rdbEnableStatus->Disable();

		if(eventTrigger->GetEventName().Lower() == wxT("ddl command start"))
			rdbEvents->SetSelection(0);
		else if(eventTrigger->GetEventName().Lower() == wxT("ddl command end"))
			rdbEvents->SetSelection(1);
		else
			rdbEvents->SetSelection(2);

		cbFunction->SetValue(eventTrigger->GetFunction());
		cbOwner->SetValue(eventTrigger->GetOwner());
		(!eventTrigger->GetWhen().IsEmpty()) ? txtWhen->SetValue(eventTrigger->GetWhen()) : txtWhen->SetValue(wxEmptyString);
	}
	else
	{
		// Create mode
		chkEnable->SetValue(true);
		rdbEnableStatus->Disable();
	}

	pgSet *funcSet = connection->ExecuteSet(
	                     wxT("SELECT quote_ident(nspname) || '.' || quote_ident(proname)\n")
	                     wxT(" FROM pg_proc p, pg_namespace n, pg_language l\n")
	                     wxT(" WHERE p.pronamespace = n.oid AND p.prolang = l.oid AND p.pronargs = 0 AND l.lanname != 'sql' AND prorettype::regtype::text = 'event_trigger'\n")
	                     wxT(" ORDER BY nspname ASC, proname ASC "));
	if (funcSet)
	{
		while (!funcSet->Eof())
		{
			cbFunction->Append(funcSet->GetVal(0));
			funcSet->MoveNext();
		}
		delete funcSet;
	}

	pgSet *userSet = connection->ExecuteSet(
	                     wxT("SELECT usename ")
	                     wxT("FROM	pg_user ")
	                     wxT("WHERE  usesuper IS TRUE"));
	if (userSet)
	{
		while (!userSet->Eof())
		{
			cbOwner->Append(userSet->GetVal(0));
			userSet->MoveNext();
		}
		delete userSet;
	}

	return  dlgProperty::Go(modal);
}

void dlgEventTrigger::CheckChange()
{
	bool enable = true;

	wxString function = cbFunction->GetValue();
	wxString name = GetName();
	wxString owner = cbOwner->GetValue();

	(chkEnable->GetValue()) ? rdbEnableStatus->Enable() : rdbEnableStatus->Disable();

	CheckValid(enable, !name.IsEmpty(), _("Please specify event trigger name."));
	CheckValid(enable, !owner.IsEmpty(), _("Please specify owner of event trigger."));
	CheckValid(enable, !function.IsEmpty(), _("Please specify event trigger function."));

	if (eventTrigger)
	{
		EnableOK(enable &&
		         (txtComment->GetValue() != eventTrigger->GetComment() ||
		          txtName->GetValue() != eventTrigger->GetName() ||
		          txtWhen->GetValue() != eventTrigger->GetWhen() ||
		          chkEnable->GetValue() != eventTrigger->GetEnabled() ||
		          rdbEvents->GetStringSelection().Lower() != eventTrigger->GetEventName().Lower() ||
		          rdbEnableStatus->GetStringSelection().Lower() != eventTrigger->GetEnableStatus().Lower() ||
		          !function.IsEmpty() ||
		          !owner.IsEmpty()
		         )
		        );
	}
	else
	{
		EnableOK(enable);
	}
}

bool dlgEventTrigger::IsUpToDate()
{
	if (eventTrigger && !eventTrigger->IsUpToDate())
		return false;
	else
		return true;
}

void dlgEventTrigger::OnChange(wxCommandEvent &ev)
{
	CheckChange();
}

void dlgEventTrigger::OnChangeEnable(wxCommandEvent &ev)
{
	CheckChange();
}
