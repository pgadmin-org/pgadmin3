//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTextSearchParser.cpp - PostgreSQL Text Search Parser Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgTextSearchParser.h"
#include "schema/pgSchema.h"
#include "schema/pgTextSearchParser.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define cbStart             CTRL_COMBOBOX2("cbStart")
#define cbGetToken          CTRL_COMBOBOX2("cbGetToken")
#define cbEnd               CTRL_COMBOBOX2("cbEnd")
#define cbLextypes          CTRL_COMBOBOX2("cbLextypes")
#define cbHeadline          CTRL_COMBOBOX2("cbHeadline")


BEGIN_EVENT_TABLE(dlgTextSearchParser, dlgTypeProperty)
	EVT_TEXT(XRCID("cbStart"),                  dlgTextSearchParser::OnChange)
	EVT_COMBOBOX(XRCID("cbStart"),              dlgTextSearchParser::OnChange)
	EVT_TEXT(XRCID("cbGetToken"),               dlgTextSearchParser::OnChange)
	EVT_COMBOBOX(XRCID("cbGetToken"),           dlgTextSearchParser::OnChange)
	EVT_TEXT(XRCID("cbEnd"),                    dlgTextSearchParser::OnChange)
	EVT_COMBOBOX(XRCID("cbEnd"),                dlgTextSearchParser::OnChange)
	EVT_TEXT(XRCID("cbLextypes"),               dlgTextSearchParser::OnChange)
	EVT_COMBOBOX(XRCID("cbLextypes"),           dlgTextSearchParser::OnChange)
	EVT_TEXT(XRCID("cbHeadline"),               dlgTextSearchParser::OnChange)
	EVT_COMBOBOX(XRCID("cbHeadline"),           dlgTextSearchParser::OnChange)
END_EVENT_TABLE();



dlgProperty *pgTextSearchParserFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTextSearchParser(this, frame, (pgTextSearchParser *)node, (pgSchema *)parent);
}

dlgTextSearchParser::dlgTextSearchParser(pgaFactory *f, frmMain *frame, pgTextSearchParser *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgTextSearchParser"))
{
	schema = sch;
	parser = node;
}


pgObject *dlgTextSearchParser::GetObject()
{
	return parser;
}


int dlgTextSearchParser::Go(bool modal)
{
	wxString qry;
	pgSet *set;

	qry = wxT("SELECT proname, nspname\n")
	      wxT("  FROM pg_proc\n")
	      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
	      wxT("  WHERE proargtypes='2281 23'\n")
	      wxT("  ORDER BY proname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbStart->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	qry = wxT("SELECT proname, nspname\n")
	      wxT("  FROM pg_proc\n")
	      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
	      wxT("  WHERE proargtypes='2281 2281 2281'\n")
	      wxT("  ORDER BY proname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbGetToken->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	qry = wxT("SELECT proname, nspname, prorettype\n")
	      wxT("  FROM pg_proc\n")
	      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
	      wxT("  WHERE prorettype=2278 AND proargtypes='2281'\n")
	      wxT("  ORDER BY proname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbEnd->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	qry = wxT("SELECT proname, nspname, prorettype\n")
	      wxT("  FROM pg_proc\n")
	      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
	      wxT("  WHERE prorettype=2281 AND proargtypes='2281'\n")
	      wxT("  ORDER BY proname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbLextypes->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	cbHeadline->Append(wxT(""));

	qry = wxT("SELECT proname, nspname\n")
	      wxT("  FROM pg_proc\n")
	      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
	      wxT("  WHERE proargtypes='2281 2281 3615'\n")
	      wxT("  ORDER BY proname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbHeadline->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	if (parser)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(9, 1));
		cbStart->SetValue(parser->GetStart());
		cbStart->Disable();
		cbGetToken->SetValue(parser->GetGettoken());
		cbGetToken->Disable();
		cbEnd->SetValue(parser->GetEnd());
		cbEnd->Disable();
		cbLextypes->SetValue(parser->GetLextypes());
		cbLextypes->Disable();
		cbHeadline->SetValue(parser->GetHeadline());
		cbHeadline->Disable();
	}
	else
	{
		// create mode
	}

	cbOwner->Disable();

	return dlgProperty::Go(modal);
}


pgObject *dlgTextSearchParser::CreateObject(pgCollection *collection)
{
	pgObject *obj = textSearchParserFactory.CreateObjects(collection, 0,
	                wxT("\n   AND prs.prsname=") + qtDbString(GetName()) +
	                wxT("\n   AND prs.prsnamespace=") + schema->GetOidStr());

	return obj;
}


void dlgTextSearchParser::CheckChange()
{
	if (parser)
	{
		EnableOK(txtName->GetValue() != parser->GetName()
		         || cbSchema->GetValue() != parser->GetSchema()->GetName()
		         || txtComment->GetValue() != parser->GetComment());
	}
	else
	{
		wxString name = GetName();
		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbStart->GetValue().Length() > 0 , _("Please select a start function."));
		CheckValid(enable, cbGetToken->GetValue().Length() > 0 , _("Please select a gettoken function."));
		CheckValid(enable, cbEnd->GetValue().Length() > 0 , _("Please select an end function."));
		CheckValid(enable, cbLextypes->GetValue().Length() > 0 , _("Please select a lextypes function."));

		EnableOK(enable);
	}
}


void dlgTextSearchParser::OnChange(wxCommandEvent &ev)
{
	CheckChange();
}


wxString dlgTextSearchParser::GetSql()
{
	wxString sql;
	wxString objname;

	if (parser)
	{
		// edit mode
		objname = schema->GetQuotedPrefix() + qtIdent(GetName());
		AppendNameChange(sql, wxT("TEXT SEARCH PARSER ") + parser->GetQuotedFullIdentifier());
		AppendSchemaChange(sql, wxT("TEXT SEARCH PARSER ") + objname);
	}
	else
	{
		// create mode
		objname = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());
		sql = wxT("CREATE TEXT SEARCH PARSER ")
		      + objname
		      + wxT(" (")
		      + wxT("\n  START = ") + cbStart->GetValue()
		      + wxT(",\n  GETTOKEN = ") + cbGetToken->GetValue()
		      + wxT(",\n  END = ") + cbEnd->GetValue()
		      + wxT(",\n  LEXTYPES = ") + cbLextypes->GetValue();

		AppendIfFilled(sql, wxT(",\n   HEADLINE="), cbHeadline->GetValue());

		sql += wxT("\n);\n");

	}
	AppendComment(sql, wxT("TEXT SEARCH PARSER ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), parser);

	return sql;
}
