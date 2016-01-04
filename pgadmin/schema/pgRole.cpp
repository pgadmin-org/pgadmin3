//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgRole.cpp - PostgreSQL Role
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/choicdlg.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgRole.h"
#include "frm/frmMain.h"
#include "dlg/dlgReassignDropOwned.h"
#include "utils/pgDefs.h"
#include "schema/pgDatabase.h"
#include "schema/pgTablespace.h"


pgLoginRole::pgLoginRole(const wxString &newName)
	: pgRole(loginRoleFactory, newName)
{
}


pgGroupRole::pgGroupRole(const wxString &newName)
	: pgRole(groupRoleFactory, newName)
{
}


pgRole::pgRole(pgaFactory &factory, const wxString &newName)
	: pgServerObject(factory, newName)
{
}


wxString pgLoginRole::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on login role");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing login role");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop login role \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop login role \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop login role cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop login role?");
			break;
		case PROPERTIESREPORT:
			message = _("Login role properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Login role properties");
			break;
		case DDLREPORT:
			message = _("Login role DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Login role DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Login role dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Login role dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Login role dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Login role dependents");
			break;
	}

	return message;
}


wxString pgGroupRole::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on group role");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing group role");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop group role \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop group role \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop group role cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop group role?");
			break;
		case PROPERTIESREPORT:
			message = _("Group role properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Group role properties");
			break;
		case DDLREPORT:
			message = _("Group role DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Group role DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Group role dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Group role dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Group role dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Group role dependents");
			break;
	}

	return message;
}


int pgRole::GetIconId()
{
	if (GetCanLogin())
		return loginRoleFactory.GetIconId();
	else
		return groupRoleFactory.GetIconId();
}


bool pgRole::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	if ((GetUpdateCatalog() && !server->GetConnection()->BackendMinimumVersion(9, 5)) ||
	        (server->GetConnection()->BackendMinimumVersion(9, 5) && this->GetSuperuser()))
	{
		wxMessageDialog dlg(frame,
		                    _("Deleting a superuser might result in unwanted behaviour (e.g. when restoring the database).\nAre you sure?"),
		                    _("Confirm superuser deletion"),
		                    wxICON_EXCLAMATION | wxYES_NO | wxNO_DEFAULT);
		if (dlg.ShowModal() != wxID_YES)
			return false;
	}
	return server->ExecuteVoid(wxT("DROP ROLE ") + GetQuotedFullIdentifier() + wxT(";"));
}


wxString pgRole::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Role: ") + GetName() + wxT("\n\n")
		      + wxT("-- DROP ROLE ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE ROLE ") + GetQuotedIdentifier();

		if (GetCanLogin())
		{
			sql += wxT(" LOGIN");
			if (GetPassword() != wxT("********") && !GetPassword().IsEmpty())
				AppendIfFilled(sql, wxT("\n  ENCRYPTED PASSWORD "), qtDbString(GetPassword()));
		}
		sql += wxT("\n ");
		if (this->GetSuperuser())   sql += wxT(" SUPERUSER");
		else                        sql += wxT(" NOSUPERUSER");
		if (GetInherits())          sql += wxT(" INHERIT");
		else                        sql += wxT(" NOINHERIT");
		if (GetCreateDatabase())    sql += wxT(" CREATEDB");
		else                        sql += wxT(" NOCREATEDB");
		if (GetCreateRole())        sql += wxT(" CREATEROLE");
		else                        sql += wxT(" NOCREATEROLE");
		if (server->GetConnection()->BackendMinimumVersion(9, 1))
		{
			if (GetReplication())       sql += wxT(" REPLICATION");
			else                        sql += wxT(" NOREPLICATION");
		}
		if (GetConnectionLimit() > 0)
			sql += wxT(" CONNECTION LIMIT ") + NumToStr(GetConnectionLimit());
		if (GetAccountExpires().IsValid())
			AppendIfFilled(sql, wxT(" VALID UNTIL "), qtDbString(DateToAnsiStr(GetAccountExpires())));
		if (GetRolQueueName().Length() > 0)
			AppendIfFilled(sql, wxT(" RESOURCE QUEUE "), GetRolQueueName());
		sql += wxT(";\n");

		if (this->GetSuperuser() && !GetUpdateCatalog() &&
		        !server->GetConnection()->BackendMinimumVersion(9, 5))
			sql += wxT("UPDATE pg_authid SET rolcatupdate=false WHERE rolname=") + qtDbString(GetIdentifier()) + wxT(";\n");

		size_t index;
		if (variables.GetCount() > 0)
		{
			wxString dbname = wxEmptyString;
			wxString parameter = wxEmptyString;
			wxString value = wxEmptyString;

			sql += wxT("\n");
			for (index = 0 ; index < variables.GetCount() ; index += 3)
			{
				dbname = variables.Item(index);
				parameter = variables.Item(index + 1);
				value = variables.Item(index + 2);

				if (dbname.Length() == 0)
				{
					sql += wxT("ALTER ROLE ") + GetQuotedFullIdentifier();
				}
				else
				{
					sql += wxT("ALTER ROLE ") + GetQuotedFullIdentifier() + wxT(" IN DATABASE ") + dbname;
				}

				if (parameter != wxT("search_path") && parameter != wxT("temp_tablespaces"))
				{
					sql += wxT("\n  SET ") + parameter + wxT(" = '") + value + wxT("';\n");
				}
				else
				{
					sql += wxT("\n  SET ") + parameter + wxT(" = ") + value + wxT(";\n");
				}
			}
		}

		for (index = 0 ; index < rolesIn.GetCount() ; index++)
		{
			wxString role = rolesIn.Item(index);
			bool admin = false;
			if (role.Right(PGROLE_ADMINOPTION_LEN) == PGROLE_ADMINOPTION)
			{
				admin = true;
				role = role.Left(role.Length() - PGROLE_ADMINOPTION_LEN);
			}
			sql += wxT("GRANT ") + qtIdent(role)
			       +  wxT(" TO ") + GetQuotedIdentifier();

			if (admin)
				sql += wxT(" WITH ADMIN OPTION");

			sql += wxT(";\n");
		}

		if (!GetComment().IsNull())
		{
			sql += wxT("COMMENT ON ROLE ") + GetQuotedFullIdentifier() + wxT(" IS ")
			       +  qtDbString(GetComment()) + wxT(";\n");
		}

		if (GetConnection()->BackendMinimumVersion(9, 2))
			sql += GetSeqLabelsSql();
	}
	return sql;
}



void pgRole::ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where)
{
	form->StartMsg(_(" Retrieving Role owned objects"));

	referencedBy->ClearAll();
	referencedBy->AddColumn(_("Type"), 60);
	referencedBy->AddColumn(_("Database"), 80);
	referencedBy->AddColumn(_("Name"), 300);

	wxString sysoid = NumToStr(GetConnection()->GetLastSystemOID());

	wxArrayString dblist;

	pgSet *set;
	set = GetConnection()->ExecuteSet(
	          wxT("SELECT 'd' as type, datname, datallowconn, datdba\n")
	          wxT("  FROM pg_database db\n")
	          wxT("UNION\n")
	          wxT("SELECT 'M', spcname, null, null\n")
	          wxT("  FROM pg_tablespace where spcowner=") + GetOidStr() + wxT("\n")
	          wxT(" ORDER BY 1, 2"));

	if (set)
	{
		while (!set->Eof())
		{
			wxString name = set->GetVal(wxT("datname"));
			if (set->GetVal(wxT("type")) == wxT("d"))
			{
				if (set->GetBool(wxT("datallowconn")))
					dblist.Add(name);
				if (GetOidStr() == set->GetVal(wxT("datdba")))
					referencedBy->AppendItem(databaseFactory.GetIconId(), _("Database"), name);
			}
			else
				referencedBy->AppendItem(tablespaceFactory.GetIconId(), _("Tablespace"), wxEmptyString, name);

			set->MoveNext();
		}
		delete set;
	}

	// We ignore classid and refclassid here because we hope that oids are unique
	// across system tables.
	// Strictly speaking, we'd need to join pg_shdepend to each subquery

	wxString depOids = wxT("(SELECT objid FROM pg_shdepend WHERE refobjid=") + GetOidStr() + wxT(")");

	FillOwned(form->GetBrowser(), referencedBy, dblist,
	          wxT("SELECT cl.relkind, COALESCE(cin.nspname, cln.nspname) as nspname, COALESCE(ci.relname, cl.relname) as relname, cl.relname as indname\n")
	          wxT("  FROM pg_class cl\n")
	          wxT("  JOIN pg_namespace cln ON cl.relnamespace=cln.oid\n")
	          wxT("  LEFT OUTER JOIN pg_index ind ON ind.indexrelid=cl.oid\n")
	          wxT("  LEFT OUTER JOIN pg_class ci ON ind.indrelid=ci.oid\n")
	          wxT("  LEFT OUTER JOIN pg_namespace cin ON ci.relnamespace=cin.oid\n")
	          wxT(" WHERE cl.oid IN ") + depOids + wxT(" AND cl.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT 'n', null, nspname, null\n")
	          wxT("  FROM pg_namespace nsp WHERE nsp.oid IN ") + depOids + wxT(" AND nsp.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT CASE WHEN typtype='d' THEN 'd' ELSE 'y' END, null, typname, null\n")
	          wxT("  FROM pg_type ty WHERE ty.oid IN ") + depOids + wxT(" AND ty.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT 'C', null, conname, null\n")
	          wxT("  FROM pg_conversion co WHERE co.oid IN ") + depOids + wxT(" AND co.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT CASE WHEN prorettype=") + NumToStr(PGOID_TYPE_TRIGGER) + wxT(" THEN 'T' ELSE 'p' END, null, proname, null\n")
	          wxT("  FROM pg_proc pr WHERE pr.oid IN ") + depOids + wxT(" AND pr.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT 'o', null, oprname || '('::text || ")
	          wxT("COALESCE(tl.typname, ''::text) || ")
	          wxT("CASE WHEN tl.oid IS NOT NULL AND tr.oid IS NOT NULL THEN ','::text END || ")
	          wxT("COALESCE(tr.typname, ''::text) || ')'::text, null\n")
	          wxT("  FROM pg_operator op\n")
	          wxT("  LEFT JOIN pg_type tl ON tl.oid=op.oprleft\n")
	          wxT("  LEFT JOIN pg_type tr ON tr.oid=op.oprright\n")
	          wxT(" WHERE op.oid IN ") + depOids + wxT(" AND op.oid > ") + sysoid + wxT("\n")
	          wxT(" ORDER BY 1,2,3"));

	form->EndMsg(set != 0);
}


void pgRole::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;
		wxString rolesquery;

		if (GetConnection()->BackendMinimumVersion(8, 2))
			rolesquery = wxT("SELECT rolname, admin_option,\n")
			             wxT(" pg_catalog.shobj_description(r.oid, 'pg_authid') AS description\n");
		else
			rolesquery = wxT("SELECT rolname, admin_option\n");

		rolesquery += wxT("  FROM pg_roles r\n")
		              wxT("  JOIN pg_auth_members ON r.oid=roleid\n")
		              wxT(" WHERE member=") + GetOidStr() + wxT("\n")
		              wxT(" ORDER BY rolname");

		pgSetIterator roles(GetConnection(), rolesquery);

		while (roles.RowsLeft())
		{
			wxString role = roles.GetVal(wxT("rolname"));
			if (roles.GetBool(wxT("admin_option")))
				role += PGROLE_ADMINOPTION;

			rolesIn.Add(role);
		}

		// Get configuration
		wxString query;
		if (GetConnection()->BackendMinimumVersion(9, 0))
		{
			query = wxT("WITH configs AS ")
			        wxT("(SELECT datname, unnest(setconfig) AS config")
			        wxT(" FROM pg_db_role_setting s")
			        wxT(" LEFT JOIN pg_database d ON d.oid=s.setdatabase")
			        wxT(" WHERE s.setrole=") + NumToStr(GetOid()) + wxT(")\n")
			        wxT("SELECT datname, split_part(config, '=', 1) AS variable, replace(config, split_part(config, '=', 1) || '=', '') AS value\n")
			        wxT("FROM configs");
		}
		else
		{
			wxString query_myrole = wxT("SELECT rolconfig FROM pg_roles WHERE oid=") + NumToStr(GetOid());
			query = wxT("SELECT '' AS datname, split_part(config, '=', 1) AS variable,\n")
			        wxT("       replace(config,split_part(config, '=', 1) || '=', '') AS value\n")
			        wxT("FROM (\n")
			        wxT("    SELECT\n")
			        wxT("      (\n")
			        wxT("      SELECT rolconfig[i]\n")
			        wxT("      FROM pg_roles\n")
			        wxT("      WHERE oid=") + NumToStr(GetOid()) + wxT("\n")
			        wxT("      ) AS config\n")
			        wxT("    FROM generate_series(array_lower((") + query_myrole + wxT("),1), array_upper((") + query_myrole + wxT("),1)) AS i\n")
			        wxT("     ) configs");
		}
		pgSet *configs = GetConnection()->ExecuteSet(query);
		if (configs)
		{
			while (!configs->Eof())
			{
				variables.Add(configs->GetVal(wxT("datname")));
				variables.Add(configs->GetVal(wxT("variable")));
				variables.Add(configs->GetVal(wxT("value")));
				configs->MoveNext();
			}
			delete configs;
		}
	}
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Account expires"), DateToAnsiStr(GetAccountExpires()));
		properties->AppendItem(_("Can login?"), BoolToYesNo(GetCanLogin()));
		properties->AppendItem(_("Superuser?"), BoolToYesNo(GetSuperuser()));
		properties->AppendItem(_("Create databases?"), BoolToYesNo(GetCreateDatabase()));
		properties->AppendItem(_("Create roles?"), BoolToYesNo(GetCreateRole()));

		if (!server->GetConnection()->BackendMinimumVersion(9, 5))
			properties->AppendItem(_("Update catalogs?"), BoolToYesNo(GetUpdateCatalog()));
		properties->AppendItem(_("Inherits?"), BoolToYesNo(GetInherits()));
		if (server->GetConnection()->BackendMinimumVersion(9, 1))
		{
			properties->AppendItem(_("Replication?"), BoolToYesNo(GetReplication()));
		}

		wxString strConnLimit;
		strConnLimit.Printf(wxT("%ld"), GetConnectionLimit());
		properties->AppendItem(_("Connection Limit"), strConnLimit);

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

		wxString roleList;

		size_t index;
		for (index = 0 ; index < rolesIn.GetCount() ; index++)
		{
			if (!roleList.IsEmpty())
				roleList += wxT(", ");
			roleList += rolesIn.Item(index);
		}
		properties->AppendItem(_("Member of"), roleList);

		wxString dbname;
		wxString parameter;
		wxString value;
		for (index = 0; index < variables.GetCount() ; index += 3)
		{
			dbname = variables.Item(index);
			parameter = variables.Item(index + 1);
			value = variables.Item(index + 2);

			if (dbname.Length() == 0)
			{
				properties->AppendItem(parameter, value);
			}
			else
			{
				properties->AppendItem(parameter + wxT(" (database ") + dbname + wxT(")"), value);
			}
		}

		if (!GetLabels().IsEmpty())
		{
			wxArrayString seclabels = GetProviderLabelArray();
			if (seclabels.GetCount() > 0)
			{
				for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
				{
					properties->AppendItem(seclabels.Item(index), seclabels.Item(index + 1));
				}
			}
		}
	}
}

void pgRole::ReassignDropOwnedTo(frmMain *form)
{
	wxString query;

	dlgReassignDropOwned rdo(form, GetConnection(), this, GetServer()->GetDbRestriction());
	if (rdo.ShowModal() != wxID_CANCEL)
	{
		pgConn *conn;
		conn = new pgConn(GetConnection()->GetHost(),
		                  GetConnection()->GetService(),
		                  GetConnection()->GetHostAddr(),
		                  rdo.GetDatabase(),
		                  GetConnection()->GetUser(),
		                  GetConnection()->GetPassword(),
		                  GetConnection()->GetPort(),
		                  GetConnection()->GetRole(),
		                  GetConnection()->GetSslMode(),
		                  0,
		                  GetConnection()->GetApplicationName(),
		                  GetConnection()->GetSSLCert(),
		                  GetConnection()->GetSSLKey(),
		                  GetConnection()->GetSSLRootCert(),
		                  GetConnection()->GetSSLCrl(),
		                  GetConnection()->GetSSLCompression());

		if (conn->GetStatus() == PGCONN_OK)
		{
			if (rdo.IsReassign())
			{
				if (wxMessageBox(_("Are you sure you wish to reassign all objects owned by the selected role?"), _("Reassign objects"), wxYES_NO) != wxYES)
					return;

				query = wxT("REASSIGN OWNED BY ") + GetQuotedFullIdentifier() + wxT(" TO ") + qtIdent(rdo.GetRole());
			}
			else
			{
				if (wxMessageBox(_("Are you sure you wish to drop all objects owned by the selected role?"), _("Drop objects"), wxYES_NO) != wxYES)
					return;

				query = wxT("DROP OWNED BY ") + GetQuotedFullIdentifier();
			}
			conn->ExecuteVoid(query);
		}
		else
		{
			wxMessageBox(wxT("Connection failed: ") + conn->GetLastError());
		}
	}
}


pgObject *pgRole::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *role = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
	{
		if (coll->GetServer()->GetConnection()->BackendMinimumVersion(8, 5))
			role = loginRoleFactory.CreateObjects(coll, 0, wxT("\n WHERE tab.oid=") + GetOidStr());
		else
			role = loginRoleFactory.CreateObjects(coll, 0, wxT("\n WHERE oid=") + GetOidStr());
	}

	return role;
}



pgObject *pgRoleBaseFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgRole *role = 0;
	pgSet *roles = 0;
	wxString query;

	wxString tabname;

	if (collection->GetServer()->HasPrivilege(wxT("table"), wxT("pg_authid"), wxT("SELECT")))
		tabname = wxT("pg_authid");
	else
		tabname = wxT("pg_roles");

	// In 9.0+, role config options are in pg_db_role_setting
	if (collection->GetServer()->GetConnection()->BackendMinimumVersion(8, 5))
	{
		query = wxT("SELECT tab.oid, tab.*, pg_catalog.shobj_description(tab.oid, 'pg_authid') AS description");
		if (collection->GetServer()->GetConnection()->GetIsGreenplum())
			query += wxT(", (SELECT rsqname FROM pg_resqueue WHERE pg_resqueue.oid = rolresqueue) AS rsqname");
		if (collection->GetServer()->GetConnection()->BackendMinimumVersion(9, 2))
		{
			query += wxT(",\n(SELECT array_agg(label) FROM pg_shseclabel sl1 WHERE sl1.objoid=tab.oid) AS labels")
			         wxT(",\n(SELECT array_agg(provider) FROM pg_shseclabel sl2 WHERE sl2.objoid=tab.oid) AS providers");
		}
		query += wxT(" FROM ") + tabname + wxT(" tab") +
		         restriction +  wxT(" ORDER BY rolname");
	}
	else if (collection->GetServer()->GetConnection()->BackendMinimumVersion(8, 2))
	{
		query = wxT("SELECT oid, *, pg_catalog.shobj_description(oid, 'pg_authid') AS description ");
		if (collection->GetServer()->GetConnection()->GetIsGreenplum())
			query += wxT(", (SELECT rsqname FROM pg_resqueue WHERE pg_resqueue.oid = rolresqueue) AS rsqname");
		query += wxT(" FROM ") + tabname + restriction + wxT(" ORDER BY rolname");
	}
	else
	{
		query = wxT("SELECT oid, *, '' AS description ") ;
		if (collection->GetServer()->GetConnection()->GetIsGreenplum())
			query += wxT(", (SELECT rsqname FROM pg_resqueue WHERE pg_resqueue.oid = rolresqueue) AS rsqname");
		query += wxT(" FROM ") + tabname + restriction + wxT(" ORDER BY rolname");
	}
	roles = collection->GetServer()->ExecuteSet(query);

	if (roles)
	{
		while (!roles->Eof())
		{
			if (roles->GetBool(wxT("rolcanlogin")))
				role = new pgLoginRole(roles->GetVal(wxT("rolname")));
			else
				role = new pgGroupRole(roles->GetVal(wxT("rolname")));
			role->iSetServer(collection->GetServer());
			role->iSetOid(roles->GetOid(wxT("oid")));
			role->iSetCanLogin(roles->GetBool(wxT("rolcanlogin")));
			role->iSetInherits(roles->GetBool(wxT("rolinherit")));
			role->iSetCreateRole(roles->GetBool(wxT("rolcreaterole")));
			role->iSetCreateDatabase(roles->GetBool(wxT("rolcreatedb")));
			role->iSetSuperuser(roles->GetBool(wxT("rolsuper")));

			if (!collection->GetServer()->GetConnection()->BackendMinimumVersion(9, 5))
				role->iSetUpdateCatalog(roles->GetBool(wxT("rolcatupdate")));
			role->iSetAccountExpires(roles->GetDateTime(wxT("rolvaliduntil")));
			role->iSetIsValidInfinity(roles->GetVal(wxT("rolvaliduntil")) == wxT("infinity") ? true : false);
			role->iSetPassword(roles->GetVal(wxT("rolpassword")));
			role->iSetComment(roles->GetVal(wxT("description")));
			role->iSetConnectionLimit(roles->GetLong(wxT("rolconnlimit")));

			if (collection->GetServer()->GetConnection()->BackendMinimumVersion(9, 1))
			{
				role->iSetReplication(roles->GetBool(wxT("rolreplication")));
			}

			if (collection->GetServer()->GetConnection()->GetIsGreenplum())
			{
				role->iSetRolQueueName(roles->GetVal(wxT("rsqname")));
			}

			if (collection->GetServer()->GetConnection()->BackendMinimumVersion(9, 2))
			{
				role->iSetProviders(roles->GetVal(wxT("providers")));
				role->iSetLabels(roles->GetVal(wxT("labels")));
			}

			if (browser)
			{
				browser->AppendObject(collection, role);
				roles->MoveNext();
			}
			else
				break;
		}

		delete roles;
	}
	return role;
}


/////////////////////////////

pgLoginRoleCollection::pgLoginRoleCollection(pgaFactory *factory, pgServer *sv)
	: pgServerObjCollection(factory, sv)
{
}


wxString pgLoginRoleCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on login roles");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing login roles");
			break;
		case OBJECTSLISTREPORT:
			message = _("Login roles list report");
			break;
	}

	return message;
}


/////////////////////////////

pgGroupRoleCollection::pgGroupRoleCollection(pgaFactory *factory, pgServer *sv)
	: pgServerObjCollection(factory, sv)
{
}


wxString pgGroupRoleCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on group roles");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing group roles");
			break;
		case OBJECTSLISTREPORT:
			message = _("Group roles list report");
			break;
	}

	return message;
}


pgObject *pgLoginRoleFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	if (restriction.IsEmpty())
		return pgRoleBaseFactory::CreateObjects(collection, browser, wxT(" WHERE rolcanlogin"));
	else
		return pgRoleBaseFactory::CreateObjects(collection, browser, restriction);
}


pgObject *pgGroupRoleFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	if (restriction.IsEmpty())
		return pgRoleBaseFactory::CreateObjects(collection, browser, wxT(" WHERE NOT rolcanlogin"));
	else
		return pgRoleBaseFactory::CreateObjects(collection, browser, restriction);
}


#include "images/user.pngc"
#include "images/group.pngc"
#include "images/roles.pngc"
#include "images/loginroles.pngc"


pgRoleBaseFactory::pgRoleBaseFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img)
	: pgServerObjFactory(tn, ns, nls, img)
{
	metaType = PGM_ROLE;
}

pgLoginRoleFactory::pgLoginRoleFactory()
	: pgRoleBaseFactory(__("Login Role"), __("New Login Role..."), __("Create a new Login Role."), user_png_img)
{
}

pgCollection *pgLoginRoleFactory::CreateCollection(pgObject *obj)
{
	return new pgLoginRoleCollection(GetCollectionFactory(), (pgServer *)obj);
}

pgLoginRoleFactory loginRoleFactory;
static pgaCollectionFactory lcf(&loginRoleFactory, __("Login Roles"), loginroles_png_img);


pgGroupRoleFactory::pgGroupRoleFactory()
	: pgRoleBaseFactory(__("Group Role"), __("New Group Role..."), __("Create a new Group Role."), group_png_img)
{
}

pgCollection *pgGroupRoleFactory::CreateCollection(pgObject *obj)
{
	return new pgGroupRoleCollection(GetCollectionFactory(), (pgServer *)obj);
}

pgGroupRoleFactory groupRoleFactory;
static pgaCollectionFactory gcf(&groupRoleFactory, __("Group Roles"), roles_png_img);


reassignDropOwnedFactory::reassignDropOwnedFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Reassign/Drop Owned..."), _("Reassigned or drop objects owned by the selected role."));
}


wxWindow *reassignDropOwnedFactory::StartDialog(frmMain *form, pgObject *obj)
{
	((pgRole *)obj)->ReassignDropOwnedTo(form);

	return 0;
}

bool reassignDropOwnedFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(loginRoleFactory) && ((pgRole *)obj)->GetConnection()->BackendMinimumVersion(8, 2);
}
