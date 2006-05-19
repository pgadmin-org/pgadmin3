//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgDatabase.cpp 4936 2006-01-19 14:13:54Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgDatabase.cpp - PostgreSQL Database
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgfeatures.h"
#include "frmMain.h"
#include "pgCast.h"
#include "pgLanguage.h"
#include "pgSchema.h"
#include "slCluster.h"
#include "frmHint.h"
#include "frmReport.h"

pgDatabase::pgDatabase(const wxString& newName)
: pgServerObject(databaseFactory, newName)
{
    wxLogInfo(wxT("Creating a pgDatabase object"));

    allowConnections = true;
    connected = false;
    conn = NULL;
    missingFKs=0;
}


pgDatabase::~pgDatabase()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));
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
    wxMenu *menu=pgObject::GetNewMenu();

    if (GetCreatePrivilege())
    {
        castFactory.AppendMenu(menu);
        languageFactory.AppendMenu(menu);
        schemaFactory.AppendMenu(menu);
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
            conn=0;
        }
        else
        {
            useServerConnection = false;
		    conn = CreateConn();

            if (!conn)
            {
                connected = false;
                return PGCONN_BAD;
            }
        }

        // Now we're connected.
        iSetComment(connection()->ExecuteScalar(wxT("SELECT description FROM pg_description WHERE objoid=") + GetOidStr()));

        // check for extended ruleutils with pretty-print option
        wxString exprname=connection()->ExecuteScalar(wxT("SELECT proname FROM pg_proc WHERE proname='pg_get_viewdef' AND proargtypes[1]=16"));
        if (!exprname.IsEmpty())
            prettyOption = wxT(", true");
    
        UpdateDefaultSchema();

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
    connected=false;
    if (conn)
        delete conn;
    conn=0;
}


bool pgDatabase::GetCanHint()
{
    if (encoding == wxT("SQL_ASCII"))
        return true;

    if (encoding == wxT("UNICODE"))
    {
        wxString ver=GetServer()->GetVersionString();
        if (ver.Find(wxT("mingw32")) > 0 && ver.Find(wxT("SQL 8.0.")) > 0)
            return true;
    }

    if (GetServer()->GetConnection() == GetConnection() && 
        GetConnection()->BackendMinimumVersion(8,0) && 
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
        wxString ver=GetServer()->GetVersionString();
        if (ver.Find(wxT("mingw32")) > 0 && ver.Find(wxT("SQL 8.0.")) > 0)
            hints.Add(HINT_ENCODING_UNICODE);
    }

    if (GetServer()->GetConnection() == GetConnection() && 
        GetConnection()->BackendMinimumVersion(8,0) && 
       !GetConnection()->HasFeature(FEATURE_FILEREAD))
        hints.Add(HINT_INSTRUMENTATION);
    
    if (force || !hintShown)
        frmHint::ShowHint(form, hints, GetFullIdentifier(), force);
    hintShown=true;
}


pgSet *pgDatabase::ExecuteSet(const wxString& sql)
{
    pgSet *set=0;
    if (connection())
    {
        set=connection()->ExecuteSet(sql);
        if (!set)
            CheckAlive();
    }
    return set;
}


wxString pgDatabase::ExecuteScalar(const wxString& sql)
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


bool pgDatabase::ExecuteVoid(const wxString& sql)
{
    bool rc=0;
    if (connection())
    {
        rc = connection()->ExecuteVoid(sql);
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
        pgSet *set=ExecuteSet(wxT("SELECT nspname, session_user=nspname AS isuser FROM pg_namespace"));
        if (set)
        {
            while (tk.HasMoreTokens())
            {
                wxString str=tk.GetNextToken();
                str.Strip(wxString::both);

                if (str.IsEmpty())
                    continue;
                long row;
                for (row=1 ; row <= set->NumRows() ; row++)
                {
                    set->Locate(row);
                    defaultSchema = set->GetVal(wxT("nspname"));
                    if (str == defaultSchema || 
                            (str == wxT("$user") && set->GetBool(wxT("isuser"))))
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
    wxString str=GetSchemaPrefix(name);
    if (!str.IsEmpty())
        return qtIdent(str.Left(str.Length()-1)) + wxT(".");
    return str;
}


bool pgDatabase::GetSystemObject() const
{
    if (server) {
        if (this->GetName() == wxT("template0")) return true;
        return (this->GetOid() <= server->GetLastSystemOID());
    } else {
        return false;
    }
}


wxArrayString pgDatabase::GetSlonyClusters(ctlTree *browser)
{
    wxArrayString clusters;

    pgCollection *collection=browser->FindCollection(slClusterFactory, GetId());
    if (collection)
    {
        treeObjectIterator clusterIterator(browser, collection);

        slCluster *cluster;
        while ((cluster=(slCluster*)clusterIterator.GetNextObject()) != 0)
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

    bool done=server->ExecuteVoid(wxT("DROP DATABASE ") + GetQuotedIdentifier() + wxT(";"));
    if (!done)
        Connect();

    return done;
}



wxString pgDatabase::GetSql(ctlTree *browser)
{
    if (sql.IsEmpty())
    {
        sql = wxT("-- Database: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP DATABASE ") + GetQuotedIdentifier() + wxT(";")
            + wxT("\n\nCREATE DATABASE ") + GetQuotedIdentifier()
            + wxT("\n  WITH OWNER = ") + qtIdent(GetOwner())
            + wxT("\n       ENCODING = ") + qtDbString(GetEncoding());
        if (!tablespace.IsEmpty())
            sql += wxT("\n       TABLESPACE = ") + qtIdent(GetTablespace());
        
        sql += wxT(";\n");

        size_t i;
        for (i=0 ; i < variables.GetCount() ; i++)
            sql += wxT("ALTER DATABASE ") + GetQuotedFullIdentifier()
                +  wxT(" SET ") + variables.Item(i) + wxT(";\n");

		// If we can't connect to this database, use the maintenance DB
		pgConn *myConn = GetConnection();
		if (!myConn)
			myConn = GetServer()->GetConnection();

		if (myConn)
		{
			if (!myConn->BackendMinimumVersion(8, 2))
				sql += GetGrant(wxT("CT"));
			else
				sql += GetGrant(wxT("CTc"));
		}

        sql += GetCommentSql();
    }
    return sql;
}



void pgDatabase::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (Connect() == PGCONN_OK)
    {
        // Set the icon if required
        if (UpdateIcon(browser))
        {
//            if (form)
//                form->->SetButtons(this);
        }

            // Add child nodes if necessary
        if (browser->GetChildrenCount(GetId(), false) == 0)
        {
            wxLogInfo(wxT("Adding child object to database ") + GetIdentifier());

            browser->AppendCollection(this, castFactory);
            browser->AppendCollection(this, languageFactory);
            browser->AppendCollection(this, schemaFactory);
            browser->AppendCollection(this, slClusterFactory);
            
            missingFKs = StrToLong(connection()->ExecuteScalar(
                wxT("SELECT COUNT(*) FROM\n")
                wxT("   (SELECT tgargs from pg_trigger tr\n")
                wxT("      LEFT JOIN pg_depend dep ON dep.objid=tr.oid AND deptype = 'i'\n")
                wxT("      LEFT JOIN pg_constraint co ON refobjid = co.oid AND contype = 'f'\n")
                wxT("     WHERE tgisconstraint AND co.oid IS NULL\n")
                wxT("     GROUP BY tgargs\n")
                wxT("    HAVING count(1) = 3) AS foo")));
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
        if (!tablespace.IsEmpty())
            properties->AppendItem(_("Tablespace"), GetTablespace());
        properties->AppendItem(_("Encoding"), GetEncoding());

        if (!defaultSchema.IsEmpty())
            properties->AppendItem(_("Default schema"), defaultSchema);

        size_t i;
        for (i=0 ; i < variables.GetCount() ; i++)
        {
            wxString item=variables.Item(i);
            properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
        }
        properties->AppendItem(_("Allow connections?"), GetAllowConnections());
        properties->AppendItem(_("Connected?"), GetConnected());
        properties->AppendItem(_("System database?"), GetSystemObject());
        if (GetMissingFKs())
            properties->AppendItem(_("Old style FKs"), GetMissingFKs());
        if (!GetSchemaRestriction().IsEmpty())
            properties->AppendItem(_("Schema restriction"), GetSchemaRestriction());
        properties->AppendItem(_("Comment"), GetComment());
    }
    if (form && GetCanHint() && !hintShown)
    {
        ShowHint(form, false);
    }
}



pgObject *pgDatabase::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgDatabase *database=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
    {
        database = (pgDatabase*)databaseFactory.CreateObjects(coll, 0, wxT(" WHERE db.oid=") + GetOidStr() + wxT("\n"));
        if (database)
        {
            sql=wxT("");
            iSetAcl(database->GetAcl());
            size_t i;
            for (i=0 ; i < database->GetVariables().GetCount() ; i++)
                variables.Add(database->GetVariables().Item(i));

            iSetComment(connection()->ExecuteScalar(wxT("SELECT description FROM pg_description WHERE objoid=") + GetOidStr()));
            delete database;
        }
    }

    UpdateDefaultSchema();

    return this;
}


pgObject *pgDatabaseFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgDatabase *database=0;

    pgSet *databases;

    wxString restr=restriction;
    if (!collection->GetServer()->GetDbRestriction().IsEmpty())
    {
        if (restr.IsEmpty())
            restr = wxT(" WHERE (");
        else
            restr = wxT("   AND (");

        restr += collection->GetServer()->GetDbRestriction() + wxT(")\n");
    }
    
    if (collection->GetConnection()->BackendMinimumVersion(7, 5))
        databases = collection->GetServer()->ExecuteSet(
           wxT("SELECT db.oid, datname, spcname, datallowconn, datconfig, datacl, ")
                  wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
                  wxT("has_database_privilege(db.oid, 'CREATE') as cancreate\n")
           wxT("  FROM pg_database db\n")
           wxT("  LEFT OUTER JOIN pg_tablespace ta ON db.dattablespace=ta.OID\n")
           + restr +
           wxT(" ORDER BY datname"));
    else
        databases = collection->GetServer()->ExecuteSet(
           wxT("SELECT db.oid, datname, datpath, datallowconn, datconfig, datacl, ")
                  wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
                  wxT("has_database_privilege(db.oid, 'CREATE') as cancreate\n")
           wxT("  FROM pg_database db\n")
           + restr +
           wxT(" ORDER BY datname"));
    
    if (databases)
    {
        while (!databases->Eof())
        {
            wxString name=databases->GetVal(wxT("datname"));
            database = new pgDatabase(name);
            database->iSetServer(collection->GetServer());
            database->iSetOid(databases->GetOid(wxT("oid")));
            database->iSetOwner(databases->GetVal(wxT("datowner")));
            database->iSetAcl(databases->GetVal(wxT("datacl")));
            database->iSetEncoding(databases->GetVal(wxT("serverencoding")));
            database->iSetCreatePrivilege(databases->GetBool(wxT("cancreate")));
            wxString str=databases->GetVal(wxT("datconfig"));
            if (!str.IsEmpty())
                FillArray(database->GetVariables(), str.Mid(1, str.Length()-2));
            database->iSetAllowConnections(databases->GetBool(wxT("datallowconn")));

            if (collection->GetConnection()->BackendMinimumVersion(7, 5))
                database->iSetTablespace(databases->GetVal(wxT("spcname")));
            else
                database->iSetPath(databases->GetVal(wxT("datpath")));

            if (collection->GetServer()->GetServerIndex())
            {
                wxString value;
                settings->Read(wxT("Servers/") + NumToStr(collection->GetServer()->GetServerIndex())
                    + wxT("/Databases/") + name + wxT("/SchemaRestriction"), &value, wxEmptyString);

                database->iSetSchemaRestriction(value);
            }

            // Add the treeview node if required
            if (settings->GetShowSystemObjects() ||!database->GetSystemObject()) 
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


pgDatabaseCollection::pgDatabaseCollection(pgaFactory *factory, pgServer *sv)
: pgServerObjCollection(factory, sv)
{
}


void pgDatabaseCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    wxLogInfo(wxT("Displaying statistics for databases on ") + GetServer()->GetIdentifier());

    bool hasSize=GetConnection()->HasFeature(FEATURE_SIZE);

    wxString sql=wxT("SELECT datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit");

    if (hasSize)
        sql += wxT(", pg_size_pretty(pg_database_size(datid)) as size");

    sql += wxT("\n  FROM pg_stat_database db ORDER BY datname");

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->AddColumn(_("Database"), 60);
    statistics->AddColumn(_("Backends"), 50);
    statistics->AddColumn(_("Xact Committed"), 60);
    statistics->AddColumn(_("Xact Rolled Back"), 60);
    statistics->AddColumn(_("Blocks Read"), 60);
    statistics->AddColumn(_("Blocks Hit"), 60);
    if (hasSize)
        statistics->AddColumn(_("Size"), 60);

    pgSet *stats = GetServer()->ExecuteSet(sql);
    if (stats)
    {
        while (!stats->Eof())
        {
            statistics->InsertItem(stats->CurrentPos() - 1, stats->GetVal(wxT("datname")), PGICON_STATISTICS);
            statistics->SetItem(stats->CurrentPos() - 1, 1, stats->GetVal(wxT("numbackends")));
            statistics->SetItem(stats->CurrentPos() - 1, 2, stats->GetVal(wxT("xact_commit")));
            statistics->SetItem(stats->CurrentPos() - 1, 3, stats->GetVal(wxT("xact_rollback")));
            statistics->SetItem(stats->CurrentPos() - 1, 4, stats->GetVal(wxT("blks_read")));
            statistics->SetItem(stats->CurrentPos() - 1, 5, stats->GetVal(wxT("blks_hit")));
            if (hasSize)
                statistics->SetItem(stats->CurrentPos() - 1, 6, stats->GetVal(wxT("size")));

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
    server= database->GetServer();
}


bool pgDatabaseObjCollection::CanCreate()
{
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
    return new pgDatabaseCollection(GetCollectionFactory(), (pgServer*)obj);
}

pgCollection *pgDatabaseObjFactory::CreateCollection(pgObject *obj)
{
    return new pgDatabaseObjCollection(GetCollectionFactory(), (pgDatabase*)obj);
}


pgDatabaseFactory databaseFactory;
static pgaCollectionFactory cf(&databaseFactory, __("Databases"), databases_xpm);
