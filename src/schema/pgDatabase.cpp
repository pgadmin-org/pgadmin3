//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#include "pgDatabase.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgCollection.h"
#include "pgaAgent.h"


pgDatabase::pgDatabase(const wxString& newName)
: pgServerObject(PG_DATABASE, newName)
{
    wxLogInfo(wxT("Creating a pgDatabase object"));

    allowConnections = TRUE;
    connected = FALSE;
	conn = NULL;
}


pgDatabase::~pgDatabase()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));
	if (conn)
		delete conn;
}


wxMenu *pgDatabase::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();

    if (GetCreatePrivilege())
    {
        AppendMenu(menu, PG_CAST);
        AppendMenu(menu, PG_LANGUAGE);
        AppendMenu(menu, PG_SCHEMA);
    }
    return menu;
}


int pgDatabase::Connect()
{
    if (!allowConnections)
        return PGCONN_REFUSED;

    if (connected)
        return conn->GetStatus();
    else
    {
		conn = new pgConn(GetServer()->GetName(), GetName(), GetServer()->GetUsername(), GetServer()->GetPassword(), GetServer()->GetPort(), GetServer()->GetSSL());
		if (conn->GetStatus() == PGCONN_OK)
        {
            // Now we're connected.
            connected = TRUE;
            iSetComment(conn->ExecuteScalar(wxT("SELECT description FROM pg_description WHERE objoid=") + GetOidStr()));

            // check for extended ruleutils with pretty-print option
            wxString exprname=conn->ExecuteScalar(wxT("SELECT proname FROM pg_proc WHERE proname='pg_get_viewdef' AND proargtypes[1]=16"));
            if (!exprname.IsEmpty())
                prettyOption = wxT(", true");
        
            searchPath = conn->ExecuteScalar(wxT("SHOW search_path"));

            return PGCONN_OK;
        }
        else
        {
			wxLogError(wxT("%s"), conn->GetLastError().c_str());
            return PGCONN_BAD;
        }
    }
}


wxString pgDatabase::GetSchemaPrefix(const wxString &name) const
{
    if (name.IsEmpty())
        return name;

    wxString sp=settings->GetSearchPath();
    if (sp.IsEmpty())
        sp = GetSearchPath();
    sp += wxT(",pg_catalog");

    wxStringTokenizer spt(sp, wxT(","));
    while (spt.HasMoreTokens())
    {
        wxString tk=spt.GetNextToken().Strip(wxString::both);
        if (tk == wxT("$user"))
            continue;

        if (tk == name)
            return wxEmptyString;
    }
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

bool pgDatabase::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    if (conn)
    {
        delete conn;
        conn=0;
        connected = FALSE;
    }
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
            + wxT("\n  WITH ENCODING = ") + qtString(GetEncoding()) + wxT(";\n");
        wxStringTokenizer vars(GetVariables());
        while (vars.HasMoreTokens())
            sql += wxT("ALTER DATABASE ") + GetQuotedFullIdentifier()
                +  wxT(" SET ") + vars.GetNextToken() + wxT(";\n");
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
        if (browser->GetItemImage(GetId(), wxTreeItemIcon_Normal) != 2)
        {
            browser->SetItemImage(GetId(), PGICON_DATABASE, wxTreeItemIcon_Normal);
		    browser->SetItemImage(GetId(), PGICON_DATABASE, wxTreeItemIcon_Selected);
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

            // pgAgent
            pgaAgent::ReadObjects(this, browser);

            missingFKs = StrToLong(conn->ExecuteScalar(
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
        properties->AppendItem(_("Encoding"), GetEncoding());
        wxStringTokenizer vars(GetVariables());
        while (vars.HasMoreTokens())
        {
            wxString str=vars.GetNextToken();
            properties->AppendItem(str.BeforeFirst('='), str.AfterFirst('='));
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
                iSetVariables(database->GetVariables());
                iSetComment(conn->ExecuteScalar(wxT("SELECT description FROM pg_description WHERE objoid=") + GetOidStr()));
                delete database;
            }
        }
    }
    return this;
}


pgObject *pgDatabase::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgDatabase *database=0;

    pgSet *databases = collection->GetServer()->ExecuteSet(
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
            database->iSetPath(databases->GetVal(wxT("datpath")));
            database->iSetEncoding(databases->GetVal(wxT("serverencoding")));
            database->iSetCreatePrivilege(databases->GetBool(wxT("cancreate")));
            wxString str=databases->GetVal(wxT("datconfig"));
            if (!str.IsEmpty())
                database->iSetVariables(str.Mid(1, str.Length()-2));
            database->iSetAllowConnections(databases->GetBool(wxT("datallowconn")));

            // Add the treeview node if required
            if (settings->GetShowSystemObjects() ||!database->GetSystemObject()) 
            {
                if (browser)
                    browser->AppendItem(collection->GetId(), database->GetIdentifier(), PGICON_CLOSEDDATABASE, -1, database);   
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

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->AddColumn(_("Database"), 60);
    statistics->AddColumn(_("Backends"), 50);
    statistics->AddColumn(_("Xact Committed"), 60);
    statistics->AddColumn(_("Xact Rolled Back"), 60);
    statistics->AddColumn(_("Blocks Read"), 60);
    statistics->AddColumn(_("Blocks Hit"), 60);

    pgSet *stats = collection->GetServer()->ExecuteSet(wxT("SELECT datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit FROM pg_stat_database ORDER BY datname"));
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
            stats->MoveNext();
        }

	    delete stats;
    }
}

