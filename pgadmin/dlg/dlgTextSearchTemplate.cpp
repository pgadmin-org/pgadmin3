//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTextSearchTemplate.cpp - PostgreSQL Text Search Template Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgTextSearchTemplate.h"
#include "schema/pgSchema.h"
#include "schema/pgTextSearchTemplate.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define cbInit              CTRL_COMBOBOX2("cbInit")
#define cbLexize            CTRL_COMBOBOX2("cbLexize")


BEGIN_EVENT_TABLE(dlgTextSearchTemplate, dlgTypeProperty)
	EVT_TEXT(XRCID("cbInit"),                   dlgTextSearchTemplate::OnChange)
	EVT_COMBOBOX(XRCID("cbInit"),               dlgTextSearchTemplate::OnChange)
	EVT_TEXT(XRCID("cbLexize"),                 dlgTextSearchTemplate::OnChange)
	EVT_COMBOBOX(XRCID("cbLexize"),             dlgTextSearchTemplate::OnChange)
END_EVENT_TABLE();



dlgProperty *pgTextSearchTemplateFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTextSearchTemplate(this, frame, (pgTextSearchTemplate *)node, (pgSchema *)parent);
}

dlgTextSearchTemplate::dlgTextSearchTemplate(pgaFactory *f, frmMain *frame, pgTextSearchTemplate *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgTextSearchTemplate"))
{
	schema = sch;
	tmpl = node;
}


pgObject *dlgTextSearchTemplate::GetObject()
{
	return tmpl;
}


int dlgTextSearchTemplate::Go(bool modal)
{
	wxString qry;
	pgSet *set;

	cbInit->Append(wxT(""));

	qry = wxT("SELECT proname, nspname\n")
	      wxT("  FROM pg_proc\n")
	      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
	      wxT("  WHERE prorettype=2281 and proargtypes='2281'\n")
	      wxT("  ORDER BY proname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbInit->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	qry = wxT("SELECT proname, nspname\n")
	      wxT("  FROM pg_proc\n")
	      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
	      wxT("  WHERE prorettype=2281 and proargtypes='2281 2281 2281 2281'\n")
	      wxT("  ORDER BY proname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbLexize->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	if (tmpl)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(9, 1));
		cbInit->SetValue(tmpl->GetInit());
		cbInit->Disable();
		cbLexize->SetValue(tmpl->GetLexize());
		cbLexize->Disable();
	}
	else
	{
		// create mode
	}

	cbOwner->Disable();

	return dlgProperty::Go(modal);
}


pgObject *dlgTextSearchTemplate::CreateObject(pgCollection *collection)
{
	pgObject *obj = textSearchTemplateFactory.CreateObjects(collection, 0,
	                wxT("\n   AND tmpl.tmplname=") + qtDbString(GetName()) +
	                wxT("\n   AND tmpl.tmplnamespace=") + schema->GetOidStr());

	return obj;
}


void dlgTextSearchTemplate::CheckChange()
{
	if (tmpl)
	{
		EnableOK(txtName->GetValue() != tmpl->GetName()
		         || cbSchema->GetValue() != tmpl->GetSchema()->GetName()
		         || txtComment->GetValue() != tmpl->GetComment());
	}
	else
	{
		wxString name = GetName();
		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbLexize->GetGuessedSelection() > 0 , _("Please select a lexize function."));

		EnableOK(enable);
	}
}


void dlgTextSearchTemplate::OnChange(wxCommandEvent &ev)
{
	CheckChange();
}


wxString dlgTextSearchTemplate::GetSql()
{
	wxString sql;
	wxString objname;

	if (tmpl)
	{
		// edit mode
		objname = schema->GetQuotedPrefix() + qtIdent(GetName());
		AppendNameChange(sql, wxT("TEXT SEARCH TEMPLATE ") + tmpl->GetQuotedFullIdentifier());
		AppendSchemaChange(sql, wxT("TEXT SEARCH TEMPLATE ") + objname);
	}
	else
	{
		// create mode
		objname = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());
		sql = wxT("CREATE TEXT SEARCH TEMPLATE ")
		      + objname
		      + wxT(" (");

		AppendIfFilled(sql, wxT("\n   INIT="), cbInit->GetValue());
		if (cbInit->GetValue().Length() > 0)
			sql += wxT(",");
		AppendIfFilled(sql, wxT("\n   LEXIZE="), cbLexize->GetValue());

		sql += wxT("\n);\n");

	}

	AppendComment(sql, wxT("TEXT SEARCH TEMPLATE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), tmpl);

	return sql;
}
