//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgDatabase.cpp - PostgreSQL Database
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgfeatures.h"
#include "frm/frmMain.h"
#include "schema/edbSynonym.h"
#include "schema/pgCast.h"
#include "schema/pgLanguage.h"
#include "schema/pgSchema.h"
#include "slony/slCluster.h"
#include "frm/frmHint.h"
#include "frm/frmReport.h"

pgDatabase::pgDatabase(const wxString &newName)
	: pgServerObject(databaseFactory, newName)
{
	useServerConnection = true;
	allowConnections = true;
	connected = false;
	conn = NULL;
	missingFKs = 0;
	canDebugPlpgsql = 0;
	canDebugEdbspl = 0;
}


pgDatabase::~pgDatabase()
{
	Disconnect();
}


int pgDatabase::GetIconId()
{
	if (GetConnected())
		return databaseFactory.GetIconId();
	else
		return databaseFactory.GetClosedIconId();
}


wxMenu *pgDatabase::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();

	if (GetConnection() && GetCreatePrivilege())
	{
		if (settings->GetDisplayOption(_("Casts")))
			castFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Languages")))
			languageFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Synonyms")) && GetConnection()->EdbMinimumVersion(8, 0))
			if (!GetConnection()->BackendMinimumVersion(8, 4))
				synonymFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Schemas")))
			schemaFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Slony")))
			slClusterFactory.AppendMenu(menu);
	}
	return menu;
}

int pgDatabase::Connect()
{
	if (!allowConnections)
		return PGCONN_REFUSED;

	if (!connected)
	{
		if (GetName() == server->GetDatabaseName() && server->connection()->GetStatus() == PGCONN_OK)
		{
			useServerConnection = true;
			conn = 0;
		}
		else
		{
			useServerConnection = false;
			wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Browser");
			conn = CreateConn(applicationname);

			if (!conn)
			{
				connected = false;
				return PGCONN_BAD;
			}
		}

		// Now we're connected.

		// check for extended ruleutils with pretty-print option
		wxString exprname = connection()->ExecuteScalar(wxT("SELECT proname FROM pg_proc WHERE proname='pg_get_viewdef' AND proargtypes[1]=16"));
		if (!exprname.IsEmpty())
			prettyOption = wxT(", true");

		UpdateDefaultSchema();

		if (connection()->BackendMinimumVersion(9, 0))
		{
			m_defPrivsOnTables = connection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = 0::OID AND defaclobjtype='r'"));
			m_defPrivsOnSeqs   = connection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = 0::OID AND defaclobjtype='S'"));
			m_defPrivsOnFuncs  = connection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = 0::OID AND defaclobjtype='f'"));
		}

		connected = true;
	}

	return connection()->GetStatus();
}


pgConn *pgDatabase::connection()
{
	if (useServerConnection)
		return server->connection();
	return conn;

}

void pgDatabase::CheckAlive()
{
	if (connected)
		connected = connection()->IsAlive();
}

void pgDatabase::Disconnect()
{
	connected = false;
	if (conn)
		delete conn;
	conn = 0;
}


bool pgDatabase::GetCanHint()
{
	if (encoding == wxT("SQL_ASCII"))
		return true;

	if (encoding == wxT("UNICODE"))
	{
		wxString ver = GetServer()->GetVersionString();
		if (ver.Find(wxT("mingw32")) > 0 && ver.Find(wxT("SQL 8.0.")) > 0)
			return true;
	}

	if (GetServer()->GetConnection() == GetConnection() &&
	        GetConnection()->BackendMinimumVersion(8, 0) &&
	        !GetConnection()->HasFeature(FEATURE_FILEREAD))
		return true;

	return false;
}


void pgDatabase::ShowHint(frmMain *form, bool force)
{
	wxArrayString hints;

	if (encoding == wxT("SQL_ASCII"))
		hints.Add(HINT_ENCODING_ASCII);
	else if (encoding == wxT("UNICODE"))
	{
		wxString ver = GetServer()->GetVersionString();
		if (ver.Find(wxT("mingw32")) > 0 && ver.Find(wxT("SQL 8.0.")) > 0)
			hints.Add(HINT_ENCODING_UNICODE);
	}

	if (GetServer()->GetConnection() == GetConnection() &&
	        GetConnection()->BackendMinimumVersion(8, 0) &&
	        !GetConnection()->HasFeature(FEATURE_FILEREAD))
		hints.Add(HINT_INSTRUMENTATION);

	if (force || !hintShown)
		frmHint::ShowHint(form, hints, GetFullIdentifier(), force);
	hintShown = true;
}


void pgDatabase::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	bool hasSize = connection()->HasFeature(FEATURE_SIZE);

	wxString sql = wxT("SELECT numbackends AS ") + qtIdent(_("Backends")) +
	               wxT(", xact_commit AS ") + qtIdent(_("Xact Committed")) +
	               wxT(", xact_rollback AS ") + qtIdent(_("Xact Rolled Back")) +
	               wxT(", blks_read AS ") + qtIdent(_("Blocks Read")) +
	               wxT(", blks_hit AS ") + qtIdent(_("Blocks Hit"));

	if (connection()->BackendMinimumVersion(8, 3))
		sql += wxT(", tup_returned AS ") + qtIdent(_("Tuples Returned")) +
		       wxT(", tup_fetched AS ") + qtIdent(_("Tuples Fetched")) +
		       wxT(", tup_inserted AS ") + qtIdent(_("Tuples Inserted")) +
		       wxT(", tup_updated AS ") + qtIdent(_("Tuples Updated")) +
		       wxT(", tup_deleted AS ") + qtIdent(_("Tuples Deleted"));

	if (hasSize)
		sql += wxT(", pg_size_pretty(pg_database_size(datid)) AS ") + qtIdent(_("Size"));

	sql += wxT("\n  FROM pg_stat_database db WHERE datname=") + qtDbString(GetName());

	// DisplayStatistics is not available for this object

	CreateListColumns(statistics, _("Statistic"), _("Value"));

	pgSet *stats = connection()->ExecuteSet(sql);

	if (stats)
	{
		int col;
		for (col = 0 ; col < stats->NumCols() ; col++)
		{
			if (!stats->ColName(col).IsEmpty())
				statistics->AppendItem(stats->ColName(col), stats->GetVal(col));
		}
		delete stats;
	}

}


pgSet *pgDatabase::ExecuteSet(const wxString &sql)
{
	pgSet *set = 0;
	if (connection())
	{
		set = connection()->ExecuteSet(sql);
		if (!set)
			CheckAlive();
	}
	return set;
}


wxString pgDatabase::ExecuteScalar(const wxString &sql)
{
	wxString str;
	if (connection())
	{
		str = connection()->ExecuteScalar(sql);
		if (str.IsEmpty() && connection()->GetLastResultStatus() != PGRES_TUPLES_OK)
			CheckAlive();
	}
	return str;
}


bool pgDatabase::ExecuteVoid(const wxString &sql, bool reportError)
{
	bool rc = 0;
	if (connection())
	{
		rc = connection()->ExecuteVoid(sql, reportError);
		if (!rc)
			CheckAlive();
	}
	return rc;
}


void pgDatabase::UpdateDefaultSchema()
{
	searchPath = connection()->ExecuteScalar(wxT("SHOW search_path"));

	if (!searchPath.IsEmpty())
	{
		wxStringTokenizer tk(searchPath, wxT(","));
		pgSet *set = ExecuteSet(wxT("SELECT nspname, session_user=nspname AS isuser FROM pg_namespace"));
		if (set)
		{
			while (tk.HasMoreTokens())
			{
				wxString str = tk.GetNextToken();
				str.Strip(wxString::both);

				if (str.IsEmpty())
					continue;
				long row;
				for (row = 1 ; row <= set->NumRows() ; row++)
				{
					set->Locate(row);
					defaultSchema = set->GetVal(wxT("nspname"));
					if (str == defaultSchema ||
					        ((str == wxT("$user") || str == wxT("\"$user\"")) && set->GetBool(wxT("isuser"))))
					{
						delete set;
						return;
					}
				}
			}
			delete set;
		}
	}
	defaultSchema = wxEmptyString;
}


wxString pgDatabase::GetSchemaPrefix(const wxString &name) const
{
	if (name.IsEmpty())
		return name;

	if (name == wxT("pg_catalog") || name == defaultSchema)
		return wxEmptyString;

	return name + wxT(".");
}


wxString pgDatabase::GetQuotedSchemaPrefix(const wxString &name) const
{
	wxString str = GetSchemaPrefix(name);
	if (!str.IsEmpty())
		return qtIdent(str.Left(str.Length() - 1)) + wxT(".");
	return str;
}


bool pgDatabase::GetSystemObject() const
{
	if (server)
	{
		if (this->GetName() == wxT("template0")) return true;
		return (this->GetOid() <= server->GetLastSystemOID());
	}
	else
	{
		return false;
	}
}


wxArrayString pgDatabase::GetSlonyClusters(ctlTree *browser)
{
	wxArrayString clusters;

	pgCollection *collection = browser->FindCollection(slClusterFactory, GetId());
	if (collection)
	{
		treeObjectIterator clusterIterator(browser, collection);

		slCluster *cluster;
		while ((cluster = (slCluster *)clusterIterator.GetNextObject()) != 0)
			clusters.Add(cluster->GetName());
	}
	return clusters;
}


void pgDatabase::AppendSchemaChange(const wxString &sql)
{
	wxDateTime dt;
	dt.Now();
	schemaChanges.Append(wxT("-- ") + DateToStr(dt) + wxT("\n"));
	schemaChanges.Append(sql);
	schemaChanges.Append(wxT("\n\n"));
}

bool pgDatabase::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	if (useServerConnection)
	{
		wxMessageDialog(frame, _("Maintenance database can't be dropped."),
		                _("Dropping database not allowed"), wxICON_EXCLAMATION | wxOK).ShowModal();

		return false;
	}
	Disconnect();

	bool done = server->ExecuteVoid(wxT("DROP DATABASE ") + GetQuotedIdentifier() + wxT(";"));
	if (!done)
		Connect();

	return done;
}



wxString pgDatabase::GetSql(ctlTree *browser)
{
	if (sql.IsEmpty())
	{
		// If we can't connect to this database, use the maintenance DB
		pgConn *myConn = GetConnection();
		if (!myConn)
			myConn = GetServer()->GetConnection();

		sql = wxT("-- Database: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP DATABASE ") + GetQuotedIdentifier() + wxT(";")
		      + wxT("\n\nCREATE DATABASE ") + GetQuotedIdentifier()
		      + wxT("\n  WITH OWNER = ") + qtIdent(GetOwner())
		      + wxT("\n       ENCODING = ") + qtDbString(GetEncoding());
		if (!GetTablespace().IsEmpty())
			sql += wxT("\n       TABLESPACE = ") + qtIdent(GetTablespace());
		if (myConn && myConn->BackendMinimumVersion(8, 4))
		{
			sql += wxT("\n       LC_COLLATE = ") + qtDbString(GetCollate());
			sql += wxT("\n       LC_CTYPE = ") + qtDbString(GetCType());
		}
		if (myConn && myConn->BackendMinimumVersion(8, 1))
		{
			sql += wxT("\n       CONNECTION LIMIT = ");
			sql << GetConnectionLimit();
		}
		sql += wxT(";\n");

		size_t i;
		wxString username;
		wxString varname;
		wxString varvalue;
		for (i = 0 ; i < variables.GetCount() ; i++)
		{
			wxStringTokenizer tkz(variables.Item(i), wxT("="));
			while (tkz.HasMoreTokens())
			{
				username = tkz.GetNextToken();
				varname = tkz.GetNextToken();
				varvalue = tkz.GetNextToken();
			}

			if (username.Length() == 0)
			{
				sql += wxT("ALTER DATABASE ") + GetQuotedFullIdentifier();
			}
			else
			{
				sql += wxT("ALTER ROLE ") + username + wxT(" IN DATABASE ") + GetQuotedFullIdentifier();
			}
			if (varname != wxT("search_path") && varname != wxT("temp_tablespaces"))
				sql += wxT(" SET ") + varname + wxT("='") + varvalue + wxT("';\n");
			else
				sql += wxT(" SET ") + varname + wxT("=") + varvalue + wxT(";\n");
		}

		if (myConn)
		{
			if (!myConn->BackendMinimumVersion(8, 2))
				sql += GetGrant(wxT("CT"));
			else
				sql += GetGrant(wxT("CTc"));
		}

		sql += wxT("\n") + pgDatabase::GetDefaultPrivileges('r', m_defPrivsOnTables, wxT(""));
		sql += pgDatabase::GetDefaultPrivileges('S', m_defPrivsOnSeqs, wxT(""));
		sql += pgDatabase::GetDefaultPrivileges('f', m_defPrivsOnFuncs, wxT(""));

		sql += GetCommentSql();
	}
	return sql;
}



void pgDatabase::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (Connect() == PGCONN_OK)
	{
		// Set the icon if required
		UpdateIcon(browser);

		// Add child nodes if necessary
		if (browser->GetChildrenCount(GetId(), false) == 0)
		{
			wxLogInfo(wxT("Adding child object to database %s"), GetIdentifier().c_str());

			if (settings->GetDisplayOption(_("Catalogs")))
				browser->AppendCollection(this, catalogFactory);
			if (settings->GetDisplayOption(_("Casts")))
				browser->AppendCollection(this, castFactory);
			if (settings->GetDisplayOption(_("Languages")))
				browser->AppendCollection(this, languageFactory);
			if (settings->GetDisplayOption(_("Synonyms")) && connection()->EdbMinimumVersion(8, 0))
				if (!GetConnection()->BackendMinimumVersion(8, 4))
					browser->AppendCollection(this, synonymFactory);
			if (settings->GetDisplayOption(_("Schemas")))
				browser->AppendCollection(this, schemaFactory);
			if (settings->GetDisplayOption(_("Slony-I Clusters")))
				browser->AppendCollection(this, slClusterFactory);

			wxString missingFKsql = wxT("SELECT COUNT(*) FROM\n")
			                        wxT("   (SELECT tgargs from pg_trigger tr\n")
			                        wxT("      LEFT JOIN pg_depend dep ON dep.objid=tr.oid AND deptype = 'i'\n")
			                        wxT("      LEFT JOIN pg_constraint co ON refobjid = co.oid AND contype = 'f'\n")
			                        wxT("     WHERE \n");
			if (connection()->BackendMinimumVersion(8, 5))
				missingFKsql += wxT("tgconstraint <> 0\n");
			else
				missingFKsql += wxT("tgisconstraint\n");
			missingFKsql += wxT("     AND co.oid IS NULL\n")
			                wxT("     GROUP BY tgargs\n")
			                wxT("    HAVING count(1) = 3) AS foo");
			missingFKs = StrToLong(connection()->ExecuteScalar(missingFKsql));
		}
	}

	GetServer()->iSetLastDatabase(GetName());

	if (properties)
	{
		// Setup listview
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), NumToStr(GetOid()));
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		if (!GetPath().IsEmpty())
			properties->AppendItem(_("Path"), GetPath());

		// We may not actually be connected...
		if (GetConnection() && GetConnection()->BackendMinimumVersion(8, 0))
		{
			properties->AppendItem(_("Tablespace"), GetTablespace());
			properties->AppendItem(_("Default tablespace"), GetDefaultTablespace());
		}
		properties->AppendItem(_("Encoding"), GetEncoding());

		if (GetConnection() && GetConnection()->BackendMinimumVersion(8, 4))
		{
			properties->AppendItem(_("Collation"), GetCollate());
			properties->AppendItem(_("Character type"), GetCType());
		}

		properties->AppendItem(_("Default schema"), defaultSchema);

		properties->AppendItem(_("Default table ACL"), m_defPrivsOnTables);
		properties->AppendItem(_("Default sequence ACL"), m_defPrivsOnSeqs);
		properties->AppendItem(_("Default function ACL"), m_defPrivsOnFuncs);

		size_t i;
		wxString username;
		wxString varname;
		wxString varvalue;
		for (i = 0 ; i < variables.GetCount() ; i++)
		{
			wxStringTokenizer tkz(variables.Item(i), wxT("="));
			while (tkz.HasMoreTokens())
			{
				username = tkz.GetNextToken();
				varname = tkz.GetNextToken();
				varvalue = tkz.GetNextToken();
			}

			if (username.Length() == 0)
			{
				properties->AppendItem(varname, varvalue);
			}
			else
			{
				// should we add the parameters for the username?
				// I don't think so
				// but if we want this, how will we display that?
			}
		}
		properties->AppendItem(_("Allow connections?"), GetAllowConnections());
		properties->AppendItem(_("Connected?"), GetConnected());
		if (GetConnection() && GetConnection()->BackendMinimumVersion(8, 1))
		{
			wxString strConnLimit;
			strConnLimit.Printf(wxT("%ld"), GetConnectionLimit());
			properties->AppendItem(_("Connection limit"), strConnLimit);
		}
		properties->AppendItem(_("System database?"), GetSystemObject());
		if (GetMissingFKs())
			properties->AppendItem(_("Old style FKs"), GetMissingFKs());
		if (!GetSchemaRestriction().IsEmpty())
			properties->AppendItem(_("Schema restriction"), GetSchemaRestriction());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
	if (form && GetCanHint() && !hintShown)
	{
		ShowHint(form, false);
	}
}



pgObject *pgDatabase::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgDatabase *database = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		database = (pgDatabase *)databaseFactory.CreateObjects(coll, 0, wxT(" WHERE db.oid=") + GetOidStr() + wxT("\n"));

	return database;
}


pgObject *pgDatabaseFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgDatabase *database = 0;

	pgSet *databases;

	wxString datcollate, datctype, datconnlimit;

	if (collection->GetConnection()->BackendMinimumVersion(8, 1))
	{
		datconnlimit = wxT(", db.datconnlimit as connectionlimit");
	}
	if (collection->GetConnection()->BackendMinimumVersion(8, 4))
	{
		datctype = wxT(", db.datctype as ctype");
		datcollate = wxT(", db.datcollate as collate");
	}

	wxString restr = restriction;
	if (!collection->GetServer()->GetDbRestriction().IsEmpty())
	{
		if (restr.IsEmpty())
			restr = wxT(" WHERE datname IN (");
		else
			restr = wxT("   AND datname IN (");

		restr += collection->GetServer()->GetDbRestriction() + wxT(")\n");
	}

	// In 9.0+, database config options are in pg_db_role_setting
	if (collection->GetConnection()->BackendMinimumVersion(9, 0))
	{
		wxString setconfig = wxT("SELECT array(select coalesce('''' || rolname || '''', '') || '=' || unnest(setconfig) ")
		                     wxT("FROM pg_db_role_setting setting LEFT JOIN pg_roles role ON setting.setrole=role.oid ")
		                     wxT("WHERE setdatabase = db.oid)");
		databases = collection->GetServer()->ExecuteSet(
		                wxT("SELECT db.oid, datname, db.dattablespace AS spcoid, spcname, datallowconn, (") + setconfig + wxT(") AS datconfig, datacl, ")
		                wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
		                wxT("has_database_privilege(db.oid, 'CREATE') as cancreate, \n")
		                wxT("current_setting('default_tablespace') AS default_tablespace, \n")
		                wxT("descr.description\n") +
		                datconnlimit + datcollate + datctype +
		                wxT("  FROM pg_database db\n")
		                wxT("  LEFT OUTER JOIN pg_tablespace ta ON db.dattablespace=ta.OID\n")
		                wxT("  LEFT OUTER JOIN pg_shdescription descr ON db.oid=descr.objoid\n")
		                + restr +
		                wxT(" ORDER BY datname"));
	}
	else if (collection->GetConnection()->BackendMinimumVersion(8, 0))
		databases = collection->GetServer()->ExecuteSet(
		                wxT("SELECT db.oid, datname, db.dattablespace AS spcoid, spcname, datallowconn, datconfig, datacl, ")
		                wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
		                wxT("has_database_privilege(db.oid, 'CREATE') as cancreate, \n")
		                wxT("current_setting('default_tablespace') AS default_tablespace, \n")
		                wxT("descr.description\n") +
		                datconnlimit + datcollate + datctype +
		                wxT("  FROM pg_database db\n")
		                wxT("  LEFT OUTER JOIN pg_tablespace ta ON db.dattablespace=ta.OID\n")
		                wxT("  LEFT OUTER JOIN ")
		                + wxString(collection->GetConnection()->BackendMinimumVersion(8, 2) ? wxT("pg_shdescription") : wxT("pg_description")) +
		                wxT(" descr ON db.oid=descr.objoid\n")
		                + restr +
		                wxT(" ORDER BY datname"));
	else
		databases = collection->GetServer()->ExecuteSet(
		                wxT("SELECT db.oid, datname, datpath, datallowconn, datconfig, datacl, ")
		                wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
		                wxT("has_database_privilege(db.oid, 'CREATE') as cancreate,\n")
		                wxT("descr.description\n")
		                wxT("  FROM pg_database db\n")
		                wxT("  LEFT OUTER JOIN pg_description descr ON db.oid=descr.objoid\n")
		                + restr +
		                wxT(" ORDER BY datname"));

	if (databases)
	{
		while (!databases->Eof())
		{
			wxString name = databases->GetVal(wxT("datname"));
			database = new pgDatabase(name);
			database->iSetServer(collection->GetServer());
			database->iSetOid(databases->GetOid(wxT("oid")));
			database->iSetOwner(databases->GetVal(wxT("datowner")));
			database->iSetAcl(databases->GetVal(wxT("datacl")));
			database->iSetEncoding(databases->GetVal(wxT("serverencoding")));
			database->iSetCreatePrivilege(databases->GetBool(wxT("cancreate")));
			database->iSetComment(databases->GetVal(wxT("description")));
			wxString str = databases->GetVal(wxT("datconfig"));
			if (!collection->GetConnection()->BackendMinimumVersion(8, 5))
			{
				str = TransformToNewDatconfig(str.Mid(1, str.Length() - 2));
				//str = tmp;
			}
			else
				str = str.Mid(1, str.Length() - 2);
			if (!str.IsEmpty())
				FillArray(database->GetVariables(), str);
			database->iSetAllowConnections(databases->GetBool(wxT("datallowconn")));

			if (collection->GetConnection()->BackendMinimumVersion(8, 0))
			{
				database->iSetTablespace(databases->GetVal(wxT("spcname")));
				database->iSetTablespaceOid(databases->GetOid(wxT("spcoid")));
				if (databases->GetVal(wxT("default_tablespace")) == wxEmptyString || databases->GetVal(wxT("default_tablespace")) == wxT("unset"))
					database->iSetDefaultTablespace(databases->GetVal(wxT("spcname")));
				else
					database->iSetDefaultTablespace(databases->GetVal(wxT("default_tablespace")));
			}
			else
				database->iSetPath(databases->GetVal(wxT("datpath")));

			if (collection->GetConnection()->BackendMinimumVersion(8, 1))
			{
				database->iSetConnectionLimit(databases->GetLong(wxT("connectionlimit")));
			}
			if (collection->GetConnection()->BackendMinimumVersion(8, 4))
			{
				database->iSetCollate(databases->GetVal(wxT("collate")));
				database->iSetCType(databases->GetVal(wxT("ctype")));
			}

			if (collection->GetServer()->GetServerIndex())
			{
				wxString value;
				settings->Read(wxT("Servers/") + NumToStr(collection->GetServer()->GetServerIndex())
				               + wxT("/Databases/") + name + wxT("/SchemaRestriction"), &value, wxEmptyString);

				database->iSetSchemaRestriction(value);
			}

			// Add the treeview node if required
			if (settings->GetShowSystemObjects() || !database->GetSystemObject())
			{
				if (browser)
				{
					int icon;
					if (database->GetName() == database->GetServer()->GetDatabaseName())
						icon = databaseFactory.GetIconId();
					else
						icon = databaseFactory.GetClosedIconId();

					browser->AppendItem(collection->GetId(), database->GetIdentifier(), icon, -1, database);
				}
				else
					break;
			}
			else
				delete database;

			databases->MoveNext();
		}
		delete databases;
	}
	return database;
}

wxString pgDatabase::GetDefaultPrivileges(const wxChar &cType, wxString strDefPrivs, const wxString &strSchema)
{
	wxString strDefPrivsSql;

	if (!strDefPrivs.IsEmpty())
	{
		wxString strRole, strPriv, strSupportedPrivs, strType;
		strDefPrivs.Replace(wxT("\\\""), wxT("\""), true);
		strDefPrivs.Replace(wxT("\\\\"), wxT("\\"), true);

		switch(cType)
		{
			case 'r':
				strType = wxT("TABLES");
				strSupportedPrivs = wxT("arwdDxt");
				break;
			case 'S':
				strType = wxT("SEQUENCES");
				strSupportedPrivs = wxT("rwU");
				break;
			case 'f':
				strType = wxT("FUNCTIONS");
				strSupportedPrivs = wxT("X");
				break;
			default:
				return wxT("");
		}

		// Removing starting brace '{' and ending brace '}'
		strDefPrivs = strDefPrivs.SubString(1, strDefPrivs.Length() - 1);

		while (pgObject::findUserPrivs(strDefPrivs, strRole, strPriv))
		{
			strDefPrivsSql += pgObject::GetDefaultPrivileges(strType, strSupportedPrivs, strSchema, wxT(""), strPriv, qtIdent(strRole));

			strRole = wxT("");
			strPriv = wxT("");
		}
	}
	return strDefPrivsSql;
}

bool pgDatabase::CanCreate()
{
	return GetDatabase()->GetCreatePrivilege();
}

bool pgDatabase::CanDebugPlpgsql()
{
	wxString preload_option;

	// Result cache - 0 = not tested, 1 = false, 2 = true.
	if (canDebugPlpgsql == 1)
		return false;
	else if (canDebugPlpgsql == 2)
		return true;

	// "show shared_preload_libraries" does not work for other than
	// the super users.
	if (GetServer()->GetSuperUser())
	{
		// Parameter's name depends of the backend's version
		if (server->GetConnection()->BackendMinimumVersion(8, 2))
		{
			preload_option = wxT("shared_preload_libraries");
		}
		else
		{
			preload_option = wxT("preload_libraries");
		}

		// Check the appropriate plugin is loaded
		if (!ExecuteScalar(wxT("SHOW ") + preload_option).Contains(wxT("plugin_debugger")))
		{
			canDebugPlpgsql = 1;
			return false;
		}
	}

	if (ExecuteScalar(wxT("SELECT count(*) FROM pg_proc WHERE proname = 'pldbg_get_target_info';")) == wxT("0"))
	{
		canDebugPlpgsql = 1;
		return false;
	}

	// If this is EDBAS81, the debuggers will be built into the PLs
	// so we don't need to check any further.
	if (server->GetConnection()->EdbMinimumVersion(8, 1))
	{
		canDebugPlpgsql = 2;
		return true;
	}

	// On EDBAS82 and PostgreSQL, we need to check to make sure that
	// the debugger library is also available.
	if (ExecuteScalar(wxT("SELECT count(*) FROM pg_proc WHERE proname = 'plpgsql_oid_debug';")) == wxT("0"))
	{
		canDebugPlpgsql = 1;
		return false;
	}
	else
	{
		canDebugPlpgsql = 2;
		return true;
	}

	return true;
}

bool pgDatabase::CanDebugEdbspl()
{
	// Result cache - 0 = not tested, 1 = false, 2 = true.
	if (canDebugEdbspl == 1)
		return false;
	else if (canDebugEdbspl == 2)
		return true;

	// "show shared_preload_libraries" does not work for other than
	// the super users.
	if (GetServer()->GetSuperUser())
	{
		// Check the appropriate plugin is loaded
		if (!ExecuteScalar(wxT("SHOW shared_preload_libraries;")).Contains(wxT("plugin_spl_debugger")))
		{
			canDebugEdbspl = 1;
			return false;
		}
	}

	if (ExecuteScalar(wxT("SELECT count(*) FROM pg_proc WHERE proname = 'pldbg_get_target_info';")) == wxT("0"))
	{
		canDebugEdbspl = 1;
		return false;
	}

	// If this is EDBAS81, the debuggers will be built into the PLs
	// so we don't need to check any further.
	if (server->GetConnection()->EdbMinimumVersion(8, 1))
	{
		canDebugEdbspl = 2;
		return true;
	}

	// On EDBAS82 and PostgreSQL, we need to check to make sure that
	// the debugger library is also available.
	if (ExecuteScalar(wxT("SELECT count(*) FROM pg_proc WHERE proname = 'edb_oid_debug';")) == wxT("0"))
	{
		canDebugEdbspl = 1;
		return false;
	}
	else
	{
		canDebugEdbspl = 2;
		return true;
	}

	return true;
}

pgDatabaseCollection::pgDatabaseCollection(pgaFactory *factory, pgServer *sv)
	: pgServerObjCollection(factory, sv)
{
}


void pgDatabaseCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxLogInfo(wxT("Displaying statistics for databases on ") + GetServer()->GetIdentifier());

	bool hasSize = GetConnection()->HasFeature(FEATURE_SIZE);

	wxString restr;
	if (!GetServer()->GetDbRestriction().IsEmpty())
	{
		if (restr.IsEmpty())
			restr = wxT(" WHERE datname IN (");
		else
			restr = wxT("   AND datname IN (");

		restr += GetServer()->GetDbRestriction() + wxT(")\n");
	}

	wxString sql = wxT("SELECT datid, datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit");

	if (GetConnection()->BackendMinimumVersion(8, 3))
		sql += wxT(", tup_returned, tup_fetched, tup_inserted, tup_updated, tup_deleted");
	if (hasSize)
		sql += wxT(", pg_size_pretty(pg_database_size(datid)) as size");

	sql += wxT("\n  FROM pg_stat_database db\n") + restr + wxT(" ORDER BY datname");

	// Add the statistics view columns
	statistics->ClearAll();
	statistics->AddColumn(_("Database"), 60);
	statistics->AddColumn(_("Backends"), 50);
	if (hasSize)
		statistics->AddColumn(_("Size"), 60);
	statistics->AddColumn(_("Xact Committed"), 60);
	statistics->AddColumn(_("Xact Rolled Back"), 60);
	statistics->AddColumn(_("Blocks Read"), 60);
	statistics->AddColumn(_("Blocks Hit"), 60);
	if (GetConnection()->BackendMinimumVersion(8, 3))
	{
		statistics->AddColumn(_("Tuples Returned"), 60);
		statistics->AddColumn(_("Tuples Fetched"), 60);
		statistics->AddColumn(_("Tuples Inserted"), 60);
		statistics->AddColumn(_("Tuples Updated"), 60);
		statistics->AddColumn(_("Tuples Deleted"), 60);
	}

	bool sysobj;
	pgSet *stats = GetServer()->ExecuteSet(sql);
	if (stats)
	{
		while (!stats->Eof())
		{
			if (stats->GetVal(wxT("datname")) == wxT("template0"))
				sysobj = true;
			else
				sysobj = stats->GetOid(wxT("datid")) <= GetServer()->GetLastSystemOID();

			if (settings->GetShowSystemObjects() || !sysobj)
			{
				statistics->InsertItem(statistics->GetItemCount(), stats->GetVal(wxT("datname")), PGICON_STATISTICS);
				statistics->SetItem(statistics->GetItemCount() - 1, 1, stats->GetVal(wxT("numbackends")));
				if (hasSize)
					statistics->SetItem(statistics->GetItemCount() - 1, 2, stats->GetVal(wxT("size")));
				statistics->SetItem(statistics->GetItemCount() - 1, 2 + (hasSize ? 1 : 0), stats->GetVal(wxT("xact_commit")));
				statistics->SetItem(statistics->GetItemCount() - 1, 3 + (hasSize ? 1 : 0), stats->GetVal(wxT("xact_rollback")));
				statistics->SetItem(statistics->GetItemCount() - 1, 4 + (hasSize ? 1 : 0), stats->GetVal(wxT("blks_read")));
				statistics->SetItem(statistics->GetItemCount() - 1, 5 + (hasSize ? 1 : 0), stats->GetVal(wxT("blks_hit")));
				if (GetConnection()->BackendMinimumVersion(8, 3))
				{
					statistics->SetItem(statistics->GetItemCount() - 1, 6 + (hasSize ? 1 : 0), stats->GetVal(wxT("tup_returned")));
					statistics->SetItem(statistics->GetItemCount() - 1, 7 + (hasSize ? 1 : 0), stats->GetVal(wxT("tup_fetched")));
					statistics->SetItem(statistics->GetItemCount() - 1, 8 + (hasSize ? 1 : 0), stats->GetVal(wxT("tup_inserted")));
					statistics->SetItem(statistics->GetItemCount() - 1, 9 + (hasSize ? 1 : 0), stats->GetVal(wxT("tup_updated")));
					statistics->SetItem(statistics->GetItemCount() - 1, 10 + (hasSize ? 1 : 0), stats->GetVal(wxT("tup_deleted")));
				}
			}
			stats->MoveNext();
		}

		delete stats;
	}
}



/////////////////////////////////////////////////////

pgDatabaseObjCollection::pgDatabaseObjCollection(pgaFactory *factory, pgDatabase *db)
	: pgCollection(factory)
{
	database = db;
	server = database->GetServer();
}


bool pgDatabaseObjCollection::CanCreate()
{
	if (IsCollectionForType(PGM_CATALOG))
		return false;

	return GetDatabase()->GetCreatePrivilege();
}


#include "images/database.xpm"
#include "images/database-sm.xpm"
#include "images/databases.xpm"
#include "images/closeddatabase.xpm"
#include "images/closeddatabase-sm.xpm"

pgDatabaseFactory::pgDatabaseFactory()
	: pgServerObjFactory(__("Database"), __("New Database..."), __("Create a new Database."), database_xpm, database_sm_xpm)
{
	metaType = PGM_DATABASE;
	closedId = addIcon(closeddatabase_xpm);
	smallClosedId = addIcon(closeddatabase_sm_xpm);
}

pgCollection *pgDatabaseFactory::CreateCollection(pgObject *obj)
{
	return new pgDatabaseCollection(GetCollectionFactory(), (pgServer *)obj);
}

pgCollection *pgDatabaseObjFactory::CreateCollection(pgObject *obj)
{
	return new pgDatabaseObjCollection(GetCollectionFactory(), (pgDatabase *)obj);
}


pgDatabaseFactory databaseFactory;
static pgaCollectionFactory cf(&databaseFactory, __("Databases"), databases_xpm);
