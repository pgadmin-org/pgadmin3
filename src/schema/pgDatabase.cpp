//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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



pgDatabase::pgDatabase(const wxString& newName)
: pgObject(PG_DATABASE, newName)
{
    wxLogInfo(wxT("Creating a pgDatabase object"));

    allowConnections = TRUE;
    connected = FALSE;

	// Keith 2003.03.05
	// Must set to null to see if we can delete it later
	database = NULL;
}

pgDatabase::~pgDatabase()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));

	// Keith 2003.03.05
	// Fixing memory leak 
	if (database)
		delete database;
}

int pgDatabase::Connect() {
    if (!allowConnections) {
        return PGCONN_REFUSED;
    }
    if (connected) {
        return database->GetStatus();
    } else {

		database = new pgConn(this->GetServer()->GetName(), this->GetName(), this->GetServer()->GetUsername(), this->GetServer()->GetPassword(), this->GetServer()->GetPort());       
		if (database->GetStatus() == PGCONN_OK) {

            // As we connected, we should now get the comments
            wxString sql, rawcomment;
            sql.Printf(wxT("SELECT description FROM pg_description WHERE objoid = %s"), NumToStr(this->GetOid()).c_str());
            rawcomment = database->ExecuteScalar(sql);
            if (rawcomment != "(null)") {
                this->iSetComment(rawcomment);
            }

            // Now we're connected.
            connected = TRUE;
            return PGCONN_OK;

        } else {

            wxString msg;
			msg.Printf(wxT("%s"), database->GetLastError().c_str());
			wxLogError(msg);
            return PGCONN_BAD;
        }
    }
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

wxString pgDatabase::GetSql(wxTreeCtrl *browser)
{
    wxString sql;
    sql.Printf(wxT("CREATE DATABASE %s WITH ENCODING = %s;"),
                 this->GetQuotedIdentifier().c_str(), qtString(this->GetEncoding()).c_str());

    if (!this->GetComment().IsEmpty())
        sql.Printf(wxT("%s\nCOMMENT ON DATABASE %s IS %s;"),
                     sql.c_str(), this->GetQuotedIdentifier().c_str(), qtString(this->GetComment()).c_str());

    return sql;
}



void pgDatabase::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
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
        if (browser->GetChildrenCount(GetId(), FALSE) != 2)
        {
            wxLogInfo(wxT("Adding child object to database ") + GetIdentifier());

            // Languages
            pgCollection *collection = new pgCollection(PG_LANGUAGES, wxString("Languages"));
            collection->SetServer(GetServer());
            collection->SetDatabase(this);
            browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_LANGUAGE, -1, collection);

            // Schemas
            collection = new pgCollection(PG_SCHEMAS, wxString("Schemas"));
            collection->SetServer(GetServer());
            collection->SetDatabase(this);
            browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_SCHEMA, -1, collection);
        }
    }

    GetServer()->SetLastDatabase(GetName());

    if (properties)
    {
        // Setup listview
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 350);

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
        InsertListItem(properties, pos++, wxT("Path"), GetPath());
        InsertListItem(properties, pos++, wxT("Encoding"), GetEncoding());
        InsertListItem(properties, pos++, wxT("Variables"), GetVariables());
        InsertListItem(properties, pos++, wxT("Allow Connections?"), BoolToYesNo(GetAllowConnections()));
        InsertListItem(properties, pos++, wxT("Connected?"), BoolToYesNo(GetConnected()));
        InsertListItem(properties, pos++, wxT("System Database?"), BoolToYesNo(GetSystemObject()));
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



void pgDatabase::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    extern sysSettings *settings;
    pgDatabase *database;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0) {

        // Log
        wxLogInfo(wxT("Adding databases to server ") + collection->GetServer()->GetIdentifier());

        // Get the databases
        pgSet *databases = collection->GetServer()->ExecuteSet(wxT(
           "SELECT oid, datname, datpath, datallowconn, datconfig, datacl, pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner\n"
           "  FROM pg_database\n"
           " ORDER BY datname"));

        while (!databases->Eof()) {

            database = new pgDatabase(databases->GetVal(wxT("datname")));
            database->SetServer(collection->GetServer());
            database->iSetOid(StrToDouble(databases->GetVal(wxT("oid"))));
            database->iSetOwner(databases->GetVal(wxT("datowner")));
            database->iSetAcl(databases->GetVal(wxT("datacl")));
            database->iSetPath(databases->GetVal(wxT("datpath")));
            database->iSetEncoding(databases->GetVal(wxT("serverencoding")));
            database->iSetVariables(databases->GetVal(wxT("datconfig")));
            database->iSetAllowConnections(StrToBool(databases->GetVal(wxT("datallowconn"))));

            // Add the treeview node if required
            if (settings->GetShowSystemObjects())
                browser->AppendItem(collection->GetId(), database->GetIdentifier(), PGICON_CLOSEDDATABASE, -1, database);
            else if (!database->GetSystemObject()) 
				browser->AppendItem(collection->GetId(), database->GetIdentifier(), PGICON_CLOSEDDATABASE, -1, database);

			// Keith 2003.03.05
			// We never gave the database to the treeview control, so we
			// have to manually delete it's object to fix a memory leak
            else 
				delete database;
	
			databases->MoveNext();
        }

		// Keith 2003.03.05
		// Fixing memory leak
		delete databases;
    }

    if (statistics)
    {
        wxLogInfo(wxT("Displaying statistics for databases on ") + collection->GetServer()->GetIdentifier());

        // Add the statistics view columns
        statistics->ClearAll();
        statistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
        statistics->InsertColumn(1, wxT("Backends"), wxLIST_FORMAT_LEFT, 75);
        statistics->InsertColumn(2, wxT("Xact Committed"), wxLIST_FORMAT_LEFT, 100);
        statistics->InsertColumn(3, wxT("Xact Rolled Back"), wxLIST_FORMAT_LEFT, 100);
        statistics->InsertColumn(4, wxT("Blocks Read"), wxLIST_FORMAT_LEFT, 100);
        statistics->InsertColumn(5, wxT("Blocks Hit"), wxLIST_FORMAT_LEFT, 100);

        pgSet *stats = collection->GetServer()->ExecuteSet(wxT("SELECT datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit FROM pg_stat_database ORDER BY datname"));
        if (stats)
        {
            while (!stats->Eof()) {
                statistics->InsertItem(stats->CurrentPos() - 1, stats->GetVal(wxT("datname")), 0);
                statistics->SetItem(stats->CurrentPos() - 1, 1, stats->GetVal(wxT("numbackends")));
                statistics->SetItem(stats->CurrentPos() - 1, 2, stats->GetVal(wxT("xact_commit")));
                statistics->SetItem(stats->CurrentPos() - 1, 3, stats->GetVal(wxT("xact_rollback")));
                statistics->SetItem(stats->CurrentPos() - 1, 4, stats->GetVal(wxT("blks_read")));
                statistics->SetItem(stats->CurrentPos() - 1, 5, stats->GetVal(wxT("blks_hit")));
                stats->MoveNext();
            }

	        // Keith 2003.03.05
	        // Fixed memory leak
	        delete stats;
        }
    }
}

