//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSynonym.cpp - EnterpriseDB Synonym Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgSynonym.h"
#include "schema/edbSynonym.h"
#include "schema/edbPrivateSynonym.h"

// pointer to controls
#define txtName             CTRL_TEXT("txtName")
#define cbTargetType        CTRL_COMBOBOX2("cbTargetType")
#define cbTargetSchema      CTRL_COMBOBOX2("cbTargetSchema")
#define cbTargetObject      CTRL_COMBOBOX("cbTargetObject")
#define stComment           CTRL_STATIC("stComment")

dlgProperty *edbSynonymFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgSynonym(this, frame, (edbSynonym *)node);
}

dlgProperty *edbPrivateSynonymFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgSynonym(this, frame, (edbPrivateSynonym *)node, (pgSchema *)parent);
}


BEGIN_EVENT_TABLE(dlgSynonym, dlgProperty)
	EVT_TEXT(XRCID("cbTargetType"),                 dlgSynonym::OnChangeTargetType)
	EVT_COMBOBOX(XRCID("cbTargetType"),             dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbTargetSchema"),               dlgSynonym::OnChangeTargetSchema)
	EVT_COMBOBOX(XRCID("cbTargetSchema"),           dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbTargetObject"),           dlgProperty::OnChange)
END_EVENT_TABLE();


dlgSynonym::dlgSynonym(pgaFactory *f, frmMain *frame, edbSynonym *node)
	: dlgProperty(f, frame, wxT("dlgSynonym"))
{
	synonym = node;
	privSynonym = NULL;
	synonymSchema = NULL;
	cbOwner->Disable();
}

dlgSynonym::dlgSynonym(edbPrivateSynonymFactory *factory, frmMain *frame, edbPrivateSynonym *syn, pgSchema *schema)
	: dlgProperty((pgaFactory *)factory, frame, wxT("dlgSynonym"))
{
	synonym = NULL;
	privSynonym = syn;
	synonymSchema = schema;
	cbOwner->Disable();
}


pgObject *dlgSynonym::GetObject()
{
	if (!synonymSchema)
		return synonym;
	return (pgObject *)privSynonym;
}


int dlgSynonym::Go(bool modal)
{
	if (synonym)
	{
		// edit mode
		txtName->Disable();

		cbTargetType->SetSelection(cbTargetType->FindString(synonym->GetTargetType()));
		ProcessTypeChange();

		if (cbTargetType->GetValue() != _("Public synonym"))
		{
			cbTargetSchema->SetSelection(cbTargetSchema->FindString(synonym->GetTargetSchema()));
			ProcessSchemaChange();
		}
		cbTargetObject->SetSelection(cbTargetObject->FindString(synonym->GetTargetObject()));
	}
	else if (privSynonym)
	{
		// edit mode
		txtName->Disable();

		cbTargetType->SetSelection(cbTargetType->FindString(privSynonym->GetTargetType()));
		ProcessTypeChange();

		cbTargetSchema->SetSelection(cbTargetSchema->FindString(privSynonym->GetTargetSchema()));
		ProcessSchemaChange();

		cbTargetObject->SetSelection(cbTargetObject->FindString(privSynonym->GetTargetObject()));
	}
	else
	{
		// create mode
		cbTargetType->Enable();
		cbTargetSchema->Disable();
		cbTargetObject->Disable();
	}

	txtComment->Disable();

	// Functions and Procedures are available 8.4 onwards. So check for the
	// same. Unfortunately we need a connection object for that and so we
	// re-use an existing one..
	if (connection && connection->BackendMinimumVersion(8, 4))
	{
		cbTargetType->Append(_("Function"));
		cbTargetType->Append(_("Procedure"));
	}
	cbTargetType->Append(_("Sequence"));
	cbTargetType->Append(_("Public synonym"));
	cbTargetType->Append(_("Table"));
	cbTargetType->Append(_("View"));

	return dlgProperty::Go(modal);
}


pgObject *dlgSynonym::CreateObject(pgCollection *collection)
{
	pgObject *obj = NULL;
	if (!synonymSchema)
		obj = synonymFactory.CreateObjects(collection, 0,
		                                   wxT(" WHERE synname = ") + qtDbString(GetName()));
	else
		obj = edbPrivFactory.CreateObjects(collection, 0,
		                                   wxT(" WHERE s.synname=") + qtDbString(GetName()) +
		                                   wxT(" AND s.synnamespace=") + collection->GetSchema()->GetOidStr() + wxT(" \n"));

	return obj;
}


void dlgSynonym::CheckChange()
{
	bool enable = true;
	CheckValid(enable, !txtName->GetValue().IsEmpty(), _("Please specify name."));
	CheckValid(enable, !cbTargetType->GetValue().IsEmpty(), _("Please select target type."));
	// Public Synonyms does supported in public only
	if (!synonymSchema && cbTargetType->GetValue() != _("Public synonym"))
		CheckValid(enable, !cbTargetSchema->GetValue().IsEmpty(), _("Please select target schema."));
	CheckValid(enable, !cbTargetObject->GetValue().IsEmpty(), _("Please select target object."));

	if (!enable)
	{
		EnableOK(enable);
		return;
	}

	if (synonym)
		EnableOK(synonym->GetTargetObject() != cbTargetObject->GetValue());
	else if (privSynonym)
		EnableOK(privSynonym->GetTargetObject() != cbTargetObject->GetValue());
	else
		EnableOK(txtName->GetValue() != wxEmptyString && cbTargetObject->GetValue() != wxEmptyString);
}

void dlgSynonym::ProcessTypeChange()
{
	cbTargetSchema->Clear();
	cbTargetObject->Clear();
	if (cbTargetType->GetValue() != _("Public synonym"))
	{
		pgSet *schemas;
		if (connection->BackendMinimumVersion(8, 2))
			schemas = connection->ExecuteSet(wxT("SELECT nspname FROM pg_namespace WHERE nspparent = 0 AND nspname NOT LIKE E'pg\\_%' AND nspname NOT IN ('pg_catalog', 'sys', 'dbo', 'pgagent', 'information_schema', 'dbms_job_procedure') ORDER BY nspname;"));
		else if (connection->BackendMinimumVersion(8, 1))
			schemas = connection->ExecuteSet(wxT("SELECT nspname FROM pg_namespace WHERE nspname NOT LIKE E'pg\\_%' ORDER BY nspname;"));
		else
			schemas = connection->ExecuteSet(wxT("SELECT nspname FROM pg_namespace WHERE nspname NOT LIKE 'pg\\_%' ORDER BY nspname;"));

		for (int x = 0; x < schemas->NumRows(); x++)
		{
			cbTargetSchema->Append(schemas->GetVal(0));
			schemas->MoveNext();
		}
		delete schemas;

		cbTargetSchema->Enable();
		cbTargetSchema->SetValue(wxT("public"));
		ProcessSchemaChange();
	}
	else
	{
		pgSet *synonyms = connection->ExecuteSet(wxT("SELECT synname FROM pg_synonym ORDER BY synname;"));
		for (int x = 0; x < synonyms->NumRows(); x++)
		{
			cbTargetObject->Append(synonyms->GetVal(0));
			synonyms->MoveNext();
		}
		delete synonyms;

		cbTargetSchema->Disable();
		cbTargetObject->Enable();
	}
}

void dlgSynonym::ProcessSchemaChange()
{
	cbTargetObject->Clear();

	wxString restriction;
	if (cbTargetType->GetValue() == _("Sequence"))
		restriction = wxT("S");
	else if (cbTargetType->GetValue() == _("Table"))
		restriction = wxT("r");
	else if (cbTargetType->GetValue() == _("View"))
		restriction = wxT("v");
	else if (cbTargetType->GetValue() == _("Function"))
		restriction = wxT("0");
	else if (cbTargetType->GetValue() == _("Procedure"))
		restriction = wxT("1");

	wxString sql;
	if (cbTargetType->GetValue() == _("Synonym"))
	{
		sql = wxT("SELECT synname FROM pg_synonym s JOIN pg_namespace n\n")
		      wxT("  ON s.synnamespace = n.oid AND \n")
		      wxT("     n.nspname = ") + qtDbString(cbTargetSchema->GetValue()) +
		      wxT("  ORDER BY synname;");
	}
	else if (cbTargetType->GetValue() == _("Function") ||
	         cbTargetType->GetValue() == _("Procedure"))
	{
		// "protype" is available, no need to check for version again here..
		sql = wxT("SELECT DISTINCT proname from pg_proc p, pg_namespace n\n")
		      wxT("  WHERE p.pronamespace = n.oid AND\n")
		      wxT("        n.nspname = ") + qtDbString(cbTargetSchema->GetValue()) + wxT(" AND\n")
		      wxT("        p.protype  = '") + restriction + wxT("' ORDER BY proname;");
	}
	else if (cbTargetType->GetValue() == _("View"))
	{
		wxString mviewRestriction = wxT("m");
		sql = wxT("SELECT relname FROM pg_class c, pg_namespace n\n")
		      wxT("  WHERE c.relnamespace = n.oid AND\n")
		      wxT("        n.nspname = ") + qtDbString(cbTargetSchema->GetValue()) + wxT(" AND\n")
		      wxT("        (c.relkind = '") + restriction + wxT("' OR c.relkind = '") + mviewRestriction + wxT("') ORDER BY relname;");
	}
	else
	{
		sql = wxT("SELECT relname FROM pg_class c, pg_namespace n\n")
		      wxT("  WHERE c.relnamespace = n.oid AND\n")
		      wxT("        n.nspname = ") + qtDbString(cbTargetSchema->GetValue()) + wxT(" AND\n")
		      wxT("        c.relkind = '") + restriction + wxT("' ORDER BY relname;");
	}

	pgSet *objects = connection->ExecuteSet(sql);
	for (int x = 0; x < objects->NumRows(); x++)
	{
		cbTargetObject->Append(objects->GetVal(0));
		objects->MoveNext();
	}
	delete objects;

	cbTargetObject->Enable();
}


wxString dlgSynonym::GetSql()
{
	wxString sql;

	if (!synonymSchema)
	{

		sql = wxT("CREATE OR REPLACE PUBLIC SYNONYM ") + qtIdent(txtName->GetValue()) + wxT("\n FOR ");

		if (cbTargetSchema->GetValue() != wxEmptyString)
			sql += qtIdent(cbTargetSchema->GetValue()) + wxT(".");

		sql += qtIdent(cbTargetObject->GetValue()) + wxT(";\n");

		AppendComment(sql, wxT("PUBLIC SYNONYM ") + qtIdent(txtName->GetValue()), synonym);
	}
	else
	{
		wxString createSql, commentSql;
		if (synonymSchema->GetName() == wxT("public"))
		{
			createSql = wxT("CREATE OR REPLACE PUBLIC SYNONYM ");
			commentSql = wxT("PUBLIC SYNONYM ");
		}
		else
		{
			createSql = wxT("CREATE OR REPLACE SYNONYM ") + qtIdent(synonymSchema->GetName()) + wxT(".");
			commentSql = wxT("PRIVATE SYNONYM ");
		}

		sql = createSql + qtIdent(txtName->GetValue()) + wxT("\n FOR ");

		if (cbTargetSchema->GetValue() != wxEmptyString)
			sql += qtIdent(cbTargetSchema->GetValue()) + wxT(".");

		sql += qtIdent(cbTargetObject->GetValue()) + wxT(";\n");

		AppendComment(sql, commentSql + qtIdent(txtName->GetValue()), synonym);
	}

	return sql;
}
