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
#include "frmQuery.h"
#include "ctlSQLBox.h"
#include "pgConn.h"
#include "pgLanguage.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"



// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
    EVT_MENU(BTN_ADDSERVER,                 OnAddServer)
    EVT_MENU(BTN_DROP,                      OnDrop)
    EVT_MENU(BTN_REFRESH,                   OnRefresh)
    EVT_MENU(BTN_PROPERTIES,                OnProperties)
    EVT_MENU(BTN_SQL,                       OnSql)
    EVT_MENU(MNU_ABOUT,                     OnAbout)
    EVT_MENU(MNU_ADDSERVER,                 OnAddServer)
	EVT_MENU(MNU_REFRESH,                   OnRefresh)
	EVT_MENU(MNU_CONNECT,                   OnConnect)
	EVT_MENU(MNU_DISCONNECT,                OnDisconnect)
	EVT_MENU(MNU_DROP,                      OnDrop)
	EVT_MENU(MNU_PROPERTIES,                OnProperties)
    EVT_MENU(MNU_EXIT,                      OnExit)
    EVT_MENU(MNU_OPTIONS,                   OnOptions)
    EVT_MENU(MNU_PASSWORD,                  OnPassword)
    EVT_MENU(MNU_SAVEDEFINITION,            OnSaveDefinition)
    EVT_MENU(MNU_SYSTEMOBJECTS,             OnShowSystemObjects)
    EVT_MENU(MNU_TIPOFTHEDAY,               OnTipOfTheDay)
    EVT_MENU(MNU_UPGRADEWIZARD,             OnUpgradeWizard)
    EVT_MENU(MNU_QUERYBUILDER,              OnQueryBuilder)
    EVT_LIST_ITEM_SELECTED(CTL_PROPVIEW,    OnPropSelChanged)
    EVT_TREE_SEL_CHANGED(CTL_BROWSER,       OnTreeSelChanged)
    EVT_TREE_ITEM_COLLAPSING(CTL_BROWSER,   OnCollapse)
    EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER,    OnSelActivated)
	EVT_TREE_ITEM_RIGHT_CLICK(CTL_BROWSER,  OnSelRightClick) 
    EVT_CLOSE(                              OnClose)
END_EVENT_TABLE()



// Event handlers
void frmMain::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(FALSE);   // Allow sub windows to stop us
}

void frmMain::OnClose(wxCloseEvent& event)
{
    wxFrame *fr;
    frameList::Node *node;
    while ((node=frames.GetFirst()) != NULL)
    {
        fr=node->GetData();
        if (!fr->Close(!event.CanVeto()))
        {
            if (event.CanVeto())
            {
                event.Veto();
                return;
            }
        }
        delete node;
        fr->Destroy();
    }
    Destroy();
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

void frmMain::OnCollapse(wxTreeEvent &event)
{
#ifdef WIN32
    // This is weird stuff, but somewhere comes a collapse after we have done
    // connecting the server and expanding the tree.
    // Possibly not necessary 
    if (event.GetItem() == denyCollapseItem)
        event.Veto();
#endif
    denyCollapseItem=wxTreeItemId();
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



void frmMain::OnSql(wxCommandEvent &ev)
{
    wxTreeItemId item=browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();

    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());
    if (conn->GetStatus() == PGCONN_OK)
    {
        wxString txt = "pgAdmin III Query - " + server->GetName() + ":" + NumToStr((long)server->GetPort()) + " - " + db->GetName();

        wxPoint pos(settings->Read(wxT("frmQuery/Left"), 100), settings->Read(wxT("frmQuery/Top"), 100));
        wxSize size(settings->Read(wxT("frmQuery/Width"), 600), settings->Read(wxT("frmQuery/Height"), 500));
        CheckOnScreen(pos, size, 200, 150);

        frmQuery *fq= new frmQuery(this, txt, conn, pos, size, sqlPane->GetText());
        frames.Append(fq);
        fq->Go();
    }
    else
    {
		wxLogError(conn->GetLastError());
        delete conn;
    }
}


void frmMain::OnSaveDefinition(wxCommandEvent& event)
{

    wxLogInfo(wxT("Saving object definition"));

    if (sqlPane->GetText().IsNull()) {
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
    pgObject *serversObj = new pgServers();
    servers = browser->AddRoot(wxT("Servers"), PGICON_SERVER, -1, serversObj);
    browser->Expand(servers);
    browser->SelectItem(servers);

    RetrieveServers();
}

void frmMain::OnAddServer(wxCommandEvent &ev)
{
    // Create a server object and connec it.
    pgServer *server = new pgServer(settings->GetLastServer(), settings->GetLastDatabase(), settings->GetLastUsername(), settings->GetLastPort());
    int res = server->Connect(this);

    // Check the result, and handle it as appropriate
    if (res == PGCONN_OK) {
        wxLogInfo(wxT("pgServer object initialised as required."));
        browser->AppendItem(servers, server->GetFullName(), PGICON_SERVERBAD, -1, server);
        browser->Expand(servers);

    } else if (res == PGCONN_DNSERR)  {
        delete server;
        OnAddServer(wxCommandEvent());

    } else if (res == PGCONN_BAD)  {
        wxString msg;
        msg.Printf(wxT("Error connecting to the server: %s"), server->GetLastError().c_str());
        wxLogError(wxT(msg));
        delete server;
        OnAddServer(wxCommandEvent());

    } else {
        wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
        delete server;
    }

    // Reset the Servers node text
    wxString label;
    label.Printf(wxT("Servers (%d)"), browser->GetChildrenCount(servers, FALSE));
    browser->SetItemText(servers, label);
    StoreServers();
}


void frmMain::OnPropSelChanged(wxListEvent& event)
{
    wxTreeItemId item=browser->GetSelection();
    pgObject *data=(pgObject*)browser->GetItemData(item);
    if (data)
        data->SetSql(browser, sqlPane, event.GetIndex());
}


void frmMain::OnTreeSelChanged(wxTreeEvent& event)
{
    denyCollapseItem=wxTreeItemId();
	// Reset the listviews/SQL pane
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    properties->InsertItem(0, wxT("No properties are available for the current selection"), 0);
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Statistics"), wxLIST_FORMAT_LEFT, 500);
    statistics->InsertItem(0, wxT("No statistics are available for the current selection"), 0);
    sqlPane->Clear();

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

    switch (type)
    {
        case PG_SERVER:
            StartMsg(wxT("Retrieving server properties"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);

            server = (pgServer *)data;
            if (!server->GetConnected())
            {
                toolsMenu->Enable(MNU_CONNECT, TRUE);
                treeContextMenu->Enable(MNU_CONNECT, TRUE);               
            }
			else
            {
                toolsMenu->Enable(MNU_DISCONNECT, TRUE);
                treeContextMenu->Enable(MNU_DISCONNECT, TRUE);               
            }
            data->ShowTree(this, browser, properties, statistics, sqlPane);
            EndMsg();
            break;

        case PG_DATABASES:
        case PG_GROUPS:
        case PG_USERS:
            data->ShowTree(this, browser, properties, statistics, sqlPane);
            SetButtons(true, true, false, false, false, false, false);
            break;

        case PG_DATABASE:
            data->ShowTree(this, browser, properties, statistics, sqlPane);
            SetButtons(true, true, true, true, true, false, true);
            break;

        case PG_GROUP:
        case PG_USER:
            data->ShowTree(this, browser, properties, statistics, sqlPane);
            SetButtons(true, true, true, true, false, false, false);
            break;
        case PG_LANGUAGES:
        case PG_LANGUAGE:
        case PG_SCHEMAS:
        case PG_SCHEMA:
        case PG_AGGREGATES:
        case PG_AGGREGATE:
        case PG_DOMAINS:
        case PG_DOMAIN:
        case PG_FUNCTIONS:
        case PG_FUNCTION:
        case PG_TRIGGERFUNCTIONS:
        case PG_TRIGGERFUNCTION:
        case PG_OPERATORS:
        case PG_OPERATOR:
        case PG_SEQUENCES:
        case PG_SEQUENCE:
        case PG_TABLES:
        case PG_TABLE:
        case PG_TYPES:
        case PG_TYPE:
        case PG_VIEWS:
        case PG_VIEW:
        case PG_CHECKS:
        case PG_CHECK:
        case PG_COLUMNS:
        case PG_COLUMN:
        case PG_FOREIGNKEYS:
        case PG_FOREIGNKEY:
        case PG_INDEXES:
        case PG_INDEX:
        case PG_RULES:
        case PG_RULE:
        case PG_TRIGGERS:
        case PG_TRIGGER:
            data->ShowTree(this, browser, properties, statistics, sqlPane);
            break;
        default:        
			break;
    }
    sqlPane->SetText(data->GetSql(browser));
}

void frmMain::OnConnect(wxCommandEvent &ev)
{
    wxTreeItemId item = browser->GetSelection();

    pgServer *server = (pgServer *)browser->GetItemData(item);
    if (server->GetType() == PG_SERVER && !server->GetConnected())
        ReconnectServer(server);
}


void frmMain::OnSelActivated(wxTreeEvent &event)
{
    // This handler will primarily deal with displaying item
    // properties in seperate windows and 'Add xxx...' clicks

    // Get the item data, and feed it to the relevant handler,
    // cast as required.

    wxTreeItemId item = event.GetItem();
    pgObject *data = (pgObject *)browser->GetItemData(item);
    if (!data)
        return;
    int type = data->GetType();
    pgServer *server;

    switch (type) {
        case PG_ADD_SERVER:
            OnAddServer(wxCommandEvent());
            break;

        case PG_SERVER:
            server = (pgServer *)data;
            if (!server->GetConnected())
            {
                ReconnectServer(server);
                // prevent from being collapsed immediately
                denyCollapseItem=item;
            }
            break;

        default:
            break;
    }

#ifndef __WXMSW__
    browser->Expand(item);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// This handler will display a popup menu for the item
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnSelRightClick(wxTreeEvent& event)
{   
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

void frmMain::OnDrop(wxCommandEvent &ev)
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
                label.Printf(wxT("Servers (%d)"), browser->GetChildrenCount(servers, FALSE));
                browser->SetItemText(servers, label);
                StoreServers();
            }
            break;

        default:
            break;
    }
}

void frmMain::OnRefresh(wxCommandEvent &ev)
{
    // Refresh - Clear the treeview below the current selection

    long cookie;
    wxTreeItemId item1 = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item1);
    data->SetDirty();

    wxTreeItemId item2 = browser->GetFirstChild(item1, cookie);
    while (item2) {
        browser->Delete(item2);
        item2 = browser->GetFirstChild(item1, cookie);
    }

		// refresh information about the object
    wxTreeEvent event;
	OnTreeSelChanged(event);
}

void frmMain::OnDisconnect(wxCommandEvent &ev)
{
    // Disconnect -- does nothing yet
	int res = wxMessageBox("This is not yet implemented" );
}


void frmMain::OnProperties(wxCommandEvent &ev)
{
    // Properties -- does nothing yet
	int res = wxMessageBox("This is not yet implemented" );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnQueryBuilder(wxCommandEvent &ev)
{

    wxTreeItemId item=browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();
    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());
    if (conn->GetStatus() == PGCONN_OK)
    {
	    // Create the Query Builder Form
	    qbform = new frmQueryBuilder(this, db); 

	    // Show the Query Builder
	    qbform->Show(TRUE);
    }
    else
    {
		wxLogError(conn->GetLastError());
        delete conn;
    }
}
