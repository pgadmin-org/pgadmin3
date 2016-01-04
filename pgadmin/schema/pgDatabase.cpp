//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "schema/pgExtension.h"
#include "schema/pgForeignDataWrapper.h"
#include "schema/pgLanguage.h"
#include "schema/pgSchema.h"
#include "schema/pgEventTrigger.h"
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


wxString pgDatabase::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on database");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing database");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop database \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop database \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop database cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop database?");
			break;
		case PROPERTIESREPORT:
			message = _("Database properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Database properties");
			break;
		case DDLREPORT:
			message = _("Database DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Database DDL");
			break;
		case STATISTICSREPORT:
			message = _("Database statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Database statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Database dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Database dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Database dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Database dependents");
			break;
		case BACKUPTITLE:
			message = wxString::Format(_("Backup database \"%s\""),
			                           GetFullIdentifier().c_str());
			break;
		case RESTORETITLE:
			message = wxString::Format(_("Restore database \"%s\""),
			                           GetFullIdentifier().c_str());
			break;
	}

	return message;
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
		if (settings->GetDisplayOption(_("Extensions")) && GetConnection()->BackendMinimumVersion(9, 1))
			extensionFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Foreign Data Wrappers")) && GetConnection()->BackendMinimumVersion(8, 4))
			foreignDataWrapperFactory.AppendMenu(menu);
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
		if (connection()->BackendMinimumVersion(9, 2))
		{
			m_defPrivsOnTypes = connection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = 0::OID AND defaclobjtype='T'"));
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
	int rc = -1;

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
	{
		// if the server release is 9.1 or more and the server has no adminpack
		if (GetConnection()->BackendMinimumVersion(9, 1))
		{
			// Search the adminpack extension
			pgSet *set = GetConnection()->ExecuteSet(wxT("SELECT 1 FROM pg_available_extensions WHERE name='adminpack'"));
			if (set->NumRows() == 1)
				hints.Add(HINT_INSTRUMENTATION_91_WITH);
			else
				hints.Add(HINT_INSTRUMENTATION_91_WITHOUT);
			delete set;
		}
		else
			hints.Add(HINT_INSTRUMENTATION);
	}

	if (force || !hintShown)
	{
		rc = frmHint::ShowHint(form, hints, GetFullIdentifier(), force);
		if (rc == HINT_RC_FIX)
			GetConnection()->ExecuteVoid(wxT("CREATE EXTENSION adminpack"), true);
	}
	hintShown = true;
}


void pgDatabase::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	if (GetConnection())
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

		if (connection()->BackendMinimumVersion(9, 1))
			sql += wxT(", stats_reset AS ") + qtIdent(_("Last statistics reset")) +
			       wxT(", slave.confl_tablespace AS ") + qtIdent(_("Tablespace conflicts")) +
			       wxT(", slave.confl_lock AS ") + qtIdent(_("Lock conflicts")) +
			       wxT(", slave.confl_snapshot AS ") + qtIdent(_("Snapshot conflicts")) +
			       wxT(", slave.confl_bufferpin AS ") + qtIdent(_("Bufferpin conflicts")) +
			       wxT(", slave.confl_deadlock AS ") + qtIdent(_("Deadlock conflicts"));

		if (connection()->BackendMinimumVersion(9, 2))
			sql += wxT(", temp_files AS ") + qtIdent(_("Temporary files")) +
			       wxT(", pg_size_pretty(temp_bytes) AS ") + qtIdent(_("Size of temporary files")) +
			       wxT(", deadlocks AS ") + qtIdent(_("Deadlocks")) +
			       wxT(", blk_read_time AS ") + qtIdent(_("Block read time")) +
			       wxT(", blk_write_time AS ") + qtIdent(_("Block write time"));

		if (hasSize)
			sql += wxT(", pg_size_pretty(pg_database_size(db.datid)) AS ") + qtIdent(_("Size"));

		sql += wxT("\n  FROM pg_stat_database db");
		if (connection()->BackendMinimumVersion(9, 1))
			sql += wxT(" JOIN pg_stat_database_conflicts slave ON db.datid=slave.datid");
		sql += wxT(" WHERE db.datname=") + qtDbString(GetName());

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

		if (variables.GetCount() > 0)
		{
			size_t i;
			wxString username = wxEmptyString;
			wxString parameter = wxEmptyString;
			wxString value = wxEmptyString;

			sql += wxT("\n");
			for (i = 0 ; i < variables.GetCount() ; i += 3)
			{
				username = variables.Item(i);
				parameter = variables.Item(i + 1);
				value = variables.Item(i + 2);

				if (username.Length() == 0)
				{
					sql += wxT("ALTER DATABASE ") + GetQuotedFullIdentifier();
				}
				else
				{
					sql += wxT("ALTER ROLE ") + username + wxT(" IN DATABASE ") + GetQuotedFullIdentifier();
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
		sql += pgDatabase::GetDefaultPrivileges('T', m_defPrivsOnTypes, wxT(""));

		sql += GetCommentSql();

		if (myConn->BackendMinimumVersion(9, 2))
			sql += GetSeqLabelsSql();
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
			if (connection()->BackendMinimumVersion(9, 3) && settings->GetDisplayOption(_("Event Triggers")))
				browser->AppendCollection(this, eventTriggerFactory);
			if (settings->GetDisplayOption(_("Extensions")) && GetConnection()->BackendMinimumVersion(9, 1))
				browser->AppendCollection(this, extensionFactory);
			if (settings->GetDisplayOption(_("Foreign Data Wrappers")) && GetConnection()->BackendMinimumVersion(8, 4))
				browser->AppendCollection(this, foreignDataWrapperFactory);
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
			if (connection()->BackendMinimumVersion(9, 0))
				missingFKsql += wxT("tgisinternal\n");
			else
				missingFKsql += wxT("tgisconstraint\n");

			missingFKsql += wxT("     AND co.oid IS NULL\n")
			                wxT("     GROUP BY tgargs\n")
			                wxT("    HAVING count(1) = 3) AS foo");
			missingFKs = StrToLong(connection()->ExecuteScalar(missingFKsql));

			// Get configuration
			wxString query;
			if (connection()->BackendMinimumVersion(9, 0))
			{
				query = wxT("WITH configs AS ")
				        wxT("(SELECT rolname, unnest(setconfig) AS config")
				        wxT(" FROM pg_db_role_setting s")
				        wxT(" LEFT JOIN pg_roles r ON r.oid=s.setrole")
				        wxT(" WHERE s.setdatabase=") + NumToStr(GetOid()) + wxT(")\n")
				        wxT("SELECT rolname, split_part(config, '=', 1) AS variable, ")
				        wxT("       replace(config, split_part(config, '=', 1) || '=', '') AS value\n")
				        wxT("FROM configs");
			}
			else
			{
				wxString query_mydb = wxT("SELECT datconfig FROM pg_database WHERE oid=") + NumToStr(GetOid());
				query = wxT("SELECT '' AS rolname, split_part(config, '=', 1) AS variable,\n")
				        wxT("       replace(config,split_part(config, '=', 1) || '=', '') AS value\n")
				        wxT("FROM (\n")
				        wxT("    SELECT\n")
				        wxT("      (\n")
				        wxT("      SELECT datconfig[i]\n")
				        wxT("      FROM pg_database\n")
				        wxT("      WHERE oid=") + NumToStr(GetOid()) + wxT("\n")
				        wxT("      ) AS config\n")
				        wxT("    FROM generate_series(array_lower((") + query_mydb + wxT("),1), array_upper((") + query_mydb + wxT("),1)) AS i\n")
				        wxT("     ) configs");
			}
			pgSet *configs = connection()->ExecuteSet(query);
			if (configs)
			{
				while (!configs->Eof())
				{
					variables.Add(configs->GetVal(wxT("rolname")));
					variables.Add(configs->GetVal(wxT("variable")));
					variables.Add(configs->GetVal(wxT("value")));
					configs->MoveNext();
				}
				delete configs;
			}
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

		if (GetConnection() && GetConnection()->BackendMinimumVersion(9, 0))
		{
			properties->AppendItem(_("Default table ACL"), m_defPrivsOnTables);
			properties->AppendItem(_("Default sequence ACL"), m_defPrivsOnSeqs);
			properties->AppendItem(_("Default function ACL"), m_defPrivsOnFuncs);
		}
		if (GetConnection() && GetConnection()->BackendMinimumVersion(9, 2))
			properties->AppendItem(_("Default type ACL"), m_defPrivsOnTypes);

		size_t i;
		wxString username;
		wxString parameter;
		wxString value;
		for (i = 0 ; i < variables.GetCount() ; i += 3)
		{
			username = variables.Item(i);
			parameter = variables.Item(i + 1);
			value = variables.Item(i + 2);

			if (username.Length() == 0)
			{
				properties->AppendItem(parameter, value);
			}
			else
			{
				properties->AppendItem(parameter + wxT(" (role ") + username + wxT(")"), value);
			}
		}
		properties->AppendYesNoItem(_("Allow connections?"), GetAllowConnections());
		properties->AppendYesNoItem(_("Connected?"), GetConnected());
		if (GetConnection() && GetConnection()->BackendMinimumVersion(8, 1))
		{
			wxString strConnLimit;
			strConnLimit.Printf(wxT("%ld"), GetConnectionLimit());
			properties->AppendItem(_("Connection limit"), strConnLimit);
		}
		properties->AppendYesNoItem(_("System database?"), GetSystemObject());
		if (GetMissingFKs())
			properties->AppendItem(_("Old style FKs"), GetMissingFKs());
		if (!GetSchemaRestriction().IsEmpty())
			properties->AppendItem(_("Schema restriction"), GetSchemaRestriction());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

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

	wxString datcollate, datctype, datconnlimit, seclabelsql;

	if (collection->GetConnection()->BackendMinimumVersion(8, 1))
	{
		datconnlimit = wxT(", db.datconnlimit as connectionlimit");
	}
	if (collection->GetConnection()->BackendMinimumVersion(8, 4))
	{
		datctype = wxT(", db.datctype as ctype");
		datcollate = wxT(", db.datcollate as collate");
	}
	if (collection->GetConnection()->BackendMinimumVersion(9, 2))
	{
		seclabelsql = wxT(",\n(SELECT array_agg(label) FROM pg_shseclabel sl1 WHERE sl1.objoid=db.oid) AS labels")
		              wxT(",\n(SELECT array_agg(provider) FROM pg_shseclabel sl2 WHERE sl2.objoid=db.oid) AS providers");
	}

	wxString restr = restriction;
	if (!collection->GetServer()->GetDbRestriction().IsEmpty())
	{
		if (restr.IsEmpty())
			restr = wxT(" WHERE datname IN (");
		else
			restr += wxT("   AND datname IN (");

		restr += collection->GetServer()->GetDbRestriction() + wxT(")\n");
	}

	// In 9.0+, database config options are in pg_db_role_setting
	if (collection->GetConnection()->BackendMinimumVersion(9, 0))
	{
		databases = collection->GetServer()->ExecuteSet(
		                wxT("SELECT db.oid, datname, db.dattablespace AS spcoid, spcname, datallowconn, datacl, ")
		                wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
		                wxT("has_database_privilege(db.oid, 'CREATE') as cancreate, \n")
		                wxT("current_setting('default_tablespace') AS default_tablespace, \n")
		                wxT("descr.description\n") +
		                datconnlimit + datcollate + datctype + seclabelsql +
		                wxT("  FROM pg_database db\n")
		                wxT("  LEFT OUTER JOIN pg_tablespace ta ON db.dattablespace=ta.OID\n")
		                wxT("  LEFT OUTER JOIN pg_shdescription descr ON (db.oid=descr.objoid AND descr.classoid='pg_database'::regclass)\n")
		                + restr +
		                wxT(" ORDER BY datname"));
	}
	else if (collection->GetConnection()->BackendMinimumVersion(8, 0))
		databases = collection->GetServer()->ExecuteSet(
		                wxT("SELECT db.oid, datname, db.dattablespace AS spcoid, spcname, datallowconn, datacl, ")
		                wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
		                wxT("has_database_privilege(db.oid, 'CREATE') as cancreate, \n")
		                wxT("current_setting('default_tablespace') AS default_tablespace, \n")
		                wxT("descr.description\n") +
		                datconnlimit + datcollate + datctype +
		                wxT("  FROM pg_database db\n")
		                wxT("  LEFT OUTER JOIN pg_tablespace ta ON db.dattablespace=ta.OID\n")
		                wxT("  LEFT OUTER JOIN ")
		                + wxString(collection->GetConnection()->BackendMinimumVersion(8, 2) ? wxT("pg_shdescription") : wxT("pg_description")) +
		                wxT(" descr ON (db.oid=descr.objoid AND descr.classoid='pg_database'::regclass)\n")
		                + restr +
		                wxT(" ORDER BY datname"));
	else
		databases = collection->GetServer()->ExecuteSet(
		                wxT("SELECT db.oid, datname, datpath, datallowconn, datacl, ")
		                wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
		                wxT("has_database_privilege(db.oid, 'CREATE') as cancreate,\n")
		                wxT("descr.description\n")
		                wxT("  FROM pg_database db\n")
		                wxT("  LEFT OUTER JOIN pg_description descr ON (db.oid=descr.objoid AND descr.classoid='pg_database'::regclass)\n")
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

			if (collection->GetConnection()->BackendMinimumVersion(9, 2))
			{
				database->iSetProviders(databases->GetVal(wxT("providers")));
				database->iSetLabels(databases->GetVal(wxT("labels")));
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
			case 'T':
				strType = wxT("TYPES");
				strSupportedPrivs = wxT("U");
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

		// Before EDBAS92, there was a separate library for SPL and PL/pgSQL.
		// Starting with 9.2, EDB uses the community version of pldebugger,
		// and support for both languages is built into plugin_debugger.so
		wxString library_name;

		if (server->GetConnection()->EdbMinimumVersion(9, 2))
			library_name = wxT("plugin_debugger");
		else
			library_name = wxT("plugin_spl_debugger");

		if (!ExecuteScalar(wxT("SHOW shared_preload_libraries;")).Contains(library_name))
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


wxString pgDatabaseCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on databases");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing databases");
			break;
		case STATISTICSREPORT:
			message = _("Databases statistics report");
			break;
		case OBJSTATISTICS:
			message = _("Databases statistics");
			break;
		case OBJECTSLISTREPORT:
			message = _("Databases list report");
			break;
	}

	return message;
}


void pgDatabaseCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxLogInfo(wxT("Displaying statistics for databases on ") + GetServer()->GetIdentifier());

	bool hasSize = GetConnection()->HasFeature(FEATURE_SIZE);

	wxString restr;
	if (!GetServer()->GetDbRestriction().IsEmpty())
	{
		if (restr.IsEmpty())
			restr = wxT(" WHERE db.datname IN (");
		else
			restr = wxT("   AND db.datname IN (");

		restr += GetServer()->GetDbRestriction() + wxT(")\n");
	}

	wxString sql = wxT("SELECT db.datid, db.datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit");

	if (GetConnection()->BackendMinimumVersion(8, 3))
		sql += wxT(", tup_returned, tup_fetched, tup_inserted, tup_updated, tup_deleted");
	if (GetConnection()->BackendMinimumVersion(9, 1))
		sql += wxT(", stats_reset, slave.confl_tablespace, slave.confl_lock, slave.confl_snapshot, slave.confl_bufferpin, slave.confl_deadlock");
	if (hasSize)
		sql += wxT(", pg_size_pretty(pg_database_size(db.datid)) as size");

	sql += wxT("\n  FROM pg_stat_database db\n");
	if (GetConnection()->BackendMinimumVersion(9, 1))
		sql += wxT("  JOIN pg_stat_database_conflicts slave ON db.datid=slave.datid\n");
	sql += restr + wxT(" ORDER BY db.datname");

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
	if (GetConnection()->BackendMinimumVersion(9, 1))
	{
		statistics->AddColumn(_("Last statistics reset"), 60);
		statistics->AddColumn(_("Tablespace conflicts"), 60);
		statistics->AddColumn(_("Lock conflicts"), 60);
		statistics->AddColumn(_("Snapshot conflicts"), 60);
		statistics->AddColumn(_("Bufferpin conflicts"), 60);
		statistics->AddColumn(_("Deadlock conflicts"), 60);
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
				if (GetConnection()->BackendMinimumVersion(9, 1))
				{
					statistics->SetItem(statistics->GetItemCount() - 1, 11 + (hasSize ? 1 : 0), stats->GetVal(wxT("stats_reset")));
					statistics->SetItem(statistics->GetItemCount() - 1, 12 + (hasSize ? 1 : 0), stats->GetVal(wxT("confl_tablespace")));
					statistics->SetItem(statistics->GetItemCount() - 1, 13 + (hasSize ? 1 : 0), stats->GetVal(wxT("confl_lock")));
					statistics->SetItem(statistics->GetItemCount() - 1, 14 + (hasSize ? 1 : 0), stats->GetVal(wxT("confl_snapshot")));
					statistics->SetItem(statistics->GetItemCount() - 1, 15 + (hasSize ? 1 : 0), stats->GetVal(wxT("confl_bufferpin")));
					statistics->SetItem(statistics->GetItemCount() - 1, 16 + (hasSize ? 1 : 0), stats->GetVal(wxT("confl_deadlock")));
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


#include "images/database.pngc"
#include "images/database-sm.pngc"
#include "images/databases.pngc"
#include "images/closeddatabase.pngc"
#include "images/closeddatabase-sm.pngc"

pgDatabaseFactory::pgDatabaseFactory()
	: pgServerObjFactory(__("Database"), __("New Database..."), __("Create a new Database."), database_png_img, database_sm_png_img)
{
	metaType = PGM_DATABASE;
	closedId = addIcon(closeddatabase_png_img);
	smallClosedId = addIcon(closeddatabase_sm_png_img);
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
static pgaCollectionFactory cf(&databaseFactory, __("Databases"), databases_png_img);


disconnectDatabaseFactory::disconnectDatabaseFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Disconnec&t database"), _("Disconnect from the selected database."));
}


wxWindow *disconnectDatabaseFactory::StartDialog(frmMain *form, pgObject *obj)
{
	ctlTree *browser = form->GetBrowser();
	pgDatabase *database = (pgDatabase *)obj;

	if (obj->CheckOpenDialogs(browser, browser->GetSelection()))
	{
		wxString msg = _("There are properties dialogues open for one or more objects that would be refreshed. Please close the properties dialogues and try again.");
		wxMessageBox(msg, _("Cannot disconnect database"), wxICON_WARNING | wxOK);
	}
	else
	{
		database->Disconnect();
		database->UpdateIcon(browser);
		browser->DeleteChildren(obj->GetId());
		browser->SelectItem(browser->GetItemParent(obj->GetId()));
		form->execSelChange(browser->GetItemParent(obj->GetId()), true);
	}

	return 0;
}


bool disconnectDatabaseFactory::CheckEnable(pgObject *obj)
{
	if (obj && obj->IsCreatedBy(databaseFactory))
		return ((pgDatabase *)obj)->GetConnected() && (((pgDatabase *)obj)->GetName() != ((pgDatabase *)obj)->GetServer()->GetDatabaseName());

	return false;
}
