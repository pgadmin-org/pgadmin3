//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include <wx/sysopt.h>
#include <wx/clipbrd.h>

// wxAUI
#include <wx/aui/aui.h>

// App headers
#include "utils/misc.h"
#include "frm/menu.h"
#include "utils/pgfeatures.h"
#include "debugger/debugger.h"
#include "frm/frmMain.h"
#include "ctl/ctlMenuToolbar.h"
#include "ctl/ctlSQLBox.h"
#include "db/pgConn.h"
#include "db/pgSet.h"
#include "agent/pgaJob.h"
#include "schema/pgDatabase.h"
#include "schema/pgServer.h"
#include "schema/pgObject.h"
#include "schema/pgCollection.h"
#include "frm/frmOptions.h"
#include "frm/frmAbout.h"
#include "frm/frmHint.h"
#include "frm/frmGrantWizard.h"
#include "frm/frmMainConfig.h"
#include "frm/frmHbaConfig.h"
#include "frm/frmPgpassConfig.h"
#include "frm/frmBackup.h"
#include "frm/frmBackupGlobals.h"
#include "frm/frmBackupServer.h"
#include "frm/frmRestore.h"
#include "frm/frmReport.h"
#include "frm/frmMaintenance.h"
#include "frm/frmStatus.h"
#include "frm/frmPassword.h"
#include "frm/frmQuery.h"
#include "frm/frmEditGrid.h"
#include "dlg/dlgServer.h"
#include "dlg/dlgDatabase.h"
#include "schema/pgTable.h"
#include "schema/pgIndex.h"
#include "schema/pgTrigger.h"
#include "schema/pgRole.h"
#include "schema/pgRule.h"
#include "schema/pgServer.h"
#include "slony/slCluster.h"
#include "slony/slSet.h"


#if wxDIALOG_UNIT_COMPATIBILITY
#error wxWindows must be compiled with wxDIALOG_UNIT_COMPATIBILITY=0!
#endif

frmMain::frmMain(const wxString &title)
	: pgFrame((wxFrame *)NULL, title)
{
	msgLevel = 0;
	lastPluginUtility = NULL;
	pluginUtilityCount = 0;

	dlgName = wxT("frmMain");
	SetMinSize(wxSize(400, 300));
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
	denyCollapseItem = wxTreeItemId();
	currentObject = 0;

	appearanceFactory->SetIcons(this);

	// notify wxAUI which frame to use
	manager.SetManagedWindow(this);
	manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE);

	SetMinSize(wxSize(600, 450));

	// wxGTK needs this deferred
	pgaFactory::RealizeImages();

	CreateMenus();

	// Setup the object browser
	browser = new ctlTree(this, CTL_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
	browser->SetImageList(imageList);

	// Setup the listview
	listViews = new wxNotebook(this, CTL_NOTEBOOK, wxDefaultPosition, wxDefaultSize);

	// Switch to the generic list control. Native doesn't play well with
	// multi-row select on Mac.
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
#endif

	properties = new ctlListView(listViews, CTL_PROPVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	statistics = new ctlListView(listViews, CTL_STATVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	dependencies = new ctlListView(listViews, CTL_DEPVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	dependents = new ctlListView(listViews, CTL_REFVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);

	// Switch back to the native list control.
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), false);
#endif

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
	settings->Read(wxT("frmMain/Perspective-") + wxString(FRMMAIN_PERSPECTIVE_VER), &perspective, FRMMAIN_DEFAULT_PERSPECTIVE);
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
	wxTreeItemId root = browser->AddRoot(_("Server Groups"), serversObj->GetIconId(), -1, serversObj);

	// Load servers
	RetrieveServers();

	browser->Expand(root);
	browser->SortChildren(root);
}


frmMain::~frmMain()
{
	StoreServers();

	settings->Write(wxT("frmMain/Perspective-") + wxString(FRMMAIN_PERSPECTIVE_VER), manager.SavePerspective());
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
	pluginsMenu = new wxMenu();
	viewMenu = new wxMenu();
	editMenu = new wxMenu();
	newMenu = new wxMenu();
	toolsMenu = new wxMenu();
	slonyMenu = new wxMenu();
	scriptingMenu = new wxMenu();
	viewDataMenu = new wxMenu();
	debuggingMenu = new wxMenu();
	reportMenu = new wxMenu();
	wxMenu *cfgMenu = new wxMenu();
	helpMenu = new wxMenu();
	newContextMenu = new wxMenu();

	toolBar = new ctlMenuToolbar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER );
	toolBar->SetToolBitmapSize(wxSize(32, 32));
	menuFactories = new menuFactoryList();

	// Load plugins - must do this after creating the menus and the factories
	LoadPluginUtilities();

	//--------------------------
	fileMenu->Append(MNU_SAVEDEFINITION, _("&Save Definition..."), _("Save the SQL definition of the selected object."));
	fileMenu->AppendSeparator();
	new addServerFactory(menuFactories, fileMenu, toolBar);

	viewMenu->Append(MNU_OBJECTBROWSER, _("&Object browser\tCtrl-Alt-O"),     _("Show or hide the object browser."), wxITEM_CHECK);
	viewMenu->Append(MNU_SQLPANE, _("&SQL pane\tCtrl-Alt-S"),     _("Show or hide the SQL pane."), wxITEM_CHECK);
	viewMenu->Append(MNU_TOOLBAR, _("&Tool bar\tCtrl-Alt-T"),     _("Show or hide the tool bar."), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MNU_DEFAULTVIEW, _("&Default view\tCtrl-Alt-V"),     _("Restore the default view."));
	viewMenu->AppendSeparator();
	actionFactory *refFact = new refreshFactory(menuFactories, viewMenu, toolBar);
	new countRowsFactory(menuFactories, viewMenu, 0);
	new executePgstattupleFactory(menuFactories, viewMenu, 0);
	new executePgstatindexFactory(menuFactories, viewMenu, 0);
	new enabledisableRuleFactory(menuFactories, toolsMenu, 0);
	new enabledisableTriggerFactory(menuFactories, toolsMenu, 0);
	new disableAllTriggersFactory(menuFactories, toolsMenu, 0);
	new enableAllTriggersFactory(menuFactories, toolsMenu, 0);
	toolsMenu->AppendSeparator();

	//--------------------------
	new separatorFactory(menuFactories);

	toolBar->AddSeparator();

	new passwordFactory(menuFactories, fileMenu, 0);
	fileMenu->AppendSeparator();
	optionsFactory *optFact = new optionsFactory(menuFactories, fileMenu, 0);
	fileMenu->AppendSeparator();
	new mainConfigFileFactory(menuFactories, fileMenu, 0);
	new hbaConfigFileFactory(menuFactories, fileMenu, 0);
	new pgpassConfigFileFactory(menuFactories, fileMenu, 0);

	fileMenu->AppendSeparator();
	fileMenu->Append(MNU_EXIT, _("E&xit\tCtrl-Q"),                _("Quit this program."));

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
	new reloadconfServiceFactory(menuFactories, toolsMenu, 0);

	new createFactory(menuFactories, editMenu, toolBar);
	new dropFactory(menuFactories, editMenu, toolBar);
	new dropCascadedFactory(menuFactories, editMenu, 0);
	new truncateFactory(menuFactories, editMenu, 0);
	new truncateCascadedFactory(menuFactories, editMenu, 0);
	new reassignDropOwnedFactory(menuFactories, editMenu, 0);
	editMenu->AppendSeparator();

	new separatorFactory(menuFactories);

	toolBar->AddSeparator();
	toolsMenu->AppendSeparator();

	debuggingMenuFactory = new submenuFactory(menuFactories);     // placeholder where "Debugging" submenu will be inserted
	toolsMenu->Append(debuggingMenuFactory->GetId(), _("&Debugging"), debuggingMenu,    _("Debugging options for the selected item."));
	new debuggerFactory(menuFactories, debuggingMenu, 0);
	new breakpointFactory(menuFactories, debuggingMenu, 0);

	new queryToolFactory(menuFactories, toolsMenu, toolBar);
	scriptingMenuFactory = new submenuFactory(menuFactories);    // placeholder where "Query Template" submenu will be inserted
	toolsMenu->Append(scriptingMenuFactory->GetId(), _("Scripts"), scriptingMenu, _("Start Query Tool with scripted query."));
	new queryToolSqlFactory(menuFactories, scriptingMenu, 0);
	new queryToolSelectFactory(menuFactories, scriptingMenu, 0);
	new queryToolExecFactory(menuFactories, scriptingMenu, 0);
	new queryToolInsertFactory(menuFactories, scriptingMenu, 0);
	new queryToolUpdateFactory(menuFactories, scriptingMenu, 0);
	new queryToolDeleteFactory(menuFactories, scriptingMenu, 0);

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
	new backupGlobalsFactory(menuFactories, toolsMenu, 0);
	new backupServerFactory(menuFactories, toolsMenu, 0);
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

	// Add the plugin toolbar button/menu
	new pluginButtonMenuFactory(menuFactories, pluginsMenu, toolBar, pluginUtilityCount);

	//--------------------------
	toolBar->AddSeparator();

	actionFactory *helpFact = new contentsFactory(menuFactories, helpMenu, 0);
	new hintFactory(menuFactories, helpMenu, toolBar, true);
	new faqFactory(menuFactories, helpMenu, 0);
	new bugReportFactory(menuFactories, helpMenu, 0);

	helpMenu->AppendSeparator();

	new pgsqlHelpFactory(menuFactories, helpMenu, toolBar, true);
	if (!appearanceFactory->GetHideEnterprisedbHelp())
		new edbHelpFactory(menuFactories, helpMenu, toolBar, true);
	if (!appearanceFactory->GetHideGreenplumHelp())
		new greenplumHelpFactory(menuFactories, helpMenu, toolBar, true);
	new slonyHelpFactory(menuFactories, helpMenu, toolBar, true);

	// Don't include this seperator on Mac, because the only option
	// under it will be moved to the application menu.
#ifndef __WXMAC__
	helpMenu->AppendSeparator();
#endif

	actionFactory *abFact = new aboutFactory(menuFactories, helpMenu, 0);

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
	// Changing the caption of the plugins menu also needs a similar change below
	menuBar->Append(pluginsMenu, _("&Plugins"));
	menuBar->Append(viewMenu, _("&View"));
	menuBar->Append(toolsMenu, _("&Tools"));
	menuBar->Append(helpMenu, _("&Help"));
	SetMenuBar(menuBar);

	// Disable the plugins menu if there aren't any.
	if (!pluginUtilityCount)
	{
		pluginsMenu->Append(MNU_DUMMY, _("No plugins installed"));
		pluginsMenu->Enable(MNU_DUMMY, false);
	}

	treeContextMenu = 0;

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

	wxTreeItemId currentItem = data->GetId();

	// Scan the child nodes and make a list of those that are expanded
	// This is not an exact science as node names may change etc.
	wxArrayString expandedNodes;
	GetExpandedChildNodes(currentItem, expandedNodes);

	browser->DeleteChildren(currentItem);

	// refresh information about the object
	data->SetDirty();

	pgObject *newData = data->Refresh(browser, currentItem);

	bool done = !data->GetConnection() || data->GetConnection()->GetStatus() == PGCONN_OK;

	if (newData != data)
	{
		wxLogInfo(wxT("Deleting %s %s for refresh"), data->GetTypeName().c_str(), data->GetQuotedFullIdentifier().c_str());

		if (data == currentObject)
			currentObject = newData;

		if (newData)
		{
			wxLogInfo(wxT("Replacing with new node %s %s for refresh"), newData->GetTypeName().c_str(), newData->GetQuotedFullIdentifier().c_str());

			newData->SetId(currentItem);    // not done automatically
			browser->SetItemData(currentItem, newData);

			// Update the node text if this is an object, as it may have been renamed
			if (!newData->IsCollection())
				browser->SetItemText(currentItem, newData->GetDisplayName());

			delete data;
		}
		else
		{
			wxLogInfo(wxT("No object to replace: vanished after refresh."));

			// If the connection is dead, just return here
			if (data->GetConnection()->GetStatus() != PGCONN_OK)
			{
				CheckAlive();
				browser->Thaw();
				return;
			}

			wxTreeItemId delItem = currentItem;
			currentItem = browser->GetItemParent(currentItem);
			browser->SelectItem(currentItem);
			browser->Delete(delItem);
		}
	}

	if (currentItem)
	{
		execSelChange(currentItem, currentItem == browser->GetSelection());

		// Attempt to expand any child nodes that were previously expanded
		ExpandChildNodes(currentItem, expandedNodes);
	}

	browser->Thaw();
	EndMsg(done);
}

void frmMain::OnCopy(wxCommandEvent &ev)
{
	for (unsigned int i = 0; i < manager.GetAllPanes().GetCount(); i++)
	{
		wxAuiPaneInfo &pane = manager.GetAllPanes()[i];
		if (pane.HasFlag(wxAuiPaneInfo::optionActive))
		{
			if (pane.name == wxT("sqlPane"))
			{
				sqlPane->Copy();
			}
			else
			{
				ctlListView *list;
				int row, col;
				wxString text;

				switch(listViews->GetSelection())
				{
					case NBP_PROPERTIES:
						list = properties;
						break;
					case NBP_STATISTICS:
						list = statistics;
						break;
					case NBP_DEPENDENCIES:
						list = dependencies;
						break;
					case NBP_DEPENDENTS:
						list = dependents;
						break;
					default:
						// This shouldn't happen.
						// If it does, it's no big deal, we just need to get out.
						return;
						break;
				}

				row = list->GetFirstSelected();

				while (row >= 0)
				{
					for (col = 0; col < list->GetColumnCount(); col++)
					{
						text.Append(list->GetText(row, col) + wxT("\t"));
					}
#ifdef __WXMSW__
					text.Append(wxT("\r\n"));
#else
					text.Append(wxT("\n"));
#endif
					row = list->GetNextSelected(row);
				}

				if (text.Length() > 0 && wxTheClipboard->Open())
				{
					wxTheClipboard->SetData(new wxTextDataObject(text));
					wxTheClipboard->Close();
				}
			}
		}
	}
}

void frmMain::GetExpandedChildNodes(wxTreeItemId node, wxArrayString &expandedNodes)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = browser->GetFirstChild(node, cookie);

	while (child.IsOk())
	{
		if (browser->IsExpanded(child))
		{
			GetExpandedChildNodes(child, expandedNodes);
			expandedNodes.Add(GetNodePath(child));
		}

		child = browser->GetNextChild(node, cookie);
	}
}

void frmMain::ExpandChildNodes(wxTreeItemId node, wxArrayString &expandedNodes)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = browser->GetFirstChild(node, cookie);

	while (child.IsOk())
	{
		if (expandedNodes.Index(GetNodePath(child)) != wxNOT_FOUND)
		{
			browser->Expand(child);
			ExpandChildNodes(child, expandedNodes);
		}

		child = browser->GetNextChild(node, cookie);
	}
}

wxString frmMain::GetNodePath(wxTreeItemId node)
{
	wxString path;
	path = browser->GetItemText(node).BeforeFirst('(');

	wxTreeItemId parent = browser->GetItemParent(node);
	while (parent.IsOk())
	{
		path = browser->GetItemText(parent).BeforeFirst('(') + wxT("/") + path;
		parent = browser->GetItemParent(parent);
	}

	return path;
}

// Return the path for the current node
wxString frmMain::GetCurrentNodePath()
{
	return GetNodePath(currentObject->GetId());
}

// Attempt to reselect the node with the given path
bool frmMain::SetCurrentNode(wxTreeItemId node, const wxString &path)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = browser->GetFirstChild(node, cookie);

	while (child.IsOk())
	{
		if (GetNodePath(child) == path)
		{
			browser->SelectItem(child, true);
			return true;
		}
		else if (SetCurrentNode(child, path))
			return true;

		child = browser->GetNextChild(node, cookie);
	}

	return false;
}

void frmMain::ShowObjStatistics(pgObject *data, int sel)
{
	switch (sel)
	{
		case NBP_STATISTICS:
		{
			statistics->Freeze();
			statistics->ClearAll();
			statistics->AddColumn(_("Statistics"), statistics->GetSize().GetWidth() - 10);
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


void frmMain::OnPageChange(wxNotebookEvent &event)
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

	wxTreeItemIdValue foldercookie;
	wxTreeItemId folderitem = browser->GetFirstChild(browser->GetRootItem(), foldercookie);
	while (folderitem)
	{
		if (browser->ItemHasChildren(folderitem))
		{
			wxCookieType cookie;
			wxTreeItemId serverItem = browser->GetFirstChild(folderitem, cookie);
			while (serverItem)
			{
				pgServer *server = (pgServer *)browser->GetObject(serverItem);

				if (server && server->IsCreatedBy(serverFactory) && server->connection())
				{
					if (server->connection()->IsAlive())
					{
						wxCookieType cookie2;
						wxTreeItemId item = browser->GetFirstChild(serverItem, cookie2);
						while (item)
						{
							pgObject *obj = browser->GetObject(item);
							if (obj && obj->IsCreatedBy(databaseFactory.GetCollectionFactory()))
							{
								wxCookieType cookie3;
								item = browser->GetFirstChild(obj->GetId(), cookie3);
								while (item)
								{
									pgDatabase *db = (pgDatabase *)browser->GetObject(item);
									if (db && db->IsCreatedBy(databaseFactory))
									{
										pgConn *conn = db->GetConnection();
										if (conn)
										{
											if (!conn->IsAlive() && (conn->GetStatus() == PGCONN_BROKEN || conn->GetStatus() == PGCONN_BAD))
											{
												conn->Close();
												if (!userInformed)
												{
													wxMessageDialog dlg(this, _("Do you want to attempt to reconnect to the database?"),
													                    wxString::Format(_("Connection to database %s lost."), db->GetName().c_str()),
													                    wxICON_EXCLAMATION | wxYES_NO | wxYES_DEFAULT);

													closeIt = (dlg.ShowModal() == wxID_NO);
													userInformed = true;
												}
												if (closeIt)
												{
													db->Disconnect();

													browser->DeleteChildren(db->GetId());
													db->UpdateIcon(browser);
												}
												else
												{
													// Create a server object and connect it.
													wxBusyInfo waiting(wxString::Format(_("Reconnecting to database %s"),
													                                    db->GetName().c_str()), this);

													// Give the UI a chance to redraw
													wxSafeYield();
													wxMilliSleep(100);
													wxSafeYield();

													if (!conn->Reconnect())
													{
														db->Disconnect();

														browser->DeleteChildren(db->GetId());
														db->UpdateIcon(browser);
													}
													else
														// Indicate things are back to normal
														userInformed = false;
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
								wxMessageDialog dlg(this, _("Do you want to attempt to reconnect to the server?"),
								                    wxString::Format(_("Connection to server %s lost."), server->GetName().c_str()),
								                    wxICON_EXCLAMATION | wxYES_NO | wxYES_DEFAULT);

								closeIt = (dlg.ShowModal() == wxID_NO);
								userInformed = true;
							}
							if (closeIt)
							{
								server->Disconnect(this);
								browser->SelectItem(serverItem);
								execSelChange(serverItem, true);
								browser->DeleteChildren(serverItem);
							}
							else
							{
								// Create a server object and connect it.
								wxBusyInfo waiting(wxString::Format(_("Reconnecting to server %s (%s:%d)"),
								                                    server->GetDescription().c_str(), server->GetName().c_str(), server->GetPort()), this);

								// Give the UI a chance to redraw
								wxSafeYield();
								wxMilliSleep(100);
								wxSafeYield();

								if (!server->connection()->Reconnect())
								{
									server->Disconnect(this);
									browser->SelectItem(serverItem);
									execSelChange(serverItem, true);
									browser->DeleteChildren(serverItem);
								}
								else
									// Indicate things are back to normal
									userInformed = false;
							}
						}
					}
				}

				serverItem = browser->GetNextChild(serversObj->GetId(), cookie);
			}
		}
		folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
	}
	return userInformed;
}


wxTreeItemId frmMain::RestoreEnvironment(pgServer *server)
{
	wxTreeItemId item, lastItem;
	wxString lastDatabase = server->GetLastDatabase();
	if (lastDatabase.IsNull())
		return item;

	wxCookieType cookie;
	pgObject *data = 0;
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
	listViews->SetSelection(NBP_PROPERTIES);
	data->ShowTree(this, browser, 0, 0);
	lastItem = item;

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

	wxString lastSchema = server->GetLastSchema();
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
	lastItem = item;

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

	// Give the UI a chance to redraw
	wxSafeYield();
	wxMilliSleep(100);
	wxSafeYield();

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

			browser->Freeze();
			if (restore && server->GetRestore())
				item = RestoreEnvironment(server);
			else
				item = server->GetId();
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
			{
				if (restore && server->GetRestore())
					EndMsg(false);
				else
					EndMsg(true);
			}
			if (item)
				GetMenuFactories()->CheckMenu((pgObject *)browser->GetItemData(item), GetMenuBar(), (ctlMenuToolbar *)GetToolBar());
			else
				GetMenuFactories()->CheckMenu(server, GetMenuBar(), (ctlMenuToolbar *)GetToolBar());
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
	bool identified = false;

	wxString translated = wxGetTranslation(msgToIdentify);
	if (translated != msgToIdentify)
	{
		identified = (error.Find(translated) >= 0);
	}

	if (!identified)
	{
		if (msgToIdentify.Left(20) == wxT("Translator attention"))
			identified = (error.Find(msgToIdentify.Mid(msgToIdentify.Find('!') + 1)) >= 0);
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
	wxString error = server->GetLastError();
	bool wantHint = false;

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

	wxTreeItemIdValue foldercookie;
	wxTreeItemId folderitem = browser->GetFirstChild(browser->GetRootItem(), foldercookie);
	while (folderitem)
	{
		if (browser->ItemHasChildren(folderitem))
		{
			wxTreeItemIdValue servercookie;
			wxTreeItemId serveritem = browser->GetFirstChild(folderitem, servercookie);
			while (serveritem)
			{
				server = (pgServer *)browser->GetItemData(serveritem);
				if (server->IsCreatedBy(serverFactory))
				{
					wxString key;
					++numServers;

					key.Printf(wxT("Servers/%d/"), numServers);
					settings->Write(key + wxT("Server"), server->GetName());
					settings->Write(key + wxT("Description"), server->GetDescription());
					settings->Write(key + wxT("ServiceID"), server->GetServiceID());
					settings->Write(key + wxT("DiscoveryID"), server->GetDiscoveryID());
					settings->Write(key + wxT("Port"), server->GetPort());
					settings->Write(key + wxT("StorePwd"), server->GetStorePwd());
					settings->Write(key + wxT("Restore"), server->GetRestore());
					settings->Write(key + wxT("Database"), server->GetDatabaseName());
					settings->Write(key + wxT("Username"), server->GetUsername());
					settings->Write(key + wxT("LastDatabase"), server->GetLastDatabase());
					settings->Write(key + wxT("LastSchema"), server->GetLastSchema());
					settings->Write(key + wxT("DbRestriction"), server->GetDbRestriction());
					settings->Write(key + wxT("Colour"), server->GetColour());
					settings->Write(key + wxT("SSL"), server->GetSSL());
					settings->Write(key + wxT("Group"), server->GetGroup());

					pgCollection *coll = browser->FindCollection(databaseFactory, server->GetId());
					if (coll)
					{
						treeObjectIterator dbs(browser, coll);
						pgDatabase *db;

						while ((db = (pgDatabase *)dbs.GetNextObject()) != 0)
							settings->Write(key + wxT("Databases/") + db->GetName() + wxT("/SchemaRestriction"), db->GetSchemaRestriction());
					}
				}
				serveritem = browser->GetNextChild(folderitem, servercookie);
			}
		}
		folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
	}

	// Write the server count
	settings->Write(wxT("Servers/Count"), numServers);
	wxLogInfo(wxT("Stored %d servers."), numServers);
}


void frmMain::RetrieveServers()
{
	wxString label;
	int total = 0;
	wxTreeItemIdValue groupcookie;
	wxTreeItemId groupitem;

	// Retrieve previously stored servers
	wxLogInfo(wxT("Reloading servers..."));

	// Create all servers' nodes
	serverFactory.CreateObjects(serversObj, browser);

	// Count number of servers and groups
	groupitem = browser->GetFirstChild(browser->GetRootItem(), groupcookie);
	while (groupitem)
	{
		total = browser->GetChildrenCount(groupitem, false);
		label = browser->GetItemText(groupitem) + wxT(" (") + NumToStr((long)total) + wxT(")");
		browser->SetItemText(groupitem, label);
		browser->SortChildren(groupitem);
		browser->Expand(groupitem);
		groupitem = browser->GetNextChild(browser->GetRootItem(), groupcookie);
	}
}

pgServer *frmMain::ConnectToServer(const wxString &servername, bool restore)
{
	wxTreeItemIdValue foldercookie, servercookie;
	wxTreeItemId folderitem, serveritem;
	pgObject *object;
	pgServer *server;

	folderitem = browser->GetFirstChild(browser->GetRootItem(), foldercookie);
	while (folderitem)
	{
		if (browser->ItemHasChildren(folderitem))
		{
			serveritem = browser->GetFirstChild(folderitem, servercookie);
			while (serveritem)
			{
				object = browser->GetObject(serveritem);
				if (object->IsCreatedBy(serverFactory))
				{
					server = (pgServer *)object;
					if (server->GetDescription() == servername)
					{
						ReconnectServer(server, restore);
						return server;
					}
				}
				serveritem = browser->GetNextChild(folderitem, servercookie);
			}
		}
		folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
	}

	return 0;
}

void frmMain::StartMsg(const wxString &msg)
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
		time.Printf(_("%.2f secs"), (timeval / 1000));
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

void frmMain::SetItemBackgroundColour(wxTreeItemId item, wxColour colour)
{
	wxTreeItemIdValue cookie;

	browser->SetItemBackgroundColour(item, wxColour(colour));
	if (browser->ItemHasChildren(item))
	{
		wxTreeItemId childitem = browser->GetFirstChild(item, cookie);
		while (childitem)
		{
			SetItemBackgroundColour(childitem, colour);
			childitem = browser->GetNextChild(item, cookie);
		}
	}
}


/////////////////////////////////////////


contentsFactory::contentsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("&Help contents"), _("Open the helpfile."));
}


wxWindow *contentsFactory::StartDialog(frmMain *form, pgObject *obj)
{
	DisplayHelp(wxT("index"), HELP_PGADMIN);
	return 0;
}


faqFactory::faqFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("&FAQ"), _("Frequently asked questions."));
}


wxWindow *faqFactory::StartDialog(frmMain *form, pgObject *obj)
{
	wxLaunchDefaultBrowser(wxT("http://www.pgadmin.org/support/faq.php"));

	return 0;
}

#include "images/help.xpm"
#include "images/help2.xpm"
pgsqlHelpFactory::pgsqlHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigIcon) : actionFactory(list)
{
	mnu->Append(id, _("&PostgreSQL Help"), _("Display help on the PostgreSQL database system."));
	if (toolbar)
	{
		if (bigIcon)
			toolbar->AddTool(id, _("SQL Help"), wxBitmap(help2_xpm), _("Display help on SQL commands."));
		else
			toolbar->AddTool(id, _("SQL Help"), wxBitmap(help_xpm), _("Display help on SQL commands."));
	}
}


wxWindow *pgsqlHelpFactory::StartDialog(frmMain *form, pgObject *obj)
{
	DisplayHelp(wxT("index"), HELP_POSTGRESQL);
	return 0;
}

edbHelpFactory::edbHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigIcon) : actionFactory(list)
{
	mnu->Append(id, _("&EnterpriseDB Help"), _("Display help on the EnterpriseDB database system."));
}


wxWindow *edbHelpFactory::StartDialog(frmMain *form, pgObject *obj)
{
	DisplayHelp(wxT("index"), HELP_ENTERPRISEDB);
	return 0;
}

greenplumHelpFactory::greenplumHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigIcon) : actionFactory(list)
{
	mnu->Append(id, _("&Greenplum Database Help"), _("Display help on the Greenplum Database system."));
}


wxWindow *greenplumHelpFactory::StartDialog(frmMain *form, pgObject *obj)
{
	DisplayHelp(wxT("index"), HELP_GREENPLUM);
	return 0;
}


slonyHelpFactory::slonyHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigIcon) : actionFactory(list)
{
	mnu->Append(id, _("&Slony Help"), _("Display help on the Slony replication system."));
}


wxWindow *slonyHelpFactory::StartDialog(frmMain *form, pgObject *obj)
{
	DisplayHelp(wxT("index"), HELP_SLONY);
	return 0;
}

bugReportFactory::bugReportFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("&Bug Report"), _("How to send a bugreport to the pgAdmin Development Team."));
}


wxWindow *bugReportFactory::StartDialog(frmMain *form, pgObject *obj)
{
	DisplayHelp(wxT("bugreport"), HELP_PGADMIN);
	return 0;
}
