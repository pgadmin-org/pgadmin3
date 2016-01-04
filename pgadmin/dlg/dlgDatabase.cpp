//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgDatabase.cpp - PostgreSQL Database Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/spinbutt.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "dlg/dlgDatabase.h"
#include "schema/pgDatabase.h"
#include "ctl/ctlDefaultSecurityPanel.h"
#include "ctl/ctlSeclabelPanel.h"


// pointer to controls
#define cbEncoding      CTRL_COMBOBOX("cbEncoding")
#define cbTemplate      CTRL_COMBOBOX("cbTemplate")
#define stPath          CTRL_STATIC("stPath")
#define txtPath         CTRL_TEXT("txtPath")
#define stTablespace    CTRL_STATIC("stTablespace")
#define cbTablespace    CTRL_COMBOBOX("cbTablespace")
#define txtSchemaRestr  CTRL_TEXT("txtSchemaRestr")

#define lstVariables    CTRL_LISTVIEW("lstVariables")
#define cbVarname       CTRL_COMBOBOX2("cbVarname")
#define cbVarUsername   CTRL_COMBOBOX2("cbVarUsername")
#define txtValue        CTRL_TEXT("txtValue")
#define chkValue        CTRL_CHECKBOX("chkValue")
#define btnAdd          CTRL_BUTTON("wxID_ADD")
#define btnRemove       CTRL_BUTTON("wxID_REMOVE")
#define cbCollate       CTRL_COMBOBOX2("cbCollate")
#define cbCType         CTRL_COMBOBOX2("cbCType")
#define txtConnLimit    CTRL_TEXT("txtConnLimit")

dlgProperty *pgDatabaseFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	dlgDatabase *dlg = new dlgDatabase(this, frame, (pgDatabase *)node);
	if (dlg && !node)
	{
		// use the server's connection to avoid "template1 in use"
		dlg->SetConnection(parent->GetConnection());
	}
	return dlg;
}


BEGIN_EVENT_TABLE(dlgDatabase, dlgDefaultSecurityProperty)
	EVT_TEXT(XRCID("txtPath"),                      dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbEncoding"),                   dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbEncoding"),               dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtSchemaRestr"),               dlgDatabase::OnChangeRestr)
	EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgDatabase::OnVarSelChange)
	EVT_BUTTON(wxID_ADD,                            dlgDatabase::OnVarAdd)
	EVT_BUTTON(wxID_REMOVE,                         dlgDatabase::OnVarRemove)
	EVT_TEXT(XRCID("cbVarname"),                    dlgDatabase::OnVarnameSelChange)
	EVT_TEXT(XRCID("cbVarUsername"),                dlgDatabase::OnVarnameSelChange)
	EVT_COMBOBOX(XRCID("cbVarname"),                dlgDatabase::OnVarnameSelChange)
	EVT_BUTTON(wxID_OK,                             dlgDatabase::OnOK)
	EVT_TEXT(XRCID("cbCollate"),                    dlgDatabase::OnCollateSelChange)
	EVT_TEXT(XRCID("cbCType"),                      dlgDatabase::OnCTypeSelChange)
	EVT_TEXT(XRCID("txtConnLimit"),                 dlgDatabase::OnConnLimitChange)
#ifdef __WXMAC__
	EVT_SIZE(                                       dlgDatabase::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgDatabase::dlgDatabase(pgaFactory *f, frmMain *frame, pgDatabase *node)
	: dlgDefaultSecurityProperty(f, frame, node, wxT("dlgDatabase"), wxT("CREATE,TEMP,CONNECT"), "CTc", node != NULL ? true : false)
{
	database = node;
	schemaRestrictionOk = true;
	lstVariables->CreateColumns(0, _("Username"), _("Variable"), _("Value"));

	chkValue->Hide();

	dirtyVars = false;

	seclabelPage = new ctlSeclabelPanel(nbNotebook);

	if (!node)
	{
		int icon = PGICON_PUBLIC;
		wxString name = wxT("public");
		wxString value = wxT("Tc");
		securityPage->lbPrivileges->AppendItem(icon, name, value);
		AppendCurrentAcl(name, value);
	}
}

pgObject *dlgDatabase::GetObject()
{
	return database;
}


wxString dlgDatabase::GetHelpPage() const
{
	if (nbNotebook->GetSelection() == 1)
		return wxT("pg/runtime-config");
	return dlgDefaultSecurityProperty::GetHelpPage();
}


int dlgDatabase::Go(bool modal)
{
	bool createDefPriv = false;
	wxString strDefPrivsOnTables, strDefPrivsOnSeqs, strDefPrivsOnFuncs, strDefPrivsOnTypes;

	if (connection->BackendMinimumVersion(9, 2))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(database);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgDatabase::OnChange));
	}
	else
		seclabelPage->Disable();

	if (connection->BackendMinimumVersion(9, 0))
	{
		cbVarUsername->Append(wxT(""));
		// AddUsers function of dlgDefaultSecurity has already been called.
		// Hence, calling dlgProperty::AddUsers instead of that.
		dlgProperty::AddUsers(cbVarUsername);
	}
	else
		cbVarUsername->Enable(false);

	if (connection->BackendMinimumVersion(8, 0))
	{
		stPath->Hide();
		txtPath->Hide();
	}
	else
	{
		stTablespace->Hide();
		cbTablespace->Hide();
	}

	if (!connection->BackendMinimumVersion(8, 1))
	{
		txtConnLimit->Disable();
	}
	else
		txtConnLimit->SetValidator(numericValidator);

	if (!connection->BackendMinimumVersion(8, 4))
	{
		cbCollate->Disable();
		cbCType->Disable();
	}

	pgSet *set;
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

		if (connection->BackendMinimumVersion(9, 0))
		{
			cbVarUsername->SetSelection(0);
		}
		SetupVarEditor(0);
	}

	if (database)
	{
		// edit mode

		if (!connection->BackendMinimumVersion(7, 4))
			txtName->Disable();

		if (!connection->BackendMinimumVersion(8, 0))
			cbOwner->Disable();

		readOnly = !database->GetServer()->GetCreatePrivilege();


		if (connection->BackendMinimumVersion(9, 0))
		{
			createDefPriv = true;
			strDefPrivsOnTables = database->GetDefPrivsOnTables();
			strDefPrivsOnSeqs   = database->GetDefPrivsOnSequences();
			strDefPrivsOnFuncs  = database->GetDefPrivsOnFunctions();
		}
		if (connection->BackendMinimumVersion(9, 2))
			strDefPrivsOnTypes = database->GetDefPrivsOnTypes();

		if (readOnly)
		{
			cbVarname->Disable();
			cbVarUsername->Disable();
			txtValue->Disable();
			btnAdd->Disable();
			btnRemove->Disable();
		}

		size_t i;
		wxString username;
		wxString varname;
		wxString varvalue;
		for (i = 0 ; i < database->GetVariables().GetCount() ; i += 3)
		{
			username = database->GetVariables().Item(i);
			varname = database->GetVariables().Item(i + 1);
			varvalue = database->GetVariables().Item(i + 2);

			lstVariables->AppendItem(0, username, varname, varvalue);
		}

		PrepareTablespace(cbTablespace, database->GetTablespaceOid());
		if (connection->BackendMinimumVersion(8, 4))
			cbTablespace->Enable();
		else
			cbTablespace->Disable();
		txtPath->SetValue(database->GetPath());
		txtPath->Disable();

		cbEncoding->Append(database->GetEncoding());
		cbEncoding->SetSelection(0);

		if (connection->BackendMinimumVersion(8, 1))
		{
			wxString strConnLimit;
			strConnLimit.Printf(wxT("%ld"), database->GetConnectionLimit());
			txtConnLimit->SetValue(strConnLimit);
		}

		if (connection->BackendMinimumVersion(8, 4))
		{
			cbCollate->Append(database->GetCollate());
			cbCollate->SetSelection(0);
			cbCType->Append(database->GetCType());
			cbCType->SetSelection(0);
		}

		cbTemplate->Disable();
		cbEncoding->Disable();
		cbCollate->Disable();
		cbCType->Disable();

		txtSchemaRestr->SetValue(database->GetSchemaRestriction());
	}
	else
	{
		// create mode
		if (!connection->BackendMinimumVersion(8, 2))
			txtComment->Disable();

		PrepareTablespace(cbTablespace);

		// Add the default tablespace
		cbTablespace->Insert(_("<default tablespace>"), 0, (void *)0);
		cbTablespace->SetSelection(0);

		cbTemplate->Append(wxEmptyString);
		FillCombobox(wxT("SELECT datname FROM pg_database ORDER BY datname"), cbTemplate);
		cbTemplate->SetSelection(0);

		if (connection->BackendMinimumVersion(8, 4))
		{
			FillCombobox(wxT("select DISTINCT(datctype) from pg_database UNION SELECT DISTINCT(datcollate) from pg_database"), cbCollate, cbCType);
			if (cbCollate->FindString(wxT("C")) < 0)
			{
				cbCollate->AppendString(wxT("C"));
				cbCType->AppendString(wxT("C"));
			}
			if (cbCollate->FindString(wxT("POSIX")) < 0)
			{
				cbCollate->AppendString(wxT("POSIX"));
				cbCType->AppendString(wxT("POSIX"));
			}
		}
		if (connection->BackendMinimumVersion(8, 1))
		{
			txtConnLimit->SetValue(wxT("-1"));
		}


		long encNo = 0;
		wxString encStr;
		do
		{
			encStr = connection->ExecuteScalar(
			             wxT("SELECT pg_encoding_to_char(") + NumToStr(encNo) + wxT(")"));
			if (pgConn::IsValidServerEncoding(encNo) && !encStr.IsEmpty())
				cbEncoding->Append(encStr);

			encNo++;
		}
		while (!encStr.IsEmpty());

		encStr = connection->ExecuteScalar(wxT("SELECT pg_encoding_to_char(encoding) FROM pg_database WHERE datname = 'template0'"));
		encNo = cbEncoding->FindString(encStr);

		if (encNo < 0)
		{
			encNo = cbEncoding->FindString(wxT("UNICODE"));
			if (encNo < 0)
				encNo = cbEncoding->FindString(wxT("UTF8"));
		}

		if (encNo >= 0)
			cbEncoding->SetSelection(encNo);

	}

	// Find, and disable the CONNECT ACL option if we're on pre 8.2
	if (!connection->BackendMinimumVersion(8, 2))
	{
		// Disable the checkbox
		if (!DisablePrivilege(wxT("CONNECT")))
		{
			wxLogError(_("Failed to disable the CONNECT privilege checkbox!"));
		}
	}

	return dlgDefaultSecurityProperty::Go(modal, createDefPriv, strDefPrivsOnTables, strDefPrivsOnSeqs, strDefPrivsOnFuncs, strDefPrivsOnTypes);
}


pgObject *dlgDatabase::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = databaseFactory.CreateObjects(collection, 0, wxT(" WHERE datname=") + qtDbString(name) + wxT("\n"));
	return obj;
}


#ifdef __WXMAC__
void dlgDatabase::OnChangeSize(wxSizeEvent &ev)
{
	lstVariables->SetSize(wxDefaultCoord, wxDefaultCoord,
	                      ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 550);
	dlgSecurityProperty::OnChangeSize(ev);
}
#endif


void dlgDatabase::OnChangeRestr(wxCommandEvent &ev)
{
	if (txtSchemaRestr->GetValue().IsEmpty())
		schemaRestrictionOk = true;
	else
	{
		wxString sql = wxT("EXPLAIN SELECT 1 FROM pg_namespace\n")
		               wxT("WHERE nspname IN (") + txtSchemaRestr->GetValue() + wxT(")");

		wxLogNull nix;
		wxString result = connection->ExecuteScalar(sql);

		schemaRestrictionOk = !result.IsEmpty();
	}
	OnChange(ev);
}


void dlgDatabase::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	if (database)
	{
		database->iSetSchemaRestriction(txtSchemaRestr->GetValue().Trim());
		settings->Write(wxString::Format(wxT("Servers/%ld/Databases/%s/SchemaRestriction"), database->GetServer()->GetServerIndex(), database->GetName().c_str()), txtSchemaRestr->GetValue().Trim());

		/*
		 * The connection from the database will get disconnected before execution of any
		 * sql statements for the database.
		 *
		 * Hence, we need to hack the execution of the default privileges statements(sqls)
		 * before getting disconnected from this database. So that, these statements will
		 * run against the current database connection, and not against the server connection.
		 */
		// defaultSecurityChanged will be true only for PostgreSQL 9.0 or later
		if (defaultSecurityChanged)
		{
			wxString strDefPrivs = GetDefaultPrivileges();
			if (!executeDDLSql(strDefPrivs))
			{
				EnableOK(true);
				return;
			}
			defaultSecurityChanged = false;
		}
	}
	dlgDefaultSecurityProperty::OnOK(ev);
}

/*
 * Execute default privileges statement
 *
 * - Hacked to execute the default privileges statement (sql) for dlgDatabse against this database,
 *   because connection for this database object is getting disconnected, and replaced by the server
 *   connection, before execution of any statements (sqls) in dlgPropery::apply function called
 *   from dlgPropery::OnOK event handler.
 *
 *  NOTE: This will work only if the database object exists.
 */
bool dlgDatabase::executeDDLSql(const wxString &strSql)
{
	pgConn *myConn = connection;

	if (!strSql.IsEmpty())
	{
		wxString tmp;
		if (cbClusterSet && cbClusterSet->GetSelection() > 0)
		{
			replClientData *data = (replClientData *)cbClusterSet->wxItemContainer::GetClientData(cbClusterSet->GetSelection());

			if (data->majorVer > 1 || (data->majorVer == 1 && data->minorVer >= 2))
			{
				// From slony version 2.2.0 onwards ddlscript_prepare() method is removed and
				// ddlscript_complete() method arguments got changed so we have to use ddlcapture() method
				// instead of ddlscript_prepare() and changed the argument of ddlscript_complete() method
				if ((data->majorVer == 2 && data->minorVer >= 2) || (data->majorVer > 2))
				{
					tmp = wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlcapture(") + qtDbString(strSql) + wxT(", ") + wxT("NULL::text") + wxT(");\n")
					      + wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlscript_complete(") + wxT("NULL::text") + wxT(");\n");
				}
				else
				{
					tmp = wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlscript_prepare(") + NumToStr(data->setId) + wxT(", -1);\n")
					      + strSql + wxT(";\n")
					      + wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlscript_complete(") + NumToStr(data->setId) + wxT(", ")
					      + qtDbString(strSql) + wxT(", -1);\n");
				}
			}
			else
			{
				tmp = wxT("SELECT ") + qtIdent(data->cluster)
				      + wxT(".ddlscript(") + NumToStr(data->setId) + wxT(", ")
				      + qtDbString(strSql) + wxT(", 0);\n");
			}
		}
		else
			tmp = strSql;

		if (!myConn->ExecuteVoid(tmp))
			// error message is displayed inside ExecuteVoid
			return false;
	}
	return true;
}


void dlgDatabase::CheckChange()
{
	bool enable = true;

	if (database)
	{
		long connLimit;
		if (!txtConnLimit->GetValue().ToLong(&connLimit))
			connLimit = database->GetConnectionLimit();

		enable = txtSchemaRestr->GetValue() != database->GetSchemaRestriction()
		         || txtComment->GetValue() != database->GetComment()
		         || txtName->GetValue() != database->GetName()
		         || cbOwner->GetValue() != database->GetOwner()
		         || cbTablespace->GetValue() != database->GetTablespace()
		         || connLimit != database->GetConnectionLimit()
		         || dirtyVars;
		if (seclabelPage && connection->BackendMinimumVersion(9, 2))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}

	CheckValid(enable, !GetName().IsEmpty(), _("Please specify name."));
	CheckValid(enable, schemaRestrictionOk, _("Restriction not valid."));

	// If there's a schema restriction, we need to ignore the SQL
	// for the dialogue when enabling the OK button.
	if (schemaRestrictionOk)
		EnableOK(enable, true);
	else
		EnableOK(enable);
}


void dlgDatabase::OnVarnameSelChange(wxCommandEvent &ev)
{
	int sel = cbVarname->GuessSelection(ev);

	SetupVarEditor(sel);
}

void dlgDatabase::OnCollateSelChange(wxCommandEvent &ev)
{
	cbCollate->GuessSelection(ev);
}

void dlgDatabase::OnCTypeSelChange(wxCommandEvent &ev)
{
	cbCType->GuessSelection(ev);
}

void dlgDatabase::OnConnLimitChange(wxCommandEvent &ev)
{
	if (ev.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED)
	{
		wxString strConnLimit = txtConnLimit->GetValue();
		long val = 0;
		if (strConnLimit.ToLong(&val))
		{
			CheckChange();
		}
		else if (strConnLimit.Contains(wxT(".")))
		{
			double val;

			// Stop Propagation of the event to the parents
			ev.StopPropagation();
			if (strConnLimit.ToDouble(&val))
			{
				strConnLimit.Printf(wxT("%ld"), (long)val);
				txtConnLimit->SetValue(strConnLimit);
				txtConnLimit->SetInsertionPointEnd();
				return;
			}
		}
		else if (strConnLimit.length() > 9)
		{
			// Maximum value support is 2147483647
			wxString newVal = strConnLimit.substr(0, 10);
			if (!newVal.ToLong(&val))
			{
				newVal = strConnLimit.substr(0, 9);
			}
			ev.StopPropagation();
			txtConnLimit->SetValue(newVal);
			txtConnLimit->SetInsertionPointEnd();
			return;
		}
	}
}

void dlgDatabase::SetupVarEditor(int var)
{
	if (var >= 0 && varInfo.Count() > 0)
	{
		wxStringTokenizer vals(varInfo.Item(var));
		wxString typ = vals.GetNextToken();

		if (typ == wxT("bool"))
		{
			txtValue->Hide();
			chkValue->Show();
			chkValue->SetMinSize(wxSize(cbVarname->GetSize().GetWidth(), cbVarname->GetSize().GetHeight()));
			chkValue->GetParent()->Layout();
		}
		else
		{
			chkValue->Hide();
			txtValue->Show();
			if (typ == wxT("string") || typ == wxT("enum"))
				txtValue->SetValidator(wxTextValidator());
			else
				txtValue->SetValidator(numericValidator);
			txtValue->SetMinSize(wxSize(cbVarname->GetSize().GetWidth(), cbVarname->GetSize().GetHeight()));
			txtValue->GetParent()->Layout();
		}
	}
}

void dlgDatabase::OnVarSelChange(wxListEvent &ev)
{
	long pos = lstVariables->GetSelection();
	if (pos >= 0)
	{
		cbVarUsername->SetValue(lstVariables->GetText(pos));
		cbVarname->SetValue(lstVariables->GetText(pos, 1));

		// We used to raise an OnVarnameSelChange() event here, but
		// at this point the combo box hasn't necessarily updated.
		wxString value = lstVariables->GetText(pos, 2);
		int sel = cbVarname->FindString(lstVariables->GetText(pos, 1));
		SetupVarEditor(sel);

		txtValue->SetValue(value);
		chkValue->SetValue(value == wxT("on"));
	}
}


void dlgDatabase::OnVarAdd(wxCommandEvent &ev)
{
	wxString username = cbVarUsername->GetValue();
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
				if (username == lstVariables->GetText(item))
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
				lstVariables->InsertItem(prevpos, username, 1);
				lstVariables->SetItem(prevpos, 1, name);
				lstVariables->SetItem(prevpos, 2, value);
			}
			else
			{
				long pos = lstVariables->GetItemCount();
				lstVariables->InsertItem(pos, username, 1);
				lstVariables->SetItem(pos, 1, name);
				lstVariables->SetItem(pos, 2, value);
			}
		}
	}
	dirtyVars = true;
	CheckChange();
}


void dlgDatabase::OnVarRemove(wxCommandEvent &ev)
{
	if (lstVariables->GetSelection() == wxNOT_FOUND)
		return;
	lstVariables->DeleteCurrentItem();
	dirtyVars = true;
	CheckChange();
}


// Note: CREATE DATABASE cannot be part of a multi-statement query as of
//       PG83, and never actually would have been transaction-safe prior
//       to then. Therefore, when creating a new database, only the CREATE
//       statement comes from GetSql(), subsequent ALTERs come from GetSql2()
wxString dlgDatabase::GetSql()
{
	wxString sql, name;
	name = GetName();

	if (database)
	{
		// edit mode

		AppendNameChange(sql);
		AppendOwnerChange(sql, wxT("DATABASE ") + qtIdent(name));

		AppendComment(sql, wxT("DATABASE"), 0, database);

		if (seclabelPage && connection->BackendMinimumVersion(9, 2))
			sql += seclabelPage->GetSqlForSecLabels(wxT("DATABASE"), qtIdent(name));

		if (connection->BackendMinimumVersion(8, 4))
		{
			if (cbTablespace->GetCurrentSelection() > 0 && cbTablespace->GetOIDKey() > 0
			        && cbTablespace->GetOIDKey() != database->GetTablespaceOid())
				sql += wxT("ALTER DATABASE ") + qtIdent(name)
				       +  wxT("\n  SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
				       +  wxT(";\n");
		}
		if (connection->BackendMinimumVersion(8, 1))
		{
			long connLimit;

			if (txtConnLimit->GetValue().IsEmpty())
				connLimit = -1;
			else if (!txtConnLimit->GetValue().ToLong(&connLimit))
				connLimit = database->GetConnectionLimit();

			if (connLimit != database->GetConnectionLimit())
			{
				wxString strConnLimit;
				strConnLimit << connLimit;
				sql += wxT("ALTER DATABASE ") + qtIdent(name)
				       +  wxT("\n  WITH CONNECTION LIMIT = ")
				       +  strConnLimit
				       +  wxT(";\n");
			}
		}

		if (!connection->BackendMinimumVersion(8, 2))
			sql += GetGrant(wxT("CT"), wxT("DATABASE ") + qtIdent(name));
		else
			sql += GetGrant(wxT("CTc"), wxT("DATABASE ") + qtIdent(name));

		wxArrayString vars;
		wxString username;
		wxString varname;
		wxString varvalue;
		size_t index;
		int pos;

		// copy database->GetVariables() into vars
		for (index = 0 ; index < database->GetVariables().GetCount() ; index++)
			vars.Add(database->GetVariables().Item(index));

		// check for changed or added vars
		for (pos = 0 ; pos < lstVariables->GetItemCount() ; pos++)
		{
			wxString newUsr = lstVariables->GetText(pos);
			wxString newVar = lstVariables->GetText(pos, 1);
			wxString newVal = lstVariables->GetText(pos, 2);

			wxString oldVal;

			for (index = 0 ; index < vars.GetCount() ; index += 3)
			{
				username = vars.Item(index);
				varname = vars.Item(index + 1);
				varvalue = vars.Item(index + 2);

				if (newUsr == username && newVar == varname)
				{
					oldVal = varvalue;
					vars.RemoveAt(index);
					vars.RemoveAt(index);
					vars.RemoveAt(index);
					break;
				}
			}
			if (oldVal != newVal)
			{
				if (newUsr.Length() == 0)
					sql += wxT("ALTER DATABASE ") + qtIdent(name);
				else
					sql += wxT("ALTER ROLE ") + newUsr + wxT(" IN DATABASE ") + qtIdent(name);

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
			username = vars.Item(pos);
			varname = vars.Item(pos + 1);
			varvalue = vars.Item(pos + 2);

			if (username.Length() == 0)
			{
				sql += wxT("ALTER DATABASE ") + qtIdent(name)
				       +  wxT("\n  RESET ") + varname
				       + wxT(";\n");
			}
			else
			{
				sql += wxT("ALTER ROLE ") + username + wxT(" IN DATABASE ") + qtIdent(name)
				       +  wxT("\n  RESET ") + varname + wxT(";\n");
			}
		}

		if (defaultSecurityChanged)
			sql += wxT("\n") + GetDefaultPrivileges();
	}
	else
	{
		// create mode
		sql = wxT("CREATE DATABASE ") + qtIdent(name)
		      + wxT("\n  WITH ENCODING=") + qtDbString(cbEncoding->GetValue());

		AppendIfFilled(sql, wxT("\n       OWNER="), qtIdent(cbOwner->GetValue()));
		AppendIfFilled(sql, wxT("\n       TEMPLATE="), qtIdent(cbTemplate->GetValue()));
		AppendIfFilled(sql, wxT("\n       LOCATION="), txtPath->GetValue());
		if (connection->BackendMinimumVersion(8, 4))
		{
			wxString strCollate = cbCollate->GetValue();
			if (!strCollate.IsEmpty())
				AppendIfFilled(sql, wxT("\n       LC_COLLATE="), qtDbString(strCollate));
			wxString strCType = cbCType->GetValue();
			if (!strCType.IsEmpty())
				AppendIfFilled(sql, wxT("\n       LC_CTYPE="), qtDbString(strCType));
		}
		if (connection->BackendMinimumVersion(8, 1))
		{
			AppendIfFilled(sql, wxT("\n       CONNECTION LIMIT="), (txtConnLimit->GetValue() == wxT("-") ? wxT("-1") : txtConnLimit->GetValue()));
		}
		if (cbTablespace->GetCurrentSelection() > 0 && cbTablespace->GetOIDKey() > 0)
			sql += wxT("\n       TABLESPACE=") + qtIdent(cbTablespace->GetValue());

		sql += wxT(";\n");
	}

	return sql.Trim(false);
}

wxString dlgDatabase::GetSql2()
{
	wxString sql, name;
	name = GetName();

	// We only use GetSql2() in the CREATE case
	if (!database)
	{
		if (connection->BackendMinimumVersion(8, 2))
			AppendComment(sql, wxT("DATABASE"), 0, database);

		if (!connection->BackendMinimumVersion(8, 2))
			sql += GetGrant(wxT("CT"), wxT("DATABASE ") + qtIdent(name));
		else
			sql += GetGrant(wxT("CTc"), wxT("DATABASE ") + qtIdent(name));

		int cnt = lstVariables->GetItemCount();
		int pos;

		// check for changed or added vars
		for (pos = 0 ; pos < cnt ; pos++)
		{
			wxString newUsr = lstVariables->GetText(pos);
			wxString newVar = lstVariables->GetText(pos, 1);
			wxString newVal = lstVariables->GetText(pos, 2);

			if (newUsr.Length() == 0)
				sql += wxT("ALTER DATABASE ") + qtIdent(name);
			else
				sql += wxT("ALTER ROLE ") + newUsr + wxT(" IN DATABASE ") + qtIdent(name);

			if (newVar != wxT("search_path") && newVar != wxT("temp_tablespaces"))
			{
				sql += wxT("\n  SET ") + newVar + wxT(" = '") + newVal + wxT("';\n");
			}
			else
			{
				sql += wxT("\n  SET ") + newVar + wxT(" = ") + newVal + wxT(";\n");
			}
		}
		if (seclabelPage && connection->BackendMinimumVersion(9, 2))
			sql += seclabelPage->GetSqlForSecLabels(wxT("DATABASE"), qtIdent(name));
	}

	return sql;
}

bool dlgDatabase::GetDisconnectFirst()
{
	if (database)
		return true;
	return false;
}

void dlgDatabase::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
