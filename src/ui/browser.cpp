//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// browser.cpp - Browser/property functions for the main form
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/stc/stc.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "ctlSQLBox.h"
#include "pgConn.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"

void frmMain::tvServer(pgServer *server)
{
    // This handler will primarily deal with displaying item
    // properties in the main window.

    wxString msg;

    // Add child nodes if necessary
    if (server->GetConnected()) {

        // Reset password menu option
        fileMenu->Enable(MNU_PASSWORD, TRUE);

        if (browser->GetChildrenCount(server->GetId(), FALSE) != 3) {

            // Log
            msg.Printf(wxT("Adding child object to server %s"), server->GetIdentifier().c_str());
            wxLogInfo(msg);
    
            // Databases
            pgCollection *collection = new pgCollection(PG_DATABASES, wxString("Databases"));
            collection->SetServer(server);
            browser->AppendItem(server->GetId(), collection->GetTypeName(), 2, -1, collection);
      
            // Groups
            collection = new pgCollection(PG_GROUPS, wxString("Groups"));
            collection->SetServer(server);
            browser->AppendItem(server->GetId(), collection->GetTypeName(), 13, -1, collection);
    
            // Users
            collection = new pgCollection(PG_USERS, wxString("Users"));
            collection->SetServer(server);
            browser->AppendItem(server->GetId(), collection->GetTypeName(), 12, -1, collection);
        }
    }


    msg.Printf(wxT("Displaying properties for server %s"), server->GetIdentifier().c_str());
    wxLogInfo(msg);

    // Add the properties view columns
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 400);

    // Display the Server properties
    properties->InsertItem(0, wxT("Hostname"), 0);
    properties->SetItem(0, 1, server->GetName());

    properties->InsertItem(1, wxT("Port"), 0);
    properties->SetItem(1, 1, NumToStr((double)server->GetPort()));

    properties->InsertItem(2, wxT("Initial Database"), 0);
    properties->SetItem(2, 1, server->GetDatabase());

    properties->InsertItem(3, wxT("Username"), 0);
    properties->SetItem(3, 1, server->GetUsername());

    properties->InsertItem(4, wxT("Version String"), 0);
    properties->SetItem(4, 1, server->GetVersionString());

    properties->InsertItem(5, wxT("Version Number"), 0);
    properties->SetItem(5, 1, NumToStr(server->GetVersionNumber()));

    properties->InsertItem(6, wxT("Last System OID"), 0);
    properties->SetItem(6, 1, NumToStr(server->GetLastSystemOID()));

    properties->InsertItem(7, wxT("Connected?"), 0);
    properties->SetItem(7, 1, BoolToYesNo(server->GetConnected()));
}

void frmMain::tvDatabases(pgCollection *collection)
{
    extern sysSettings *settings;
    wxString msg;
    pgDatabase *database;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0) {

        // Log
        msg.Printf(wxT("Adding databases to server %s"), collection->GetServer()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Add Database node
        pgObject *addDatabaseObj = new pgObject(PG_ADD_DATABASE, wxString("Add Database"));
        browser->AppendItem(collection->GetId(), wxT("Add Database..."), 2, -1, addDatabaseObj);

        // Get the databases
        pgSet *databases = collection->GetServer()->ExecuteSet(wxT("SELECT oid, datname, datpath, datallowconn, datconfig, datacl, pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner FROM pg_database"));

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
                browser->AppendItem(collection->GetId(), database->GetIdentifier(), 15, -1, database);
            else if (!database->GetSystemObject()) 
				browser->AppendItem(collection->GetId(), database->GetIdentifier(), 15, -1, database);

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

        // Reset the Databases node text
        wxString label;
        label.Printf(wxT("Databases (%d)"), browser->GetChildrenCount(collection->GetId(), FALSE) - 1);
        browser->SetItemText(collection->GetId(), label);

    }

    // Display the properties.

    long cookie;
    int count = 0;
    wxString key;
    pgObject *data;

    // Setup listview
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    properties->InsertColumn(1, wxT("Comment"), wxLIST_FORMAT_LEFT, 400);

    wxTreeItemId item = browser->GetFirstChild(collection->GetId(), cookie);
    while (item) {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetType() == PG_DATABASE) {

            database = (pgDatabase *)data;

            properties->InsertItem(0, database->GetName(), 0);
            properties->SetItem(0, 1, database->GetComment());
        }

        // Get the next item
        item = browser->GetNextChild(servers, cookie);
    }
}

void frmMain::tvDatabase(pgDatabase *database)
{
    wxString msg;
    if (database->Connect() == PGCONN_OK) {

        // Set the icon if required
        if (browser->GetItemImage(database->GetId(), wxTreeItemIcon_Normal) != 2) {
            browser->SetItemImage(database->GetId(), 2, wxTreeItemIcon_Normal);
			browser->SetItemImage(database->GetId(), 2, wxTreeItemIcon_Selected);

			// Keith 2002.03.05 
			// NOTE IF YOU AREN'T USING wx2.4 IGNORE THIS CHANGE
			// wx 2.4 changed GetParent to GetItemParent 
			//wxTreeItemId databases = browser->GetParent(database->GetId());
			wxTreeItemId databases = browser->GetItemParent(database->GetId());
			
			browser->Collapse(databases);
			browser->Expand(databases);
			browser->SelectItem(database->GetId());
        }

        // Add child nodes if necessary
        if (browser->GetChildrenCount(database->GetId(), FALSE) != 2) {

            // Log
            msg.Printf(wxT("Adding child object to database %s"), database->GetIdentifier().c_str());
            wxLogInfo(msg);

            // Languages
            pgCollection *collection = new pgCollection(PG_LANGUAGES, wxString("Languages"));
            collection->SetServer(database->GetServer());
            browser->AppendItem(database->GetId(), collection->GetTypeName(), 3, -1, collection);
  
            // Schemas
            collection = new pgCollection(PG_SCHEMAS, wxString("Schemas"));
            collection->SetServer(database->GetServer());
            browser->AppendItem(database->GetId(), collection->GetTypeName(), 4, -1, collection);
        }
    }

    // Setup listview
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 350);

    properties->InsertItem(0, wxT("Name"), 0);
    properties->SetItem(0, 1, database->GetName());
    properties->InsertItem(1, wxT("OID"), 0);
    properties->SetItem(1, 1, NumToStr(database->GetOid()));
    properties->InsertItem(2, wxT("Owner"), 0);
    properties->SetItem(2, 1, database->GetOwner());
    properties->InsertItem(3, wxT("ACL"), 0);
    properties->SetItem(3, 1, database->GetAcl());
    properties->InsertItem(4, wxT("Path"), 0);
    properties->SetItem(4, 1, database->GetPath());
    properties->InsertItem(5, wxT("Encoding"), 0);
    properties->SetItem(5, 1, database->GetEncoding());
    properties->InsertItem(6, wxT("Variables"), 0);
    properties->SetItem(6, 1, database->GetVariables());
    properties->InsertItem(7, wxT("Allow Connections?"), 0);
    properties->SetItem(7, 1, BoolToYesNo(database->GetAllowConnections()));
    properties->InsertItem(8, wxT("Connected?"), 0);
    properties->SetItem(8, 1, BoolToYesNo(database->GetConnected()));
    properties->InsertItem(9, wxT("System Database?"), 0);
    properties->SetItem(9, 1, BoolToYesNo(database->GetSystemObject()));
    properties->InsertItem(10, wxT("Comment?"), 0);
    properties->SetItem(10, 1, database->GetComment());

    // Set the SQL Pane text
    sqlPane->SetReadOnly(FALSE);
    sqlPane->SetText(database->GetSql());
    sqlPane->SetReadOnly(TRUE);
}
