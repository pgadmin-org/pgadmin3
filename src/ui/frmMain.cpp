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
    EVT_MENU(MNU_TIPOFTHEDAY, frmMain::OnTipOfTheDay)
    EVT_MENU(MNU_UPGRADEWIZARD, frmMain::OnUpgradeWizard)
    EVT_TREE_SEL_CHANGED(CTL_BROWSER, frmMain::OnSelChanged)
    EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER, frmMain::OnSelActivated)
END_EVENT_TABLE()

frmMain::frmMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

    // Build menus
    mnuBar = new wxMenuBar();

    // File Menu
    wxMenu *mnuFile = new wxMenu;
    mnuFile->Append(MNU_ADDSERVER, wxT("&Add Server..."), wxT("Add a connection to a server."));
    mnuFile->Append(MNU_PASSWORD, wxT("C&hange password..."), wxT("Change your password."));
    mnuFile->AppendSeparator();
    mnuFile->Append(MNU_SAVEDEFINITION, wxT("&Save definition..."), wxT("Save the SQL definition of the selected object."));
    mnuFile->Append(MNU_SAVESCHEMA, wxT("S&ave DB schema..."), wxT("Save the schema of the current database."));
    mnuFile->AppendSeparator();
    mnuFile->Append(MNU_EXIT, wxT("E&xit"), wxT("Quit this program."));
    mnuBar->Append(mnuFile, wxT("&File"));

    // Tools Menu
    wxMenu *mnuTools = new wxMenu;
    mnuTools->Append(MNU_UPGRADEWIZARD, wxT("&Upgrade Wizard..."), wxT("Run the upgrade wizard."));
    mnuTools->AppendSeparator();
    mnuTools->Append(MNU_OPTIONS, wxT("&Options..."), wxT("Show options dialog."));
    mnuBar->Append(mnuTools, wxT("&Tools"));

    // View Menu
    wxMenu *mnuView = new wxMenu;
    mnuView->Append(MNU_SYSTEMOBJECTS, wxT("&System objects"), wxT("Show or hide system objects."));
    mnuBar->Append(mnuView, wxT("&View"));

    // Help Menu
    wxMenu *mnuHelp = new wxMenu;
    mnuHelp->Append(MNU_CONTENTS, wxT("&Help..."), wxT("Open the helpfile."));
    mnuHelp->Append(MNU_TIPOFTHEDAY, wxT("&Tip of the day..."), wxT("Show a tip of the day."));
    mnuHelp->AppendSeparator();
    mnuHelp->Append(MNU_ABOUT, wxT("&About..."), wxT("Show about dialog."));
    mnuBar->Append(mnuHelp, wxT("&Help"));

    // Add the Menubar
    SetMenuBar(mnuBar);

    // Status bar
    CreateStatusBar(3);
    static const int iWidths[3] = {0, -1, 100};
    SetStatusWidths(3, iWidths);
    SetStatusText(wxT(""), 0);
    SetStatusText(wxT("Ready."), 1);
    SetStatusText(wxT("0 Secs"), 2);

    // Toolbar bar

    CreateToolBar();

    // Return objects
    stBar = GetStatusBar();
    tlBar = GetToolBar();

    // Set up toolbar
    wxBitmap tlBarBitmaps[10];
    tlBar->SetToolBitmapSize(wxSize(32, 32));
    tlBarBitmaps[0] = wxBitmap(connect_xpm);
    tlBarBitmaps[1] = wxBitmap(refresh_xpm);
    tlBarBitmaps[2] = wxBitmap(create_xpm);
    tlBarBitmaps[3] = wxBitmap(drop_xpm);
    tlBarBitmaps[4] = wxBitmap(properties_xpm);
    tlBarBitmaps[5] = wxBitmap(sql_xpm);
    tlBarBitmaps[6] = wxBitmap(viewdata_xpm);
    tlBarBitmaps[7] = wxBitmap(vacuum_xpm);
    tlBarBitmaps[8] = wxBitmap(record_xpm);
    tlBarBitmaps[9] = wxBitmap(stop_xpm);

    tlBar->AddTool(BTN_ADDSERVER, wxT("Add Server"), tlBarBitmaps[0], wxT("Add a connection to a server."), wxITEM_NORMAL);
    tlBar->AddTool(BTN_REFRESH, wxT("Refresh"), tlBarBitmaps[1], wxT("Refrsh the data below the selected object."), wxITEM_NORMAL);
    tlBar->AddSeparator();
    tlBar->AddTool(BTN_CREATE, wxT("Create"), tlBarBitmaps[2], wxT("Create a new object of the same type as the selected object."), wxITEM_NORMAL);
    tlBar->AddTool(BTN_DROP, wxT("Drop"), tlBarBitmaps[3], wxT("Drop the currently selected object."), wxITEM_NORMAL);
    tlBar->AddTool(BTN_PROPERTIES, wxT("Properties"), tlBarBitmaps[4], wxT("Display/edit the properties of the selected object."), wxITEM_NORMAL);
    tlBar->AddSeparator();
    tlBar->AddTool(BTN_SQL, wxT("SQL"), tlBarBitmaps[5], wxT("Execute arbitrary SQL queries."), wxITEM_NORMAL);
    tlBar->AddTool(BTN_VIEWDATA, wxT("View Data"), tlBarBitmaps[6], wxT("View the data in the selected object."), wxITEM_NORMAL);
    tlBar->AddTool(BTN_VACUUM, wxT("Vacuum"), tlBarBitmaps[7], wxT("Vacuum the current database or table."), wxITEM_NORMAL);
    tlBar->AddSeparator();
    tlBar->AddTool(BTN_RECORD, wxT("Record"), tlBarBitmaps[8], wxT("Record a query log."), wxITEM_NORMAL);
    tlBar->AddTool(BTN_STOP, wxT("Stop"), tlBarBitmaps[9], wxT("Stop recording the query log."), wxITEM_NORMAL);

    // Display the bar and configure buttons. 
    tlBar->Realize();
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
    tlBar->EnableTool(BTN_STOP, FALSE);
    
    // Setup the vertical splitter & treeview
    wxSplitterWindow* splVertical = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    wxSplitterWindow* splHorizontal = new wxSplitterWindow(splVertical, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    tvBrowser = new wxTreeCtrl(splVertical, CTL_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
    splVertical->SplitVertically(tvBrowser, splHorizontal, 200);
    splVertical->SetMinimumPaneSize(50);

    // Setup the horizontal splitter for the listview & sql pane
    wxNotebook* nbListViews = new wxNotebook(splHorizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    lvProperties = new wxListCtrl(nbListViews, CTL_PROPVIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    lvStatistics = new wxListCtrl(nbListViews, CTL_STATVIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    nbListViews->AddPage(lvProperties, wxT("Properties"));
    nbListViews->AddPage(lvStatistics, wxT("Statistics"));
    txtSQLPane = new ctlSQLBox(splHorizontal, CTL_SQLPANE, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
    txtSQLPane->SetBackgroundColour(*wxLIGHT_GREY);
    splHorizontal->SplitHorizontally(nbListViews, txtSQLPane, 300);
    splHorizontal->SetMinimumPaneSize(50);

    //Setup a Browser imagelist
    wxImageList *ilBrowser = new wxImageList(16, 16);
    tvBrowser->SetImageList(ilBrowser);

    //Stuff the Image List
    ilBrowser->Add(wxIcon(server_xpm));
    ilBrowser->Add(wxIcon(serverbad_xpm));
    ilBrowser->Add(wxIcon(database_xpm));
    ilBrowser->Add(wxIcon(language_xpm));
    ilBrowser->Add(wxIcon(namespace_xpm));
    ilBrowser->Add(wxIcon(aggregate_xpm));
    ilBrowser->Add(wxIcon(function_xpm));
    ilBrowser->Add(wxIcon(operator_xpm));
    ilBrowser->Add(wxIcon(sequence_xpm));
    ilBrowser->Add(wxIcon(table_xpm));
    ilBrowser->Add(wxIcon(type_xpm));
    ilBrowser->Add(wxIcon(view_xpm));
    ilBrowser->Add(wxIcon(user_xpm));
    ilBrowser->Add(wxIcon(group_xpm));
    ilBrowser->Add(wxIcon(baddatabase_xpm));
    ilBrowser->Add(wxIcon(closeddatabase_xpm));


    // Add the root node
    pgObject *objServers = new pgObject(PG_SERVERS, wxString("Servers"));
    itmServers = tvBrowser->AddRoot(wxT("Servers"), 0, -1, objServers);
    pgObject *objAddServer = new pgObject(PG_ADD_SERVER, wxString("Add Server"));
    wxTreeItemId itmAddServer = tvBrowser->AppendItem(itmServers, wxT("Add Server..."), 0, -1, objAddServer);
    tvBrowser->Expand(itmServers);

    // Setup the property imagelist
    wxImageList *ilProperties = new wxImageList(16, 16);
    lvProperties->SetImageList(ilProperties, wxIMAGE_LIST_SMALL);
    ilProperties->Add(wxIcon(property_xpm));

    // Add the property view columns
    lvProperties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    lvProperties->InsertItem(0, wxT("No properties are available for the current selection"), 0);

    // Setup a statistics view imagelist
    wxImageList *ilStatistics = new wxImageList(16, 16);
    lvStatistics->SetImageList(ilStatistics, wxIMAGE_LIST_SMALL);
    ilStatistics->Add(wxIcon(statistics_xpm));

    // Add the statistics view columns & set the colour
    lvStatistics->InsertColumn(0, wxT("Statistics"), wxLIST_FORMAT_LEFT, 500);
    lvStatistics->InsertItem(0, wxT("No statistics are available for the current selection"), 0);
    wxColour colBack;
    colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    lvStatistics->SetBackgroundColour(colBack);

    // Load servers
    RetrieveServers();
}

frmMain::~frmMain()
{
    extern sysSettings *objSettings;
    objSettings->SetFrmMainWidth(GetSize().x);
    objSettings->SetFrmMainHeight(GetSize().y);
    objSettings->SetFrmMainLeft(GetPosition().x);
    objSettings->SetFrmMainTop(GetPosition().y);
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
    extern sysSettings *objSettings;
    wxTipProvider *tipProvider = wxCreateFileTipProvider(wxT("tips.txt"), objSettings->GetNextTipOfTheDay());
    objSettings->SetShowTipOfTheDay(wxShowTip(this, tipProvider));
    objSettings->SetNextTipOfTheDay(tipProvider->GetCurrentTip());
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
    winPassword->Show(TRUE);
}

void frmMain::OnAddServer()
{
    extern sysSettings *objSettings;

    // Create a server object and connec it.
    pgServer *objServer = new pgServer(objSettings->GetLastServer(), objSettings->GetLastDatabase(), objSettings->GetLastUsername(), objSettings->GetLastPort());
    int iRes = objServer->Connect();

    // Check the result, and handle it as appropriate
    if (iRes == PGCONN_OK) {
        wxLogInfo(wxT("pgServer object initialised as required."));
        tvBrowser->AppendItem(itmServers, objServer->GetIdentifier(), 0, -1, objServer);
        tvBrowser->Expand(itmServers);

    } else if (iRes == PGCONN_DNSERR)  {
        delete objServer;
        OnAddServer();

    } else if (iRes == PGCONN_BAD)  {
        wxString szMsg;
        szMsg.Printf(wxT("Error connecting to the server: %s"), objServer->GetLastError().c_str());
        wxLogError(wxT(szMsg));
        delete objServer;
        OnAddServer();

    } else {
        wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
        delete objServer;
    }

    // Reset the Servers node text
    wxString szLabel;
    szLabel.Printf(wxT("Servers (%d)"), tvBrowser->GetChildrenCount(itmServers, FALSE) - 1);
    tvBrowser->SetItemText(itmServers, szLabel);
    StoreServers();
}

void frmMain::ReconnectServer(pgServer *objServer)
{
    // Create a server object and connect it.
    int iRes = objServer->Connect(TRUE);

    // Check the result, and handle it as appropriate
    if (iRes == PGCONN_OK) {
        wxLogInfo(wxT("pgServer object initialised as required."));
        tvBrowser->SetItemImage(objServer->GetId(), 0, wxTreeItemIcon_Normal);
        tvBrowser->SetItemImage(objServer->GetId(), 0, wxTreeItemIcon_Selected);
        tvBrowser->Collapse(itmServers);
        tvBrowser->Expand(itmServers);
        tvBrowser->SelectItem(objServer->GetId());

    } else if (iRes == PGCONN_DNSERR)  {
        delete objServer;
        OnAddServer();

    } else if (iRes == PGCONN_BAD)  {
        wxString szMsg;
        szMsg.Printf(wxT("%s"), objServer->GetLastError().c_str());
        wxLogError(wxT(szMsg));
        ReconnectServer(objServer);

    } else {
        wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
    }
}

void frmMain::OnSelChanged()
{
    // Reset the listviews
    lvProperties->ClearAll();
    lvProperties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    lvProperties->InsertItem(0, wxT("No properties are available for the current selection"), 0);
    lvStatistics->ClearAll();
    lvStatistics->InsertColumn(0, wxT("Statistics"), wxLIST_FORMAT_LEFT, 500);
    lvStatistics->InsertItem(0, wxT("No statistics are available for the current selection"), 0);

    // Reset the toolbar
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);

    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId itmX = tvBrowser->GetSelection();
    pgObject *itmData = (pgObject *)tvBrowser->GetItemData(itmX);
    int iType(itmData->GetType());

    switch (iType) {
        case PG_SERVER:
            StartMsg(wxT("Retrieving server properties"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);
            tvServer((pgServer *)itmData);
            svServer((pgServer *)itmData);
            EndMsg();
            break;

        case PG_DATABASES:
            StartMsg(wxT("Retrieving database details"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);
            tvDatabases((pgCollection *)itmData);
            svDatabases((pgCollection *)itmData);
            EndMsg();
            break;

        case PG_DATABASE:
            StartMsg(wxT("Retrieving database details"));
            SetButtons(TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE);
            tvDatabase((pgDatabase *)itmData);
            //svDatabases((pgCollection *)itmData);
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
    wxTreeItemId itmX = tvBrowser->GetSelection();
    pgObject *itmData = (pgObject *)tvBrowser->GetItemData(itmX);
    int iType(itmData->GetType());
    pgServer *objServer;

    switch (iType) {
        case PG_ADD_SERVER:
            OnAddServer();
            break;

        case PG_SERVER:
            objServer = (pgServer *)itmData;
            if (!objServer->GetConnected()) {
                ReconnectServer(objServer);
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
#ifdef __WXMSW__
    wxConfig *sysConfig = new wxConfig(APPNAME_L);
#else 
    wxConfig *sysConfig = new wxConfig(APPNAME_S);
#endif

    // Write the individual servers
    // Iterate through all the child nodes of the Servers node
    long lCookie;
    wxString szKey;
    pgObject *itmData;
    pgServer *objServer;
    int iServers = 0;

    wxTreeItemId itmX = tvBrowser->GetFirstChild(itmServers, lCookie);
    while (itmX) {
        itmData = (pgObject *)tvBrowser->GetItemData(itmX);
        if (itmData->GetType() == PG_SERVER) {
            // We have a sever, so cast the object and save the settings
            ++iServers;
            objServer = (pgServer *)itmData;

            // Hostname
            szKey.Printf("Servers/Server %d", iServers);
            sysConfig->Write(szKey, objServer->GetName());

            // Port
            szKey.Printf("Servers/Port %d", iServers);
            sysConfig->Write(szKey, objServer->GetPort());

            // Database
            szKey.Printf("Servers/Database %d", iServers);
            sysConfig->Write(szKey, objServer->GetDatabase());

            // Username
            szKey.Printf("Servers/Username %d", iServers);
            sysConfig->Write(szKey, objServer->GetUsername());
        }

        // Get the next item
        itmX = tvBrowser->GetNextChild(itmServers, lCookie);
    }

    // Write the server count
    sysConfig->Write(wxT("Servers/Count"), iServers);
    wxString szMsg;
    szMsg.Printf("Stored %d servers.", iServers);
    wxLogInfo(szMsg);

}

void frmMain::RetrieveServers()
{
    // Retrieve previously stored servers
    wxLogInfo(wxT("Reloading servers..."));

#ifdef __WXMSW__
    wxConfig *sysConfig = new wxConfig(APPNAME_L);
#else 
    wxConfig *sysConfig = new wxConfig(APPNAME_S);
#endif

    int iServers;
    sysConfig->Read(wxT("Servers/Count"), &iServers, 0);

    int iLoop, iPort;
    wxString szKey, szServer, szDatabase, szUsername;
    pgServer * objServer;

    for (iLoop = 1; iLoop <= iServers; ++iLoop) {
        
        // Server
        szKey.Printf("Servers/Server %d", iLoop);
        sysConfig->Read(szKey, &szServer, wxT(""));

        // Port
        szKey.Printf("Servers/Port %d", iLoop);
        sysConfig->Read(szKey, &iPort, 0);

        // Database
        szKey.Printf("Servers/Database %d", iLoop);
        sysConfig->Read(szKey, &szDatabase, wxT(""));

        // Username
        szKey.Printf("Servers/Username %d", iLoop);
        sysConfig->Read(szKey, &szUsername, wxT(""));

        // Add the Server node
        objServer = new pgServer(szServer, szDatabase, szUsername, iPort);
        tvBrowser->AppendItem(itmServers, objServer->GetIdentifier(), 1, -1, objServer);
    }

    // Reset the Servers node text
    wxString szLabel;
    szLabel.Printf(wxT("Servers (%d)"), tvBrowser->GetChildrenCount(itmServers, FALSE) - 1);
    tvBrowser->SetItemText(itmServers, szLabel);
}

void frmMain::OnDrop()
{
    // This handler will primarily deal with dropping items

    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId itmX = tvBrowser->GetSelection();
    pgObject *itmData = (pgObject *)tvBrowser->GetItemData(itmX);
    int iType(itmData->GetType());
    wxString szMsg, szLabel;

    switch (iType) {
        case PG_SERVER:
            szMsg.Printf(wxT("Are you sure you wish to remove the server: %s?"), itmData->GetIdentifier().c_str());
            if (wxMessageBox(szMsg, wxT("Remove Server?"), wxYES_NO | wxICON_QUESTION) == wxYES) {

                szMsg.Printf(wxT("Removing server: %s"), itmData->GetIdentifier().c_str());
                wxLogInfo(szMsg);
                tvBrowser->Delete(itmX);

                // Reset the Servers node text
                szLabel.Printf(wxT("Servers (%d)"), tvBrowser->GetChildrenCount(itmServers, FALSE) - 1);
                tvBrowser->SetItemText(itmServers, szLabel);
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

    long lCookie;
    wxTreeItemId itmY = tvBrowser->GetSelection();
    wxTreeItemId itmX = tvBrowser->GetFirstChild(itmY, lCookie);
    while (itmX) {
        tvBrowser->Delete(itmX);
        itmX = tvBrowser->GetFirstChild(itmY, lCookie);
    }
}

void frmMain::SetButtons(bool bRefresh, bool bCreate, bool bDrop, bool bProperties, bool bSQL, bool bViewData, bool bVacuum)
{
    tlBar->EnableTool(BTN_REFRESH, bRefresh);
    tlBar->EnableTool(BTN_CREATE, bCreate);
    tlBar->EnableTool(BTN_DROP, bDrop);
    tlBar->EnableTool(BTN_PROPERTIES, bProperties);
    tlBar->EnableTool(BTN_SQL, bSQL);
    tlBar->EnableTool(BTN_VIEWDATA, bViewData);
    tlBar->EnableTool(BTN_VACUUM, bVacuum);
}

void frmMain::tvServer(pgServer *objServer)
{
    // This handler will primarily deal with displaying item
    // properties in the main window.

    wxString szMsg;

    // Add child nodes if necessary
    if (objServer->GetConnected()) {
        if (tvBrowser->GetChildrenCount(objServer->GetId(), FALSE) != 3) {

            // Log
            szMsg.Printf(wxT("Adding child object to server %s"), objServer->GetIdentifier().c_str());
            wxLogInfo(szMsg);
    
            // Databases
            pgCollection *objCollection = new pgCollection(PG_DATABASES, wxString("Databases"));
            objCollection->SetServer(objServer);
            wxTreeItemId itmNewNode = tvBrowser->AppendItem(objServer->GetId(), objCollection->GetTypeName(), 2, -1, objCollection);
      
            // Groups
            objCollection = new pgCollection(PG_GROUPS, wxString("Groups"));
            objCollection->SetServer(objServer);
            itmNewNode = tvBrowser->AppendItem(objServer->GetId(), objCollection->GetTypeName(), 13, -1, objCollection);
    
            // Users
            objCollection = new pgCollection(PG_USERS, wxString("Users"));
            objCollection->SetServer(objServer);
            itmNewNode = tvBrowser->AppendItem(objServer->GetId(), objCollection->GetTypeName(), 12, -1, objCollection);
        }
    }


    szMsg.Printf(wxT("Displaying properties for server %s"), objServer->GetIdentifier().c_str());
    wxLogInfo(szMsg);

    // Add the properties view columns
    lvProperties->ClearAll();
    lvProperties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    lvProperties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 400);

    // Display the Server properties
    lvProperties->InsertItem(0, wxT("Hostname"), 0);
    lvProperties->SetItem(0, 1, objServer->GetName());

    lvProperties->InsertItem(1, wxT("Port"), 0);
    lvProperties->SetItem(1, 1, NumToStr((double)objServer->GetPort()));

    lvProperties->InsertItem(2, wxT("Initial Database"), 0);
    lvProperties->SetItem(2, 1, objServer->GetDatabase());

    lvProperties->InsertItem(3, wxT("Username"), 0);
    lvProperties->SetItem(3, 1, objServer->GetUsername());

    lvProperties->InsertItem(4, wxT("Version String"), 0);
    lvProperties->SetItem(4, 1, objServer->GetVersionString());

    lvProperties->InsertItem(5, wxT("Version Number"), 0);
    lvProperties->SetItem(5, 1, NumToStr(objServer->GetVersionNumber()));

    lvProperties->InsertItem(6, wxT("Last System OID"), 0);
    lvProperties->SetItem(6, 1, NumToStr(objServer->GetLastSystemOID()));

    lvProperties->InsertItem(7, wxT("Connected?"), 0);
    lvProperties->SetItem(7, 1, BoolToYesNo(objServer->GetConnected()));
}

void frmMain::svServer(pgServer *objServer)
{
    if(!objServer->GetConnected()) return;
    
    wxString szMsg;
    szMsg.Printf(wxT("Displaying statistics for server %s"), objServer->GetIdentifier().c_str());
    wxLogInfo(szMsg);

    // Add the statistics view columns
    lvStatistics->ClearAll();
    lvStatistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    lvStatistics->InsertColumn(1, wxT("PID"), wxLIST_FORMAT_LEFT, 50);
    lvStatistics->InsertColumn(2, wxT("User"), wxLIST_FORMAT_LEFT, 100);
    lvStatistics->InsertColumn(3, wxT("Current Query"), wxLIST_FORMAT_LEFT, 400);

    pgSet rsStat = objServer->ExecuteSet(wxT("SELECT datname, procpid, usename, current_query FROM pg_stat_activity"));

    while (!rsStat.Eof()) {
        lvStatistics->InsertItem(rsStat.CurrentPos() - 1, rsStat.GetVal(wxT("datname")), 0);
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 1, rsStat.GetVal(wxT("procpid")));
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 2, rsStat.GetVal(wxT("usename")));
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 3, rsStat.GetVal(wxT("current_query")));
        rsStat.MoveNext();
    }


}


void frmMain::tvDatabases(pgCollection *objCollection)
{
    wxString szMsg;
    pgDatabase *objDatabase;

    if (tvBrowser->GetChildrenCount(objCollection->GetId(), FALSE) == 0) {

        // Log
        szMsg.Printf(wxT("Adding databases to server %s"), objCollection->GetServer()->GetIdentifier().c_str());
        wxLogInfo(szMsg);

        // Add Database node
        pgObject *objAddDatabase = new pgObject(PG_ADD_DATABASE, wxString("Add Database"));
        wxTreeItemId itmAddDatabase = tvBrowser->AppendItem(objCollection->GetId(), wxT("Add Database..."), 2, -1, objAddDatabase);

        // Get the databases
        pgSet rsDatabases = objCollection->GetServer()->ExecuteSet(wxT("SELECT oid, datname, datpath, datallowconn, datconfig, datacl, pg_encoding_to_char(encoding) AS serverencoding, pg_get_userbyid(datdba) AS datowner FROM pg_database"));

        wxTreeItemId itmNewNode;

        while (!rsDatabases.Eof()) {

            objDatabase = new pgDatabase(rsDatabases.GetVal(wxT("datname")));
            objDatabase->SetServer(objCollection->GetServer());
            objDatabase->iSetOid(StrToDouble(rsDatabases.GetVal(wxT("oid"))));
            objDatabase->iSetOwner(rsDatabases.GetVal(wxT("datowner")));
            objDatabase->iSetAcl(rsDatabases.GetVal(wxT("datacl")));
            objDatabase->iSetPath(rsDatabases.GetVal(wxT("datpath")));
            objDatabase->iSetEncoding(rsDatabases.GetVal(wxT("serverencoding")));
            objDatabase->iSetVariables(rsDatabases.GetVal(wxT("datconfig")));
            objDatabase->iSetAllowConnections(StrToBool(rsDatabases.GetVal(wxT("datallowconn"))));

            // Add the treeview node
            itmNewNode = tvBrowser->AppendItem(objCollection->GetId(), objDatabase->GetIdentifier(), 15, -1, objDatabase);

            rsDatabases.MoveNext();
        }

        // Reset the Databases node text
        wxString szLabel;
        szLabel.Printf(wxT("Databases (%d)"), tvBrowser->GetChildrenCount(objCollection->GetId(), FALSE) - 1);
        tvBrowser->SetItemText(objCollection->GetId(), szLabel);

    }

    // Display the properties.

    long lCookie;
    int iCount = 0;
    wxString szKey;
    pgObject *itmData;

    // Setup listview
    lvProperties->ClearAll();
    lvProperties->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    lvProperties->InsertColumn(1, wxT("Comment"), wxLIST_FORMAT_LEFT, 400);

    wxTreeItemId itmCollection = objCollection->GetId();
    wxTreeItemId itmX = tvBrowser->GetFirstChild(itmCollection, lCookie);
    while (itmX) {
        itmData = (pgObject *)tvBrowser->GetItemData(itmX);
        if (itmData->GetType() == PG_DATABASE) {

            objDatabase = (pgDatabase *)itmData;

            lvProperties->InsertItem(0, objDatabase->GetName(), 0);
            lvProperties->SetItem(0, 1, objDatabase->GetComment());
        }

        // Get the next item
        itmX = tvBrowser->GetNextChild(itmServers, lCookie);
    }
}

void frmMain::svDatabases(pgCollection *objCollection)
{
    
    wxString szMsg;
    szMsg.Printf(wxT("Displaying statistics for databases on %s"), objCollection->GetServer()->GetIdentifier().c_str());
    wxLogInfo(szMsg);

    // Add the statistics view columns
    lvStatistics->ClearAll();
    lvStatistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    lvStatistics->InsertColumn(1, wxT("Backends"), wxLIST_FORMAT_LEFT, 75);
    lvStatistics->InsertColumn(2, wxT("Xact Committed"), wxLIST_FORMAT_LEFT, 100);
    lvStatistics->InsertColumn(3, wxT("Xact Rolled Back"), wxLIST_FORMAT_LEFT, 100);
    lvStatistics->InsertColumn(4, wxT("Blocks Read"), wxLIST_FORMAT_LEFT, 100);
    lvStatistics->InsertColumn(5, wxT("Blocks Hit"), wxLIST_FORMAT_LEFT, 100);

    pgSet rsStat = objCollection->GetServer()->ExecuteSet(wxT("SELECT datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit FROM pg_stat_database ORDER BY datname"));

    while (!rsStat.Eof()) {
        lvStatistics->InsertItem(rsStat.CurrentPos() - 1, rsStat.GetVal(wxT("datname")), 0);
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 1, rsStat.GetVal(wxT("numbackends")));
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 2, rsStat.GetVal(wxT("xact_commit")));
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 3, rsStat.GetVal(wxT("xact_rollback")));
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 4, rsStat.GetVal(wxT("blks_read")));
        lvStatistics->SetItem(rsStat.CurrentPos() - 1, 5, rsStat.GetVal(wxT("blks_hit")));
        rsStat.MoveNext();
    }
}

void frmMain::tvDatabase(pgDatabase *objDatabase)
{

    wxString szMsg;

    if (objDatabase->Connect() == PGCONN_OK) {

        // Set the icon if required
        if (tvBrowser->GetItemImage(objDatabase->GetId(), wxTreeItemIcon_Normal) != 2) {
            tvBrowser->SetItemImage(objDatabase->GetId(), 2, wxTreeItemIcon_Normal);
            tvBrowser->SetItemImage(objDatabase->GetId(), 2, wxTreeItemIcon_Selected);
            wxTreeItemId itmDatabases = tvBrowser->GetParent(objDatabase->GetId());
            tvBrowser->Collapse(itmDatabases);
            tvBrowser->Expand(itmDatabases);
            tvBrowser->SelectItem(objDatabase->GetId());
        }

        // Add child nodes if necessary
        if (tvBrowser->GetChildrenCount(objDatabase->GetId(), FALSE) != 2) {

            // Log
            szMsg.Printf(wxT("Adding child object to database %s"), objDatabase->GetIdentifier().c_str());
            wxLogInfo(szMsg);

            // Languages
            pgCollection *objCollection = new pgCollection(PG_LANGUAGES, wxString("Languages"));
            objCollection->SetServer(objDatabase->GetServer());
            wxTreeItemId itmNewNode = tvBrowser->AppendItem(objDatabase->GetId(), objCollection->GetTypeName(), 3, -1, objCollection);
  
            // Schemas
            objCollection = new pgCollection(PG_SCHEMAS, wxString("Schemas"));
            objCollection->SetServer(objDatabase->GetServer());
            itmNewNode = tvBrowser->AppendItem(objDatabase->GetId(), objCollection->GetTypeName(), 4, -1, objCollection);
        }
    }
    // Setup listview
    lvProperties->ClearAll();
    lvProperties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    lvProperties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 350);

    lvProperties->InsertItem(0, wxT("Name"), 0);
    lvProperties->SetItem(0, 1, objDatabase->GetName());
    lvProperties->InsertItem(1, wxT("OID"), 0);
    lvProperties->SetItem(1, 1, NumToStr(objDatabase->GetOid()));
    lvProperties->InsertItem(2, wxT("Owner"), 0);
    lvProperties->SetItem(2, 1, objDatabase->GetOwner());
    lvProperties->InsertItem(3, wxT("ACL"), 0);
    lvProperties->SetItem(3, 1, objDatabase->GetAcl());
    lvProperties->InsertItem(4, wxT("Path"), 0);
    lvProperties->SetItem(4, 1, objDatabase->GetPath());
    lvProperties->InsertItem(5, wxT("Encoding"), 0);
    lvProperties->SetItem(5, 1, objDatabase->GetEncoding());
    lvProperties->InsertItem(6, wxT("Variables"), 0);
    lvProperties->SetItem(6, 1, objDatabase->GetVariables());
    lvProperties->InsertItem(7, wxT("Allow Connections?"), 0);
    lvProperties->SetItem(7, 1, BoolToYesNo(objDatabase->GetAllowConnections()));
    lvProperties->InsertItem(8, wxT("Connected?"), 0);
    lvProperties->SetItem(8, 1, BoolToYesNo(objDatabase->GetConnected()));
    lvProperties->InsertItem(9, wxT("System Database?"), 0);
    lvProperties->SetItem(9, 1, BoolToYesNo(objDatabase->GetSystemObject()));
    lvProperties->InsertItem(10, wxT("Comment?"), 0);
    lvProperties->SetItem(10, 1, objDatabase->GetComment());
}
