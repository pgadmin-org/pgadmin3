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
#include "frmQueryBuilder.h"


#include <wx/listimpl.cpp>
WX_DEFINE_LIST(frameList);

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
#include "images/key.xpm"
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

	// Current database
	m_database = NULL;
    denyCollapseItem=wxTreeItemId();

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
    fileMenu->Append(MNU_DROP, wxT("&Delete/Drop"), 
		wxT("Delete/Drop the selected object."));
    fileMenu->Append(MNU_PROPERTIES, wxT("&Properties"), 
		wxT("Display/edit the properties of the selected object."));
    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXIT, wxT("E&xit"), wxT("Quit this program."));
    menuBar->Append(fileMenu, wxT("&File"));

    // Tools Menu
    toolsMenu = new wxMenu();
    toolsMenu->Append(MNU_CONNECT, wxT("&Connect..."), 
		wxT("Connect to the selected server."));
    toolsMenu->Append(MNU_DISCONNECT, wxT("&Disconnect"), 
		wxT("Disconnect from the selected server."));
    toolsMenu->AppendSeparator();
	toolsMenu->Append(MNU_QUERYBUILDER, wxT("&Query Builder"),
		wxT("Start the query builder."));
    toolsMenu->Append(MNU_UPGRADEWIZARD, wxT("&Upgrade Wizard..."), wxT("Run the upgrade wizard."));
    toolsMenu->AppendSeparator();
    toolsMenu->Append(MNU_OPTIONS, wxT("&Options..."), wxT("Show options dialog."));
    menuBar->Append(toolsMenu, wxT("&Tools"));

    // View Menu
    viewMenu = new wxMenu();
    viewMenu->Append(MNU_SYSTEMOBJECTS, wxT("&System objects"), wxT("Show or hide system objects."), wxITEM_CHECK);
    viewMenu->AppendSeparator();
    viewMenu->Append(MNU_REFRESH, wxT("&Refresh"), 
		wxT("Refresh the selected object."));
    menuBar->Append(viewMenu, wxT("&View"));

    // Help Menu
    helpMenu = new wxMenu();
    helpMenu->Append(MNU_CONTENTS, wxT("&Help..."), wxT("Open the helpfile."));
    helpMenu->Append(MNU_TIPOFTHEDAY, wxT("&Tip of the day..."), wxT("Show a tip of the day."));
    helpMenu->AppendSeparator();
    helpMenu->Append(MNU_ABOUT, wxT("&About..."), wxT("Show about dialog."));
    menuBar->Append(helpMenu, wxT("&Help"));

    // Tree Context Menu
    treeContextMenu = new wxMenu();
    treeContextMenu->Append(MNU_CONNECT, wxT("&Connect..."), 
		wxT("Connect to the selected server."));
    treeContextMenu->Append(MNU_DISCONNECT, wxT("&Disconnect"), 
		wxT("Disconnect from the selected server."));
    treeContextMenu->AppendSeparator();
	treeContextMenu->Append(MNU_QUERYBUILDER, wxT("&Query Builder"),
		wxT("Start the query builder."));
    treeContextMenu->AppendSeparator();
    treeContextMenu->Append(MNU_DROP, wxT("&Delete/Drop"), 
		wxT("Delete/Drop the selected object."));
    treeContextMenu->Append(MNU_REFRESH, wxT("&Refresh"), 
		wxT("Refresh the selected object."));
    treeContextMenu->Append(MNU_PROPERTIES, wxT("&Properties"), 
		wxT("Display/edit the properties of the selected object."));

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


    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_NORMAL, WXK_F5, MNU_REFRESH);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);


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
    toolBar->AddTool(BTN_REFRESH, wxT("Refresh"), barBitmaps[1], wxT("Refresh the data below the selected object."), wxITEM_NORMAL);
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
    vertical = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    horizontal = new wxSplitterWindow(vertical, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    browser = new wxTreeCtrl(vertical, CTL_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
    int splitpos=settings->Read(wxT("frmMain/SplitVertical"), 200);
    if (splitpos < 50)
        splitpos = 50;
    if (splitpos > GetSize().x-50)
        splitpos = GetSize().x-50;
    vertical->SplitVertically(browser, horizontal, splitpos);
    vertical->SetMinimumPaneSize(50);

    // Setup the horizontal splitter for the listview & sql pane
    wxNotebook* listViews = new wxNotebook(horizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    properties = new wxListCtrl(listViews, CTL_PROPVIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    statistics = new wxListCtrl(listViews, CTL_STATVIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    listViews->AddPage(properties, wxT("Properties"));
    listViews->AddPage(statistics, wxT("Statistics"));
    sqlPane = new ctlSQLBox(horizontal, CTL_SQLPANE, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
    sqlPane->SetBackgroundColour(*wxLIGHT_GREY);

    splitpos=settings->Read(wxT("frmMain/SplitHorizontal"), 300);
    if (splitpos < 50)
        splitpos = 50;
    if (splitpos > GetSize().y-50)
        splitpos = GetSize().y-50;
    horizontal->SplitHorizontally(listViews, sqlPane, splitpos);
    horizontal->SetMinimumPaneSize(50);

    //Setup a Browser imagelist
	// Keith 2003.03.05
	// Fixed memory leak
	browserImages = new wxImageList(16, 16);
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
    browserImages->Add(wxIcon(domain_xpm));
    browserImages->Add(wxIcon(check_xpm));
    browserImages->Add(wxIcon(column_xpm));
    browserImages->Add(wxIcon(relationship_xpm));
    browserImages->Add(wxIcon(index_xpm));
    browserImages->Add(wxIcon(rule_xpm));
    browserImages->Add(wxIcon(trigger_xpm));
    browserImages->Add(wxIcon(key_xpm));
    browserImages->Add(wxIcon(public_xpm));
    browserImages->Add(wxIcon(public_xpm));
    browserImages->Add(wxIcon(public_xpm));

    // Add the root node
    pgObject *serversObj = new pgServers();
    servers = browser->AddRoot(wxT("Servers"), PGICON_SERVER, -1, serversObj);

    // Setup the property imagelist
	// Keith 2003.03.05
	// Fixed memory leak
    propertiesImages = new wxImageList(16, 16);
    properties->SetImageList(propertiesImages, wxIMAGE_LIST_SMALL);
    propertiesImages->Add(wxIcon(property_xpm));

    // Add the property view columns
    properties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    properties->InsertItem(0, wxT("No properties are available for the current selection"), 0);

    // Setup a statistics view imagelist
	// Keith 2003.03.05
	// Fixed memory leak
    statisticsImages = new wxImageList(16, 16);
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
    browser->Expand(servers);
}

frmMain::~frmMain()
{
    StoreServers();

    settings->Write(wxT("frmMain/Width"), GetSize().x);
    settings->Write(wxT("frmMain/Height"), GetSize().y);
    settings->Write(wxT("frmMain/Left"), GetPosition().x);
    settings->Write(wxT("frmMain/Top"), GetPosition().y);
    settings->Write(wxT("frmMain/SplitHorizontal"), horizontal->GetSashPosition());
    settings->Write(wxT("frmMain/SplitVertical"), vertical->GetSashPosition());

    // Clear the treeview
    browser->DeleteAllItems();

	// Keith 2003.03.05
	// Fixed memory leak -- These are not destroyed automatically 
	delete treeContextMenu;
	delete browserImages;
	delete statisticsImages;
	delete propertiesImages;
	delete statistics;
}



void frmMain::RemoveFrame(wxFrame *frame)
{
    frames.DeleteObject(frame);
}

wxTreeItemId frmMain::RestoreEnvironment(pgServer *server)
{
    wxTreeItemId item, lastItem;
    wxString lastDatabase=server->GetLastDatabase();
    if (lastDatabase.IsNull())
        return item;

   long cookie;
    pgObject *data;
    item = browser->GetFirstChild(server->GetId(), cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetType() == PG_DATABASES)
            break;
        // Get the next item
        item = browser->GetNextChild(server->GetId(), cookie);
    }
    if (!item)
        return item;

    // found DATABASES item
    data->ShowTree(this, browser, 0, 0, 0);
    lastItem=item;

    item = browser->GetFirstChild(lastItem, cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetType() == PG_DATABASE && data->GetName() == lastDatabase)
            break;
        // Get the next item
        item = browser->GetNextChild(lastItem, cookie);
    }
    if (!item)
        return lastItem;

    // found last DATABASE 
    data->ShowTree(this, browser, 0, 0, 0);
    lastItem = item;

    wxString lastSchema=server->GetLastSchema();
    if (lastSchema.IsNull())
        return lastItem;

    item = browser->GetFirstChild(lastItem, cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetType() == PG_SCHEMAS)
            break;
        // Get the next item
        item = browser->GetNextChild(lastItem, cookie);
    }
    if (!item)
        return lastItem;

    // found SCHEMAS item
    data->ShowTree(this, browser, 0, 0, 0);
    lastItem=item;

    item = browser->GetFirstChild(lastItem, cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetType() == PG_SCHEMA && data->GetName() == lastSchema)
            break;
        // Get the next item
        item = browser->GetNextChild(lastItem, cookie);
    }

    return (item ? item : lastItem);
}

int frmMain::ReconnectServer(pgServer *server)
{
    // Create a server object and connect it.
    int res = server->Connect(this, TRUE);

    // Check the result, and handle it as appropriate
    wxTreeItemId item;
    switch (res)
    {
        case PGCONN_OK:
            wxLogInfo(wxT("pgServer object initialised as required."));
            browser->SetItemImage(server->GetId(), PGICON_SERVER, wxTreeItemIcon_Normal);
            browser->SetItemImage(server->GetId(), PGICON_SERVER, wxTreeItemIcon_Selected);

// is this needed for *nix?
//            browser->Collapse(servers);
//            browser->Expand(servers);
        
//            browser->SelectItem(servers);
//            browser->SelectItem(server->GetId());
            server->ShowTreeDetail(browser);
            item=RestoreEnvironment(server);
            if (item)
            {
                browser->SelectItem(item);

                wxYield();
                browser->Expand(item);
                browser->EnsureVisible(item);
            }
            break;
        case PGCONN_DNSERR:
            /*
            // looks strange to me. Shouldn_t server be removed from the tree as well?
            delete server;
            OnAddServer(wxCommandEvent());
            break;
            */
        case PGCONN_BAD:
            wxLogError(server->GetLastError());
            ReconnectServer(server);
            break;
        default:
            wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
            break;
    }
    return res;
}

void frmMain::StoreServers()
{
    wxLogInfo(wxT("Storing listed servers for later..."));

    // Store the currently listed servers for later retrieval.

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

            key.Printf("Servers/Description%d", numServers);
            settings->Write(key, server->GetDescription());

            // Port
            key.Printf("Servers/Port%d", numServers);
            settings->Write(key, server->GetPort());

            // Trusted
            key.Printf("Servers/Trusted%d", numServers);
            settings->Write(key, BoolToYesNo(server->GetTrusted()));

            // Database
            key.Printf("Servers/Database%d", numServers);
            settings->Write(key, server->GetDatabase());

            // Username
            key.Printf("Servers/Username%d", numServers);
            settings->Write(key, server->GetUsername());

            // last Database
            key.Printf("Servers/LastDatabase%d", numServers);
            settings->Write(key, server->GetLastDatabase());

            // last Schema
            key.Printf("Servers/LastSchema%d", numServers);
            settings->Write(key, server->GetLastSchema());
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

    int numServers;
    settings->Read(wxT("Servers/Count"), &numServers, 0);

    int loop, port;
    wxString key, servername, description, database, username, lastDatabase, lastSchema, trusted;
    pgServer *server;

    for (loop = 1; loop <= numServers; ++loop) {
        
        // Server
        key.Printf("Servers/Server%d", loop);
        settings->Read(key, &servername, wxT(""));

        // Comment
        key.Printf("Servers/Description%d", loop);
        settings->Read(key, &description, wxT(""));

        // Trusted
        key.Printf("Servers/Trusted%d", loop);
        settings->Read(key, &trusted, wxT(""));

        // Port
        key.Printf("Servers/Port%d", loop);
        settings->Read(key, &port, 0);

        // Database
        key.Printf("Servers/Database%d", loop);
        settings->Read(key, &database, wxT(""));

        // Username
        key.Printf("Servers/Username%d", loop);
        settings->Read(key, &username, wxT(""));

        // last Database
        key.Printf("Servers/LastDatabase%d", loop);
        settings->Read(key, &lastDatabase, wxT(""));

        // last Schema
        key.Printf("Servers/LastSchema%d", loop);
        settings->Read(key, &lastSchema, wxT(""));

        // Add the Server node
        server = new pgServer(servername, database, username, port, StrToBool(trusted));
        server->iSetDescription(description);
        server->iSetLastDatabase(lastDatabase);
        server->iSetLastSchema(lastSchema);
        browser->AppendItem(servers, server->GetFullName(), PGICON_SERVERBAD, -1, server);
    }

    // Reset the Servers node text
    wxString label;
    label.Printf(wxT("Servers (%d)"), browser->GetChildrenCount(servers, FALSE));
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

	// Handle the menus associated with the buttons
	fileMenu->Enable(MNU_DROP, drop);
	fileMenu->Enable(MNU_PROPERTIES, properties);
	toolsMenu->Enable(MNU_CONNECT, FALSE);
	toolsMenu->Enable(MNU_DISCONNECT, FALSE);
	toolsMenu->Enable(MNU_QUERYBUILDER, sql);
	viewMenu->Enable(MNU_REFRESH, refresh);
	treeContextMenu->Enable(MNU_DROP, drop);
	treeContextMenu->Enable(MNU_CONNECT, FALSE);
	treeContextMenu->Enable(MNU_DISCONNECT, FALSE);
	treeContextMenu->Enable(MNU_REFRESH, refresh);
	treeContextMenu->Enable(MNU_PROPERTIES, properties);
	treeContextMenu->Enable(MNU_QUERYBUILDER, sql);

}

