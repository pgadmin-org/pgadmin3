//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.cpp - The main form
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/tbarsmpl.h>
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

// Icons
#include "images/aggregate.xpm"
#include "images/arguments.xpm"
#include "images/baddatabase.xpm"
#include "images/check.xpm"
#include "images/closeddatabase.xpm"
#include "images/column.xpm"
#include "images/connect.xpm"
#include "images/create.xpm"
#include "images/database.xpm"
#include "images/domain.xpm"
#include "images/drop.xpm"
#include "images/function.xpm"
#include "images/group.xpm"
#include "images/hiproperty.xpm"
#include "images/index.xpm"
#include "images/indexcolumn.xpm"
#include "images/language.xpm"
#include "images/namespace.xpm"
#include "images/operator.xpm"
#include "images/pgAdmin3.xpm"
#include "images/properties.xpm"
#include "images/property.xpm"
#include "images/public.xpm"
#include "images/record.xpm"
#include "images/refresh.xpm"
#include "images/relationship.xpm"
#include "images/rule.xpm"
#include "images/sequence.xpm"
#include "images/server.xpm"
#include "images/serverbad.xpm"
#include "images/sql.xpm"
#include "images/statistics.xpm"
#include "images/stop.xpm"
#include "images/table.xpm"
#include "images/trigger.xpm"
#include "images/type.xpm"
#include "images/user.xpm"
#include "images/vacuum.xpm"
#include "images/view.xpm"
#include "images/viewdata.xpm"

// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
    EVT_MENU(BTN_ADDSERVER, frmMain::OnAddServer)
    EVT_MENU(BTN_DROP, frmMain::OnDrop)
    EVT_MENU(BTN_REFRESH, frmMain::OnRefresh)
    EVT_MENU(MNU_ABOUT, frmMain::OnAbout)
    EVT_MENU(MNU_ADDSERVER, frmMain::OnAddServer)
    EVT_MENU(MNU_EXIT, frmMain::OnExit)
    EVT_MENU(MNU_OPTIONS, frmMain::OnOptions)
    EVT_MENU(MNU_PASSWORD, frmMain::OnPassword)
    EVT_MENU(MNU_SAVEDEFINITION, frmMain::OnSaveDefinition)
    EVT_MENU(MNU_SYSTEMOBJECTS, frmMain::OnShowSystemObjects)
    EVT_MENU(MNU_TIPOFTHEDAY, frmMain::OnTipOfTheDay)
    EVT_MENU(MNU_UPGRADEWIZARD, frmMain::OnUpgradeWizard)
    EVT_TREE_SEL_CHANGED(CTL_BROWSER, frmMain::OnSelChanged)
    EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER, frmMain::OnSelActivated)
END_EVENT_TABLE()

frmMain::frmMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{

    extern sysSettings *settings;

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

    // Build menus
    menuBar = new wxMenuBar();

    // File Menu
    fileMenu = new wxMenu();
    fileMenu->Append(MNU_ADDSERVER, wxT("&Add Server..."), wxT("Add a connection to a server."));
    fileMenu->Append(MNU_PASSWORD, wxT("C&hange password..."), wxT("Change your password."));
    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_SAVEDEFINITION, wxT("&Save definition..."), wxT("Save the SQL definition of the selected object."));
    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXIT, wxT("E&xit"), wxT("Quit this program."));
    menuBar->Append(fileMenu, wxT("&File"));

    // Tools Menu
    toolsMenu = new wxMenu();
    toolsMenu->Append(MNU_UPGRADEWIZARD, wxT("&Upgrade Wizard..."), wxT("Run the upgrade wizard."));
    toolsMenu->AppendSeparator();
    toolsMenu->Append(MNU_OPTIONS, wxT("&Options..."), wxT("Show options dialog."));
    menuBar->Append(toolsMenu, wxT("&Tools"));

    // View Menu
    viewMenu = new wxMenu();
    viewMenu->Append(MNU_SYSTEMOBJECTS, wxT("&System objects"), wxT("Show or hide system objects."), wxITEM_CHECK);
    menuBar->Append(viewMenu, wxT("&View"));

    // Help Menu
    helpMenu = new wxMenu();
    helpMenu->Append(MNU_CONTENTS, wxT("&Help..."), wxT("Open the helpfile."));
    helpMenu->Append(MNU_TIPOFTHEDAY, wxT("&Tip of the day..."), wxT("Show a tip of the day."));
    helpMenu->AppendSeparator();
    helpMenu->Append(MNU_ABOUT, wxT("&About..."), wxT("Show about dialog."));
    menuBar->Append(helpMenu, wxT("&Help"));

    // Add the Menubar and set some options
    SetMenuBar(menuBar);
    fileMenu->Enable(MNU_PASSWORD, FALSE);
    viewMenu->Check(MNU_SYSTEMOBJECTS, settings->GetShowSystemObjects());

    // Status bar
    CreateStatusBar(3);
    int iWidths[3] = {0, -1, 100};
    SetStatusWidths(3, iWidths);
    SetStatusText(wxT(""), 0);
    SetStatusText(wxT("Ready."), 1);
    SetStatusText(wxT("0 Secs"), 2);

    // Toolbar bar

    CreateToolBar();

    // Return objects
    statusBar = GetStatusBar();
    toolBar = GetToolBar();

    // Set up toolbar
    wxBitmap barBitmaps[10];
    toolBar->SetToolBitmapSize(wxSize(32, 32));
    barBitmaps[0] = wxBitmap(connect_xpm);
    barBitmaps[1] = wxBitmap(refresh_xpm);
    barBitmaps[2] = wxBitmap(create_xpm);
    barBitmaps[3] = wxBitmap(drop_xpm);
    barBitmaps[4] = wxBitmap(properties_xpm);
    barBitmaps[5] = wxBitmap(sql_xpm);
    barBitmaps[6] = wxBitmap(viewdata_xpm);
    barBitmaps[7] = wxBitmap(vacuum_xpm);
    barBitmaps[8] = wxBitmap(record_xpm);
    barBitmaps[9] = wxBitmap(stop_xpm);

    toolBar->AddTool(BTN_ADDSERVER, wxT("Add Server"), barBitmaps[0], wxT("Add a connection to a server."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_REFRESH, wxT("Refresh"), barBitmaps[1], wxT("Refrsh the data below the selected object."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(BTN_CREATE, wxT("Create"), barBitmaps[2], wxT("Create a new object of the same type as the selected object."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_DROP, wxT("Drop"), barBitmaps[3], wxT("Drop the currently selected object."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_PROPERTIES, wxT("Properties"), barBitmaps[4], wxT("Display/edit the properties of the selected object."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(BTN_SQL, wxT("SQL"), barBitmaps[5], wxT("Execute arbitrary SQL queries."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_VIEWDATA, wxT("View Data"), barBitmaps[6], wxT("View the data in the selected object."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_VACUUM, wxT("Vacuum"), barBitmaps[7], wxT("Vacuum the current database or table."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(BTN_RECORD, wxT("Record"), barBitmaps[8], wxT("Record a query log."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_STOP, wxT("Stop"), barBitmaps[9], wxT("Stop recording the query log."), wxITEM_NORMAL);

    // Display the bar and configure buttons. 
    toolBar->Realize();
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
    toolBar->EnableTool(BTN_STOP, FALSE);
    
    // Setup the vertical splitter & treeview
    wxSplitterWindow* vertical = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    wxSplitterWindow* horizontal = new wxSplitterWindow(vertical, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    browser = new wxTreeCtrl(vertical, CTL_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
    vertical->SplitVertically(browser, horizontal, 200);
    vertical->SetMinimumPaneSize(50);

    // Setup the horizontal splitter for the listview & sql pane
    wxNotebook* listViews = new wxNotebook(horizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    properties = new wxListCtrl(listViews, CTL_PROPVIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    statistics = new wxListCtrl(listViews, CTL_STATVIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    listViews->AddPage(properties, wxT("Properties"));
    listViews->AddPage(statistics, wxT("Statistics"));
    sqlPane = new ctlSQLBox(horizontal, CTL_SQLPANE, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
    sqlPane->SetBackgroundColour(*wxLIGHT_GREY);
    sqlPane->SetReadOnly(TRUE);
    horizontal->SplitHorizontally(listViews, sqlPane, 300);
    horizontal->SetMinimumPaneSize(50);

    //Setup a Browser imagelist
    wxImageList *browserImages = new wxImageList(16, 16);
    browser->SetImageList(browserImages);
	
    //Stuff the Image List
    browserImages->Add(wxIcon(server_xpm));
    browserImages->Add(wxIcon(serverbad_xpm));
    browserImages->Add(wxIcon(database_xpm));
    browserImages->Add(wxIcon(language_xpm));
    browserImages->Add(wxIcon(namespace_xpm));
    browserImages->Add(wxIcon(aggregate_xpm));
    browserImages->Add(wxIcon(function_xpm));
    browserImages->Add(wxIcon(operator_xpm));
    browserImages->Add(wxIcon(sequence_xpm));
    browserImages->Add(wxIcon(table_xpm));
    browserImages->Add(wxIcon(type_xpm));
    browserImages->Add(wxIcon(view_xpm));
    browserImages->Add(wxIcon(user_xpm));
    browserImages->Add(wxIcon(group_xpm));
    browserImages->Add(wxIcon(baddatabase_xpm));
    browserImages->Add(wxIcon(closeddatabase_xpm));


    // Add the root node
    pgObject *serversObj = new pgObject(PG_SERVERS, wxString("Servers"));
    servers = browser->AddRoot(wxT("Servers"), 0, -1, serversObj);
    pgObject *addServerObj = new pgObject(PG_ADD_SERVER, wxString("Add Server"));
    browser->AppendItem(servers, wxT("Add Server..."), 0, -1, addServerObj);
    browser->Expand(servers);

    // Setup the property imagelist
    wxImageList *propertiesImages = new wxImageList(16, 16);
    properties->SetImageList(propertiesImages, wxIMAGE_LIST_SMALL);
    propertiesImages->Add(wxIcon(property_xpm));

    // Add the property view columns
    properties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    properties->InsertItem(0, wxT("No properties are available for the current selection"), 0);

    // Setup a statistics view imagelist
    wxImageList *statisticsImages = new wxImageList(16, 16);
    statistics->SetImageList(statisticsImages, wxIMAGE_LIST_SMALL);
    statisticsImages->Add(wxIcon(statistics_xpm));

    // Add the statistics view columns & set the colour
    statistics->InsertColumn(0, wxT("Statistics"), wxLIST_FORMAT_LEFT, 500);
    statistics->InsertItem(0, wxT("No statistics are available for the current selection"), 0);
    wxColour background;
    background = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    statistics->SetBackgroundColour(background);
	
    // Load servers
    RetrieveServers();
}

frmMain::~frmMain()
{
    extern sysSettings *settings;
    settings->SetFrmMainWidth(GetSize().x);
    settings->SetFrmMainHeight(GetSize().y);
    settings->SetFrmMainLeft(GetPosition().x);
    settings->SetFrmMainTop(GetPosition().y);

    // Clear the treeview
    browser->DeleteAllItems();
}

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

void frmMain::ReconnectServer(pgServer *server)
{
    // Create a server object and connect it.
    int res = server->Connect(TRUE);
    // Check the result, and handle it as appropriate
    if (res == PGCONN_OK) {
        wxLogInfo(wxT("pgServer object initialised as required."));
        browser->SetItemImage(server->GetId(), 0, wxTreeItemIcon_Normal);
        browser->SetItemImage(server->GetId(), 0, wxTreeItemIcon_Selected);
		browser->Collapse(servers);
        browser->Expand(servers);
		browser->SelectItem(servers);
		browser->SelectItem(server->GetId());
		
    } else if (res == PGCONN_DNSERR)  {
        delete server;
        OnAddServer();

    } else if (res == PGCONN_BAD)  {
        wxString msg;
        msg.Printf(wxT("%s"), server->GetLastError().c_str());
        wxLogError(wxT(msg));
        ReconnectServer(server);
		
    } else {
        wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
    }
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

    // Reset the toolbar & password menu option
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
    fileMenu->Enable(MNU_PASSWORD, FALSE);

    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    // If we didn't get an object, then we may have a right click, or 
    // invalid click, so ignore.
    if (!data) return;

    int type = data->GetType();

    switch (type) {
        case PG_SERVER:
            StartMsg(wxT("Retrieving server properties"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);
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
}

void frmMain::StoreServers()
{
    wxLogInfo(wxT("Storing listed servers for later..."));

    // Store the currently listed servers for later retrieval.
    extern sysSettings *settings;

    // Write the individual servers
    // Iterate through all the child nodes of the Servers node
    long cookie;
    wxString key;
    pgObject *data;
    pgServer *server;
    int numServers = 0;

    wxTreeItemId item = browser->GetFirstChild(servers, cookie);
    while (item) {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetType() == PG_SERVER) {
            // We have a sever, so cast the object and save the settings
            ++numServers;
            server = (pgServer *)data;

            // Hostname
            key.Printf("Servers/Server%d", numServers);
            settings->Write(key, server->GetName());

            // Port
            key.Printf("Servers/Port%d", numServers);
            settings->Write(key, server->GetPort());

            // Database
            key.Printf("Servers/Database%d", numServers);
            settings->Write(key, server->GetDatabase());

            // Username
            key.Printf("Servers/Username%d", numServers);
            settings->Write(key, server->GetUsername());
        }

        // Get the next item
        item = browser->GetNextChild(servers, cookie);
    }

    // Write the server count
    settings->Write(wxT("Servers/Count"), numServers);
    wxString msg;
    msg.Printf("Stored %d servers.", numServers);
    wxLogInfo(msg);

}

void frmMain::RetrieveServers()
{
    // Retrieve previously stored servers
    wxLogInfo(wxT("Reloading servers..."));

    extern sysSettings *settings;

    int numServers;
    settings->Read(wxT("Servers/Count"), &numServers, 0);

    int loop, port;
    wxString key, servername, database, username;
    pgServer *server;

    for (loop = 1; loop <= numServers; ++loop) {
        
        // Server
        key.Printf("Servers/Server%d", loop);
        settings->Read(key, &servername, wxT(""));

        // Port
        key.Printf("Servers/Port%d", loop);
        settings->Read(key, &port, 0);

        // Database
        key.Printf("Servers/Database%d", loop);
        settings->Read(key, &database, wxT(""));

        // Username
        key.Printf("Servers/Username%d", loop);
        settings->Read(key, &username, wxT(""));

        // Add the Server node
        server = new pgServer(servername, database, username, port);
        browser->AppendItem(servers, server->GetIdentifier(), 1, -1, server);
    }

    // Reset the Servers node text
    wxString label;
    label.Printf(wxT("Servers (%d)"), browser->GetChildrenCount(servers, FALSE) - 1);
    browser->SetItemText(servers, label);
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
}

void frmMain::SetButtons(bool refresh, bool create, bool drop, bool properties, bool sql, bool viewData, bool vacuum)
{
    toolBar->EnableTool(BTN_REFRESH, refresh);
    toolBar->EnableTool(BTN_CREATE, create);
    toolBar->EnableTool(BTN_DROP, drop);
    toolBar->EnableTool(BTN_PROPERTIES, properties);
    toolBar->EnableTool(BTN_SQL, sql);
    toolBar->EnableTool(BTN_VIEWDATA, viewData);
    toolBar->EnableTool(BTN_VACUUM, vacuum);
}

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

void frmMain::svServer(pgServer *server)
{
    if(!server->GetConnected()) return;
    
    wxString msg;
    msg.Printf(wxT("Displaying statistics for server %s"), server->GetIdentifier().c_str());
    wxLogInfo(msg);

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(1, wxT("PID"), wxLIST_FORMAT_LEFT, 50);
    statistics->InsertColumn(2, wxT("User"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(3, wxT("Current Query"), wxLIST_FORMAT_LEFT, 400);

    pgSet stats = server->ExecuteSet(wxT("SELECT datname, procpid, usename, current_query FROM pg_stat_activity"));

    while (!stats.Eof()) {
        statistics->InsertItem(stats.CurrentPos() - 1, stats.GetVal(wxT("datname")), 0);
        statistics->SetItem(stats.CurrentPos() - 1, 1, stats.GetVal(wxT("procpid")));
        statistics->SetItem(stats.CurrentPos() - 1, 2, stats.GetVal(wxT("usename")));
        statistics->SetItem(stats.CurrentPos() - 1, 3, stats.GetVal(wxT("current_query")));
        stats.MoveNext();
    }


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
        pgSet databases = collection->GetServer()->ExecuteSet(wxT("SELECT oid, datname, datpath, datallowconn, datconfig, datacl, pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner FROM pg_database"));

        while (!databases.Eof()) {

            database = new pgDatabase(databases.GetVal(wxT("datname")));
            database->SetServer(collection->GetServer());
            database->iSetOid(StrToDouble(databases.GetVal(wxT("oid"))));
            database->iSetOwner(databases.GetVal(wxT("datowner")));
            database->iSetAcl(databases.GetVal(wxT("datacl")));
            database->iSetPath(databases.GetVal(wxT("datpath")));
            database->iSetEncoding(databases.GetVal(wxT("serverencoding")));
            database->iSetVariables(databases.GetVal(wxT("datconfig")));
            database->iSetAllowConnections(StrToBool(databases.GetVal(wxT("datallowconn"))));

            // Add the treeview node if required
            if (settings->GetShowSystemObjects())
                browser->AppendItem(collection->GetId(), database->GetIdentifier(), 15, -1, database);
            else {
                if (!database->GetSystemObject())
                    browser->AppendItem(collection->GetId(), database->GetIdentifier(), 15, -1, database);
            }


            databases.MoveNext();
        }

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

void frmMain::svDatabases(pgCollection *collection)
{
    
    wxString msg;
    msg.Printf(wxT("Displaying statistics for databases on %s"), collection->GetServer()->GetIdentifier().c_str());
    wxLogInfo(msg);

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(1, wxT("Backends"), wxLIST_FORMAT_LEFT, 75);
    statistics->InsertColumn(2, wxT("Xact Committed"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(3, wxT("Xact Rolled Back"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(4, wxT("Blocks Read"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(5, wxT("Blocks Hit"), wxLIST_FORMAT_LEFT, 100);

    pgSet stats = collection->GetServer()->ExecuteSet(wxT("SELECT datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit FROM pg_stat_database ORDER BY datname"));

    while (!stats.Eof()) {
        statistics->InsertItem(stats.CurrentPos() - 1, stats.GetVal(wxT("datname")), 0);
        statistics->SetItem(stats.CurrentPos() - 1, 1, stats.GetVal(wxT("numbackends")));
        statistics->SetItem(stats.CurrentPos() - 1, 2, stats.GetVal(wxT("xact_commit")));
        statistics->SetItem(stats.CurrentPos() - 1, 3, stats.GetVal(wxT("xact_rollback")));
        statistics->SetItem(stats.CurrentPos() - 1, 4, stats.GetVal(wxT("blks_read")));
        statistics->SetItem(stats.CurrentPos() - 1, 5, stats.GetVal(wxT("blks_hit")));
        stats.MoveNext();
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
			wxTreeItemId databases = browser->GetParent(database->GetId());
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
