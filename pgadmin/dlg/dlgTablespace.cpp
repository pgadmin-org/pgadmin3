//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTablespace.cpp - Tablespace property
//
//////////////////////////////////////////////////////////////////////////



#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>


// App headers
#include "utils/misc.h"
#include "dlg/dlgTablespace.h"
#include "schema/pgTablespace.h"
#include "ctl/ctlSeclabelPanel.h"

// pointer to controls
#define txtLocation     CTRL_TEXT("txtLocation")
#define lstVariables    CTRL_LISTVIEW("lstVariables")
#define cbVarname       CTRL_COMBOBOX2("cbVarname")
#define txtValue        CTRL_TEXT("txtValue")
#define chkValue        CTRL_CHECKBOX("chkValue")
#define btnAdd          CTRL_BUTTON("wxID_ADD")
#define btnRemove       CTRL_BUTTON("wxID_REMOVE")

dlgProperty *pgTablespaceFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTablespace(this, frame, (pgTablespace *)node);
}


BEGIN_EVENT_TABLE(dlgTablespace, dlgSecurityProperty)
	EVT_TEXT(XRCID("txtLocation"),                  dlgProperty::OnChange)
	EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgTablespace::OnVarSelChange)
	EVT_BUTTON(wxID_ADD,                            dlgTablespace::OnVarAdd)
	EVT_BUTTON(wxID_REMOVE,                         dlgTablespace::OnVarRemove)
	EVT_TEXT(XRCID("cbVarname"),                    dlgTablespace::OnVarnameSelChange)
	EVT_COMBOBOX(XRCID("cbVarname"),                dlgTablespace::OnVarnameSelChange)
END_EVENT_TABLE();



dlgTablespace::dlgTablespace(pgaFactory *f, frmMain *frame, pgTablespace *node)
	: dlgSecurityProperty(f, frame, node, wxT("dlgTablespace"), wxT("CREATE"), "C")
{
	tablespace = node;
	lstVariables->CreateColumns(0, _("Variable"), _("Value"));
	chkValue->Hide();
	btnOK->Disable();

	seclabelPage = new ctlSeclabelPanel(nbNotebook);
}


pgObject *dlgTablespace::GetObject()
{
	return tablespace;
}


wxString dlgTablespace::GetHelpPage() const
{
	if (nbNotebook->GetSelection() == 1)
		return wxT("pg/runtime-config");
	return dlgSecurityProperty::GetHelpPage();
}


int dlgTablespace::Go(bool modal)
{
	if (connection->BackendMinimumVersion(9, 2))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(tablespace);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgTablespace::OnChange));
	}
	else
		seclabelPage->Disable();

	pgSet *set;
	if (connection->BackendMinimumVersion(8, 5))
	{
		set = connection->ExecuteSet(wxT("SELECT name, vartype, min_val, max_val\n")
		                             wxT("  FROM pg_settings WHERE name IN ('seq_page_cost', 'random_page_cost')"));
		if (set)
		{
			while (!set->Eof())
			{
				cbVarname->Append(set->GetVal(0));
				varInfo.Add(set->GetVal(wxT("vartype")) + wxT(" ") +
				            set->GetVal(wxT("min_val")) + wxT(" ") +
				            set->GetVal(wxT("max_val")));
				set->MoveNext();
			}
			delete set;

			cbVarname->SetSelection(0);
			SetupVarEditor(0);
		}
	}
	else
	{
		lstVariables->Enable(false);
		btnAdd->Enable(false);
		btnRemove->Enable(false);
		cbVarname->Enable(false);
		txtValue->Enable(false);
		chkValue->Enable(false);
	}

	if (tablespace)
	{
		// Edit Mode
		txtName->SetValue(tablespace->GetIdentifier());
		txtLocation->SetValue(tablespace->GetLocation());
		txtComment->SetValue(tablespace->GetComment());

		txtLocation->Disable();

		size_t i;
		for (i = 0 ; i < tablespace->GetVariables().GetCount() ; i++)
		{
			wxString item = tablespace->GetVariables().Item(i);
			lstVariables->AppendItem(0, item.BeforeFirst('='), item.AfterFirst('='));
		}
	}
	else
	{
	}

	// Tablespace comments are only appropriate in 8.2+
	if (!connection->BackendMinimumVersion(8, 2))
		txtComment->Disable();

	return dlgSecurityProperty::Go(modal);
}


#ifdef __WXMAC__
void dlgTablespace::OnChangeSize(wxSizeEvent &ev)
{
	SetPrivilegesLayout();
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgTablespace::CheckChange()
{
	bool enable = true;
	if (tablespace)
	{
		enable = txtComment->GetValue() != tablespace->GetComment()
		         || GetName() != tablespace->GetName()
		         || cbOwner->GetValue() != tablespace->GetOwner()
		         || dirtyVars;
		if (seclabelPage && connection->BackendMinimumVersion(9, 2))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	else
	{
		CheckValid(enable, !GetName().IsEmpty(), _("Please specify name."));
		CheckValid(enable, !txtLocation->GetValue().IsEmpty(), _("Please specify location."));
	}
	EnableOK(enable);
}


pgObject *dlgTablespace::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = tablespaceFactory.CreateObjects(collection, 0, wxT("\n WHERE spcname=") + qtDbString(name));
	return obj;
}


wxString dlgTablespace::GetSql()
{
	wxString sql;
	wxString name = GetName();

	if (tablespace)
	{
		// Edit Mode

		AppendNameChange(sql);
		AppendOwnerChange(sql, wxT("TABLESPACE ") + qtIdent(name));

		sql += GetGrant(wxT("C"), wxT("TABLESPACE ") + qtIdent(name));
		AppendComment(sql, wxT("TABLESPACE"), 0, tablespace);

		wxArrayString vars;

		size_t index;

		for (index = 0 ; index < tablespace->GetVariables().GetCount() ; index++)
			vars.Add(tablespace->GetVariables().Item(index));

		int cnt = lstVariables->GetItemCount();
		int pos;

		// check for changed or added vars
		for (pos = 0 ; pos < cnt ; pos++)
		{
			wxString newVar = lstVariables->GetText(pos);
			wxString newVal = lstVariables->GetText(pos, 1);

			wxString oldVal;

			for (index = 0 ; index < vars.GetCount() ; index++)
			{
				wxString var = vars.Item(index);
				if (var.BeforeFirst('=').IsSameAs(newVar, false))
				{
					oldVal = var.Mid(newVar.Length() + 1);
					vars.RemoveAt(index);
					break;
				}
			}
			if (oldVal != newVal)
			{
				sql += wxT("ALTER TABLESPACE ") + qtIdent(name)
				       +  wxT("\n  SET (") + newVar
				       +  wxT("=") + newVal
				       +  wxT(");\n");
			}
		}

		// check for removed vars
		for (pos = 0 ; pos < (int)vars.GetCount() ; pos++)
		{
			sql += wxT("ALTER TABLESPACE ") + qtIdent(name)
			       +  wxT("\n  RESET (") + vars.Item(pos).BeforeFirst('=')
			       + wxT(");\n");
		}
	}
	else
	{
		// Create Mode
		sql = wxT("CREATE TABLESPACE ") + qtIdent(name);
		AppendIfFilled(sql, wxT("\n  OWNER "), qtIdent(cbOwner->GetValue()));
		sql += wxT("\n  LOCATION ") + qtDbString(txtLocation->GetValue())
		       +  wxT(";\n");
	}

	if (seclabelPage && connection->BackendMinimumVersion(9, 2))
		sql += seclabelPage->GetSqlForSecLabels(wxT("TABLESPACE"), qtIdent(name));


	return sql;
}

wxString dlgTablespace::GetSql2()
{
	wxString sql;
	wxString name = GetName();

	if (!tablespace)
	{
		sql += GetGrant(wxT("C"), wxT("TABLESPACE ") + qtIdent(name));
		AppendComment(sql, wxT("TABLESPACE"), 0, tablespace);

		// check for changed or added vars
		for (int pos = 0 ; pos < lstVariables->GetItemCount() ; pos++)
		{
			sql += wxT("ALTER TABLESPACE ") + qtIdent(name)
			       +  wxT("\n  SET (") + lstVariables->GetText(pos)
			       +  wxT("=") + lstVariables->GetText(pos, 1)
			       +  wxT(");\n");
		}
	}

	return sql;
}

void dlgTablespace::OnVarnameSelChange(wxCommandEvent &ev)
{
	int sel = cbVarname->GuessSelection(ev);

	SetupVarEditor(sel);
}

void dlgTablespace::SetupVarEditor(int var)
{
	if (var >= 0 && varInfo.Count() > 0)
	{
		wxStringTokenizer vals(varInfo.Item(var));
		wxString typ = vals.GetNextToken();

		if (typ == wxT("bool"))
		{
			txtValue->Hide();
			chkValue->Show();
			chkValue->SetSize(wxDefaultCoord, wxDefaultCoord,
			                  cbVarname->GetSize().GetWidth(), cbVarname->GetSize().GetHeight());
		}
		else
		{
			chkValue->Hide();
			txtValue->Show();
			if (typ == wxT("string") || typ == wxT("enum"))
				txtValue->SetValidator(wxTextValidator());
			else
				txtValue->SetValidator(numericValidator);
			txtValue->SetSize(wxDefaultCoord, wxDefaultCoord,
			                  cbVarname->GetSize().GetWidth(), cbVarname->GetSize().GetHeight());
		}
	}
}

void dlgTablespace::OnVarSelChange(wxListEvent &ev)
{
	long pos = lstVariables->GetSelection();
	if (pos >= 0)
	{
		wxString value = lstVariables->GetText(pos, 1);
		cbVarname->SetValue(lstVariables->GetText(pos));

		// We used to raise an OnVarnameSelChange() event here, but
		// at this point the combo box hasn't necessarily updated.
		int sel = cbVarname->FindString(lstVariables->GetText(pos));
		SetupVarEditor(sel);

		txtValue->SetValue(value);
		chkValue->SetValue(value == wxT("on"));
	}
}


void dlgTablespace::OnVarAdd(wxCommandEvent &ev)
{
	wxString name = cbVarname->GetValue();
	wxString value;
	if (chkValue->IsShown())
		value = chkValue->GetValue() ? wxT("on") : wxT("off");
	else
		value = txtValue->GetValue().Strip(wxString::both);

	if (value.IsEmpty())
		value = wxT("DEFAULT");

	if (!name.IsEmpty())
	{
		long pos = lstVariables->FindItem(-1, name);
		if (pos < 0)
		{
			pos = lstVariables->GetItemCount();
			lstVariables->InsertItem(pos, name, 0);
		}
		lstVariables->SetItem(pos, 1, value);
	}
	dirtyVars = true;
	CheckChange();
}


void dlgTablespace::OnVarRemove(wxCommandEvent &ev)
{
	if (lstVariables->GetSelection() >= 0)
	{
		lstVariables->DeleteCurrentItem();
		dirtyVars = true;
		CheckChange();
	}
}


void dlgTablespace::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
