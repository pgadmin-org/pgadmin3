//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRole.cpp - PostgreSQL Role Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "utils/misc.h"
#include "dlg/dlgRole.h"
#include "schema/pgRole.h"
#include "ctl/ctlSeclabelPanel.h"


// pointer to controls
#define txtPasswd       CTRL_TEXT("txtPasswd")
#define txtRePasswd     CTRL_TEXT("txtRePasswd")
#define datValidUntil   CTRL_CALENDAR("datValidUntil")
#define timValidUntil   CTRL_TIME("timValidUntil")
#define chkCanLogin     CTRL_CHECKBOX("chkCanLogin")
#define chkSuperuser    CTRL_CHECKBOX("chkSuperuser")
#define chkInherits     CTRL_CHECKBOX("chkInherits")
#define chkCreateDB     CTRL_CHECKBOX("chkCreateDB")
#define chkCreateRole   CTRL_CHECKBOX("chkCreateRole")
#define chkUpdateCat    CTRL_CHECKBOX("chkUpdateCat")
#define chkReplication  CTRL_CHECKBOX("chkReplication")
#define txtConnectionLimit CTRL_TEXT("txtConnectionLimit")

#define lbRolesNotIn    CTRL_LISTBOX("lbRolesNotIn")
#define lbRolesIn       CTRL_LISTBOX("lbRolesIn")
#define btnAddRole      CTRL_BUTTON("btnAddRole")
#define btnDelRole      CTRL_BUTTON("btnDelRole")
#define chkAdminOption  CTRL_CHECKBOX("chkAdminOption")

#define lstVariables    CTRL_LISTVIEW("lstVariables")
#define btnAdd          CTRL_BUTTON("wxID_ADD")
#define btnRemove       CTRL_BUTTON("wxID_REMOVE")
#define cbVarname       CTRL_COMBOBOX2("cbVarname")
#define cbVarDbname     CTRL_COMBOBOX2("cbVarDbname")
#define txtValue        CTRL_TEXT("txtValue")
#define chkValue        CTRL_CHECKBOX("chkValue")



dlgProperty *pgLoginRoleFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRole(this, frame, (pgRole *)node, true);
}

dlgProperty *pgGroupRoleFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRole(this, frame, (pgRole *)node, false);
}

BEGIN_EVENT_TABLE(dlgRole, dlgProperty)
	EVT_CALENDAR_SEL_CHANGED(XRCID("datValidUntil"), dlgRole::OnChangeCal)
	EVT_DATE_CHANGED(XRCID("datValidUntil"),        dlgRole::OnChangeDate)
	EVT_SPIN(XRCID("timValidUntil"),                dlgRole::OnChangeSpin)
	EVT_TEXT(XRCID("timValidUntil"),				dlgRole::OnChange)

	EVT_LISTBOX_DCLICK(XRCID("lbRolesNotIn"),       dlgRole::OnRoleAdd)
	EVT_LISTBOX_DCLICK(XRCID("lbRolesIn"),          dlgRole::OnRoleRemove)
	EVT_TEXT(XRCID("txtPasswd"),                    dlgRole::OnChangePasswd)
	EVT_TEXT(XRCID("txtRePasswd"),                  dlgRole::OnChangePasswd)
	EVT_CHECKBOX(XRCID("chkCanLogin"),              dlgRole::OnChange)
	EVT_CHECKBOX(XRCID("chkInherits"),              dlgRole::OnChange)
	EVT_CHECKBOX(XRCID("chkCreateDB"),              dlgRole::OnChange)
	EVT_CHECKBOX(XRCID("chkUpdateCat"),             dlgRole::OnChange)
	EVT_CHECKBOX(XRCID("chkSuperuser"),             dlgRole::OnChangeSuperuser)
	EVT_CHECKBOX(XRCID("chkCreateRole"),            dlgRole::OnChange)
	EVT_CHECKBOX(XRCID("chkReplication"),           dlgRole::OnChange)
	EVT_TEXT(XRCID("txtConnectionLimit"),           dlgRole::OnChange)

	EVT_BUTTON(XRCID("btnAddRole"),                 dlgRole::OnRoleAdd)
	EVT_BUTTON(XRCID("btnDelRole"),                 dlgRole::OnRoleRemove)

	EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgRole::OnVarSelChange)
	EVT_BUTTON(wxID_ADD,                            dlgRole::OnVarAdd)
	EVT_BUTTON(wxID_REMOVE,                         dlgRole::OnVarRemove)
	EVT_TEXT(XRCID("cbVarname"),                    dlgRole::OnVarnameSelChange)
	EVT_TEXT(XRCID("cbVarDbname"),                    dlgRole::OnVarnameSelChange)
	EVT_COMBOBOX(XRCID("cbVarname"),                dlgRole::OnVarnameSelChange)

	EVT_BUTTON(wxID_OK,                             dlgRole::OnOK)

#ifdef __WXMAC__
	EVT_SIZE(                                       dlgRole::OnChangeSize)
#endif
END_EVENT_TABLE();



dlgRole::dlgRole(pgaFactory *f, frmMain *frame, pgRole *node, bool chkLogin)
	: dlgProperty(f, frame, wxT("dlgRole"))
{
	role = node;
	lstVariables->CreateColumns(0, _("Database"), _("Variable"), _("Value"), -1);
	btnOK->Disable();
	chkValue->Hide();
	if (chkLogin)
		chkCanLogin->SetValue(true);

	seclabelPage = new ctlSeclabelPanel(nbNotebook);
}


pgObject *dlgRole::GetObject()
{
	return role;
}


#ifdef __WXMAC__
void dlgRole::OnChangeSize(wxSizeEvent &ev)
{
	lstVariables->SetSize(wxDefaultCoord, wxDefaultCoord,
	                      ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


int dlgRole::Go(bool modal)
{

// In wxMac, the text deletion of "calenderBox" is not raising the EVT_CALENDAR_SEL_CHANGED, EVT_DATE_CHANGED events properly.
// Hence, raising these events with wxEVT_CHILD_FOCUS events.
//
#ifdef __WXMAC__

	datValidUntil->Connect(wxEVT_CHILD_FOCUS, wxCommandEventHandler(dlgRole::OnChange), NULL, this);
	timValidUntil->Connect(wxEVT_CHILD_FOCUS, wxCommandEventHandler(dlgRole::OnChange), NULL, this);

#endif

	if (connection->BackendMinimumVersion(9, 0))
	{
		cbVarDbname->Append(wxT(""));
		AddDatabases(cbVarDbname);
	}
	else
	{
		cbVarDbname->Enable(false);
	}

	if (connection->BackendMinimumVersion(9, 2))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(role);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgRole::OnChange));
	}
	else
		seclabelPage->Disable();

	wxString roleSql =
	    wxT("SELECT rolname\n")
	    wxT("  FROM pg_roles r\n");

	varInfo.Add(wxT("role"));
	cbVarname->Append(wxT("role"));

	pgSet *set;
	set = connection->ExecuteSet(wxT("SELECT name, vartype, min_val, max_val\n")
	                             wxT("  FROM pg_settings WHERE context in ('user', 'superuser')"));

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

		if (connection->BackendMinimumVersion(9, 0))
		{
			cbVarDbname->SetSelection(0);
		}

		SetupVarEditor(0);
	}

	if (role)
	{
		wxArrayString roles = role->GetRolesIn();
		size_t i;
		for (i = 0 ; i < roles.GetCount() ; i++)
			lbRolesIn->Append(roles.Item(i));

		roleSql +=
		    wxT("  LEFT JOIN pg_auth_members ON r.oid=roleid AND member = ") + role->GetOidStr() + wxT("\n")
		    wxT(" WHERE r.oid <> ") + role->GetOidStr() + wxT("\n")
		    wxT("   AND roleid IS NULL");

		// Edit Mode
		if (role->GetServer()->GetSuperUser() || role->GetServer()->GetCreateRole())
			readOnly = false;
		else
			readOnly = true;

		chkCreateDB->SetValue(role->GetCreateDatabase());
		chkCreateRole->SetValue(role->GetCreateRole());
		chkSuperuser->SetValue(role->GetSuperuser());
		chkInherits->SetValue(role->GetInherits());
		if (!connection->BackendMinimumVersion(9, 5))
			chkUpdateCat->SetValue(role->GetUpdateCatalog());
		else
			chkUpdateCat->Disable();
		chkCanLogin->SetValue(role->GetCanLogin());
		chkReplication->SetValue(role->GetReplication());
		if (role->GetAccountExpires().IsValid() && role->GetAccountExpires().GetValue() != -1)
		{
			datValidUntil->SetValue(role->GetAccountExpires().GetDateOnly());
			timValidUntil->SetTime(role->GetAccountExpires());
		}
		else
		{
			wxDateTime empty;
			datValidUntil->SetValue(empty);
		}
		txtConnectionLimit->SetValue(NumToStr(role->GetConnectionLimit()));
		txtComment->SetValue(role->GetComment());

		size_t index;
		wxString dbname;
		wxString parameter;
		wxString value;
		for (index = 0 ; index < role->GetVariables().GetCount() ; index += 3)
		{
			dbname = role->GetVariables().Item(index);
			parameter = role->GetVariables().Item(index + 1);
			value = role->GetVariables().Item(index + 2);

			lstVariables->AppendItem(0, dbname, parameter, value);
		}

		timValidUntil->Enable(!readOnly && role->GetAccountExpires().IsValid());

		if (readOnly)
		{
			chkCanLogin->Disable();
			chkCreateDB->Disable();
			chkCreateRole->Disable();
			chkSuperuser->Disable();
			chkInherits->Disable();
			chkUpdateCat->Disable();
			chkReplication->Disable();
			datValidUntil->Disable();
			timValidUntil->Disable();
			btnAddRole->Disable();
			btnDelRole->Disable();
			cbVarname->Disable();
			cbVarDbname->Disable();
			txtValue->Disable();
			txtConnectionLimit->Disable();
			btnRemove->Disable();
			/* Its own password can be changed. */
			if (connection->GetUser() != role->GetName())
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
		chkCanLogin->Disable();
		wxDateTime empty;
		datValidUntil->SetValue(empty);
		timValidUntil->Disable();
	}

	// Role comments are only appropriate in 8.2+
	if (!connection->BackendMinimumVersion(8, 2))
		txtComment->Disable();

	// Replication roles added in 9.1
	if (!connection->BackendMinimumVersion(9, 1))
	{
		chkReplication->Disable();
	}


	if (!settings->GetShowUsersForPrivileges())
	{
		if (role)
			roleSql += wxT("\n   AND NOT rolcanlogin");
		else
			roleSql += wxT("\n WHERE NOT rolcanlogin");
	}
	roleSql += wxT("\n ORDER BY rolname");

	pgSetIterator roles(connection, roleSql);

	while (roles.RowsLeft())
		lbRolesNotIn->Append(roles.GetVal(wxT("rolname")));

	return dlgProperty::Go(modal);
}


wxString dlgRole::GetHelpPage() const
{
	if (nbNotebook->GetSelection() == 2)
		return wxT("pg/runtime-config");
	else
		return wxT("pg/sql-createrole");
}


void dlgRole::OnOK(wxCommandEvent &ev)
{
	dlgProperty::OnOK(ev);

	if (role && ((role->GetCanLogin() != chkCanLogin->GetValue()) == !btnOK->IsEnabled()))
	{
		// LOGIN attribute changed successfully; need to put object under different collection
	}
}


void dlgRole::OnChangeCal(wxCalendarEvent &ev)
{
	bool timEn = ev.GetDate().IsValid();
	timValidUntil->Enable(timEn);
	if (!timEn)
		timValidUntil->SetTime(wxDefaultDateTime);
	else
		timValidUntil->SetTime(wxDateTime::Today());

	CheckChange();
}


void dlgRole::OnChangeDate(wxDateEvent &ev)
{
	bool timEn = ev.GetDate().IsValid();
	timValidUntil->Enable(timEn);
	if (!timEn)
		timValidUntil->SetTime(wxDefaultDateTime);
	else
		timValidUntil->SetTime(wxDateTime::Today());

	CheckChange();
}

void dlgRole::OnChangeSpin(wxSpinEvent &ev)
{
	CheckChange();
}


void dlgRole::OnChangeSuperuser(wxCommandEvent &ev)
{
	if (role && role->GetSuperuser() && !chkSuperuser->GetValue())
	{
		wxMessageDialog dlg(this,
		                    _("Deleting a superuser might result in unwanted behaviour (e.g. when restoring the database).\nAre you sure?"),
		                    _("Confirm superuser deletion"),
		                    wxICON_EXCLAMATION | wxYES_NO | wxNO_DEFAULT);
		if (dlg.ShowModal() != wxID_YES)
		{
			chkSuperuser->SetValue(true);
			return;
		}
	}
	chkUpdateCat->SetValue(chkSuperuser->GetValue() &&
	                       !connection->BackendMinimumVersion(9, 5));
	CheckChange();
}

void dlgRole::OnChangePasswd(wxCommandEvent &ev)
{
	CheckChange();
}

void dlgRole::CheckChange()
{
	bool enable = true;
	bool timEn = datValidUntil->GetValue().IsValid();
	timValidUntil->Enable(timEn);
	if (!timEn)
		timValidUntil->SetTime(wxDefaultDateTime);

	if (!readOnly)
		chkUpdateCat->Enable(chkSuperuser->GetValue() &&
		                     !connection->BackendMinimumVersion(9, 5));

	// Check the passwords match
	if (txtPasswd->GetValue() != txtRePasswd->GetValue())
	{
		bool enable = true;
		CheckValid(enable, false, _("The passwords entered do not match!"));
		EnableOK(enable);
		return;
	}

	if (!role)
	{
		wxString name = GetName();
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
	}
	else
	{
		enable = !GetSql().IsEmpty();
		if (seclabelPage && connection->BackendMinimumVersion(9, 2))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	EnableOK(enable);
}


void dlgRole::OnRoleAdd(wxCommandEvent &ev)
{
	if (!readOnly)
	{
		int pos = lbRolesNotIn->GetSelection();
		if (pos >= 0)
		{
			wxString roleName = lbRolesNotIn->GetString(pos);
			if (chkAdminOption->GetValue())
				roleName += PGROLE_ADMINOPTION;
			lbRolesIn->Append(roleName);
			lbRolesNotIn->Delete(pos);
		}
		CheckChange();
	}
}


void dlgRole::OnRoleRemove(wxCommandEvent &ev)
{
	if (!readOnly)
	{
		int pos = lbRolesIn->GetSelection();
		if (pos >= 0)
		{
			wxString role = lbRolesIn->GetString(pos);
			if (role.Right(PGROLE_ADMINOPTION_LEN) == PGROLE_ADMINOPTION)
				role = role.Left(role.Length() - PGROLE_ADMINOPTION_LEN);

			lbRolesNotIn->Append(role);
			lbRolesIn->Delete(pos);
		}
		CheckChange();
	}
}


void dlgRole::OnVarnameSelChange(wxCommandEvent &ev)
{
	int sel = cbVarname->GuessSelection(ev);

	SetupVarEditor(sel);
}

void dlgRole::SetupVarEditor(int var)
{
	if (var >= 0 && varInfo.Count() > 0)
	{
		wxStringTokenizer vals(varInfo.Item(var));
		wxString typ = vals.GetNextToken();

		if (typ == wxT("bool"))
		{
			txtValue->Hide();
			chkValue->Show();
			chkValue->GetParent()->Layout();
		}
		else
		{
			chkValue->Hide();
			txtValue->Show();
			txtValue->GetParent()->Layout();
			if (typ == wxT("string") || typ == wxT("enum"))
				txtValue->SetValidator(wxTextValidator());
			else
				txtValue->SetValidator(numericValidator);
		}

	}
}

void dlgRole::OnVarSelChange(wxListEvent &ev)
{
	long pos = lstVariables->GetSelection();
	if (pos >= 0)
	{
		cbVarDbname->SetValue(lstVariables->GetText(pos));
		cbVarname->SetValue(lstVariables->GetText(pos, 1));

		// We used to raise an OnVarnameSelChange() event here, but
		// at this point the combo box hasn't necessarily updated.
		wxString value = lstVariables->GetText(pos, 2);
		int sel = cbVarname->FindString(lstVariables->GetText(pos, 1));

		txtValue->SetValue(value);
		chkValue->SetValue(value == wxT("on"));
	}
}


void dlgRole::OnVarAdd(wxCommandEvent &ev)
{
	wxString dbname = cbVarDbname->GetValue();
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
		bool found = false;
		long prevpos = -1;
		for (long item = 0; item < lstVariables->GetItemCount(); item++)
		{
			if (name == lstVariables->GetText(item, 1))
			{
				if (dbname == lstVariables->GetText(item))
				{
					found = true;
					lstVariables->SetItem(item, 2, value);
				}
				else
				{
					prevpos = item;
				}
			}
		}
		if (!found)
		{
			if (prevpos != -1)
			{
				lstVariables->InsertItem(prevpos, dbname, 1);
				lstVariables->SetItem(prevpos, 1, name);
				lstVariables->SetItem(prevpos, 2, value);
			}
			else
			{
				long pos = lstVariables->GetItemCount();
				lstVariables->InsertItem(pos, dbname, 1);
				lstVariables->SetItem(pos, 1, name);
				lstVariables->SetItem(pos, 2, value);
			}
		}
	}
	CheckChange();
}


void dlgRole::OnVarRemove(wxCommandEvent &ev)
{
	if (lstVariables->GetSelection() == wxNOT_FOUND)
		return;
	lstVariables->DeleteCurrentItem();
	CheckChange();
}


pgObject *dlgRole::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = loginRoleFactory.CreateObjects(collection, 0, wxT("\n WHERE rolname=") + qtDbString(name));
	return obj;
}


wxString dlgRole::GetSql()
{
	int pos;
	wxString sql;
	wxString name = GetName();

	wxString passwd = txtPasswd->GetValue();
	bool createDB = chkCreateDB->GetValue(),
	     createRole = chkCreateRole->GetValue(),
	     superuser = chkSuperuser->GetValue(),
	     inherits = chkInherits->GetValue(),
	     canLogin = chkCanLogin->GetValue(),
	     replication = chkReplication->GetValue();

	if (role)
	{
		// Edit Mode

		AppendNameChange(sql, wxT("ROLE ") + role->GetQuotedFullIdentifier());


		wxString options;
		if (canLogin != role->GetCanLogin())
		{
			if (canLogin)
				options = wxT(" LOGIN");
			else
				options = wxT(" NOLOGIN");
		}
		if (canLogin && !passwd.IsEmpty())
			options += wxT(" ENCRYPTED PASSWORD ") + qtDbString(connection->EncryptPassword(name, passwd));

		if (createDB != role->GetCreateDatabase() || createRole != role->GetCreateRole()
		        || superuser != role->GetSuperuser() || inherits != role->GetInherits()
		        || replication != role->GetReplication())
		{
			options += wxT("\n ");

			if (superuser != role->GetSuperuser())
			{
				if (superuser)
					options += wxT(" SUPERUSER");
				else
					options += wxT(" NOSUPERUSER");
			}
			if (inherits != role->GetInherits())
			{
				if (inherits)
					options += wxT(" INHERIT");
				else
					options += wxT(" NOINHERIT");
			}
			if (createDB != role->GetCreateDatabase())
			{
				if (createDB)
					options += wxT(" CREATEDB");
				else
					options += wxT(" NOCREATEDB");
			}
			if (createRole != role->GetCreateRole())
			{
				if (createRole)
					options += wxT(" CREATEROLE");
				else
					options += wxT(" NOCREATEROLE");
			}
			if (connection->BackendMinimumVersion(9, 1))
			{
				if (replication != role->GetReplication())
				{
					if (replication)
						options += wxT(" REPLICATION");
					else
						options += wxT(" NOREPLICATION");
				}
			}
		}
		if (!datValidUntil->GetValue().IsValid() || DateToStr(datValidUntil->GetValue() + timValidUntil->GetValue()) != DateToStr(role->GetAccountExpires()))
		{
			if (datValidUntil->GetValue().IsValid())
				options += wxT("\n   VALID UNTIL ") + qtDbString(DateToAnsiStr(datValidUntil->GetValue() + timValidUntil->GetValue()));
			else if (!role->GetIsValidInfinity() && role->GetAccountExpires().GetValue() != -1)
				options += wxT("\n   VALID UNTIL 'infinity'");
		}

		if (txtConnectionLimit->GetValue().Length() == 0)
		{
			if (role->GetConnectionLimit() != -1)
			{
				options += wxT(" CONNECTION LIMIT -1");
			}
		}
		else
		{
			if (txtConnectionLimit->GetValue() != NumToStr(role->GetConnectionLimit()))
			{
				options += wxT(" CONNECTION LIMIT ") + txtConnectionLimit->GetValue();
			}
		}

		if (!options.IsNull())
			sql += wxT("ALTER ROLE ") + qtIdent(name) + options + wxT(";\n");

		if (!connection->BackendMinimumVersion(9, 5) &&
		        chkUpdateCat->GetValue() != role->GetUpdateCatalog())
		{
			if (!connection->HasPrivilege(wxT("Table"), wxT("pg_authid"), wxT("update")))
				sql += wxT(" -- Can't update 'UpdateCatalog privilege: can't write to pg_authid.\n")
				       wxT("-- ");

			sql += wxT("UPDATE pg_authid SET rolcatupdate=") + BoolToStr(chkUpdateCat->GetValue())
			       + wxT(" WHERE OID=") + role->GetOidStr() + wxT(";\n");
		}
		int cnt = lbRolesIn->GetCount();
		wxArrayString tmpRoles = role->GetRolesIn();

		// check for added roles
		for (pos = 0 ; pos < cnt ; pos++)
		{
			wxString roleName = lbRolesIn->GetString(pos);

			int index = tmpRoles.Index(roleName);
			if (index >= 0)
			{
				// role membership unchanged
				tmpRoles.RemoveAt(index);
			}
			else
			{
				bool admin = false;
				if (roleName.Right(PGROLE_ADMINOPTION_LEN) == PGROLE_ADMINOPTION)
				{
					admin = true;
					roleName = roleName.Left(roleName.Length() - PGROLE_ADMINOPTION_LEN);
				}
				else
				{
					// new role membership without admin option
					index = tmpRoles.Index(roleName + PGROLE_ADMINOPTION);
					if (index >= 0)
					{
						// old membership with admin option
						tmpRoles.RemoveAt(index);
						sql += wxT("REVOKE ADMIN OPTION FOR ") + qtIdent(roleName)
						       + wxT(" FROM ") + qtIdent(name) + wxT(";\n");
						continue;
					}
				}

				index = tmpRoles.Index(roleName);
				if (index >= 0)
				{
					// admin option added to existing membership
					tmpRoles.RemoveAt(index);
				}

				sql += wxT("GRANT ") + qtIdent(roleName)
				       +  wxT(" TO ") + qtIdent(name);

				if (admin)
					sql += wxT(" WITH ADMIN OPTION");

				sql += wxT(";\n");
			}
		}

		// check for removed roles
		for (pos = 0 ; pos < (int)tmpRoles.GetCount() ; pos++)
		{
			sql += wxT("REVOKE ") + qtIdent(tmpRoles.Item(pos))
			       +  wxT(" FROM ") + qtIdent(name) + wxT(";\n");
		}
	}
	else
	{
		// Create Mode
		sql = wxT(
		          "CREATE ROLE ") + qtIdent(name);
		if (canLogin)
		{
			sql += wxT(" LOGIN");
			if (!passwd.IsEmpty())
				sql += wxT(" ENCRYPTED PASSWORD ") + qtDbString(connection->EncryptPassword(name, passwd));
		}

		if (createDB || createRole || !inherits || superuser)
			sql += wxT("\n ");
		if (superuser)
			sql += wxT(" SUPERUSER");
		if (!inherits)
			sql += wxT(" NOINHERIT");
		if (createDB)
			sql += wxT(" CREATEDB");
		if (createRole)
			sql += wxT(" CREATEROLE");
		if (connection->BackendMinimumVersion(9, 1))
		{
			if (replication)
				sql += wxT(" REPLICATION");
		}
		if (datValidUntil->GetValue().IsValid())
			sql += wxT("\n   VALID UNTIL ") + qtDbString(DateToAnsiStr(datValidUntil->GetValue() + timValidUntil->GetValue()));
		else
			sql += wxT("\n   VALID UNTIL 'infinity'");

		if (txtConnectionLimit->GetValue().Length() > 0)
		{
			sql += wxT(" CONNECTION LIMIT ") + txtConnectionLimit->GetValue();
		}

		int cnt = lbRolesIn->GetCount();
		wxString grants;

		if (cnt)
		{
			wxString roleName;

			for (pos = 0 ; pos < cnt ; pos++)
			{
				bool admin = false;
				roleName = lbRolesIn->GetString(pos);
				if (roleName.Right(PGROLE_ADMINOPTION_LEN) == PGROLE_ADMINOPTION)
				{
					roleName = roleName.Left(roleName.Length() - PGROLE_ADMINOPTION_LEN);
					admin = true;

				}
				grants += wxT("GRANT ") + qtIdent(roleName)
				          +  wxT(" TO ") + qtIdent(name);

				if (admin)
					grants += wxT(" WITH ADMIN OPTION;\n");
				else
					grants += wxT(";\n");
			}
		}
		sql += wxT(";\n") + grants;

		if (superuser && !chkUpdateCat->GetValue() &&
		        !connection->BackendMinimumVersion(9, 5))
			sql += wxT("UPDATE pg_authid SET rolcatupdate=false WHERE rolname=") + qtDbString(name) + wxT(";\n");
	}

	wxArrayString vars;
	wxString dbname;
	wxString parameter;
	wxString value;

	size_t index;

	if (role)
	{
		for (index = 0 ; index < role->GetVariables().GetCount() ; index++)
			vars.Add(role->GetVariables().Item(index));
	}

	int cnt = lstVariables->GetItemCount();

	// check for changed or added vars
	for (pos = 0 ; pos < cnt ; pos++)
	{
		wxString newDb = lstVariables->GetText(pos);
		wxString newVar = lstVariables->GetText(pos, 1);
		wxString newVal = lstVariables->GetText(pos, 2);

		wxString oldVal;

		for (index = 0 ; index < vars.GetCount() ; index += 3)
		{
			dbname = vars.Item(index);
			parameter = vars.Item(index + 1);
			value = vars.Item(index + 2);

			if (newDb == dbname && newVar == parameter)
			{
				oldVal = value;
				vars.RemoveAt(index);
				vars.RemoveAt(index);
				vars.RemoveAt(index);
				break;
			}
		}
		if (oldVal != newVal)
		{
			if (newDb.Length() == 0)
				sql += wxT("ALTER ROLE ") + qtIdent(name);
			else
				sql += wxT("ALTER ROLE ") + qtIdent(name) + wxT(" IN DATABASE ") + newDb;

			if (newVar != wxT("search_path") && newVar != wxT("temp_tablespaces"))
			{
				sql += wxT("\n  SET ") + newVar + wxT(" = '") + newVal + wxT("';\n");
			}
			else
			{
				sql += wxT("\n  SET ") + newVar + wxT(" = ") + newVal + wxT(";\n");
			}
		}
	}

	// check for removed vars
	for (pos = 0 ; pos < (int)vars.GetCount() ; pos += 3)
	{
		dbname = vars.Item(pos);
		parameter = vars.Item(pos + 1);
		value = vars.Item(pos + 2);

		if (dbname.Length() == 0)
		{
			sql += wxT("ALTER ROLE ") + qtIdent(name)
			       +  wxT(" RESET ") + parameter
			       + wxT(";\n");
		}
		else
		{
			sql += wxT("ALTER ROLE ") + qtIdent(name) + wxT(" IN DATABASE ") + dbname
			       +  wxT(" RESET ") + parameter + wxT(";\n");
		}
	}

	AppendComment(sql, wxT("ROLE"), 0, role);

	if (seclabelPage && connection->BackendMinimumVersion(9, 2))
		sql += seclabelPage->GetSqlForSecLabels(wxT("ROLE"), qtIdent(name));

	return sql;
}


void dlgRole::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
