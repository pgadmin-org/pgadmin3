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
//       functions are in events.cpp.
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
#include "frmUpdate.h"
#include "dlgServer.h"
#include "dlgDatabase.h"
#include "dlgTable.h"
#include "dlgServer.h"
#include "dlgRepCluster.h"
#include "dlgRepSet.h"


enum
{
    NBP_PROPERTIES=0,
    NBP_STATISTICS,
    NBP_DEPENDSON,
    NBP_REFERENCEDBY
};


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

    appearanceFactory->SetIcons(this);

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

    menuFactories = new menuFactoryList();



    fileMenu->Append(MNU_SAVEDEFINITION, _("&Save definition..."),_("Save the SQL definition of the selected object."));
    fileMenu->AppendSeparator();
    new addServerFactory(menuFactories, fileMenu, toolBar);

    actionFactory *refFact=new refreshFactory(menuFactories, viewMenu, toolBar);
    new countRowsFactory(menuFactories, viewMenu, 0);

    new separatorFactory(menuFactories);

    viewMenu->AppendSeparator();
    viewMenu->Append(MNU_SYSTEMOBJECTS, _("&System objects"),     _("Show or hide system objects."), wxITEM_CHECK);
    toolBar->AddSeparator();

    new passwordFactory(menuFactories, fileMenu, 0);
    fileMenu->AppendSeparator();
    optionsFactory *optFact=new optionsFactory(menuFactories, fileMenu, 0);
    fileMenu->AppendSeparator();
    new mainConfigFileFactory(menuFactories, fileMenu, 0);
    new hbaConfigFileFactory(menuFactories, fileMenu, 0);

    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"),                _("Quit this program."));

    slonyMenu=new wxMenu();
    new slonyRestartFactory(menuFactories, slonyMenu, 0);
    new slonyUpgradeFactory(menuFactories, slonyMenu, 0);
    new slonyFailoverFactory(menuFactories, slonyMenu, 0);
    new slonyMergeSetFactory(menuFactories, slonyMenu, 0);
    new slonyMoveSetFactory(menuFactories, slonyMenu, 0);
    toolsMenu->Append(MNU_SLONY_SUBMENU, _("Replication"), slonyMenu);

    propFactory = new propertyFactory(menuFactories, 0, toolBar);
    new separatorFactory(menuFactories);

    newMenuFactory = new dummyActionFactory(menuFactories);     // placeholder where "New objects" submenu will be inserted
    editMenu->Append(newMenuFactory->GetId(), _("New &Object"), newMenu,    _("Create a new object."));
    editMenu->AppendSeparator();

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
    new editGridFactory(menuFactories, toolsMenu, toolBar);
    new editGridFilteredFactory(menuFactories, toolsMenu, toolBar);
    new maintenanceFactory(menuFactories, toolsMenu, toolBar);

    new backupFactory(menuFactories, toolsMenu, 0);
    new restoreFactory(menuFactories, toolsMenu, 0);

    new grantWizardFactory(menuFactories, toolsMenu, 0);
    new mainConfigFactory(menuFactories, cfgMenu, 0);
    new hbaConfigFactory(menuFactories, cfgMenu, 0);
    toolsMenu->Append(MNU_CONFIGSUBMENU, _("Server configuration"), cfgMenu);
    toolsMenu->AppendSeparator();

    new separatorFactory(menuFactories);

    new propertyFactory(menuFactories, editMenu, 0);
    new serverStatusFactory(menuFactories, toolsMenu, 0);

    toolBar->AddSeparator();

    new hintFactory(menuFactories, helpMenu, toolBar, true);
    actionFactory *helpFact = new pgsqlHelpFactory(menuFactories, helpMenu, toolBar, true);
    new contentsFactory(menuFactories, helpMenu, 0);
    new faqFactory(menuFactories, helpMenu, 0);

    new tipOfDayFactory(menuFactories, helpMenu, 0);
    helpMenu->AppendSeparator();
    new onlineUpdateFactory(menuFactories, helpMenu, 0);
    new bugReportFactory(menuFactories, helpMenu, 0);
    actionFactory *abFact=new aboutFactory(menuFactories, helpMenu, 0);
    
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
    
    // Setup the vertical splitter & treeview
    vertical = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    horizontal = new wxSplitterWindow(vertical, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    browser = new ctlTree(vertical, CTL_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
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

    properties->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    statistics->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    dependsOn->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    referencedBy->SetImageList(imageList, wxIMAGE_LIST_SMALL);

        // Add the property view columns
    properties->AddColumn(_("Properties"), 500);
    properties->InsertItem(0, _("No properties are available for the current selection"), PGICON_PROPERTY);

    wxColour background;
    background = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    statistics->SetBackgroundColour(background);
    dependsOn->SetBackgroundColour(background);
    referencedBy->SetBackgroundColour(background);
    sqlPane->SetBackgroundColour(background);

    pgaFactory::RegisterMenu(this, wxCommandEventHandler(frmMain::OnNew));
    menuFactories->RegisterMenu(this, wxCommandEventHandler(frmMain::OnAction));
    menuFactories->CheckMenu(0, menuBar, toolBar);

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


bool frmMain::CheckAlive()
{
    bool userInformed = false;
    bool closeIt = false;

    wxCookieType cookie;
    wxTreeItemId serverItem=browser->GetFirstChild(serversObj->GetId(), cookie);
    while (serverItem)
    {
        pgServer *server=(pgServer*)browser->GetItemData(serverItem);

        if (server && server->IsCreatedBy(serverFactory) && server->connection())
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
    wxTreeItemId item = browser->GetFirstChild(serversObj->GetId(), cookie);
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data->IsCreatedBy(serverFactory))
        {
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
        item = browser->GetNextChild(serversObj->GetId(), cookie);
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


