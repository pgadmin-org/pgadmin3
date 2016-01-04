//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgForeignDataWrapper.cpp - PostgreSQL ForeignDataWrapper Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgForeignDataWrapper.h"
#include "schema/pgForeignDataWrapper.h"


// pointer to controls
#define cbHandler           CTRL_COMBOBOX("cbHandler")
#define cbValidator         CTRL_COMBOBOX("cbValidator")
#define lstOptions          CTRL_LISTVIEW("lstOptions")
#define txtOption           CTRL_TEXT("txtOption")
#define txtValue            CTRL_TEXT("txtValue")
#define btnAdd              CTRL_BUTTON("wxID_ADD")
#define btnRemove           CTRL_BUTTON("wxID_REMOVE")


dlgProperty *pgForeignDataWrapperFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgForeignDataWrapper(this, frame, (pgForeignDataWrapper *)node);
}


BEGIN_EVENT_TABLE(dlgForeignDataWrapper, dlgSecurityProperty)
	EVT_TEXT(XRCID("cbHandler"),                dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbHandler"),            dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbValidator"),              dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbValidator"),          dlgProperty::OnChange)
	EVT_LIST_ITEM_SELECTED(XRCID("lstOptions"), dlgForeignDataWrapper::OnSelChangeOption)
	EVT_TEXT(XRCID("txtOption"),                dlgForeignDataWrapper::OnChangeOptionName)
	EVT_BUTTON(wxID_ADD,                        dlgForeignDataWrapper::OnAddOption)
	EVT_BUTTON(wxID_REMOVE,                     dlgForeignDataWrapper::OnRemoveOption)
END_EVENT_TABLE();


dlgForeignDataWrapper::dlgForeignDataWrapper(pgaFactory *f, frmMain *frame, pgForeignDataWrapper *node)
	: dlgSecurityProperty(f, frame, node, wxT("dlgForeignDataWrapper"), wxT("USAGE"), "U")
{
	fdw = node;
}


pgObject *dlgForeignDataWrapper::GetObject()
{
	return fdw;
}


int dlgForeignDataWrapper::Go(bool modal)
{
	wxString val;

	if(!connection->BackendMinimumVersion(9, 1))
		cbHandler->Disable();

	// Fill handler combobox
	cbHandler->Append(wxT(""));
	pgSet *set = connection->ExecuteSet(
	                 wxT("SELECT nspname, proname\n")
	                 wxT("  FROM pg_proc p\n")
	                 wxT("  JOIN pg_namespace nsp ON nsp.oid=pronamespace\n")
	                 wxT(" WHERE pronargs=0")
	                 wxT(" AND prorettype=") + NumToStr(PGOID_TYPE_HANDLER));
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbHandler->Append(procname);
			set->MoveNext();
		}
		delete set;
	}
	cbHandler->SetSelection(0);

	// Fill validator combobox
	cbValidator->Append(wxT(""));
	set = connection->ExecuteSet(
	          wxT("SELECT nspname, proname\n")
	          wxT("  FROM pg_proc p\n")
	          wxT("  JOIN pg_namespace nsp ON nsp.oid=pronamespace\n")
	          wxT(" WHERE proargtypes[0]=") + NumToStr(PGOID_TYPE_TEXT_ARRAY) +
	          wxT(" AND proargtypes[1]=") + NumToStr(PGOID_TYPE_OID));
	if (set)
	{
		while (!set->Eof())
		{
			wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
			cbValidator->Append(procname);
			set->MoveNext();
		}
		delete set;
	}
	cbValidator->SetSelection(0);

	// Initialize options listview and buttons
	lstOptions->AddColumn(_("Option"), 80);
	lstOptions->AddColumn(_("Value"), 40);
	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnAdd->Disable();
	btnRemove->Disable();

	if (fdw)
	{
		// edit mode
		txtName->Enable(connection->BackendMinimumVersion(9, 2));

		val = fdw->GetHandlerProc();
		if (!val.IsEmpty())
		{
			for (unsigned int i = 0 ; i < cbHandler->GetCount() ; i++)
			{
				if (cbHandler->GetString(i) == val)
					cbHandler->SetSelection(i);
			}
		}

		val = fdw->GetValidatorProc();
		if (!val.IsEmpty())
		{
			for (unsigned int i = 0 ; i < cbValidator->GetCount() ; i++)
			{
				if (cbValidator->GetString(i) == val)
					cbValidator->SetSelection(i);
			}
		}

		wxString options = fdw->GetOptions();
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


pgObject *dlgForeignDataWrapper::CreateObject(pgCollection *collection)
{
	wxString name = txtName->GetValue();

	pgObject *obj = foreignDataWrapperFactory.CreateObjects(collection, 0, wxT("\n   AND fdwname ILIKE ") + qtDbString(name));
	return obj;
}


void dlgForeignDataWrapper::CheckChange()
{
	bool didChange = true;
	wxString name = txtName->GetValue();
	if (fdw)
	{
		didChange = name != fdw->GetName()
		            || cbOwner->GetValue() != fdw->GetOwner()
		            || txtComment->GetValue() != fdw->GetComment()
		            || cbHandler->GetValue() != fdw->GetHandlerProc()
		            || cbValidator->GetValue() != fdw->GetValidatorProc()
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



void dlgForeignDataWrapper::OnChangeOptionName(wxCommandEvent &ev)
{
	btnAdd->Enable(txtOption->GetValue().Length() > 0);
}


void dlgForeignDataWrapper::OnSelChangeOption(wxListEvent &ev)
{
	int row = lstOptions->GetSelection();
	if (row >= 0)
	{
		txtOption->SetValue(lstOptions->GetText(row, 0));
		txtValue->SetValue(lstOptions->GetText(row, 1));
	}

	btnRemove->Enable(row >= 0);
}


void dlgForeignDataWrapper::OnAddOption(wxCommandEvent &ev)
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


void dlgForeignDataWrapper::OnRemoveOption(wxCommandEvent &ev)
{
	int sel = lstOptions->GetSelection();
	lstOptions->DeleteItem(sel);

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnRemove->Disable();

	CheckChange();
}


wxString dlgForeignDataWrapper::GetOptionsSql()
{
	wxString options = fdw->GetOptions();
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
		options = fdw->GetOptions();
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


wxString dlgForeignDataWrapper::GetSql()
{
	wxString sql, name;
	name = txtName->GetValue();

	if (fdw)
	{
		// edit mode
		sql = wxEmptyString;

		AppendNameChange(sql);

		if (cbHandler->GetValue() != fdw->GetHandlerProc())
		{
			if (cbHandler->GetValue().IsEmpty())
				sql += wxT("ALTER FOREIGN DATA WRAPPER ") + qtIdent(name)
				       + wxT("\n   NO HANDLER;\n");
			else
				sql += wxT("ALTER FOREIGN DATA WRAPPER ") + qtIdent(name)
				       + wxT("\n   HANDLER ") + qtIdent(cbHandler->GetValue())
				       + wxT(";\n");
		}

		if (cbValidator->GetValue() != fdw->GetValidatorProc())
		{
			if (cbValidator->GetValue().IsEmpty())
				sql += wxT("ALTER FOREIGN DATA WRAPPER ") + qtIdent(name)
				       + wxT("\n   NO VALIDATOR;\n");
			else
				sql += wxT("ALTER FOREIGN DATA WRAPPER ") + qtIdent(name)
				       + wxT("\n   VALIDATOR ") + qtIdent(cbValidator->GetValue())
				       + wxT(";\n");
		}

		wxString sqloptions = GetOptionsSql();
		if (sqloptions.Length() > 0)
		{
			sql += wxT("ALTER FOREIGN DATA WRAPPER ") + name
			       + wxT(" OPTIONS (") + sqloptions + wxT(");");
		}

		AppendOwnerChange(sql, wxT("FOREIGN DATA WRAPPER ") + qtIdent(name));
	}
	else
	{
		// create mode
		sql = wxT("CREATE FOREIGN DATA WRAPPER ") + qtIdent(name);
		AppendIfFilled(sql, wxT("\n   HANDLER "), qtIdent(cbHandler->GetValue()));
		AppendIfFilled(sql, wxT("\n   VALIDATOR "), qtIdent(cbValidator->GetValue()));

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
		AppendOwnerNew(sql, wxT("FOREIGN DATA WRAPPER ") + qtIdent(name));
	}

	sql += GetGrant(wxT("U"), wxT("FOREIGN DATA WRAPPER ") + qtIdent(name));
	AppendComment(sql, wxT("FOREIGN DATA WRAPPER"), 0, fdw);

	return sql;
}


