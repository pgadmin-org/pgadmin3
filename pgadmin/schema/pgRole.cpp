//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
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


int pgRole::GetIconId()
{
	if (GetCanLogin())
		return loginRoleFactory.GetIconId();
	else
		return groupRoleFactory.GetIconId();
}


bool pgRole::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	if (GetUpdateCatalog())
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
		if (GetConnectionLimit() > 0)
			sql += wxT(" CONNECTION LIMIT ") + NumToStr(GetConnectionLimit());
		if (GetAccountExpires().IsValid())
			AppendIfFilled(sql, wxT(" VALID UNTIL "), qtDbString(DateToAnsiStr(GetAccountExpires())));
		if (GetRolQueueName().Length() > 0)
			AppendIfFilled(sql, wxT(" RESOURCE QUEUE "), GetRolQueueName());
		sql += wxT(";\n");

		if (this->GetSuperuser() && !GetUpdateCatalog())
			sql += wxT("UPDATE pg_authid SET rolcatupdate=false WHERE rolname=") + qtDbString(GetIdentifier()) + wxT(";\n");

		size_t index;
		for (index = 0 ; index < configList.GetCount() ; index++)
		{
			if (configList.Item(index).BeforeFirst('=') != wxT("search_path") &&
			        configList.Item(index).BeforeFirst('=') != wxT("temp_tablespaces"))
				sql += wxT("ALTER ROLE ") + GetQuotedIdentifier()
				       + wxT(" SET ") + configList.Item(index).BeforeFirst('=') + wxT("='") + configList.Item(index).AfterFirst('=') + wxT("';\n");
			else
				sql += wxT("ALTER ROLE ") + GetQuotedIdentifier()
				       + wxT(" SET ") + configList.Item(index).BeforeFirst('=') + wxT("=") + configList.Item(index).AfterFirst('=') + wxT(";\n");
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
				if (GetOidStr() == set->GetLong(wxT("datdba")))
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
		properties->AppendItem(_("Update catalogs?"), BoolToYesNo(GetUpdateCatalog()));
		properties->AppendItem(_("Inherits?"), BoolToYesNo(GetInherits()));

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

		for (index = 0; index < configList.GetCount() ; index++)
		{
			wxString item = configList.Item(index);
			properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
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
		                  rdo.GetDatabase(),
		                  GetConnection()->GetUser(),
		                  GetConnection()->GetPassword(),
		                  GetConnection()->GetPort(),
		                  GetConnection()->GetSslMode(),
		                  0,
		                  GetConnection()->GetApplicationName());

		if (conn->GetStatus() == PGCONN_OK)
		{
			if (rdo.IsReassign())
			{
				if (wxMessageBox(_("Are you sure you wish to reassign all objects owned by the selected role?"), _("Reassign objects"), wxYES_NO) == wxNO)
					return;

				query = wxT("REASSIGN OWNED BY ") + GetQuotedFullIdentifier() + wxT(" TO ") + qtIdent(rdo.GetRole());
			}
			else
			{
				if (wxMessageBox(_("Are you sure you wish to drop all objects owned by the selected role?"), _("Drop objects"), wxYES_NO) == wxNO)
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

	// In 8.5+, role config options are in pg_db_role_setting
	if (collection->GetServer()->GetConnection()->BackendMinimumVersion(8, 5))
	{
		query = wxT("SELECT tab.oid, tab.*, pg_catalog.shobj_description(tab.oid, 'pg_authid') AS description, setting.setconfig AS rolconfig");
		if (collection->GetServer()->GetConnection()->GetIsGreenplum())
			query += wxT(", (SELECT rsqname FROM pg_resqueue WHERE pg_resqueue.oid = rolresqueue) AS rsqname");
		query += wxT(" FROM ") + tabname + wxT(" tab") +
		         wxT("  LEFT OUTER JOIN pg_db_role_setting setting ON (tab.oid=setting.setrole AND setting.setdatabase=0)\n") +
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
			role->iSetOid(roles->GetLong(wxT("oid")));
			role->iSetCanLogin(roles->GetBool(wxT("rolcanlogin")));
			role->iSetInherits(roles->GetBool(wxT("rolinherit")));
			role->iSetCreateRole(roles->GetBool(wxT("rolcreaterole")));
			role->iSetCreateDatabase(roles->GetBool(wxT("rolcreatedb")));
			role->iSetSuperuser(roles->GetBool(wxT("rolsuper")));
			role->iSetUpdateCatalog(roles->GetBool(wxT("rolcatupdate")));
			role->iSetAccountExpires(roles->GetDateTime(wxT("rolvaliduntil")));
			role->iSetPassword(roles->GetVal(wxT("rolpassword")));
			role->iSetComment(roles->GetVal(wxT("description")));
			role->iSetConnectionLimit(roles->GetLong(wxT("rolconnlimit")));

			if (collection->GetServer()->GetConnection()->GetIsGreenplum())
			{
				role->iSetRolQueueName(roles->GetVal(wxT("rsqname")));
			}

			wxString cfg = roles->GetVal(wxT("rolconfig"));
			if (!cfg.IsEmpty())
				FillArray(role->GetConfigList(), cfg.Mid(1, cfg.Length() - 2));

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


#include "images/user.xpm"
#include "images/group.xpm"
#include "images/roles.xpm"
#include "images/loginroles.xpm"


pgRoleBaseFactory::pgRoleBaseFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, const char **img)
	: pgServerObjFactory(tn, ns, nls, img)
{
	metaType = PGM_ROLE;
}

pgLoginRoleFactory::pgLoginRoleFactory()
	: pgRoleBaseFactory(__("Login Role"), __("New Login Role..."), __("Create a new Login Role."), user_xpm)
{
}

pgLoginRoleFactory loginRoleFactory;
static pgaCollectionFactory lcf(&loginRoleFactory, __("Login Roles"), loginroles_xpm);


pgGroupRoleFactory::pgGroupRoleFactory()
	: pgRoleBaseFactory(__("Group Role"), __("New Group Role..."), __("Create a new Group Role."), group_xpm)
{
}

pgGroupRoleFactory groupRoleFactory;
static pgaCollectionFactory gcf(&groupRoleFactory, __("Group Roles"), roles_xpm);


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
