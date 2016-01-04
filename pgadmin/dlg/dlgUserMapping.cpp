//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgUserMapping.cpp - PostgreSQL User Mapping Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgUserMapping.h"
#include "schema/pgUserMapping.h"


// pointer to controls
#define cbUser               CTRL_COMBOBOX("cbUser")
#define lstOptions           CTRL_LISTVIEW("lstOptions")
#define txtOption            CTRL_TEXT("txtOption")
#define txtValue             CTRL_TEXT("txtValue")
#define btnAdd               CTRL_BUTTON("wxID_ADD")
#define btnRemove            CTRL_BUTTON("wxID_REMOVE")


dlgProperty *pgUserMappingFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgUserMapping(this, frame, (pgUserMapping *)node, (pgForeignServer *)parent);
}


BEGIN_EVENT_TABLE(dlgUserMapping, dlgProperty)
	EVT_TEXT(XRCID("cbUser"),                   dlgUserMapping::OnChange)
	EVT_COMBOBOX(XRCID("cbUser"),               dlgUserMapping::OnChange)
	EVT_LIST_ITEM_SELECTED(XRCID("lstOptions"), dlgUserMapping::OnSelChangeOption)
	EVT_TEXT(XRCID("txtOption"),                dlgUserMapping::OnChangeOptionName)
	EVT_BUTTON(wxID_ADD,                        dlgUserMapping::OnAddOption)
	EVT_BUTTON(wxID_REMOVE,                     dlgUserMapping::OnRemoveOption)
END_EVENT_TABLE();


dlgUserMapping::dlgUserMapping(pgaFactory *f, frmMain *frame, pgUserMapping *node, pgForeignServer *parent)
	: dlgProperty(f, frame, wxT("dlgUserMapping"))
{
	foreignserver = parent;
	usermapping = node;
}


pgObject *dlgUserMapping::GetObject()
{
	return usermapping;
}


int dlgUserMapping::Go(bool modal)
{
	// Fill user combobox
	cbUser->Append(wxT("CURRENT_USER"));
	cbUser->Append(wxT("PUBLIC"));
	pgSet *set = connection->ExecuteSet(
	                 wxT("SELECT rolname\n")
	                 wxT("  FROM pg_roles\n")
	                 wxT("  ORDER BY rolname"));
	if (set)
	{
		while (!set->Eof())
		{
			wxString rolname = set->GetVal(wxT("rolname"));
			cbUser->Append(rolname);
			set->MoveNext();
		}
		delete set;
	}
	cbUser->SetSelection(0);

	// Initialize options listview and buttons
	lstOptions->AddColumn(_("Option"), 80);
	lstOptions->AddColumn(_("Value"), 40);
	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnAdd->Disable();
	btnRemove->Disable();

	if (usermapping)
	{
		// edit mode
		cbUser->SetValue(usermapping->GetUsr());
		cbUser->Disable();

		wxString options = usermapping->GetOptions();
		wxString option, optionname, optionvalue;
		while (options.Length() > 0)
		{
			option = options.BeforeFirst(',');
			optionname = option.BeforeFirst(wxT('=')).Trim(false).Trim();
			optionvalue = option.AfterFirst(wxT('=')).Trim(false).Trim();
			lstOptions->AppendItem(optionname, optionvalue);
			options = options.AfterFirst(',');
		}
	}
	else
	{
		// create mode
	}

	txtComment->Disable();

	return dlgProperty::Go(modal);
}


pgObject *dlgUserMapping::CreateObject(pgCollection *collection)
{
	pgObject *obj = userMappingFactory.CreateObjects(collection, 0, wxT("\n   AND true")); //srvname ILIKE ") + qtDbString(name));
	return obj;
}


void dlgUserMapping::CheckChange()
{
	bool didChange = true;
	if (usermapping)
	{
		didChange = GetOptionsSql().Length() > 0;
		EnableOK(didChange);
	}
	else
	{
		bool enable = true;

		CheckValid(enable, !cbUser->GetValue().IsEmpty(), _("Please specify user."));
		EnableOK(enable);
	}
}



void dlgUserMapping::OnChange(wxCommandEvent &ev)
{
	CheckChange();
}


void dlgUserMapping::OnChangeOptionName(wxCommandEvent &ev)
{
	btnAdd->Enable(txtOption->GetValue().Length() > 0);
}


void dlgUserMapping::OnSelChangeOption(wxListEvent &ev)
{
	int row = lstOptions->GetSelection();
	if (row >= 0)
	{
		txtOption->SetValue(lstOptions->GetText(row, 0));
		txtValue->SetValue(lstOptions->GetText(row, 1));
	}

	btnRemove->Enable(row >= 0);
}


void dlgUserMapping::OnAddOption(wxCommandEvent &ev)
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


void dlgUserMapping::OnRemoveOption(wxCommandEvent &ev)
{
	int sel = lstOptions->GetSelection();
	lstOptions->DeleteItem(sel);

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnRemove->Disable();

	CheckChange();
}


wxString dlgUserMapping::GetOptionsSql()
{
	wxString options = usermapping->GetOptions();
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
				sqloptions += wxT("SET ") + optionname + wxT(" '") + lstOptions->GetText(pos, 1) + wxT("'");
			}
		}
		else
		{
			if (sqloptions.Length() > 0)
				sqloptions += wxT(", ");
			sqloptions += wxT("DROP ") + optionname;
		}

		options = options.AfterFirst(',');
	}

	for (pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
	{
		options = usermapping->GetOptions();
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
			sqloptions += wxT("ADD ") + lstOptions->GetText(pos, 0) + wxT(" '") + lstOptions->GetText(pos, 1) + wxT("'");
		}
	}

	return sqloptions;
}


wxString dlgUserMapping::GetSql()
{
	wxString sql;

	if (usermapping)
	{
		// edit mode
		wxString sqloptions = GetOptionsSql();
		if (sqloptions.Length() > 0)
		{
			sql += wxT("ALTER USER MAPPING FOR ") + usermapping->GetUsr() + wxT(" SERVER ") + qtIdent(foreignserver->GetName())
			       + wxT("\n  OPTIONS (") + sqloptions + wxT(");\n");
		}
	}
	else
	{
		// create mode
		sql = wxT("CREATE USER MAPPING FOR ") + cbUser->GetValue() + wxT(" SERVER ") + qtIdent(foreignserver->GetName());

		// check for options
		if (lstOptions->GetItemCount() > 0)
		{
			wxString options = wxEmptyString;
			for (int pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
			{
				if (options.Length() > 0)
					options += wxT(", ");

				options += lstOptions->GetText(pos, 0)
				           + wxT(" '") + lstOptions->GetText(pos, 1) + wxT("' ");
			}
			sql += wxT("\n  OPTIONS (") + options + wxT(")");
		}

		sql += wxT(";\n");
	}

	return sql;
}



