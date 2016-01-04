//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgUser.cpp - PostgreSQL User (only used on pre-8.1 versions, pgRole
//              is used on newer versions).
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgUser.h"
#include "frm/frmMain.h"
#include "utils/pgDefs.h"
#include "schema/pgDatabase.h"
#include "schema/pgTablespace.h"

pgUser::pgUser(const wxString &newName)
	: pgServerObject(userFactory, newName)
{
}

wxString pgUser::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on user");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing user");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for user");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop user \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop user \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop user cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop user?");
			break;
		case PROPERTIESREPORT:
			message = _("User properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("User properties");
			break;
		case DDLREPORT:
			message = _("User DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("User DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("User dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("User dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("User dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("User dependents");
			break;
	}

	return message;
}


bool pgUser::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
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
	return server->ExecuteVoid(wxT("DROP USER ") + GetQuotedFullIdentifier() + wxT(";"));
}


wxString pgUser::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- User: ") + GetName() + wxT("\n\n")
		      + wxT("-- DROP USER ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE USER ") + GetQuotedIdentifier()
		      + wxT("\n  WITH SYSID ") + NumToStr(userId);
		if (GetPassword() != wxT("********"))
			AppendIfFilled(sql, wxT("\n  ENCRYPTED PASSWORD "), qtDbString(GetPassword()));
		sql += wxT("\n ");
		if (GetCreateDatabase())    sql += wxT(" CREATEDB");
		else                        sql += wxT(" NOCREATEDB");
		if (GetUpdateCatalog())     sql += wxT(" CREATEUSER");
		else                        sql += wxT(" NOCREATEUSER");
		if (GetAccountExpires().IsValid())
			AppendIfFilled(sql, wxT(" VALID UNTIL "), qtDbString(DateToAnsiStr(GetAccountExpires())));
		sql += wxT(";\n");

		size_t index;
		for (index = 0 ; index < configList.GetCount() ; index++)
		{
			if (configList.Item(index).BeforeFirst('=') != wxT("search_path") &&
			        configList.Item(index).BeforeFirst('=') != wxT("temp_tablespaces"))
				sql += wxT("ALTER USER ") + GetQuotedIdentifier()
				       + wxT(" SET ") + configList.Item(index).BeforeFirst('=') + wxT("='") + configList.Item(index).AfterFirst('=') + wxT("';\n");
			else
				sql += wxT("ALTER USER ") + GetQuotedIdentifier()
				       + wxT(" SET ") + configList.Item(index).BeforeFirst('=') + wxT("=") + configList.Item(index).AfterFirst('=') + wxT(";\n");
		}
		for (index = 0 ; index < groupsIn.GetCount() ; index++)
			sql += wxT("ALTER GROUP ") + qtIdent(groupsIn.Item(index))
			       +  wxT(" ADD USER ") + GetQuotedIdentifier() + wxT(";\n");

	}
	return sql;
}



void pgUser::ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where)
{
	form->StartMsg(_(" Retrieving user owned objects"));

	referencedBy->ClearAll();
	referencedBy->AddColumn(_("Type"), 60);
	referencedBy->AddColumn(_("Database"), 80);
	referencedBy->AddColumn(_("Name"), 300);

	wxString uid = NumToStr(GetUserId());
	wxString sysoid = NumToStr(GetConnection()->GetLastSystemOID());

	wxArrayString dblist;

	pgSet *set;
	if (GetConnection()->BackendMinimumVersion(7, 5))
		set = GetConnection()->ExecuteSet(
		          wxT("SELECT 'd' as type, datname, datallowconn, datdba\n")
		          wxT("  FROM pg_database db\n")
		          wxT("UNION\n")
		          wxT("SELECT 'M', spcname, null, null\n")
		          wxT("  FROM pg_tablespace where spcowner=") + uid + wxT("\n")
		          wxT(" ORDER BY 1, 2"));
	else
		set = GetConnection()->ExecuteSet(
		          wxT("SELECT 'd' as type, datname, datallowconn, datdba\n")
		          wxT("  FROM pg_database db"));

	if (set)
	{
		while (!set->Eof())
		{
			wxString name = set->GetVal(wxT("datname"));
			if (set->GetVal(wxT("type")) == wxT("d"))
			{
				if (set->GetBool(wxT("datallowconn")))
					dblist.Add(name);
				if (GetUserId() == set->GetLong(wxT("datdba")))
					referencedBy->AppendItem(databaseFactory.GetIconId(), _("Database"), name);
			}
			else
				referencedBy->AppendItem(tablespaceFactory.GetIconId(), _("Tablespace"), wxEmptyString, name);

			set->MoveNext();
		}
		delete set;
	}

	FillOwned(form->GetBrowser(), referencedBy, dblist,
	          wxT("SELECT cl.relkind, COALESCE(cin.nspname, cln.nspname) as nspname, COALESCE(ci.relname, cl.relname) as relname, cl.relname as indname\n")
	          wxT("  FROM pg_class cl\n")
	          wxT("  JOIN pg_namespace cln ON cl.relnamespace=cln.oid\n")
	          wxT("  LEFT OUTER JOIN pg_index ind ON ind.indexrelid=cl.oid\n")
	          wxT("  LEFT OUTER JOIN pg_class ci ON ind.indrelid=ci.oid\n")
	          wxT("  LEFT OUTER JOIN pg_namespace cin ON ci.relnamespace=cin.oid\n")
	          wxT(" WHERE cl.relowner = ") + uid + wxT(" AND cl.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT 'n', null, nspname, null\n")
	          wxT("  FROM pg_namespace nsp WHERE nspowner = ") + uid + wxT(" AND nsp.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT CASE WHEN typtype='d' THEN 'd' ELSE 'y' END, null, typname, null\n")
	          wxT("  FROM pg_type ty WHERE typowner = ") + uid + wxT(" AND ty.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT 'C', null, conname, null\n")
	          wxT("  FROM pg_conversion co WHERE conowner = ") + uid + wxT(" AND co.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT CASE WHEN prorettype=") + NumToStr(PGOID_TYPE_TRIGGER) + wxT(" THEN 'T' ELSE 'p' END, null, proname, null\n")
	          wxT("  FROM pg_proc pr WHERE proowner = ") + uid + wxT(" AND pr.oid > ") + sysoid + wxT("\n")
	          wxT("UNION ALL\n")
	          wxT("SELECT 'o', null, oprname || '('::text || ")
	          wxT("COALESCE(tl.typname, ''::text) || ")
	          wxT("CASE WHEN tl.oid IS NOT NULL AND tr.oid IS NOT NULL THEN ','::text END || ")
	          wxT("COALESCE(tr.typname, ''::text) || ')'::text, null\n")
	          wxT("  FROM pg_operator op\n")
	          wxT("  LEFT JOIN pg_type tl ON tl.oid=op.oprleft\n")
	          wxT("  LEFT JOIN pg_type tr ON tr.oid=op.oprright\n")
	          wxT(" WHERE oprowner = ") + uid + wxT(" AND op.oid > ") + sysoid + wxT("\n")
	          wxT(" ORDER BY 1,2,3"));

	form->EndMsg(set != 0);
}


void pgUser::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		pgSet *set = GetServer()->ExecuteSet(wxT("SELECT groname, grolist FROM pg_group ORDER BY groname"));
		if (set)
		{
			while (!set->Eof())
			{
				wxString groupName = set->GetVal(wxT("groname"));
				wxString str = set->GetVal(wxT("grolist"));
				if (!str.IsNull())
				{
					wxStringTokenizer ids(str.Mid(1, str.Length() - 2), wxT(","));
					while (ids.HasMoreTokens())
					{
						if (StrToLong(ids.GetNextToken()) == GetUserId())
						{
							groupsIn.Add(groupName);
							break;
						}
					}
				}
				set->MoveNext();
			}
			delete set;
		}
	}
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("User ID"), GetUserId());
		properties->AppendItem(_("Account expires"), DateToAnsiStr(GetAccountExpires()));
		properties->AppendItem(_("Superuser?"), BoolToYesNo(GetSuperuser()));
		properties->AppendItem(_("Create databases?"), BoolToYesNo(GetCreateDatabase()));
		properties->AppendItem(_("Update catalogs?"), BoolToYesNo(GetUpdateCatalog()));

		wxString groupList;

		size_t index;
		for (index = 0 ; index < groupsIn.GetCount() ; index++)
		{
			if (!groupList.IsEmpty())
				groupList += wxT(", ");
			groupList += groupsIn.Item(index);
		}
		properties->AppendItem(_("Member of"), groupList);

		for (index = 0; index < configList.GetCount() ; index++)
		{
			wxString item = configList.Item(index);
			properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
		}
	}
}



pgObject *pgUser::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *user = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		user = userFactory.CreateObjects(coll, 0, wxT("\n WHERE usesysid=") + NumToStr(GetUserId()));

	return user;
}

/////////////////////////////

wxString pgUserCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on users");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing users");
			break;
		case OBJECTSLISTREPORT:
			message = _("Users list report");
			break;
	}

	return message;
}

/////////////////////////////

pgObject *pgUserFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgUser *user = 0;

	wxString tabname;

	if (collection->GetServer()->HasPrivilege(wxT("table"), wxT("pg_shadow"), wxT("SELECT")))
		tabname = wxT("pg_shadow");
	else
		tabname = wxT("pg_user");

	pgSet *users = collection->GetServer()->ExecuteSet(wxT(
	                   "SELECT * FROM ") + tabname + restriction + wxT(" ORDER BY usename"));

	if (users)
	{
		while (!users->Eof())
		{

			user = new pgUser(users->GetVal(wxT("usename")));
			user->iSetServer(collection->GetServer());
			user->iSetUserId(users->GetLong(wxT("usesysid")));
			user->iSetCreateDatabase(users->GetBool(wxT("usecreatedb")));
			user->iSetSuperuser(users->GetBool(wxT("usesuper")));
			user->iSetUpdateCatalog(users->GetBool(wxT("usecatupd")));
			user->iSetAccountExpires(users->GetDateTime(wxT("valuntil")));
			user->iSetPassword(users->GetVal(wxT("passwd")));

			wxString cfg = users->GetVal(wxT("useconfig"));
			if (!cfg.IsEmpty())
				FillArray(user->GetConfigList(), cfg.Mid(1, cfg.Length() - 2));

			if (browser)
			{
				browser->AppendObject(collection, user);
				users->MoveNext();
			}
			else
				break;
		}

		delete users;
	}
	return user;
}


#include "images/user.pngc"
#include "images/users.pngc"

pgUserFactory::pgUserFactory()
	: pgServerObjFactory(__("User"), __("New User..."), __("Create a new User."), user_png_img)
{
}


pgUserFactory userFactory;
static pgaCollectionFactory cf(&userFactory, __("Users"), users_png_img);
