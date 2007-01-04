//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmMain.cpp - The main form
//
// Note: Due to the size of frmMain, event handler, browser and statistics 
//       functions are in events.cpp.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#ifndef WIN32
#include <unistd.h>
#endif

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/imaglist.h>
#include <wx/busyinfo.h>

// wxAUI
#include <wx/aui/aui.h>

// App headers
#include "utils/misc.h"
#include "frm/menu.h"
#include "utils/pgfeatures.h"
#include "frm/frmMain.h"
#include "ctl/ctlSQLBox.h"
#include "db/pgConn.h"
#include "db/pgSet.h"
#include "agent/pgaJob.h"
#include "schema/pgDatabase.h"
#include "schema/pgServer.h"
#include "schema/pgObject.h"
#include "schema/pgCollection.h"
#include "frm/frmOptions.h"
#include "frm/frmHelp.h"
#include "frm/frmAbout.h"
#include "frm/frmHint.h"
#include "frm/frmGrantWizard.h"
#include "frm/frmMainConfig.h"
#include "frm/frmHbaConfig.h"
#include "frm/frmPgpassConfig.h"
#include "frm/frmBackup.h"
#include "frm/frmRestore.h"
#include "frm/frmReport.h"
#include "frm/frmMaintenance.h"
#include "frm/frmStatus.h"
#include "frm/frmPassword.h"
#include "frm/frmQuery.h"
#include "frm/frmEditGrid.h"
#include "dlg/dlgServer.h"
#include "dlg/dlgDatabase.h"
#include "dlg/dlgTable.h"
#include "dlg/dlgServer.h"
#include "slony/dlgRepCluster.h"
#include "slony/dlgRepSet.h"


enum
{
    NBP_PROPERTIES=0,
    NBP_STATISTICS,
    NBP_DEPENDENCIES,
    NBP_DEPENDENTS
};


#if wxDIALOG_UNIT_COMPATIBILITY
#error wxWindows must be compiled with wxDIALOG_UNIT_COMPATIBILITY=0!
#endif

frmMain::frmMain(const wxString& title)
: pgFrame((wxFrame *)NULL, title)
{
    msgLevel=0;

    dlgName = wxT("frmMain");
    SetMinSize(wxSize(400,300));
    RestorePosition(50, 50, 750, 550, 600, 450);

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

    appearanceFactory->SetIcons(this);

    // notify wxAUI which frame to use
    manager.SetManagedWindow(this);
    manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG);

    SetMinSize(wxSize(600, 450)); 

    // wxGTK needs this deferred
    pgaFactory::RealizeImages();

	CreateMenus();
    
    // Setup the object browser
    browser = new ctlTree(this, CTL_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
    browser->SetImageList(imageList);

    // Setup the listview
    listViews = new wxNotebook(this, CTL_NOTEBOOK, wxDefaultPosition, wxDefaultSize);
    properties = new ctlListView(listViews, CTL_PROPVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    statistics = new ctlListView(listViews, CTL_STATVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    dependencies = new ctlListView(listViews, CTL_DEPVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    dependents = new ctlListView(listViews, CTL_REFVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);

    listViews->AddPage(properties, _("Properties"));        // NBP_PROPERTIES
    listViews->AddPage(statistics, _("Statistics"));        // NBP_STATISTICS
    listViews->AddPage(dependencies, _("Dependencies"));    // NBP_DEPENDENCIES
    listViews->AddPage(dependents, _("Dependents"));        // NBP_DEPENDENTS

    properties->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    statistics->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    dependencies->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    dependents->SetImageList(imageList, wxIMAGE_LIST_SMALL);

    properties->AddColumn(_("Properties"), 500);
    properties->InsertItem(0, _("No properties are available for the current selection"), PGICON_PROPERTY);

    wxColour background;
    background = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    statistics->SetBackgroundColour(background);
    dependencies->SetBackgroundColour(background);
    dependents->SetBackgroundColour(background);

    // Setup the SQL pane
    sqlPane = new ctlSQLBox(this, CTL_SQLPANE, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
    sqlPane->SetBackgroundColour(background);

    // Setup menus
    pgaFactory::RegisterMenu(this, wxCommandEventHandler(frmMain::OnNew));
    menuFactories->RegisterMenu(this, wxCommandEventHandler(frmMain::OnAction));
    menuFactories->CheckMenu(0, menuBar, toolBar);

    // Kickstart wxAUI
    manager.AddPane(browser, wxAuiPaneInfo().Name(wxT("objectBrowser")).Caption(_("Object browser")).Left().MinSize(wxSize(100, 200)).BestSize(wxSize(200, 450)));
    manager.AddPane(listViews, wxAuiPaneInfo().Name(wxT("listViews")).Caption(_("Info pane")).Center().CaptionVisible(false).CloseButton(false).MinSize(wxSize(200, 100)).BestSize(wxSize(400, 200)));
    manager.AddPane(sqlPane, wxAuiPaneInfo().Name(wxT("sqlPane")).Caption(_("SQL pane")).Bottom().MinSize(wxSize(200, 100)).BestSize(wxSize(400, 200)));
    manager.AddPane(toolBar, wxAuiPaneInfo().Name(wxT("toolBar")).Caption(_("Tool bar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));

    // Now load the layout
    wxString perspective;
    settings->Read(wxT("frmMain/Perspective-") + VerFromRev(FRMMAIN_PERPSECTIVE_VER), &perspective, FRMMAIN_DEFAULT_PERSPECTIVE);
    manager.LoadPerspective(perspective, true);

    // and reset the captions for the current language
    manager.GetPane(wxT("objectBrowser")).Caption(_("Object browser"));
    manager.GetPane(wxT("listViews")).Caption(_("Info pane"));
    manager.GetPane(wxT("sqlPane")).Caption(_("SQL pane"));
    manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));

    // Sync the View menu options
    viewMenu->Check(MNU_SQLPANE, manager.GetPane(wxT("sqlPane")).IsShown());
    viewMenu->Check(MNU_OBJECTBROWSER, manager.GetPane(wxT("objectBrowser")).IsShown());
    viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());

    // tell the manager to "commit" all the changes just made
    manager.Update();

    // Add the root node
    serversObj = new pgServerCollection(serverFactory.GetCollectionFactory());
    wxTreeItemId servers = browser->AddRoot(wxGetTranslation(serverFactory.GetCollectionFactory()->GetTypeName()),
        serversObj->GetIconId(), -1, serversObj);

    // Load servers
    RetrieveServers();
    browser->Expand(servers);
}


frmMain::~frmMain()
{
    StoreServers();

    settings->Write(wxT("frmMain/Perspective-") + VerFromRev(FRMMAIN_PERPSECTIVE_VER), manager.SavePerspective());
    manager.UnInit();

    // Clear the treeview
    browser->DeleteAllItems();

    if (treeContextMenu)
        delete treeContextMenu;
}



void frmMain::CreateMenus()
{
	// to add a new menu or context menu to the main window, i.e. define a possible
	// action on a pgObject, everything has to go into this method. Doing menu related
	// stuff elsewhere is plain wrong!
	// Create a proper actionFactory  (or contextActionFactory) for each of your new actions 
	// in the new frmXXX.cpp and register it here.

    fileMenu = new wxMenu();
    viewMenu = new wxMenu();
	editMenu = new wxMenu();
    newMenu=new wxMenu();
	toolsMenu = new wxMenu();
    slonyMenu=new wxMenu();
	scriptingMenu=new wxMenu();
	viewDataMenu = new wxMenu();
    reportMenu=new wxMenu();
    wxMenu *cfgMenu=new wxMenu();
    helpMenu = new wxMenu();
    newContextMenu = new wxMenu();


    toolBar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER );
    toolBar->SetToolBitmapSize(wxSize(32, 32));
    menuFactories = new menuFactoryList();

	//--------------------------
    fileMenu->Append(MNU_SAVEDEFINITION, _("&Save Definition..."),_("Save the SQL definition of the selected object."));
    fileMenu->AppendSeparator();
    new addServerFactory(menuFactories, fileMenu, toolBar);

    viewMenu->Append(MNU_OBJECTBROWSER, _("&Object browser"),     _("Show or hide the object browser."), wxITEM_CHECK);
    viewMenu->Append(MNU_SQLPANE, _("&SQL pane"),     _("Show or hide the SQL pane."), wxITEM_CHECK);
    viewMenu->Append(MNU_TOOLBAR, _("&Tool bar"),     _("Show or hide the tool bar."), wxITEM_CHECK);
    viewMenu->AppendSeparator();
    viewMenu->Append(MNU_DEFAULTVIEW, _("&Default view"),     _("Restore the default view."));
    viewMenu->AppendSeparator();
    actionFactory *refFact=new refreshFactory(menuFactories, viewMenu, toolBar);
    new countRowsFactory(menuFactories, viewMenu, 0);

   
	//--------------------------
    new separatorFactory(menuFactories);

    viewMenu->AppendSeparator();
    viewMenu->Append(MNU_SYSTEMOBJECTS, _("&System Objects"),     _("Show or hide system objects."), wxITEM_CHECK);
    toolBar->AddSeparator();

    new passwordFactory(menuFactories, fileMenu, 0);
    fileMenu->AppendSeparator();
    optionsFactory *optFact=new optionsFactory(menuFactories, fileMenu, 0);
    fileMenu->AppendSeparator();
    new mainConfigFileFactory(menuFactories, fileMenu, 0);
    new hbaConfigFileFactory(menuFactories, fileMenu, 0);
	new pgpassConfigFileFactory(menuFactories, fileMenu, 0);

    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"),                _("Quit this program."));

    new slonyRestartFactory(menuFactories, slonyMenu, 0);
    new slonyUpgradeFactory(menuFactories, slonyMenu, 0);
    new slonyFailoverFactory(menuFactories, slonyMenu, 0);
    new slonyLockSetFactory(menuFactories, slonyMenu, 0);
    new slonyUnlockSetFactory(menuFactories, slonyMenu, 0);
    new slonyMergeSetFactory(menuFactories, slonyMenu, 0);
    new slonyMoveSetFactory(menuFactories, slonyMenu, 0);
    toolsMenu->Append(MNU_SLONY_SUBMENU, _("Replication"), slonyMenu);

    propFactory = new propertyFactory(menuFactories, 0, toolBar);
    new separatorFactory(menuFactories);


	// -------------------------

    editMenu->Append(MNU_COPY, _("&Copy\tCtrl-C"),                _("Copy selected text to clipboard"));
	editMenu->AppendSeparator();

	// -------------------------

	//--------------------------
    
    newMenuFactory = new submenuFactory(menuFactories);     // placeholder where "New objects" submenu will be inserted
    editMenu->Append(newMenuFactory->GetId(), _("New &Object"), newMenu,    _("Create a new object."));
    editMenu->AppendSeparator();


    //--------------------------

    new connectServerFactory(menuFactories, toolsMenu, 0);
    new disconnectServerFactory(menuFactories, toolsMenu, 0);

    new startServiceFactory(menuFactories, toolsMenu, 0);
    new stopServiceFactory(menuFactories, toolsMenu, 0);

    new createFactory(menuFactories, editMenu, toolBar);
    new dropFactory(menuFactories, editMenu, toolBar);
    new dropCascadedFactory(menuFactories, editMenu, 0);

    new separatorFactory(menuFactories);

    toolBar->AddSeparator();
    toolsMenu->AppendSeparator();
    
	new queryToolFactory(menuFactories, toolsMenu, toolBar);
    scriptingMenuFactory = new submenuFactory(menuFactories);    // placeholder where "Query Template" submenu will be inserted
	toolsMenu->Append(scriptingMenuFactory->GetId(), _("Scripts"), scriptingMenu, _("Start Query Tool with scripted query."));
	new queryToolSqlFactory(menuFactories, scriptingMenu, 0);
	new queryToolSelectFactory(menuFactories, scriptingMenu, 0);
	new queryToolInsertFactory(menuFactories, scriptingMenu, 0);
	new queryToolUpdateFactory(menuFactories, scriptingMenu, 0);

    viewdataMenuFactory = new submenuFactory(menuFactories);     // placeholder where "View data" submenu will be inserted
	toolsMenu->Append(viewdataMenuFactory->GetId(), _("View &Data"), viewDataMenu, _("View data."));

    reportMenuFactory = new submenuFactory(menuFactories);     // placeholder where "Reports" submenu will be inserted
    toolsMenu->Append(reportMenuFactory->GetId(), _("&Reports"), reportMenu,    _("Create reports about the selected item."));
    new reportObjectPropertiesFactory(menuFactories, reportMenu, 0);
	new reportObjectDdlFactory(menuFactories, reportMenu, 0);
	new reportObjectDataDictionaryFactory(menuFactories, reportMenu, 0);
    new reportObjectStatisticsFactory(menuFactories, reportMenu, 0);
    new reportObjectDependenciesFactory(menuFactories, reportMenu, 0);
    new reportObjectDependentsFactory(menuFactories, reportMenu, 0);
    new reportObjectListFactory(menuFactories, reportMenu, 0);


    toolsMenu->AppendSeparator();

	new editGridLimitedFactory(menuFactories, viewDataMenu, toolBar, 100);
    new editGridFactory(menuFactories, viewDataMenu, toolBar);
    new editGridFilteredFactory(menuFactories, viewDataMenu, toolBar);

	new maintenanceFactory(menuFactories, toolsMenu, toolBar);

    new backupFactory(menuFactories, toolsMenu, 0);
    new restoreFactory(menuFactories, toolsMenu, 0);

    new grantWizardFactory(menuFactories, toolsMenu, 0);
    new mainConfigFactory(menuFactories, cfgMenu, 0);
    new hbaConfigFactory(menuFactories, cfgMenu, 0);
    toolsMenu->Append(MNU_CONFIGSUBMENU, _("Server Configuration"), cfgMenu);
    toolsMenu->AppendSeparator();

    new runNowFactory(menuFactories, toolsMenu, 0);
    toolsMenu->AppendSeparator();

    new separatorFactory(menuFactories);

    new propertyFactory(menuFactories, editMenu, 0);
    new serverStatusFactory(menuFactories, toolsMenu, 0);


	//--------------------------
    toolBar->AddSeparator();

    actionFactory *helpFact = new pgsqlHelpFactory(menuFactories, helpMenu, toolBar, true);
    new contentsFactory(menuFactories, helpMenu, 0);
    new hintFactory(menuFactories, helpMenu, toolBar, true);
    new faqFactory(menuFactories, helpMenu, 0);

    new tipOfDayFactory(menuFactories, helpMenu, 0);
    helpMenu->AppendSeparator();
    // new onlineUpdateFactory(menuFactories, helpMenu, 0);
    new bugReportFactory(menuFactories, helpMenu, 0);
    actionFactory *abFact=new aboutFactory(menuFactories, helpMenu, 0);
    
#ifdef __WXMAC__
    wxApp::s_macPreferencesMenuItemId = optFact->GetId();
    wxApp::s_macExitMenuItemId = MNU_EXIT;
    wxApp::s_macAboutMenuItemId = abFact->GetId();
#else
	(void)optFact;
	(void)abFact;
#endif 


    menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _("&File"));
    menuBar->Append(editMenu, _("&Edit"));
    menuBar->Append(viewMenu, _("&View"));
    menuBar->Append(toolsMenu, _("&Tools"));
    menuBar->Append(helpMenu, _("&Help"));
    SetMenuBar(menuBar);

    treeContextMenu = 0;

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
    entries[0].Set(wxACCEL_NORMAL, WXK_F5, refFact->GetId());
    entries[1].Set(wxACCEL_NORMAL, WXK_DELETE, MNU_DELETE);
    entries[2].Set(wxACCEL_NORMAL, WXK_F1, helpFact->GetId());
    entries[3].Set(wxACCEL_SHIFT, WXK_F10, MNU_CONTEXTMENU);
    wxAcceleratorTable accel(4, entries);

    SetAcceleratorTable(accel);

    
    // Display the bar and configure buttons. 
    toolBar->Realize();
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
        case NBP_DEPENDENCIES:
        {
            dependencies->Freeze();
            dependencies->DeleteAllItems();
            data->ShowDependencies(this, dependencies);
            dependencies->Thaw();
            break;
        }
        case NBP_DEPENDENTS:
        {
            dependents->Freeze();
            dependents->DeleteAllItems();
            data->ShowDependents(this, dependents);
            dependents->Thaw();
            break;
        }
        default:
            break;
    }
}


void frmMain::OnPageChange(wxNotebookEvent& event)
{
    pgObject *data = browser->GetObject(browser->GetSelection());

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

ctlListView *frmMain::GetStatisticsCtl()
{
    return statistics;
}

ctlListView *frmMain::GetDependencies()
{
    if (listViews->GetSelection() == NBP_DEPENDENCIES)
        return dependencies;
    return 0;
}

ctlListView *frmMain::GetDependenciesCtl()
{
    return dependencies;
}

ctlListView *frmMain::GetReferencedBy()
{
    if (listViews->GetSelection() == NBP_DEPENDENTS)
        return dependents;
    return 0;
}

ctlListView *frmMain::GetReferencedByCtl()
{
    return dependents;
}


bool frmMain::CheckAlive()
{
    bool userInformed = false;
    bool closeIt = false;

    wxCookieType cookie;
    wxTreeItemId serverItem=browser->GetFirstChild(serversObj->GetId(), cookie);
    while (serverItem)
    {
        pgServer *server=(pgServer*)browser->GetObject(serverItem);

        if (server && server->IsCreatedBy(serverFactory) && server->connection())
        {
            if (server->connection()->IsAlive())
            {
                wxCookieType cookie2;
                wxTreeItemId item = browser->GetFirstChild(serverItem, cookie2);
                while (item)
                {
                    pgObject *obj=browser->GetObject(item);
                    if (obj && obj->IsCreatedBy(databaseFactory.GetCollectionFactory()))
                    {
                        wxCookieType cookie3;
                        item = browser->GetFirstChild(obj->GetId(), cookie3);
                        while (item)
                        {
                            pgDatabase *db=(pgDatabase*)browser->GetObject(item);
                            if (db && db->IsCreatedBy(databaseFactory))
                            {
                                pgConn *conn=db->GetConnection();
                                if (conn)
                                {
                                    if (!conn->IsAlive() && (conn->GetStatus() == PGCONN_BROKEN || conn->GetStatus() == PGCONN_BAD))
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
                if (server->connection()->GetStatus() == PGCONN_BROKEN || server->connection()->GetStatus() == PGCONN_BAD)
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
                        server->Disconnect(this);
                        browser->SelectItem(serverItem);
                        execSelChange(serverItem, true);
                        browser->DeleteChildren(serverItem);
                    }
                }
            }
        }

        serverItem = browser->GetNextChild(serversObj->GetId(), cookie);
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
        data = browser->GetObject(item);
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
        data = browser->GetObject(item);
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
        data = browser->GetObject(item);
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
        data = browser->GetObject(item);
        if (data->GetMetaType() == PGM_SCHEMA && data->GetName() == lastSchema)
            break;
        // Get the next item
        item = browser->GetNextChild(lastItem, cookie);
    }

    return (item ? item : lastItem);
}


int frmMain::ReconnectServer(pgServer *server, bool restore)
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
	        if (restore && server->GetRestore())
			    StartMsg(_("Restoring previous environment"));
			else
                StartMsg(_("Establishing connection"));

			wxLogInfo(wxT("pgServer object initialised as required."));

			server->ShowTreeDetail(browser);

			if (restore && server->GetRestore())
			{
				browser->Freeze();
				item=RestoreEnvironment(server);
				browser->Thaw();
			}

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
			{
				if (restore && server->GetRestore())
					EndMsg(false);
				else
					EndMsg(true);
			}
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
    pgServer *server;
    int numServers = 0;

	// Get the hostname for later...
	char buf[255];
	gethostname(buf, 255); 
    wxString hostname = wxString(buf, wxConvUTF8);

    // Write the individual servers
    // Iterate through all the child nodes of the Servers node
    treeObjectIterator servers(browser, serversObj);

    while ((server = (pgServer*)servers.GetNextObject()) != 0)
    {
        if (server->IsCreatedBy(serverFactory))
        {
            wxString key;
			++numServers;

            key.Printf(wxT("Servers/%d/"), numServers);
		    settings->Write(key + wxT("Server"), server->GetName());
	        settings->Write(key + wxT("Description"), server->GetDescription());
	        settings->Write(key + wxT("ServiceID"), server->GetServiceID());
		    settings->Write(key + wxT("Port"), server->GetPort());
	        settings->Write(key + wxT("StorePwd"), server->GetStorePwd());
		    settings->Write(key + wxT("Restore"), server->GetRestore());
	        settings->Write(key + wxT("Database"), server->GetDatabaseName());
	        settings->Write(key + wxT("Username"), server->GetUsername());
			settings->Write(key + wxT("LastDatabase"), server->GetLastDatabase());
			settings->Write(key + wxT("LastSchema"), server->GetLastSchema());
            settings->Write(key + wxT("DbRestriction"), server->GetDbRestriction());
			settings->Write(key + wxT("SSL"), server->GetSSL());

            pgCollection *coll=browser->FindCollection(databaseFactory, server->GetId());
            if (coll)
            {
                treeObjectIterator dbs(browser, coll);
                pgDatabase *db;

                while ((db=(pgDatabase*)dbs.GetNextObject()) != 0)
                    settings->Write(key + wxT("Databases/") + db->GetName() + wxT("/SchemaRestriction"), db->GetSchemaRestriction());
            }
		}
    }

    // Write the server count
    settings->Write(wxT("Servers/Count"), numServers);
    wxLogInfo(wxT("Stored %d servers."), numServers);
}


void frmMain::RetrieveServers()
{
    // Retrieve previously stored servers
    wxLogInfo(wxT("Reloading servers..."));

    serverFactory.CreateObjects(serversObj, browser);
    // Reset the Servers node text
    wxString label;
    label.Printf(_("Servers (%d)"), browser->GetChildrenCount(serversObj->GetId(), false));
    browser->SetItemText(serversObj->GetId(), label);
}

pgServer *frmMain::ConnectToServer(const wxString& servername, bool restore)
{
	for (int i = 0; ; i++)
	{
		pgObject *o = serversObj->FindChild(browser, i);
		if (!o)
			return NULL;
		if (o->IsCreatedBy(serverFactory))
		{
			pgServer *s = (pgServer *)o;
			if (s->GetDescription() == servername)
			{
				ReconnectServer(s, restore);
				return s;
			}
		}
	}
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


