//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgForeignServer.cpp - PostgreSQL Foreign Server Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgForeignServer.h"
#include "schema/pgForeignServer.h"


// pointer to controls
#define cbForeignDataWrapper CTRL_COMBOBOX("cbForeignDataWrapper")
#define txtType              CTRL_TEXT("txtType")
#define txtVersion           CTRL_TEXT("txtVersion")
#define lstOptions           CTRL_LISTVIEW("lstOptions")
#define txtOption            CTRL_TEXT("txtOption")
#define txtValue             CTRL_TEXT("txtValue")
#define btnAdd               CTRL_BUTTON("wxID_ADD")
#define btnRemove            CTRL_BUTTON("wxID_REMOVE")


dlgProperty *pgForeignServerFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgForeignServer(this, frame, (pgForeignServer *)node, (pgForeignDataWrapper *)parent);
}


BEGIN_EVENT_TABLE(dlgForeignServer, dlgSecurityProperty)
	EVT_LIST_ITEM_SELECTED(XRCID("lstOptions"), dlgForeignServer::OnSelChangeOption)
	EVT_TEXT(XRCID("txtOption"),                dlgForeignServer::OnChangeOptionName)
	EVT_BUTTON(wxID_ADD,                        dlgForeignServer::OnAddOption)
	EVT_BUTTON(wxID_REMOVE,                     dlgForeignServer::OnRemoveOption)
END_EVENT_TABLE();


dlgForeignServer::dlgForeignServer(pgaFactory *f, frmMain *frame, pgForeignServer *node, pgForeignDataWrapper *parent)
	: dlgSecurityProperty(f, frame, node, wxT("dlgForeignServer"), wxT("USAGE"), "U")
{
	foreigndatawrapper = parent;
	foreignserver = node;
}


pgObject *dlgForeignServer::GetObject()
{
	return foreignserver;
}


int dlgForeignServer::Go(bool modal)
{
	// Initialize options listview and buttons
	lstOptions->AddColumn(_("Option"), 80);
	lstOptions->AddColumn(_("Value"), 40);
	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnAdd->Disable();
	btnRemove->Disable();

	if (foreignserver)
	{
		// edit mode
		txtName->Enable(connection->BackendMinimumVersion(9, 2));
		txtType->Disable();

		txtType->SetValue(foreignserver->GetType());
		txtVersion->SetValue(foreignserver->GetVersion());

		wxString options = foreignserver->GetOptions();
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

	return dlgSecurityProperty::Go(modal);
}


pgObject *dlgForeignServer::CreateObject(pgCollection *collection)
{
	wxString name = txtName->GetValue();

	pgObject *obj = foreignServerFactory.CreateObjects(collection, 0, wxT("\n   AND srvname ILIKE ") + qtDbString(name));
	return obj;
}


void dlgForeignServer::CheckChange()
{
	bool didChange = true;
	wxString name = txtName->GetValue();
	if (foreignserver)
	{
		didChange = name != foreignserver->GetName()
		            || txtComment->GetValue() != foreignserver->GetComment()
		            || cbOwner->GetValue() != foreignserver->GetOwner()
		            || txtType->GetValue() != foreignserver->GetType()
		            || txtVersion->GetValue() != foreignserver->GetVersion()
		            || GetOptionsSql().Length() > 0;
		EnableOK(didChange);
	}
	else
	{
		bool enable = true;

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		EnableOK(enable);
	}
}



void dlgForeignServer::OnChange(wxCommandEvent &ev)
{
	CheckChange();
}


void dlgForeignServer::OnChangeOptionName(wxCommandEvent &ev)
{
	btnAdd->Enable(txtOption->GetValue().Length() > 0);
}


void dlgForeignServer::OnSelChangeOption(wxListEvent &ev)
{
	int row = lstOptions->GetSelection();
	if (row >= 0)
	{
		txtOption->SetValue(lstOptions->GetText(row, 0));
		txtValue->SetValue(lstOptions->GetText(row, 1));
	}

	btnRemove->Enable(row >= 0);
}


void dlgForeignServer::OnAddOption(wxCommandEvent &ev)
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


void dlgForeignServer::OnRemoveOption(wxCommandEvent &ev)
{
	int sel = lstOptions->GetSelection();
	lstOptions->DeleteItem(sel);

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnRemove->Disable();

	CheckChange();
}


wxString dlgForeignServer::GetOptionsSql()
{
	wxString options = foreignserver->GetOptions();
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
		options = foreignserver->GetOptions();
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


wxString dlgForeignServer::GetSql()
{
	wxString sql, name;
	name = txtName->GetValue();

	if (foreignserver)
	{
		// edit mode
		AppendNameChange(sql, wxT("SERVER ") + qtIdent(foreignserver->GetName()));

		if (txtVersion->GetValue() != foreignserver->GetVersion())
		{
			sql = wxT("ALTER SERVER ") + qtIdent(name)
			      + wxT("\n  VERSION ") + qtDbString(txtVersion->GetValue()) + wxT(";\n");
		}

		wxString sqloptions = GetOptionsSql();
		if (sqloptions.Length() > 0)
		{
			sql += wxT("ALTER SERVER ") + name
			       + wxT("\n  OPTIONS (") + sqloptions + wxT(");\n");
		}

		AppendOwnerChange(sql, wxT("SERVER ") + qtIdent(name));
	}
	else
	{
		// create mode
		sql = wxT("CREATE SERVER ") + qtIdent(name);
		if (!(txtType->GetValue()).IsEmpty())
			sql += wxT("\n   TYPE ") + qtDbString(txtType->GetValue());
		if (!(txtVersion->GetValue()).IsEmpty())
			sql += wxT("\n   VERSION ") + qtDbString(txtVersion->GetValue());
		sql += wxT("\n   FOREIGN DATA WRAPPER ") + qtIdent(foreigndatawrapper->GetName());

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
		AppendOwnerNew(sql, wxT("SERVER ") + qtIdent(name));
	}

	sql += GetGrant(wxT("U"), wxT("SERVER ") + qtIdent(name));
	AppendComment(sql, wxT("SERVER"), 0, foreignserver);

	return sql;
}



