//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#include "pgDatabase.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgCollection.h"
#include "menu.h"
#include "frmMain.h"


pgDatabase::pgDatabase(const wxString& newName)
: pgServerObject(PG_DATABASE, newName)
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


wxMenu *pgDatabase::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();

    if (GetCreatePrivilege())
    {
        AppendMenu(menu, PG_CAST);
        AppendMenu(menu, PG_LANGUAGE);
        AppendMenu(menu, PG_SCHEMA);

        extern wxString slony1BaseScript;
        extern wxString slony1FunctionScript;
        extern wxString slony1XxidScript;
        extern wxString backupExecutable;

        if ((!slony1BaseScript.IsEmpty() && !slony1FunctionScript.IsEmpty() && !slony1XxidScript.IsEmpty())
            || !backupExecutable.IsEmpty())
            AppendMenu(menu, SL_CLUSTER);
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
    bool rc;
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
        if (this->GetName() == wxT("template0")) return TRUE;
        return (this->GetOid() <= server->GetLastSystemOID());
    } else {
        return FALSE;
    }
}


void pgDatabase::AppendSchemaChange(const wxString &sql)
{
    wxDateTime dt;
    dt.Now();
    schemaChanges.Append(wxT("-- ") + DateToStr(dt) + wxT("\n"));
    schemaChanges.Append(sql);
    schemaChanges.Append(wxT("\n\n"));
}

bool pgDatabase::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    if (useServerConnection)
    {
        wxMessageDialog(frame, _("Initial database can't be dropped."),
                        _("Dropping database not allowed"), wxICON_EXCLAMATION | wxOK);

        return false;
    }
    Disconnect();

    bool done=server->ExecuteVoid(wxT("DROP DATABASE ") + GetQuotedIdentifier() + wxT(";"));
    if (!done)
        Connect();

    return done;
}



wxString pgDatabase::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsEmpty())
    {
        sql = wxT("-- Database: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP DATABASE ") + GetQuotedIdentifier() + wxT(";")
            + wxT("\n\nCREATE DATABASE ") + GetQuotedIdentifier()
            + wxT("\n  WITH OWNER = ") + qtIdent(GetOwner())
            + wxT("\n       ENCODING = ") + qtString(GetEncoding());
        if (!tablespace.IsEmpty())
            sql += wxT("\n       TABLESPACE = ") + qtIdent(GetTablespace());
        
        sql += wxT(";\n");

        size_t i;
        for (i=0 ; i < variables.GetCount() ; i++)
            sql += wxT("ALTER DATABASE ") + GetQuotedFullIdentifier()
                +  wxT(" SET ") + variables.Item(i) + wxT(";\n");
        sql += GetGrant(wxT("CT"))
            +  GetCommentSql();
    }
    return sql;
}



void pgDatabase::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (Connect() == PGCONN_OK)
    {
        // Set the icon if required
        if (browser->GetItemImage(GetId(), wxTreeItemIcon_Normal) != PGICON_DATABASE)
        {
            browser->SetItemImage(GetId(), PGICON_DATABASE, wxTreeItemIcon_Normal);
		    browser->SetItemImage(GetId(), PGICON_DATABASE, wxTreeItemIcon_Selected);
            if (form)
                form->SetButtons(this);
        }

            // Add child nodes if necessary
        if (browser->GetChildrenCount(GetId(), FALSE) == 0)
        {
            wxLogInfo(wxT("Adding child object to database ") + GetIdentifier());
            pgCollection *collection;

            // Casts
            collection = new pgCollection(PG_CASTS, this);
            AppendBrowserItem(browser, collection);

            // Languages
            collection = new pgCollection(PG_LANGUAGES, this);
            AppendBrowserItem(browser, collection);

            // Schemas
            collection = new pgCollection(PG_SCHEMAS, this);
            AppendBrowserItem(browser, collection);

            // Jobs
			// We only add the Jobs node if the appropriate objects are in this DB.
		    wxString exists = ExecuteScalar(
				wxT("SELECT cl.oid FROM pg_class cl JOIN pg_namespace ns ON ns.oid=relnamespace\n")
				wxT(" WHERE relname='pga_job' AND nspname='pgagent'"));

			if (!exists.IsNull())
			{
				collection = new pgCollection(PGA_JOBS, this);
	            AppendBrowserItem(browser, collection);
			}

            // Slony-I Clusters
            collection = new pgCollection(SL_CLUSTERS, this);
            AppendBrowserItem(browser, collection);
            
            missingFKs = StrToLong(connection()->ExecuteScalar(
                wxT("SELECT COUNT(*) FROM\n")
                wxT("   (SELECT tgargs from pg_trigger tr\n")
                wxT("      LEFT JOIN pg_depend dep ON dep.objid=tr.oid AND deptype = 'i'\n")
                wxT("      LEFT JOIN pg_constraint co ON refobjid = co.oid AND contype = 'f'\n")
                wxT("     WHERE co.oid IS NULL\n")
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
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgDatabase::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgDatabase *database=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_DATABASES)
        {
            database = (pgDatabase*)ReadObjects((pgCollection*)obj, 0, wxT(" WHERE db.oid=") + GetOidStr() + wxT("\n"));
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
    }

    UpdateDefaultSchema();

    return this;
}


pgObject *pgDatabase::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgDatabase *database=0;

    pgSet *databases;
    if (collection->GetConnection()->BackendMinimumVersion(7, 5))
        databases = collection->GetServer()->ExecuteSet(
           wxT("SELECT db.oid, datname, spcname, datallowconn, datconfig, datacl, ")
                  wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
                  wxT("has_database_privilege(db.oid, 'CREATE') as cancreate\n")
           wxT("  FROM pg_database db\n")
           wxT("  LEFT OUTER JOIN pg_tablespace ta ON db.dattablespace=ta.OID\n")
           + restriction +
           wxT(" ORDER BY datname"));
    else
        databases = collection->GetServer()->ExecuteSet(
           wxT("SELECT db.oid, datname, datpath, datallowconn, datconfig, datacl, ")
                  wxT("pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner,")
                  wxT("has_database_privilege(db.oid, 'CREATE') as cancreate\n")
           wxT("  FROM pg_database db\n")
           + restriction +
           wxT(" ORDER BY datname"));
    
    if (databases)
    {
        while (!databases->Eof())
        {
            database = new pgDatabase(databases->GetVal(wxT("datname")));
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

            // Add the treeview node if required
            if (settings->GetShowSystemObjects() ||!database->GetSystemObject()) 
            {
                if (browser)
                {
                    if (database->GetName() == database->GetServer()->GetDatabaseName())
                        browser->AppendItem(collection->GetId(), database->GetIdentifier(), PGICON_DATABASE, -1, database);   
                    else
                        browser->AppendItem(collection->GetId(), database->GetIdentifier(), PGICON_CLOSEDDATABASE, -1, database);   
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


void pgDatabase::ShowStatistics(pgCollection *collection, ctlListView *statistics)
{
    wxLogInfo(wxT("Displaying statistics for databases on ") + collection->GetServer()->GetIdentifier());

    bool hasSize=collection->GetConnection()->HasFeature(FEATURE_SIZE);

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

    pgSet *stats = collection->GetServer()->ExecuteSet(sql);
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

