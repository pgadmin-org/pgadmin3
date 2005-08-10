//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmMain.cpp - The main form
//
// Note: Due to the size of frmMain, event handler, browser and statistics 
//       functions are in events.cpp, browser.cpp and statistics.cpp.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#ifndef WIN32
#include <unistd.h>
#endif

// wxWindows headers
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/imaglist.h>
#include <wx/busyinfo.h>

// App headers
#include "misc.h"
#include "menu.h"
#include "pgfeatures.h"
#include "frmMain.h"
#include "ctl/ctlSQLBox.h"
#include "pgConn.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "slSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"
#include "frmOptions.h"
#include "frmHelp.h"
#include "frmAbout.h"
#include "frmHint.h"
#include "frmGrantWizard.h"
#include "frmMainConfig.h"
#include "frmHbaConfig.h"
#include "frmBackup.h"
#include "frmRestore.h"
#include "frmMaintenance.h"
#include "frmStatus.h"
#include "frmPassword.h"
#include "frmQuery.h"
#include "frmEditGrid.h"
#include "dlgServer.h"
#include "dlgDatabase.h"
#include "dlgTable.h"
#include "dlgServer.h"

// Icons
#include "images/pgAdmin3.xpm"
#include "images/elephant32.xpm"

#include "images/drop.xpm"
#include "images/vacuum.xpm"
#include "images/properties.xpm"
#include "images/refresh.xpm"
#include "images/help2.xpm"
#include "images/create.xpm"


#if wxDIALOG_UNIT_COMPATIBILITY
#error wxWindows must be compiled with wxDIALOG_UNIT_COMPATIBILITY=0!
#endif


frmMain::frmMain(const wxString& title)
: pgFrame((wxFrame *)NULL, title)
{
    msgLevel=0;

    dlgName = wxT("frmMain");
    RestorePosition(50, 50, 750, 550, 300, 200);

    wxWindowBase::SetFont(settings->GetSystemFont());

    {
        wxLogInfo(wxT("Using fontmetrics %d/%d, %d Point"), GetCharWidth(), GetCharHeight(), GetFont().GetPointSize());
        wxLogInfo(wxT("Native Description '%s'"), GetFont().GetNativeFontInfoDesc().c_str());
        wxWindowDC dc(this);
        dc.SetFont(GetFont());

        wxCoord w, h, d, e;

        dc.GetTextExtent(wxT("M"), &w, &h, &d, &e);
        wxLogInfo(wxT("Draw size of 'M': w=%d, h=%d, descent %d, external lead %d."), w, h, d, e);

        dc.GetTextExtent(wxT("g"), &w, &h, &d, &e);
        wxLogInfo(wxT("Draw size of 'g': w=%d, h=%d, descent %d, external lead %d."), w, h, d, e);

        dc.GetTextExtent(wxT("Mg"), &w, &h, &d, &e);
        wxLogInfo(wxT("Draw size of 'Mg': w=%d, h=%d, descent %d, external lead %d."), w, h, d, e);
    }

    // Current database
    denyCollapseItem=wxTreeItemId();
    currentObject = 0;
    m_database = 0;

    // Icon
    wxIconBundle icons;
    icons.AddIcon(wxIcon(pgAdmin3_xpm));
    icons.AddIcon(wxIcon(elephant32_xpm));
    SetIcons(icons);

    // wxGTK needs this deferred
    pgaFactory::RealizeImages();

    // Build menus
    fileMenu = new wxMenu();
    editMenu = new wxMenu();
    toolsMenu = new wxMenu();
    viewMenu = new wxMenu();
    helpMenu = new wxMenu();
    newMenu=new wxMenu();

    wxMenu *cfgMenu=new wxMenu();
    
    toolBar=CreateToolBar();
    toolBar->SetToolBitmapSize(wxSize(32, 32));




    fileMenu->Append(MNU_SAVEDEFINITION, _("&Save definition..."),_("Save the SQL definition of the selected object."));
    fileMenu->AppendSeparator();
    menuFactories.Add(new addServerFactory(fileMenu, toolBar));

    toolBar->AddTool(MNU_REFRESH, _("Refresh"), wxBitmap(refresh_xpm), _("Refresh the selected object."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_CREATE, _("Create"), wxBitmap(create_xpm), _("Create a new object of the same type as the selected object."), wxITEM_NORMAL);
    toolBar->AddTool(MNU_DROP, _("Drop"), wxBitmap(drop_xpm), _("Drop the currently selected object."), wxITEM_NORMAL);
    toolBar->AddTool(MNU_PROPERTIES, _("Properties"), wxBitmap(properties_xpm), _("Display/edit the properties of the selected object."), wxITEM_NORMAL);
    toolBar->AddSeparator();

    menuFactories.Add(new passwordFactory(fileMenu, 0));
    fileMenu->AppendSeparator();
    optionsFactory *optFact=new optionsFactory(fileMenu, 0);
    menuFactories.Add(optFact);
    fileMenu->AppendSeparator();
    menuFactories.Add(new mainConfigFileFactory(fileMenu, 0));
    menuFactories.Add(new hbaConfigFileFactory(fileMenu, 0));

    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"),                _("Quit this program."));

    editMenu->Append(MNU_NEWOBJECT, _("New &Object"), newMenu,    _("Create a new object."));
    editMenu->AppendSeparator();
    editMenu->Append(MNU_CREATE, _("&Create"),                    _("Create a new object of the same type as the selected object."));
    editMenu->Append(MNU_DROP, _("&Delete/Drop\tDel"),            _("Delete/Drop the selected object."));
    editMenu->Append(MNU_DROPCASCADED, _("Drop cascaded"),        _("Drop the selected object and all objects dependent on it."));
    editMenu->Append(MNU_PROPERTIES, _("&Properties"),    		  _("Display/edit the properties of the selected object."));

    menuFactories.Add(new startServiceFactory(toolsMenu, 0));
    menuFactories.Add(new stopServiceFactory(toolsMenu, 0));

    menuFactories.Add(new connectServerFactory(toolsMenu, 0));
    menuFactories.Add(new disconnectServerFactory(toolsMenu, 0));
    toolsMenu->AppendSeparator();
    menuFactories.Add(new queryToolFactory(toolsMenu, toolBar));
    menuFactories.Add(new editGridFactory(toolsMenu, toolBar));
    menuFactories.Add(new editGridFilteredFactory(toolsMenu, toolBar));
    menuFactories.Add(new maintenanceFactory(toolsMenu, toolBar));

    menuFactories.Add(new backupFactory(toolsMenu, 0));
    menuFactories.Add(new restoreFactory(toolsMenu, 0));

    menuFactories.Add(new grantWizardFactory(toolsMenu, 0));
    menuFactories.Add(new mainConfigFactory(cfgMenu, 0));
    menuFactories.Add(new hbaConfigFactory(cfgMenu, 0));

    toolsMenu->Append(MNU_CONFIGSUBMENU, _("Server configuration"), cfgMenu);

    slonyMenu=new wxMenu();
    slonyMenu->Append(MNU_SLONY_RESTART, _("Restart node"),       _("Restart node."));
    slonyMenu->Append(MNU_SLONY_UPGRADE, _("Upgrade node"),       _("Upgrade node to newest function version."));
    slonyMenu->Append(MNU_SLONY_FAILOVER, _("Failover"),          _("Failover to backup node."));
    slonyMenu->Append(MNU_SLONY_MERGESET, _("Merge set"),         _("Merge two replication sets."));
    slonyMenu->Append(MNU_SLONY_MOVESET, _("Move set"),           _("Move replication set to different node"));
    toolsMenu->Append(MNU_SLONY_SUBMENU, _("Replication"), slonyMenu);

    toolsMenu->AppendSeparator();

    menuFactories.Add(new serverStatusFactory(toolsMenu, 0));

    viewMenu->Append(MNU_SYSTEMOBJECTS, _("&System objects"),     _("Show or hide system objects."), wxITEM_CHECK);
    viewMenu->AppendSeparator();
    viewMenu->Append(MNU_REFRESH, _("Re&fresh\tF5"),              _("Refresh the selected object."));

    menuFactories.Add(new countRowsFactory(viewMenu, 0));

    toolBar->AddSeparator();
    helpMenu->Append(MNU_PGSQLHELP, _("&PostgreSQL Help"),        _("Display help on PostgreSQL database system."));

    menuFactories.Add(new hintFactory(helpMenu, toolBar));

    toolBar->AddTool(MNU_HELP, _("SQL Help"), wxBitmap(help2_xpm), _("Display help on SQL commands."));

    helpMenu->Append(MNU_CONTENTS, _("&Help..."),                 _("Open the pgAdmin III helpfile."));
    helpMenu->Append(MNU_FAQ, _("pgAdmin III &FAQ"),              _("Frequently asked questions about pgAdmin III."));


    helpMenu->Append(MNU_TIPOFTHEDAY, _("&Tip of the day"),       _("Show a tip of the day."));
    helpMenu->AppendSeparator();
    helpMenu->Append(MNU_ONLINEUPDATE, _("Online Update"),        _("Check online for updates"));
    helpMenu->Append(MNU_BUGREPORT, _("&Bugreport"),              _("How to send a bugreport to the pgAdmin Development Team."));
    actionFactory *abFact=new aboutFactory(helpMenu, 0);
    menuFactories.Add(abFact);

#ifdef __WXMAC__
    wxApp::s_macPreferencesMenuItemId = optFact->GetId();
    wxApp::s_macExitMenuItemId = MNU_EXIT;
    wxApp::s_macAboutMenuItemId = abFact->GetId();
#endif 


    menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _("&File"));
    menuBar->Append(editMenu, _("&Edit"));
    menuBar->Append(toolsMenu, _("&Tools"));
    menuBar->Append(viewMenu, _("&Display"));
    menuBar->Append(helpMenu, _("&Help"));
    SetMenuBar(menuBar);

    newContextMenu = new wxMenu();
    treeContextMenu = 0;

    editMenu->Enable(MNU_NEWOBJECT, false);
    viewMenu->Check(MNU_SYSTEMOBJECTS, settings->GetShowSystemObjects());

    // Status bar
    statusBar = CreateStatusBar(3);
    int iWidths[3] = {0, -1, 100};
    SetStatusWidths(3, iWidths);
    SetStatusBarPane(-1);
    statusBar->SetStatusText(wxT(""), 0);
    statusBar->SetStatusText(_("Ready."), 1);
    statusBar->SetStatusText(_("0 Secs"), 2);

    wxAcceleratorEntry entries[4];
    entries[0].Set(wxACCEL_NORMAL, WXK_F5, MNU_REFRESH);
    entries[1].Set(wxACCEL_NORMAL, WXK_DELETE, MNU_DELETE);
    entries[2].Set(wxACCEL_NORMAL, WXK_F1, MNU_HELP);
    entries[3].Set(wxACCEL_SHIFT, WXK_F10, MNU_CONTEXTMENU);
    wxAcceleratorTable accel(4, entries);

    SetAcceleratorTable(accel);

    
    // Display the bar and configure buttons. 
    toolBar->Realize();
    SetButtons();
    toolBar->EnableTool(MNU_STOP, false);
    
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
    listViews = new wxNotebook(horizontal, CTL_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    properties = new ctlListView(listViews, CTL_PROPVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    statistics = new ctlListView(listViews, CTL_STATVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    dependsOn = new ctlListView(listViews, CTL_DEPVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    referencedBy = new ctlListView(listViews, CTL_REFVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    listViews->AddPage(properties, _("Properties"));        // NBP_PROPERTIES
    listViews->AddPage(statistics, _("Statistics"));        // NBP_STATISTICS
    listViews->AddPage(dependsOn, _("Depends on"));         // NBP_DEPENDSON
    listViews->AddPage(referencedBy, _("Referenced by"));   // NBP_REFERENCEDBY
    sqlPane = new ctlSQLBox(horizontal, CTL_SQLPANE, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);

    splitpos=settings->Read(wxT("frmMain/SplitHorizontal"), 300);
    if (splitpos < 50)
        splitpos = 50;
    if (splitpos > GetSize().y-50)
        splitpos = GetSize().y-50;
    horizontal->SplitHorizontally(listViews, sqlPane, splitpos);
    horizontal->SetMinimumPaneSize(50);

    browser->SetImageList(imageList);

    // Add the root node
    pgObject *serversObj = new pgServers();
    servers = browser->AddRoot(_("Servers"), PGICON_SERVERS, -1, serversObj);

    properties->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    // Add the property view columns
    properties->AddColumn(_("Properties"), 500);
    properties->InsertItem(0, _("No properties are available for the current selection"), PGICON_PROPERTY);


    statistics->SetImageList(imageList, wxIMAGE_LIST_SMALL);

    wxColour background;
    background = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    statistics->SetBackgroundColour(background);
    dependsOn->SetBackgroundColour(background);
    referencedBy->SetBackgroundColour(background);
    sqlPane->SetBackgroundColour(background);

    pgaFactory::RegisterMenu(this, wxCommandEventHandler(frmMain::OnNew));
    actionFactory::RegisterMenu(this, wxCommandEventHandler(frmMain::OnAction));
    actionFactory::CheckMenu(0, menuBar, toolBar);

    // Load servers
    RetrieveServers();
    browser->Expand(servers);
}


frmMain::~frmMain()
{
    StoreServers();

    settings->Write(wxT("frmMain/SplitHorizontal"), horizontal->GetSashPosition());
    settings->Write(wxT("frmMain/SplitVertical"), vertical->GetSashPosition());

    // Clear the treeview
    browser->DeleteAllItems();

    if (treeContextMenu)
        delete treeContextMenu;
}


void frmMain::Refresh(pgObject *data)
{
    StartMsg(wxString::Format(_("Refreshing %s"), data->GetTranslatedTypeName().c_str()));
    browser->Freeze();

    wxTreeItemId currentItem=data->GetId();
    browser->DeleteChildren(currentItem);

	// refresh information about the object

    data->SetDirty();
    
    pgObject *newData = data->Refresh(browser, currentItem);

    bool done = !data->GetConnection() || data->GetConnection()->GetStatus() == PGCONN_OK;

    if (newData != data)
    {
        wxLogInfo(wxT("Deleting ") + data->GetTypeName() + wxT(" ") 
            + data->GetQuotedFullIdentifier() + wxT(" for Refresh"));

        if (data == currentObject)
            currentObject = newData;

        if (newData)
        {
            wxLogInfo(wxT("Replacing with new Node ") + newData->GetTypeName() + wxT(" ") 
                + newData->GetQuotedFullIdentifier() + wxT(" for Refresh"));
            newData->SetId(currentItem);    // not done automatically
            browser->SetItemData(currentItem, newData);
            delete data;
        }
        else
        {
            wxLogInfo(wxT("No object to replace: vanished after refresh."));
            wxTreeItemId delItem=currentItem;
            currentItem=browser->GetItemParent(currentItem);
            browser->SelectItem(currentItem);
            browser->Delete(delItem);
        }
    }
    if (currentItem)
        execSelChange(currentItem, currentItem == browser->GetSelection());
    browser->Thaw();
    EndMsg(done);
}


void frmMain::ShowObjStatistics(pgObject *data, int sel)
{
    switch (sel)
    {
        case NBP_STATISTICS:
        {
            statistics->Freeze();
            statistics->ClearAll();
            statistics->AddColumn(_("Statistics"), 500);
            statistics->InsertItem(0, _("No statistics are available for the current selection"), PGICON_STATISTICS);
            data->ShowStatistics(this, statistics);
            statistics->Thaw();
            break;
        }
        case NBP_DEPENDSON:
        {
            dependsOn->Freeze();
            dependsOn->DeleteAllItems();
            data->ShowDependsOn(this, dependsOn);
            dependsOn->Thaw();
            break;
        }
        case NBP_REFERENCEDBY:
        {
            referencedBy->Freeze();
            referencedBy->DeleteAllItems();
            data->ShowReferencedBy(this, referencedBy);
            referencedBy->Thaw();
            break;
        }
        default:
            break;
    }
}


void frmMain::OnPageChange(wxNotebookEvent& event)
{
    wxTreeItemId item=browser->GetSelection();
	if (!item)
		return;
    pgObject *data = (pgObject*)browser->GetItemData(item);

    if (!data)
        return;

    ShowObjStatistics(data, event.GetSelection());
}


ctlListView *frmMain::GetStatistics()
{
    if (listViews->GetSelection() == NBP_STATISTICS)
        return statistics;
    return 0;
}


ctlListView *frmMain::GetDependsOn()
{
    if (listViews->GetSelection() == NBP_DEPENDSON)
        return dependsOn;
    return 0;
}


ctlListView *frmMain::GetReferencedBy()
{
    if (listViews->GetSelection() == NBP_REFERENCEDBY)
        return referencedBy;
    return 0;
}


bool frmMain::checkAlive()
{
    bool userInformed = false;
    bool closeIt = false;

    wxCookieType cookie;
    wxTreeItemId serverItem=browser->GetFirstChild(servers, cookie);
    while (serverItem)
    {
        pgServer *server=(pgServer*)browser->GetItemData(serverItem);

        if (server && server->GetType() == PG_SERVER && server->connection())
        {
            if (server->connection()->IsAlive())
            {
                wxCookieType cookie2;
                wxTreeItemId item = browser->GetFirstChild(serverItem, cookie2);
                while (item)
                {
                    pgObject *obj=(pgObject*)browser->GetItemData(item);
                    if (obj && obj->IsCreatedBy(databaseFactory.GetCollectionFactory()))
                    {
                        wxCookieType cookie3;
                        item = browser->GetFirstChild(obj->GetId(), cookie3);
                        while (item)
                        {
                            pgDatabase *db=(pgDatabase*)browser->GetItemData(item);
                            if (db && db->IsCreatedBy(databaseFactory))
                            {
                                pgConn *conn=db->GetConnection();
                                if (conn)
                                {
                                    if (!conn->IsAlive() && conn->GetStatus() == PGCONN_BROKEN)
                                    {
                                        conn->Close();
                                        if (!userInformed)
                                        {
                                            wxMessageDialog dlg(this, _("Close database browser? If you abort, the object browser will not show accurate data."),
                                            wxString::Format(_("Connection to database %s lost."), db->GetName().c_str()), 
                                                wxICON_EXCLAMATION|wxYES_NO|wxYES_DEFAULT);

                                            closeIt = (dlg.ShowModal() == wxID_YES);
                                            userInformed = true;
                                        }
                                        if (closeIt)
                                        {
                                            db->Disconnect();

                                            browser->DeleteChildren(db->GetId());
                                            db->UpdateIcon(browser);
                                        }
                                    }
                                }
                            }
                            item = browser->GetNextChild(obj->GetId(), cookie3);
                        }
                    }
                    item = browser->GetNextChild(serverItem, cookie2);
                }
            }
            else
            {
                if (server->connection()->GetStatus() == PGCONN_BROKEN)
                {
                    server->connection()->Close();
                    if (!userInformed)
                    {
                        wxMessageDialog dlg(this, _("Close server browser? If you abort, the object browser will not show accurate data."),
                            wxString::Format(_("Connection to server %s lost."), server->GetName().c_str()), 
                            wxICON_EXCLAMATION|wxYES_NO|wxYES_DEFAULT);

                        closeIt = (dlg.ShowModal() == wxID_YES);
                        userInformed = true;
                    }
                    if (closeIt)
                    {

                        browser->SelectItem(serverItem);
                        server->Disconnect(this);
                        execSelChange(serverItem, true);
                        browser->DeleteChildren(serverItem);
                    }
                }
            }
        }

        serverItem = browser->GetNextChild(servers, cookie);
    }
    return userInformed;
}


wxTreeItemId frmMain::RestoreEnvironment(pgServer *server)
{
    wxTreeItemId item, lastItem;
    wxString lastDatabase=server->GetLastDatabase();
    if (lastDatabase.IsNull())
        return item;

    wxCookieType cookie;
    pgObject *data=0;
    item = browser->GetFirstChild(server->GetId(), cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->IsCreatedBy(databaseFactory.GetCollectionFactory()))
            break;
        // Get the next item
        item = browser->GetNextChild(server->GetId(), cookie);
    }
    if (!item)
        return item;

    // found DATABASES item
    data->ShowTree(this, browser, 0, 0);
    lastItem=item;

    item = browser->GetFirstChild(lastItem, cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->IsCreatedBy(databaseFactory) && data->GetName() == lastDatabase)
            break;
        // Get the next item
        item = browser->GetNextChild(lastItem, cookie);
    }
    if (!item)
        return lastItem;

    // found last DATABASE 
    data->ShowTree(this, browser, 0, 0);
    lastItem = item;

    wxString lastSchema=server->GetLastSchema();
    if (lastSchema.IsNull())
        return lastItem;

    item = browser->GetFirstChild(lastItem, cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetMetaType() == PGM_SCHEMA)
            break;
        // Get the next item
        item = browser->GetNextChild(lastItem, cookie);
    }
    if (!item)
        return lastItem;

    // found SCHEMAS item
    data->ShowTree(this, browser, 0, 0);
    lastItem=item;

    item = browser->GetFirstChild(lastItem, cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->GetMetaType() == PGM_SCHEMA && data->GetName() == lastSchema)
            break;
        // Get the next item
        item = browser->GetNextChild(lastItem, cookie);
    }

    return (item ? item : lastItem);
}

int frmMain::ReconnectServer(pgServer *server)
{
    // Create a server object and connect it.
    wxBusyInfo waiting(wxString::Format(_("Connecting to server %s (%s:%d)"),
        server->GetDescription().c_str(), server->GetName().c_str(), server->GetPort()), this);

    int res = server->Connect(this, true);

    // Check the result, and handle it as appropriate
    wxTreeItemId item;
    switch (res)
    {
        case PGCONN_OK:
        {
            StartMsg(_("Restoring previous environment"));
            wxLogInfo(wxT("pgServer object initialised as required."));

            server->ShowTreeDetail(browser);
            browser->Freeze();
            item=RestoreEnvironment(server);
            browser->Thaw();

            if (item)
            {
                browser->SelectItem(item);

                wxSafeYield();
                browser->Expand(item);
                browser->EnsureVisible(item);
            }
			if (item)
				EndMsg(true);
			else
				EndMsg(false);
            return res;
        }
        case PGCONN_DNSERR:
            /*
            // looks strange to me. Shouldn_t server be removed from the tree as well?
            delete server;
            OnAddServer(wxCommandEvent());
            break;
            */
        case PGCONN_BAD:
            ReportConnError(server);
            break;

        default:
            wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
            break;
    }

    server->Disconnect(this);
    return res;
}


bool frmMain::reportError(const wxString &error, const wxString &msgToIdentify, const wxString &hint)
{
    bool identified=false;

    wxString translated=wxGetTranslation(msgToIdentify);
    if (translated != msgToIdentify)
    {
        identified = (error.Find(translated) >= 0);
    }

    if (!identified)
    {
        if (msgToIdentify.Left(20) == wxT("Translator attention"))
            identified = (error.Find(msgToIdentify.Mid(msgToIdentify.Find('!')+1)) >= 0);
        else
            identified = (error.Find(msgToIdentify) >= 0);
    }

    if (identified)
    {
        if (frmHint::WantHint(hint))
            frmHint::ShowHint(this, hint, error);
    }
    return identified;
}


void frmMain::ReportConnError(pgServer *server)
{
    wxString error=server->GetLastError();
    bool wantHint=false;

    wantHint = reportError(error, __("Translator attention: must match libpq translation!Is the server running on host"), HINT_CONNECTSERVER);
    if (!wantHint)
        wantHint = reportError(error, __("Translator attention: must match backend translation!no pg_hba.conf entry for"), HINT_MISSINGHBA);
    if (!wantHint)
        wantHint = reportError(error, __("Translator attention: must match backend translation!Ident authentication failed"), HINT_MISSINGIDENT);

    if (!wantHint)
        wxLogError(__("Error connecting to the server: %s"), error.c_str());
}


void frmMain::StoreServers()
{
    wxLogInfo(wxT("Storing listed servers for later..."));

    // Store the currently listed servers for later retrieval.
    wxCookieType cookie;
    wxString key;
    pgObject *data;
    pgServer *server;
    int numServers = 0;

	// Get the hostname for later...
	char buf[255];
	gethostname(buf, 255); 
    wxString hostname = wxString(buf, wxConvUTF8);

    // Write the individual servers
    // Iterate through all the child nodes of the Servers node
    wxTreeItemId item = browser->GetFirstChild(servers, cookie);
    while (item) {

        data = (pgObject *)browser->GetItemData(item);
        if (data->GetType() == PG_SERVER) {

			// Cast the object, and check if it was autodiscovered before saving.
			server = (pgServer *)data;

			// We have an 'added' server, so save it
			++numServers;

			// Hostname
			key.Printf(wxT("Servers/Server%d"), numServers);
		    settings->Write(key, server->GetName());

			// Comment
		    key.Printf(wxT("Servers/Description%d"), numServers);
	        settings->Write(key, server->GetDescription());

			// Service ID
		    key.Printf(wxT("Servers/ServiceID%d"), numServers);
	        settings->Write(key, server->GetServiceID());

			// Port
			key.Printf(wxT("Servers/Port%d"), numServers);
		    settings->Write(key, server->GetPort());

			// Store Password
			key.Printf(wxT("Servers/StorePwd%d"), numServers);
	        settings->Write(key, server->GetStorePwd());

			// Database
			key.Printf(wxT("Servers/Database%d"), numServers);
	        settings->Write(key, server->GetDatabaseName());

			// Username
			key.Printf(wxT("Servers/Username%d"), numServers);
	        settings->Write(key, server->GetUsername());

			// last Database
			key.Printf(wxT("Servers/LastDatabase%d"), numServers);
			settings->Write(key, server->GetLastDatabase());

			// last Schema
			key.Printf(wxT("Servers/LastSchema%d"), numServers);
			settings->Write(key, server->GetLastSchema());
	        
			// SSL
			key.Printf(wxT("Servers/SSL%d"), numServers);
			settings->Write(key, server->GetSSL());
		}

        // Get the next item
        item = browser->GetNextChild(servers, cookie);
    }

    // Write the server count
    settings->Write(wxT("Servers/Count"), numServers);
    wxLogInfo(wxT("Stored %d servers."), numServers);
}


void frmMain::RetrieveServers()
{
    // Retrieve previously stored servers
    wxLogInfo(wxT("Reloading servers..."));

    long numServers=settings->Read(wxT("Servers/Count"), 0L);

    long loop, port, ssl=0;
    wxString key, servername, description, database, username, lastDatabase, lastSchema, storePwd, serviceID;
    pgServer *server;

    wxArrayString servicedServers;

	// Get the hostname for later...
	char buf[255];
	gethostname(buf, 255); 
    wxString hostname = wxString(buf, wxConvUTF8);

    for (loop = 1; loop <= numServers; ++loop) {
        
        // Server
        key.Printf(wxT("Servers/Server%d"), loop);
        settings->Read(key, &servername, wxT(""));

        // service location
        key.Printf(wxT("Servers/ServiceID%d"), loop);
        settings->Read(key, &serviceID, wxT(""));

        // Comment
        key.Printf(wxT("Servers/Description%d"), loop);
        settings->Read(key, &description, wxT(""));

        // Store Password
        key.Printf(wxT("Servers/StorePwd%d"), loop);
        settings->Read(key, &storePwd, wxT(""));

        // Port
        key.Printf(wxT("Servers/Port%d"), loop);
        settings->Read(key, &port, 0);

        // Database
        key.Printf(wxT("Servers/Database%d"), loop);
        settings->Read(key, &database, wxT(""));

        // Username
        key.Printf(wxT("Servers/Username%d"), loop);
        settings->Read(key, &username, wxT(""));

        // last Database
        key.Printf(wxT("Servers/LastDatabase%d"), loop);
        settings->Read(key, &lastDatabase, wxT(""));

        // last Schema
        key.Printf(wxT("Servers/LastSchema%d"), loop);
        settings->Read(key, &lastSchema, wxT(""));

        // SSL mode
#ifdef SSL
        key.Printf(wxT("Servers/SSL%d"), loop);
        settings->Read(key, &ssl, 0);
#endif

        // Add the Server node
        server = new pgServer(servername, description, database, username, port, StrToBool(storePwd), ssl);
        server->iSetLastDatabase(lastDatabase);
        server->iSetLastSchema(lastSchema);
        server->iSetServiceID(serviceID);
		server->iSetDiscovered(false);
        browser->AppendItem(servers, server->GetFullName(), PGICON_SERVERBAD, -1, server);


#ifdef WIN32
        int bspos = serviceID.Find('\\');
        if (bspos >= 0)
        {
            if (serviceID.Left(2) != wxT(".\\") && !serviceID.Matches(wxGetHostName() + wxT("\\*")))
                serviceID = wxEmptyString;
        }
        if (!serviceID.IsEmpty())
            servicedServers.Add(serviceID);
#endif

    }

#ifdef WIN32

	// Add local servers. Will currently only work on Win32 with >= BETA3 
	// of the Win32 PostgreSQL installer.
	wxRegKey *pgKey = new wxRegKey(wxT("HKEY_LOCAL_MACHINE\\Software\\PostgreSQL\\Services"));

	if (pgKey->Exists())
	{

		wxString svcName, temp;
		long cookie = 0;
		long *tmpport = 0;
		bool flag = false;

		flag = pgKey->GetFirstKey(svcName, cookie);

		while (flag != false)
		{
            if (servicedServers.Index(svcName, false) < 0)
            {
			    key.Printf(wxT("HKEY_LOCAL_MACHINE\\Software\\PostgreSQL\\Services\\%s"), svcName);
			    wxRegKey *svcKey = new wxRegKey(key);

                servername = wxT("localhost");
                database = wxEmptyString;
			    svcKey->QueryValue(wxT("Display Name"), description);
			    svcKey->QueryValue(wxT("Database Superuser"), username);
                svcKey->QueryValue(wxT("Port"), &port);

			    // Add the Server node
			    server = new pgServer(servername, description, database, username, port, false, 0);
			    server->iSetDiscovered(true);
			    server->iSetServiceID(svcName);
			    browser->AppendItem(servers, server->GetFullName(), PGICON_SERVERBAD, -1, server);
            }
			// Get the next one...
			flag = pgKey->GetNextKey(svcName, cookie);
		}
	}
#endif //WIN32

    // Reset the Servers node text
    wxString label;
    label.Printf(_("Servers (%d)"), browser->GetChildrenCount(servers, false));
    browser->SetItemText(servers, label);

}


void frmMain::SetButtons(pgObject *obj)
{
    bool refresh=false,
         create=false,
         drop=false,
         dropCascaded=false,
         properties=false,
         set=false,
         setissubscribed=false,
         cluster=false;

    if (obj)
    {
        pgConn *conn=obj->GetConnection();
        pgServer *server=obj->GetServer();
        refresh = true;
        create = obj->CanCreate();
        drop = obj->CanDrop();
        dropCascaded = obj->CanDropCascaded();
        properties = obj->CanEdit();

        switch (obj->GetType())
        {
            /*
            case SL_CLUSTER:
                cluster=true;
                break;
                */
            case SL_SET:
                set=true;
                if (((slSet*)obj)->GetSubscriptionCount() > 0)
                    setissubscribed = true;
                break;
        }
    }

    toolBar->EnableTool(MNU_REFRESH, refresh);
    toolBar->EnableTool(MNU_CREATE, create);
    toolBar->EnableTool(MNU_DROP, drop);
    toolBar->EnableTool(MNU_PROPERTIES, properties);

	// Handle the menus associated with the buttons
	editMenu->Enable(MNU_CREATE, create);
	editMenu->Enable(MNU_DROP, drop);
	editMenu->Enable(MNU_DROPCASCADED, dropCascaded);
	editMenu->Enable(MNU_PROPERTIES, properties);
	//toolsMenu->Enable(MNU_INDEXCHECK, false);


    toolsMenu->Enable(MNU_SLONY_SUBMENU, cluster || set);
    slonyMenu->Enable(MNU_SLONY_RESTART, cluster);
    slonyMenu->Enable(MNU_SLONY_UPGRADE, cluster);
    slonyMenu->Enable(MNU_SLONY_FAILOVER, cluster);
    slonyMenu->Enable(MNU_SLONY_MOVESET, setissubscribed);
    slonyMenu->Enable(MNU_SLONY_MERGESET, set);

    viewMenu->Enable(MNU_REFRESH, refresh);
}




void frmMain::StartMsg(const wxString& msg)
{
   if (msgLevel++)
        return;

    timermsg.Printf(wxT("%s..."), msg.c_str());
    wxBeginBusyCursor();
    stopwatch.Start(0);
    wxLogStatus(timermsg);
    statusBar->SetStatusText(timermsg, 1);
    statusBar->SetStatusText(wxT(""), 2);
}


void frmMain::EndMsg(bool done)
{
    msgLevel--;

    if (!msgLevel)
    {
        // Get the execution time & display it
        float timeval = stopwatch.Time();
        wxString time;
        time.Printf(_("%.2f secs"), (timeval/1000));
        statusBar->SetStatusText(time, 2);

        // Display the 'Done' message
        if (done)
            statusBar->SetStatusText(timermsg + _(" Done."), 1);
        else
            statusBar->SetStatusText(timermsg + _(" Failed."), 1);

        wxLogStatus(wxT("%s (%s)"), timermsg.c_str(), time.c_str());
        wxEndBusyCursor();
    }
}


void frmMain::SetStatusText(const wxString &msg)
{
    statusBar->SetStatusText(msg, 1);
    statusBar->SetStatusText(wxEmptyString, 2);
}
