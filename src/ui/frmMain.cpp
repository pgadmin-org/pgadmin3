//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.cpp - The main form
//
// Note: Due to the size of frmMain, event handler, browser and statistics 
//       functions are in events.cpp, browser.cpp and statistics.cpp.
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
