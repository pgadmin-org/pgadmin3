//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// events.cpp - Event handlers for frmMain
//
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/tipdlg.h>
#include <wx/stc/stc.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "frmAbout.h"
#include "frmConnect.h"
#include "frmOptions.h"
#include "frmPassword.h"
#include "frmUpgradeWizard.h"
#include "ctlSQLBox.h"
#include "pgConn.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"

// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
    EVT_MENU(BTN_ADDSERVER, frmMain::OnAddServer)
    EVT_MENU(BTN_DROP, frmMain::OnDrop)
    EVT_MENU(BTN_REFRESH, frmMain::OnRefresh)
    EVT_MENU(BTN_PROPERTIES, frmMain::OnProperties)
    EVT_MENU(MNU_ABOUT, frmMain::OnAbout)
    EVT_MENU(MNU_ADDSERVER, frmMain::OnAddServer)
	EVT_MENU(MNU_REFRESH, frmMain::OnRefresh)
	EVT_MENU(MNU_CONNECT, frmMain::OnSelActivated)
	EVT_MENU(MNU_DISCONNECT, frmMain::OnDisconnect)
	EVT_MENU(MNU_DROP, frmMain::OnDrop)
	EVT_MENU(MNU_PROPERTIES, frmMain::OnProperties)
    EVT_MENU(MNU_EXIT, frmMain::OnExit)
    EVT_MENU(MNU_OPTIONS, frmMain::OnOptions)
    EVT_MENU(MNU_PASSWORD, frmMain::OnPassword)
    EVT_MENU(MNU_SAVEDEFINITION, frmMain::OnSaveDefinition)
    EVT_MENU(MNU_SYSTEMOBJECTS, frmMain::OnShowSystemObjects)
    EVT_MENU(MNU_TIPOFTHEDAY, frmMain::OnTipOfTheDay)
    EVT_MENU(MNU_UPGRADEWIZARD, frmMain::OnUpgradeWizard)
    EVT_TREE_SEL_CHANGED(CTL_BROWSER, frmMain::OnSelChanged)
    EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER, frmMain::OnSelActivated)
	EVT_TREE_ITEM_RIGHT_CLICK(CTL_BROWSER, frmMain::OnSelRightClick) 
END_EVENT_TABLE()

// Event handlers
void frmMain::OnExit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void frmMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    frmAbout *winAbout = new frmAbout(this);
    winAbout->Show(TRUE);
}

void frmMain::OnUpgradeWizard(wxCommandEvent& WXUNUSED(event))
{
    frmUpgradeWizard *winUpgradeWizard = new frmUpgradeWizard(this);
    winUpgradeWizard->Show(TRUE);
}

void frmMain::OnTipOfTheDay()
{
    extern sysSettings *settings;
    wxTipProvider *tipProvider = wxCreateFileTipProvider(wxT("tips.txt"), settings->GetNextTipOfTheDay());
    settings->SetShowTipOfTheDay(wxShowTip(this, tipProvider));
    settings->SetNextTipOfTheDay(tipProvider->GetCurrentTip());
    delete tipProvider;
}

void frmMain::OnOptions(wxCommandEvent& event)
{
    frmOptions *winOptions = new frmOptions(this);
    winOptions->Show(TRUE);
}

void frmMain::OnPassword(wxCommandEvent& event)
{
    frmPassword *winPassword = new frmPassword(this);

    // We need to pass the server to the password form
    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);
    int type = data->GetType();

    switch (type) {
        case PG_SERVER:
            winPassword->SetServer((pgServer *)data);
            winPassword->Show(TRUE);
            break;

        default:
            // Should never see this
            wxLogError(wxT("You must select a server before changing your password!"));
            break;
    }
}

void frmMain::OnSaveDefinition(wxCommandEvent& event)
{

    wxLogInfo(wxT("Saving object definition"));

    if (sqlPane->GetText() == wxT("")) {
        wxLogError(wxT("There is nothing in the SQL pane to save!"));
        return;
    }

    wxFileDialog filename(this, wxT("Select output file"), wxT(""), wxT(""), wxT("SQL Scripts (*.sql)|*.sql|All files (*.*)|*.*"));
    filename.SetStyle(wxSAVE | wxOVERWRITE_PROMPT);

    // Show the dialogue
    if (filename.ShowModal() == wxID_OK) {

        // Write the file
        wxFile *file = new wxFile(filename.GetPath(), wxFile::write);
        if (!file->Write(sqlPane->GetText())) {
            wxString msg;
            msg.Printf(wxT("Failed to write to the output file: %s"), filename.GetPath().c_str());
            wxLogError(msg);
        }
        file->Close();
        return;

    } else {
        wxLogInfo(wxT("User cancelled"));
        return;
    }

}

void frmMain::OnShowSystemObjects(wxCommandEvent& event)
{
    extern sysSettings *settings;

    // Warn the user
    if (wxMessageBox(wxT("Changing the 'Show System Objects' option will cause all connections to be closed, and the treeview to be rebuilt.\n\nAre you sure you wish to continue?"),
                     wxT("Continue?"), wxYES_NO | wxICON_QUESTION) == wxNO) {
        viewMenu->Check(MNU_SYSTEMOBJECTS, settings->GetShowSystemObjects());
        return;
    }

    if (settings->GetShowSystemObjects()) {
        settings->SetShowSystemObjects(FALSE);
        viewMenu->Check(MNU_SYSTEMOBJECTS, FALSE);
    } else {
        settings->SetShowSystemObjects(TRUE);
        viewMenu->Check(MNU_SYSTEMOBJECTS, TRUE);
    }

    wxLogInfo(wxT("Clearing treeview to toggle ShowSystemObjects"));

    // Clear the treeview
    browser->DeleteAllItems();

    // Add the root node
    pgObject *serversObj = new pgObject(PG_SERVERS, wxString("Servers"));
    servers = browser->AddRoot(wxT("Servers"), 0, -1, serversObj);
    pgObject *addServerObj = new pgObject(PG_ADD_SERVER, wxString("Add Server"));
    browser->AppendItem(servers, wxT("Add Server..."), 0, -1, addServerObj);
    browser->Expand(servers);
    browser->SelectItem(servers);

    RetrieveServers();
}

void frmMain::OnAddServer()
{
    extern sysSettings *settings;

    // Create a server object and connec it.
    pgServer *server = new pgServer(settings->GetLastServer(), settings->GetLastDatabase(), settings->GetLastUsername(), settings->GetLastPort());
    int res = server->Connect();

    // Check the result, and handle it as appropriate
    if (res == PGCONN_OK) {
        wxLogInfo(wxT("pgServer object initialised as required."));
        browser->AppendItem(servers, server->GetIdentifier(), 0, -1, server);
        browser->Expand(servers);

    } else if (res == PGCONN_DNSERR)  {
        delete server;
        OnAddServer();

    } else if (res == PGCONN_BAD)  {
        wxString msg;
        msg.Printf(wxT("Error connecting to the server: %s"), server->GetLastError().c_str());
        wxLogError(wxT(msg));
        delete server;
        OnAddServer();

    } else {
        wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
        delete server;
    }

    // Reset the Servers node text
    wxString label;
    label.Printf(wxT("Servers (%d)"), browser->GetChildrenCount(servers, FALSE) - 1);
    browser->SetItemText(servers, label);
    StoreServers();
}

void frmMain::OnSelChanged()
{
	// Reset the listviews/SQL pane
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    properties->InsertItem(0, wxT("No properties are available for the current selection"), 0);
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Statistics"), wxLIST_FORMAT_LEFT, 500);
    statistics->InsertItem(0, wxT("No statistics are available for the current selection"), 0);
    sqlPane->SetReadOnly(FALSE);
    sqlPane->SetText(wxT(""));
    sqlPane->SetReadOnly(TRUE);

    // Reset the toolbar & password menu options
	// Handle the menus associated with the buttons
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);

    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    // If we didn't get an object, then we may have a right click, or 
    // invalid click, so ignore.
    if (!data) return;

    int type = data->GetType();
	pgServer *server;

    switch (type) {
        case PG_SERVER:
            StartMsg(wxT("Retrieving server properties"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);

			// Handle the the connect/disconnect menus
            server = (pgServer *)data;
            if (!server->GetConnected()) {
				toolsMenu->Enable(MNU_CONNECT, TRUE);
				treeContextMenu->Enable(MNU_CONNECT, TRUE);               
            }
			else {
				toolsMenu->Enable(MNU_DISCONNECT, TRUE);
				treeContextMenu->Enable(MNU_DISCONNECT, TRUE);               
			}

            tvServer((pgServer *)data);
            svServer((pgServer *)data);
            EndMsg();
            break;

        case PG_DATABASES:
            StartMsg(wxT("Retrieving database details"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);
            tvDatabases((pgCollection *)data);
            svDatabases((pgCollection *)data);
            EndMsg();
            break;

        case PG_DATABASE:
            StartMsg(wxT("Retrieving database details"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);
            tvDatabase((pgDatabase *)data);          
			//svDatabases((pgCollection *)data);
            EndMsg();
            break;

        default:        
			break;
    }
}

void frmMain::OnSelActivated()
{
    // This handler will primarily deal with displaying item
    // properties in seperate windows and 'Add xxx...' clicks

    // Get the item data, and feed it to the relevant handler,
    // cast as required.

    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);
    int type = data->GetType();
    pgServer *server;

    switch (type) {
        case PG_ADD_SERVER:
            OnAddServer();
            break;

        case PG_SERVER:
            server = (pgServer *)data;
            if (!server->GetConnected()) {
                ReconnectServer(server);
            }
            break;

        default:
            break;
    }

#ifndef __WXMSW__
    browser->Expand(item);
#endif
}

void frmMain::OnSelRightClick(wxTreeEvent& event)
{
    // This handler will display a popup menu for the item

	// Get mouse point data
	wxPoint point = event.GetPoint();
	wxPoint origin = GetClientAreaOrigin();

	// Because this Tree is inside a vertical splitter, we
	// must compensate for the size of the other elements
	point.x += origin.x;
	point.y += origin.y;

	// popup the menu
	PopupMenu(treeContextMenu, point);
}

void frmMain::OnDrop()
{
    // This handler will primarily deal with dropping items

    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);
    int type = data->GetType();
    wxString msg, label;

    switch (type) {
        case PG_SERVER:
            msg.Printf(wxT("Are you sure you wish to remove the server: %s?"), data->GetIdentifier().c_str());
			if (wxMessageBox(msg, wxT("Remove Server?"), wxYES_NO | wxICON_QUESTION) == wxYES) {

                msg.Printf(wxT("Removing server: %s"), data->GetIdentifier().c_str());
                wxLogInfo(msg);
                browser->Delete(item);

                // Reset the Servers node text
                label.Printf(wxT("Servers (%d)"), browser->GetChildrenCount(servers, FALSE) - 1);
                browser->SetItemText(servers, label);
                StoreServers();
            }
            break;

        default:
            break;
    }
}

void frmMain::OnRefresh()
{
    // Refresh - Clear the treeview below the current selection

    long cookie;
    wxTreeItemId item1 = browser->GetSelection();
    wxTreeItemId item2 = browser->GetFirstChild(item1, cookie);
    while (item2) {
        browser->Delete(item2);
        item2 = browser->GetFirstChild(item1, cookie);
    }

		// refresh information about the object
	OnSelChanged();
}

void frmMain::OnDisconnect()
{
    // Disconnect -- does nothing yet
	int res = wxMessageBox("This is not yet implemented" );
}

void frmMain::OnProperties()
{
    // Properties -- does nothing yet
	int res = wxMessageBox("This is not yet implemented" );
}

