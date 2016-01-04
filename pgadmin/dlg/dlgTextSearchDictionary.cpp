//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTextSearchDictionary.cpp - PostgreSQL Text Search Dictionary Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgTextSearchDictionary.h"
#include "schema/pgSchema.h"
#include "schema/pgTextSearchDictionary.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define cbTemplate          CTRL_COMBOBOX2("cbTemplate")
#define lstOptions          CTRL_LISTVIEW("lstOptions")
#define txtOption           CTRL_TEXT("txtOption")
#define txtValue            CTRL_TEXT("txtValue")
#define btnAdd              CTRL_BUTTON("wxID_ADD")
#define btnRemove           CTRL_BUTTON("wxID_REMOVE")


BEGIN_EVENT_TABLE(dlgTextSearchDictionary, dlgTypeProperty)
	EVT_TEXT(XRCID("cbTemplate"),               dlgTextSearchDictionary::OnChange)
	EVT_COMBOBOX(XRCID("cbTemplate"),           dlgTextSearchDictionary::OnChange)
	EVT_LIST_ITEM_SELECTED(XRCID("lstOptions"), dlgTextSearchDictionary::OnSelChangeOption)
	EVT_TEXT(XRCID("txtOption"),                dlgTextSearchDictionary::OnChangeOptionName)
	EVT_BUTTON(wxID_ADD,                        dlgTextSearchDictionary::OnAddOption)
	EVT_BUTTON(wxID_REMOVE,                     dlgTextSearchDictionary::OnRemoveOption)
#ifdef __WXMAC__
	EVT_SIZE(                                   dlgTextSearchDictionary::OnChangeSize)
#endif
END_EVENT_TABLE();



dlgProperty *pgTextSearchDictionaryFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTextSearchDictionary(this, frame, (pgTextSearchDictionary *)node, (pgSchema *)parent);
}

dlgTextSearchDictionary::dlgTextSearchDictionary(pgaFactory *f, frmMain *frame, pgTextSearchDictionary *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgTextSearchDictionary"))
{
	schema = sch;
	dict = node;
}


pgObject *dlgTextSearchDictionary::GetObject()
{
	return dict;
}


int dlgTextSearchDictionary::Go(bool modal)
{
	wxString qry;
	pgSet *set;

	qry = wxT("SELECT tmplname, nspname\n")
	      wxT("  FROM pg_ts_template\n")
	      wxT("  JOIN pg_namespace n ON n.oid=tmplnamespace\n")
	      wxT("  ORDER BY tmplname\n");

	set = connection->ExecuteSet(qry);
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("tmplname"));
			cbTemplate->Append(procname);
			set->MoveNext();
		}
		delete set;
	}

	lstOptions->AddColumn(_("Option"), 80);
	lstOptions->AddColumn(_("Value"), 40);

	if (dict)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(9, 1));
		cbTemplate->SetValue(dict->GetTemplate());
		cbTemplate->Disable();

		wxString options = dict->GetOptions();
		wxString option, optionname, optionvalue;
		while (options.Length() > 0)
		{
			option = options.BeforeFirst(',');
			optionname = option.BeforeFirst(wxT('=')).Trim(false).Trim();
			optionvalue = option.AfterFirst(wxT('=')).Trim(false).Trim();
			lstOptions->AppendItem(optionname, optionvalue);
			options = options.AfterFirst(',');
		}

		if (!connection->BackendMinimumVersion(8, 0))
			cbOwner->Disable();
	}
	else
	{
		// create mode
	}

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnAdd->Disable();
	btnRemove->Disable();

	return dlgProperty::Go(modal);
}


pgObject *dlgTextSearchDictionary::CreateObject(pgCollection *collection)
{
	pgObject *obj = textSearchDictionaryFactory.CreateObjects(collection, 0,
	                wxT("\n   AND dict.dictname=") + qtDbString(GetName()) +
	                wxT("\n   AND dict.dictnamespace=") + schema->GetOidStr());

	return obj;
}


#ifdef __WXMAC__
void dlgTextSearchDictionary::OnChangeSize(wxSizeEvent &ev)
{
	lstOptions->SetSize(wxDefaultCoord, wxDefaultCoord,
	                    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgTextSearchDictionary::CheckChange()
{
	if (dict)
	{
		EnableOK(txtName->GetValue() != dict->GetName()
		         || cbSchema->GetValue() != dict->GetSchema()->GetName()
		         || txtComment->GetValue() != dict->GetComment()
		         || cbOwner->GetValue() != dict->GetOwner()
		         || GetOptionsSql().Length() > 0);
	}
	else
	{
		wxString name = GetName();
		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbTemplate->GetValue().Length() > 0 , _("Please select a template."));

		EnableOK(enable);
	}
}


void dlgTextSearchDictionary::OnChange(wxCommandEvent &ev)
{
	CheckChange();
}


void dlgTextSearchDictionary::OnChangeOptionName(wxCommandEvent &ev)
{
	btnAdd->Enable(txtOption->GetValue().Length() > 0);
}


void dlgTextSearchDictionary::OnSelChangeOption(wxListEvent &ev)
{
	int row = lstOptions->GetSelection();
	if (row >= 0)
	{
		txtOption->SetValue(lstOptions->GetText(row, 0));
		txtValue->SetValue(lstOptions->GetText(row, 1));
	}

	btnRemove->Enable(row >= 0);
}


void dlgTextSearchDictionary::OnAddOption(wxCommandEvent &ev)
{
	bool found = false;

	for (int pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
	{
		if (lstOptions->GetText(pos).IsSameAs(txtOption->GetValue(), false))
		{
			lstOptions->SetItem(pos, 1, txtValue->GetValue());
			found = true;
			break;
		}
	}

	if (!found)
	{
		lstOptions->AppendItem(txtOption->GetValue(), txtValue->GetValue());
	}

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnAdd->Disable();

	CheckChange();
}


void dlgTextSearchDictionary::OnRemoveOption(wxCommandEvent &ev)
{
	int sel = lstOptions->GetSelection();
	lstOptions->DeleteItem(sel);

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnRemove->Disable();

	CheckChange();
}


wxString dlgTextSearchDictionary::GetOptionsSql()
{
	wxString options = dict->GetOptions();
	wxString option, optionname, optionvalue, sqloptions;
	bool found;
	int pos;

	while (options.Length() > 0)
	{
		option = options.BeforeFirst(',');
		optionname = option.BeforeFirst(wxT('=')).Trim(false).Trim();
		optionvalue = option.AfterFirst(wxT('=')).Trim(false).Trim();

		// check for options
		found = false;
		for (pos = 0 ; pos < lstOptions->GetItemCount() && !found; pos++)
		{
			found = lstOptions->GetText(pos, 0).Cmp(optionname) == 0;
			if (found) break;
		}

		if (found)
		{
			if (lstOptions->GetText(pos, 1).Cmp(optionvalue) != 0)
			{
				if (sqloptions.Length() > 0)
					sqloptions += wxT(", ");
				sqloptions += optionname + wxT("=") + lstOptions->GetText(pos, 1);
			}
		}
		else
		{
			if (sqloptions.Length() > 0)
				sqloptions += wxT(", ");
			sqloptions += optionname;
		}

		options = options.AfterFirst(',');
	}

	for (pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
	{
		options = dict->GetOptions();
		found = false;

		while (options.Length() > 0 && !found)
		{
			option = options.BeforeFirst(',');
			optionname = option.BeforeFirst(wxT('=')).Trim(false).Trim();
			found = lstOptions->GetText(pos, 0).Cmp(optionname) == 0;
			options = options.AfterFirst(',');
		}

		if (!found)
		{
			optionvalue = option.AfterFirst(wxT('=')).Trim(false).Trim();

			if (sqloptions.Length() > 0)
				sqloptions += wxT(", ");
			sqloptions += lstOptions->GetText(pos, 0) + wxT("=") + lstOptions->GetText(pos, 1);
		}
	}

	return sqloptions;
}


wxString dlgTextSearchDictionary::GetSql()
{
	wxString sql;
	wxString objname;

	if (dict)
	{
		// edit mode
		objname = schema->GetQuotedPrefix() + qtIdent(GetName());
		AppendNameChange(sql, wxT("TEXT SEARCH DICTIONARY ") + dict->GetQuotedFullIdentifier());

		wxString sqloptions = GetOptionsSql();
		if (sqloptions.Length() > 0)
		{
			sql += wxT("ALTER TEXT SEARCH DICTIONARY ") + objname
			       + wxT("\n  (") + sqloptions + wxT(");\n");
		}
		AppendOwnerChange(sql, wxT("TEXT SEARCH DICTIONARY ") + objname);

		AppendSchemaChange(sql, wxT("TEXT SEARCH DICTIONARY ") + objname);
	}
	else
	{
		// create mode
		objname = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());
		sql = wxT("CREATE TEXT SEARCH DICTIONARY ")
		      + objname
		      + wxT("\n  (")
		      + wxT("\n  TEMPLATE = ") + cbTemplate->GetValue();

		// check for options
		for (int pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
		{
			sql += wxT(", ") + lstOptions->GetText(pos, 0)
			       + wxT("=") + lstOptions->GetText(pos, 1);
		}

		sql += wxT("\n);\n");

		AppendOwnerNew(sql, wxT("TEXT SEARCH DICTIONARY ") + objname);
	}

	AppendComment(sql, wxT("TEXT SEARCH DICTIONARY ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), dict);

	return sql;
}
