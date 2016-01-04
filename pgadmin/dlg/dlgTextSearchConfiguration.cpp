//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTextSearchConfiguration.cpp - PostgreSQL Text Search Configuration Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgTextSearchConfiguration.h"
#include "schema/pgSchema.h"
#include "schema/pgTextSearchConfiguration.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define cbParser            CTRL_COMBOBOX2("cbParser")
#define cbCopy              CTRL_COMBOBOX2("cbCopy")
#define lstTokens           CTRL_LISTVIEW("lstTokens")
#define cbToken             CTRL_COMBOBOX2("cbToken")
#define txtDictionary       CTRL_TEXT("txtDictionary")
#define cbDictionary        CTRL_CHOICE("cbDictionary")
#define btnAdd              CTRL_BUTTON("wxID_ADD")
#define btnRemove           CTRL_BUTTON("wxID_REMOVE")


BEGIN_EVENT_TABLE(dlgTextSearchConfiguration, dlgTypeProperty)
	EVT_TEXT(XRCID("cbParser"),                 dlgTextSearchConfiguration::OnChange)
	EVT_COMBOBOX(XRCID("cbParser"),             dlgTextSearchConfiguration::OnChange)
	EVT_TEXT(XRCID("cbCopy"),                   dlgTextSearchConfiguration::OnChange)
	EVT_COMBOBOX(XRCID("cbCopy"),               dlgTextSearchConfiguration::OnChange)
	EVT_LIST_ITEM_SELECTED(XRCID("lstTokens"),  dlgTextSearchConfiguration::OnSelChangeToken)
	EVT_TEXT(XRCID("cbToken"),                  dlgTextSearchConfiguration::OnChangeCbToken)
	EVT_COMBOBOX(XRCID("cbToken"),              dlgTextSearchConfiguration::OnChangeCbToken)
	EVT_TEXT(XRCID("txtDictionary"),            dlgTextSearchConfiguration::OnChangeTxtDictionary)
	EVT_CHOICE(XRCID("cbDictionary"),           dlgTextSearchConfiguration::OnChangeCbDictionary)
	EVT_BUTTON(wxID_ADD,                        dlgTextSearchConfiguration::OnAddToken)
	EVT_BUTTON(wxID_REMOVE,                     dlgTextSearchConfiguration::OnRemoveToken)
#ifdef __WXMAC__
	EVT_SIZE(                                   dlgTextSearchConfiguration::OnChangeSize)
#endif
END_EVENT_TABLE();



dlgProperty *pgTextSearchConfigurationFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTextSearchConfiguration(this, frame, (pgTextSearchConfiguration *)node, (pgSchema *)parent);
}

dlgTextSearchConfiguration::dlgTextSearchConfiguration(pgaFactory *f, frmMain *frame, pgTextSearchConfiguration *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgTextSearchConfiguration"))
{
	schema = sch;
	config = node;
	dirtyTokens = false;

	lstTokens->CreateColumns(0, _("Token"), _("Dictionaries"));

	cbCopy->Disable();
}


pgObject *dlgTextSearchConfiguration::GetObject()
{
	return config;
}


int dlgTextSearchConfiguration::Go(bool modal)
{
	wxString qry;
	pgSet *set;

	cbParser->Append(wxT(""));

	qry = wxT("SELECT prsname, nspname\n")
	      wxT("  FROM pg_ts_parser\n")
	      wxT("  JOIN pg_namespace n ON n.oid=prsnamespace\n")
	      wxT("  ORDER BY prsname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("prsname"));
			cbParser->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	cbCopy->Append(wxT(""));

	qry = wxT("SELECT cfgname, nspname\n")
	      wxT("  FROM pg_ts_config\n")
	      wxT("  JOIN pg_namespace n ON n.oid=cfgnamespace\n")
	      wxT("  ORDER BY nspname, cfgname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		cbCopy->Enable();
		while (!set->Eof())
		{
			wxString configname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("cfgname"));
			cbCopy->Append(configname);
			set->MoveNext();
		}
		delete set;
	}

	if (config)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(9, 1));
		cbParser->SetValue(config->GetParser());
		cbCopy->Disable();

		// second tab handling
		size_t i;
		for (i = 0 ; i < config->GetTokens().GetCount() ; i++)
		{
			wxString token = config->GetTokens().Item(i);
			lstTokens->AppendItem(token.BeforeFirst('/'), token.AfterFirst('/'));
		}

		pgSet *tokens;
		tokens = connection->ExecuteSet(
		             wxT("SELECT alias FROM ts_token_type(")
		             + config->GetParserOidStr()
		             + wxT(") ORDER BY alias"));

		if (tokens)
		{
			while (!tokens->Eof())
			{
				cbToken->Append(tokens->GetVal(wxT("alias")));
				tokens->MoveNext();
			}
			delete tokens;
		}

		pgSet *dictionaries;
		dictionaries = connection->ExecuteSet(
		                   wxT("SELECT dictname FROM pg_ts_dict ORDER BY dictname"));

		if (dictionaries)
		{
			while (!dictionaries->Eof())
			{
				cbDictionary->Append(dictionaries->GetVal(wxT("dictname")));
				dictionaries->MoveNext();
			}
			delete dictionaries;
		}

		if (!connection->BackendMinimumVersion(8, 0))
			cbOwner->Disable();
	}
	else
	{
		// create mode
	}

	btnAdd->Disable();
	btnRemove->Disable();

	return dlgProperty::Go(modal);
}


pgObject *dlgTextSearchConfiguration::CreateObject(pgCollection *collection)
{
	pgObject *obj = textSearchConfigurationFactory.CreateObjects(collection, 0,
	                wxT("\n   AND cfg.cfgname=") + qtDbString(GetName()) +
	                wxT("\n   AND cfg.cfgnamespace=") + schema->GetOidStr());

	return obj;
}


#ifdef __WXMAC__
void dlgTextSearchConfiguration::OnChangeSize(wxSizeEvent &ev)
{
	lstTokens->SetSize(wxDefaultCoord, wxDefaultCoord,
	                   ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgTextSearchConfiguration::CheckChange()
{
	if (config)
	{
		EnableOK(txtName->GetValue() != config->GetName()
		         || cbSchema->GetValue() != config->GetSchema()->GetName()
		         || txtComment->GetValue() != config->GetComment()
		         || cbOwner->GetValue() != config->GetOwner()
		         || dirtyTokens);
	}
	else
	{
		wxString name = GetName();
		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbParser->GetGuessedSelection() > 0 || cbCopy->GetGuessedSelection() > 0 , _("Please select a parser or a configuration to copy."));

		EnableOK(enable);
	}
}


void dlgTextSearchConfiguration::OnChange(wxCommandEvent &ev)
{
	cbParser->Enable(cbCopy->GetValue().Length() == 0);
	cbCopy->Enable(cbParser->GetValue().Length() == 0);

	CheckChange();
}

void dlgTextSearchConfiguration::OnChangeCbToken(wxCommandEvent &ev)
{
	bool found = false;

	for (int pos = 0 ; pos < lstTokens->GetItemCount() ; pos++)
	{
		if (lstTokens->GetText(pos).IsSameAs(cbToken->GetValue(), false))
		{
			lstTokens->Select(pos);
			found = true;
			break;
		}
	}

	btnAdd->Enable(cbToken->GetValue().Length() > 0);
}


void dlgTextSearchConfiguration::OnChangeCbDictionary(wxCommandEvent &ev)
{
	if (!txtDictionary->GetValue().Matches(wxT("*") + cbDictionary->GetStringSelection() + wxT("*")))
	{
		wxString dicts = txtDictionary->GetValue();
		if (dicts.Length() > 0)
			dicts += wxT(",");
		dicts += cbDictionary->GetStringSelection();

		txtDictionary->SetValue(dicts);
	}
	btnAdd->Enable(cbToken->GetValue().Length() > 0);
}


void dlgTextSearchConfiguration::OnChangeTxtDictionary(wxCommandEvent &ev)
{
	btnAdd->Enable(cbToken->GetValue().Length() > 0);
}


void dlgTextSearchConfiguration::OnSelChangeToken(wxListEvent &ev)
{
	int row = lstTokens->GetSelection();
	if (row >= 0)
	{
		cbToken->SetValue(lstTokens->GetText(row, 0));
		txtDictionary->SetValue(lstTokens->GetText(row, 1));
	}

	btnAdd->Enable(cbToken->GetValue().Length() > 0);
	btnRemove->Enable(row >= 0);
}


void dlgTextSearchConfiguration::OnAddToken(wxCommandEvent &ev)
{
	bool found = false;

	for (int pos = 0 ; pos < lstTokens->GetItemCount() ; pos++)
	{
		if (lstTokens->GetText(pos).IsSameAs(cbToken->GetValue(), false))
		{
			lstTokens->SetItem(pos, 1, txtDictionary->GetValue());
			found = true;
			break;
		}
	}

	if (!found)
	{
		lstTokens->AppendItem(cbToken->GetValue(), txtDictionary->GetValue());
	}

	btnAdd->Disable();

	dirtyTokens = true;

	CheckChange();
}


void dlgTextSearchConfiguration::OnRemoveToken(wxCommandEvent &ev)
{
	for (int pos = 0 ; pos < lstTokens->GetItemCount() ; pos++)
	{
		if (lstTokens->GetText(pos).IsSameAs(cbToken->GetValue(), false))
		{
			lstTokens->DeleteItem(pos);
			break;
		}
	}

	cbToken->SetValue(wxT(""));
	txtDictionary->SetValue(wxT(""));

	btnRemove->Disable();

	dirtyTokens = true;

	CheckChange();
}


wxString dlgTextSearchConfiguration::GetSql()
{
	wxString sql;
	wxString objname;

	if (config)
	{
		objname = schema->GetQuotedPrefix() + qtIdent(config->GetName());
	}
	else
	{
		objname = schema->GetQuotedPrefix() + qtIdent(GetName());
	}

	if (cbParser->GetValue().Length() > 0)
	{
		wxArrayString toks;
		size_t index;

		if (config)
		{
			for (index = 0 ; index < config->GetTokens().GetCount() ; index++)
				toks.Add(config->GetTokens().Item(index));
		}

		int cnt = lstTokens->GetItemCount();
		int pos;

		// check for changed or added tokens
		for (pos = 0 ; pos < cnt ; pos++)
		{
			wxString newTok = lstTokens->GetText(pos);
			wxString newVal = lstTokens->GetText(pos, 1);

			wxString oldVal;

			for (index = 0 ; index < toks.GetCount() ; index++)
			{
				wxString tok = toks.Item(index);
				if (tok.BeforeFirst('/').IsSameAs(newTok, false))
				{
					oldVal = tok.Mid(newTok.Length() + 1);
					toks.RemoveAt(index);
					break;
				}
			}
			if (oldVal != newVal)
			{
				if (oldVal.Length() == 0)
				{
					sql += wxT("ALTER TEXT SEARCH CONFIGURATION ") + objname
					       +  wxT("\n  ADD MAPPING FOR ") + newTok
					       +  wxT("\n  WITH ") + newVal
					       +  wxT(";\n");
				}
				else
				{
					sql += wxT("ALTER TEXT SEARCH CONFIGURATION ") + objname
					       +  wxT("\n  ALTER MAPPING FOR ") + newTok
					       +  wxT("\n  WITH ") + newVal
					       +  wxT(";\n");
				}
			}
		}

		// check for removed tokens
		wxString oldTok;
		for (pos = 0 ; pos < (int)toks.GetCount() ; pos++)
		{
			if (!toks.Item(pos).BeforeFirst('/').IsSameAs(oldTok, false))
			{
				oldTok = toks.Item(pos).BeforeFirst('/');
				sql += wxT("ALTER TEXT SEARCH CONFIGURATION ") + objname
				       +  wxT(" DROP MAPPING FOR ") + oldTok
				       + wxT(";\n");
			}
		}
	}

	if (config)
	{
		// edit mode
		objname = schema->GetQuotedPrefix() + qtIdent(GetName());
		AppendNameChange(sql, wxT("TEXT SEARCH CONFIGURATION ") + config->GetQuotedFullIdentifier());
		AppendOwnerChange(sql, wxT("TEXT SEARCH CONFIGURATION ") + objname);
		AppendSchemaChange(sql, wxT("TEXT SEARCH CONFIGURATION ") + objname);
	}
	else
	{
		// create mode
		objname = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());
		sql = wxT("CREATE TEXT SEARCH CONFIGURATION ")
		      + objname
		      + wxT(" (");

		AppendIfFilled(sql, wxT("\n   PARSER="), cbParser->GetValue());
		AppendIfFilled(sql, wxT("\n   COPY="), cbCopy->GetValue());

		sql += wxT("\n);\n");

	}

	AppendComment(sql, wxT("TEXT SEARCH CONFIGURATION ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), config);

	return sql;
}
