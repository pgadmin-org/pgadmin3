//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgUser.cpp - PostgreSQL User Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "utils/misc.h"
#include "dlg/dlgUser.h"
#include "schema/pgUser.h"


// pointer to controls
#define txtID           CTRL_TEXT("txtID")
#define txtPasswd       CTRL_TEXT("txtPasswd")
#define txtRePasswd     CTRL_TEXT("txtRePasswd")
#define datValidUntil   CTRL_CALENDAR("datValidUntil")
#define timValidUntil   CTRL_TIME("timValidUntil")
#define chkCreateDB     CTRL_CHECKBOX("chkCreateDB")
#define chkCreateUser   CTRL_CHECKBOX("chkCreateUser")

#define lbGroupsNotIn   CTRL_LISTBOX("lbGroupsNotIn")
#define lbGroupsIn      CTRL_LISTBOX("lbGroupsIn")
#define btnAddGroup     CTRL_BUTTON("btnAddGroup")
#define btnDelGroup     CTRL_BUTTON("btnDelGroup")

#define lstVariables    CTRL_LISTVIEW("lstVariables")
#define btnAdd          CTRL_BUTTON("wxID_ADD")
#define btnRemove       CTRL_BUTTON("wxID_REMOVE")
#define cbVarname       CTRL_COMBOBOX2("cbVarname")
#define txtValue        CTRL_TEXT("txtValue")
#define chkValue        CTRL_CHECKBOX("chkValue")



dlgProperty *pgUserFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgUser(this, frame, (pgUser *)node);
}


BEGIN_EVENT_TABLE(dlgUser, dlgProperty)
	EVT_CALENDAR_SEL_CHANGED(XRCID("datValidUntil"), dlgUser::OnChangeCal)
	EVT_DATE_CHANGED(XRCID("datValidUntil"),        dlgUser::OnChangeDate)
	EVT_SPIN(XRCID("timValidUntil"),                dlgUser::OnChangeSpin)

	EVT_LISTBOX_DCLICK(XRCID("lbGroupsNotIn"),      dlgUser::OnGroupAdd)
	EVT_LISTBOX_DCLICK(XRCID("lbGroupsIn"),         dlgUser::OnGroupRemove)
	EVT_TEXT(XRCID("txtPasswd"),                    dlgUser::OnChangePasswd)
	EVT_TEXT(XRCID("txtRePasswd"),                  dlgUser::OnChangePasswd)
	EVT_CHECKBOX(XRCID("chkCreateDB"),              dlgUser::OnChange)
	EVT_CHECKBOX(XRCID("chkCreateUser"),            dlgUser::OnChangeSuperuser)

	EVT_BUTTON(XRCID("btnAddGroup"),                dlgUser::OnGroupAdd)
	EVT_BUTTON(XRCID("btnDelGroup"),                dlgUser::OnGroupRemove)

	EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgUser::OnVarSelChange)
	EVT_BUTTON(wxID_ADD,                            dlgUser::OnVarAdd)
	EVT_BUTTON(wxID_REMOVE,                         dlgUser::OnVarRemove)
	EVT_TEXT(XRCID("cbVarname"),                    dlgUser::OnVarnameSelChange)
	EVT_COMBOBOX(XRCID("cbVarname"),                dlgUser::OnVarnameSelChange)
END_EVENT_TABLE();



dlgUser::dlgUser(pgaFactory *f, frmMain *frame, pgUser *node)
	: dlgProperty(f, frame, wxT("dlgUser"))
{
	user = node;
	lstVariables->CreateColumns(0, _("Variable"), _("Value"), -1);
	btnOK->Disable();
	chkValue->Hide();
}


pgObject *dlgUser::GetObject()
{
	return user;
}


int dlgUser::Go(bool modal)
{
	pgSet *set = connection->ExecuteSet(wxT("SELECT groname FROM pg_group"));
	if (set)
	{
		while (!set->Eof())
		{
			wxString groupName = set->GetVal(wxT("groname"));
			if (user && user->GetGroupsIn().Index(groupName) >= 0)
				lbGroupsIn->Append(groupName);
			else
				lbGroupsNotIn->Append(groupName);

			set->MoveNext();
		}
		delete set;
	}

	if (connection->BackendMinimumVersion(7, 4))
		set = connection->ExecuteSet(wxT("SELECT name, vartype, min_val, max_val\n")
		                             wxT("  FROM pg_settings WHERE context in ('user', 'superuser')"));
	else
		set = connection->ExecuteSet(wxT("SELECT name, 'string' as vartype, '' as min_val, '' as max_val FROM pg_settings"));
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

	if (user)
	{
		// Edit Mode
		readOnly = !user->GetServer()->GetSuperUser();

		txtID->SetValue(NumToStr(user->GetUserId()));
		chkCreateDB->SetValue(user->GetCreateDatabase());
		chkCreateUser->SetValue(user->GetSuperuser());
		if (user->GetAccountExpires().IsValid())
		{
			datValidUntil->SetValue(user->GetAccountExpires().GetDateOnly());
			timValidUntil->SetTime(user->GetAccountExpires());
		}
		if (!connection->BackendMinimumVersion(7, 4))
			txtName->Disable();
		txtID->Disable();

		size_t index;
		for (index = 0 ; index < user->GetConfigList().GetCount() ; index++)
		{
			wxString item = user->GetConfigList().Item(index);
			lstVariables->AppendItem(0, item.BeforeFirst('='), item.AfterFirst('='));
		}

		timValidUntil->Enable(!readOnly && user->GetAccountExpires().IsValid());

		if (readOnly)
		{
			chkCreateDB->Disable();
			chkCreateUser->Disable();
			datValidUntil->Disable();
			timValidUntil->Disable();
			btnAddGroup->Disable();
			btnDelGroup->Disable();
			cbVarname->Disable();
			txtValue->Disable();
			btnRemove->Disable();
			if (connection->GetUser() != user->GetName())
			{
				txtPasswd->Disable();
				txtRePasswd->Disable();
				btnAdd->Disable();
			}
			else
			{
				txtPasswd->Enable();
				txtRePasswd->Enable();
				btnAdd->Enable();
			}
		}
	}
	else
	{
		wxDateTime empty;
		datValidUntil->SetValue(empty);
		txtID->SetValidator(numericValidator);
		timValidUntil->Disable();
	}

	return dlgProperty::Go(modal);
}


wxString dlgUser::GetHelpPage() const
{
	if (nbNotebook->GetSelection() == 2)
		return wxT("pg/runtime-config");
	return dlgProperty::GetHelpPage();
}


void dlgUser::OnChangeCal(wxCalendarEvent &ev)
{
	bool timEn = ev.GetDate().IsValid();
	timValidUntil->Enable(timEn);
	if (!timEn)
		timValidUntil->SetTime(wxDefaultDateTime);
	else
		timValidUntil->SetTime(wxDateTime::Today());

	CheckChange();
}


void dlgUser::OnChangeDate(wxDateEvent &ev)
{
	bool timEn = ev.GetDate().IsValid();
	timValidUntil->Enable(timEn);
	if (!timEn)
		timValidUntil->SetTime(wxDefaultDateTime);
	else
		timValidUntil->SetTime(wxDateTime::Today());

	CheckChange();
}

void dlgUser::OnChangeSpin(wxSpinEvent &ev)
{
	CheckChange();
}


void dlgUser::OnChangeSuperuser(wxCommandEvent &ev)
{
	if (user && user->GetSuperuser() && !chkCreateUser->GetValue())
	{
		wxMessageDialog dlg(this,
		                    _("Deleting a superuser might result in unwanted behaviour (e.g. when restoring the database).\nAre you sure?"),
		                    _("Confirm superuser deletion"),
		                    wxICON_EXCLAMATION | wxYES_NO | wxNO_DEFAULT);
		if (dlg.ShowModal() != wxID_YES)
		{
			chkCreateUser->SetValue(true);
			return;
		}
	}
	CheckChange();
}

void dlgUser::OnChangePasswd(wxCommandEvent &ev)
{
	CheckChange();
}

void dlgUser::CheckChange()
{
	bool timEn = datValidUntil->GetValue().IsValid();
	timValidUntil->Enable(timEn);
	if (!timEn)
		timValidUntil->SetTime(wxDefaultDateTime);

	// Check the passwords match
	if (txtPasswd->GetValue() != txtRePasswd->GetValue())
	{
		bool enable = true;
		CheckValid(enable, false, _("The passwords entered do not match!"));
		EnableOK(enable);
		return;
	}

	if (!user)
	{
		wxString name = GetName();

		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		EnableOK(enable);
	}
	else
	{
		EnableOK(!GetSql().IsEmpty());
	}
}


void dlgUser::OnGroupAdd(wxCommandEvent &ev)
{
	if (!readOnly)
	{
		int pos = lbGroupsNotIn->GetSelection();
		if (pos >= 0)
		{
			lbGroupsIn->Append(lbGroupsNotIn->GetString(pos));
			lbGroupsNotIn->Delete(pos);
		}
		CheckChange();
	}
}


void dlgUser::OnGroupRemove(wxCommandEvent &ev)
{
	if (!readOnly)
	{
		int pos = lbGroupsIn->GetSelection();
		if (pos >= 0)
		{
			lbGroupsNotIn->Append(lbGroupsIn->GetString(pos));
			lbGroupsIn->Delete(pos);
		}
		CheckChange();
	}
}


void dlgUser::OnVarnameSelChange(wxCommandEvent &ev)
{
	int sel = cbVarname->GuessSelection(ev);

	SetupVarEditor(sel);
}

void dlgUser::SetupVarEditor(int var)
{
	if (var >= 0 && varInfo.Count() > 0)
	{
		wxStringTokenizer vals(varInfo.Item(var));
		wxString typ = vals.GetNextToken();

		if (typ == wxT("bool"))
		{
			txtValue->Hide();
			chkValue->Show();
		}
		else
		{
			chkValue->Hide();
			txtValue->Show();
			if (typ == wxT("string") || typ == wxT("enum"))
				txtValue->SetValidator(wxTextValidator());
			else
				txtValue->SetValidator(numericValidator);
		}
	}
}

void dlgUser::OnVarSelChange(wxListEvent &ev)
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



void dlgUser::OnVarAdd(wxCommandEvent &ev)
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
	CheckChange();
}


void dlgUser::OnVarRemove(wxCommandEvent &ev)
{
	if (lstVariables->GetSelection() == wxNOT_FOUND)
		return;
	lstVariables->DeleteCurrentItem();
	CheckChange();
}


pgObject *dlgUser::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = userFactory.CreateObjects(collection, 0, wxT("\n WHERE usename=") + qtDbString(name));
	return obj;
}


wxString dlgUser::GetSql()
{
	int pos;
	wxString sql;
	wxString name = GetName();

	wxString passwd = txtPasswd->GetValue();
	bool createDB = chkCreateDB->GetValue(),
	     createUser = chkCreateUser->GetValue();

	if (user)
	{
		// Edit Mode

		AppendNameChange(sql);


		wxString options;
		if (!passwd.IsEmpty())
			options += wxT(" ENCRYPTED PASSWORD ") + qtDbString(connection->EncryptPassword(name, passwd));

		if (createDB != user->GetCreateDatabase() || createUser != user->GetSuperuser())
			options += wxT("\n ");

		if (createDB != user->GetCreateDatabase())
		{
			if (createDB)
				options += wxT(" CREATEDB");
			else
				options += wxT(" NOCREATEDB");
		}
		if (createUser != user->GetSuperuser())
		{
			if (createUser)
				options += wxT(" CREATEUSER");
			else
				options += wxT(" NOCREATEUSER");
		}

		if (!datValidUntil->GetValue().IsValid() || DateToStr(datValidUntil->GetValue()) != DateToStr(user->GetAccountExpires()))
		{
			if (datValidUntil->GetValue().IsValid())
				options += wxT("\n   VALID UNTIL ") + qtDbString(DateToAnsiStr(datValidUntil->GetValue() + timValidUntil->GetValue()));
			else
				options += wxT("\n   VALID UNTIL 'infinity'");
		}

		if (!options.IsNull())
			sql += wxT("ALTER USER ") + qtIdent(name) + options + wxT(";\n");

		int cnt = lbGroupsIn->GetCount();
		wxArrayString tmpGroups = user->GetGroupsIn();

		// check for added groups
		for (pos = 0 ; pos < cnt ; pos++)
		{
			wxString groupName = lbGroupsIn->GetString(pos);

			int index = tmpGroups.Index(groupName);
			if (index >= 0)
				tmpGroups.RemoveAt(index);
			else
				sql += wxT("ALTER GROUP ") + qtIdent(groupName)
				       +  wxT("\n  ADD USER ") + qtIdent(name) + wxT(";\n");
		}

		// check for removed groups
		for (pos = 0 ; pos < (int)tmpGroups.GetCount() ; pos++)
		{
			sql += wxT("ALTER GROUP ") + qtIdent(tmpGroups.Item(pos))
			       +  wxT("\n  DROP USER ") + qtIdent(name) + wxT(";\n");
		}
	}
	else
	{
		// Create Mode

		long id = StrToLong(txtID->GetValue());

		sql = wxT(
		          "CREATE USER ") + qtIdent(name);
		if (id)
			sql += wxT("\n  WITH SYSID ") + NumToStr(id);
		if (!passwd.IsEmpty())
			sql += wxT(" ENCRYPTED PASSWORD ") + qtDbString(connection->EncryptPassword(name, passwd));

		if (createDB || createUser)
			sql += wxT("\n ");
		if (createDB)
			sql += wxT(" CREATEDB");
		if (createUser)
			sql += wxT(" CREATEUSER");
		if (datValidUntil->GetValue().IsValid())
			sql += wxT("\n   VALID UNTIL ") + qtDbString(DateToAnsiStr(datValidUntil->GetValue() + timValidUntil->GetValue()));
		else
			sql += wxT("\n   VALID UNTIL 'infinity'");
		sql += wxT(";\n");

		int cnt = lbGroupsIn->GetCount();
		for (pos = 0 ; pos < cnt ; pos++)
			sql += wxT("ALTER GROUP ") + qtIdent(lbGroupsIn->GetString(pos))
			       +  wxT("\n  ADD USER ") + qtIdent(name) + wxT(";\n");
	}

	wxArrayString vars;
	size_t index;

	if (user)
	{
		for (index = 0 ; index < user->GetConfigList().GetCount() ; index++)
			vars.Add(user->GetConfigList().Item(index));
	}

	int cnt = lstVariables->GetItemCount();

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
			if (newVar != wxT("search_path") && newVar != wxT("temp_tablespaces"))
				sql += wxT("ALTER USER ") + qtIdent(name)
				       +  wxT("\n  SET ") + newVar
				       +  wxT("='") + newVal
				       +  wxT("';\n");
			else
				sql += wxT("ALTER USER ") + qtIdent(name)
				       +  wxT("\n  SET ") + newVar
				       +  wxT("=") + newVal
				       +  wxT(";\n");
		}
	}

	// check for removed vars
	for (pos = 0 ; pos < (int)vars.GetCount() ; pos++)
	{
		sql += wxT("ALTER USER ") + qtIdent(name)
		       +  wxT("\n  RESET ") + vars.Item(pos).BeforeFirst('=')
		       + wxT(";\n");
	}

	return sql;
}


